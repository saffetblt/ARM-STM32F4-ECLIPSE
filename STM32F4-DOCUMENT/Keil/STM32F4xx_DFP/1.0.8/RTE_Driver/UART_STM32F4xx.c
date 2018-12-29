/* ----------------------------------------------------------------------
 * Copyright (C) 2014 ARM Limited. All rights reserved.
 *  
 * $Date:        29. April 2014
 * $Revision:    V1.02
 *  
 * Driver:       Driver_UART1, Driver_UART2, Driver_UART3, Driver_UART4, 
 *               Driver_UART5, Driver_UART6, Driver_UART7, Driver_UART8
 * Configured:   via RTE_Device.h configuration file 
 * Project:      Middleware UART Driver for ST STM32F4xx
 * ---------------------------------------------------------------------- 
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 * 
 *   Configuration Setting                Value     UART Interface
 *   ---------------------                -----     --------------
 *   Connect to hardware via Driver_UART# = 1       use USART1
 *   Connect to hardware via Driver_UART# = 2       use USART2
 *   Connect to hardware via Driver_UART# = 3       use USART3
 *   Connect to hardware via Driver_UART# = 4       use UART4
 *   Connect to hardware via Driver_UART# = 5       use UART5
 *   Connect to hardware via Driver_UART# = 6       use USART6
 *   Connect to hardware via Driver_UART# = 7       use UART7
 *   Connect to hardware via Driver_UART# = 8       use UART8
 * -------------------------------------------------------------------- */

/* History:
 *  Version 1.02
 *    Added UART7 and UART8.
 *  Version 1.01
 *    Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.00
 *    Initial release
 */ 

#include "cmsis_os.h"
#include "stm32f4xx.h"

#include "GPIO_STM32F4xx.h"
#include "DMA_STM32F4xx.h"

#include "Driver_UART.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#include <string.h>

#define ARM_UART_DRV_VERSION ARM_DRV_VERSION_MAJOR_MINOR(1,02)   /* driver version */

#if ((defined(RTE_Drivers_UART1) || \
      defined(RTE_Drivers_UART2) || \
      defined(RTE_Drivers_UART3) || \
      defined(RTE_Drivers_UART4) || \
      defined(RTE_Drivers_UART5) || \
      defined(RTE_Drivers_UART6) || \
      defined(RTE_Drivers_UART7) || \
      defined(RTE_Drivers_UART8))   \
     && !RTE_USART1                 \
     && !RTE_USART2                 \
     && !RTE_USART3                 \
     && !RTE_UART4                  \
     && !RTE_UART5                  \
     && !RTE_USART6                 \
     && !RTE_UART7                  \
     && !RTE_UART8)
#error "UART not configured in RTE_Device.h!"
#endif

#if (RTE_USART1 && RTE_USART1_RX_DMA)
#warning "UART1 does not support receive DMA! Disable receive DMA in RTE_Device.h!"
#endif

#if (RTE_USART2 && RTE_USART2_RX_DMA)
#warning "UART2 does not support receive DMA! Disable receive DMA in RTE_Device.h!"
#endif

#if (RTE_USART3 && RTE_USART3_RX_DMA)
#warning "UART3 does not support receive DMA! Disable receive DMA in RTE_Device.h!"
#endif

#if (RTE_UART4 && RTE_UART4_RX_DMA)
#warning "UART4 does not support receive DMA! Disable receive DMA in RTE_Device.h!"
#endif

#if (RTE_UART5 && RTE_UART5_RX_DMA)
#warning "UART5 does not support receive DMA! Disable receive DMA in RTE_Device.h!"
#endif

#if (RTE_USART6 && RTE_USART6_RX_DMA)
#warning "UART6 does not support receive DMA! Disable receive DMA in RTE_Device.h!"
#endif

#if (RTE_UART7 && RTE_UART7_RX_DMA)
#warning "UART7 does not support receive DMA! Disable receive DMA in RTE_Device.h!"
#endif

#if (RTE_UART8 && RTE_UART8_RX_DMA)
#warning "UART8 does not support receive DMA! Disable receive DMA in RTE_Device.h!"
#endif

#define DMA_TX  ((RTE_USART1 && RTE_USART1_TX_DMA) ||       \
                 (RTE_USART2 && RTE_USART2_TX_DMA) ||       \
                 (RTE_USART3 && RTE_USART3_TX_DMA) ||       \
                 (RTE_UART4  && RTE_UART4_TX_DMA ) ||       \
                 (RTE_UART5 &&  RTE_UART5_TX_DMA)  ||       \
                 (RTE_USART6 && RTE_USART6_TX_DMA) ||       \
                 (RTE_UART7 &&  RTE_UART7_TX_DMA)  ||       \
                 (RTE_UART8 &&  RTE_UART8_TX_DMA))


/* Driver Version */
static const ARM_DRV_VERSION DriverVersion = {
  ARM_UART_API_VERSION,
  ARM_UART_DRV_VERSION
};

/* UART Data Information (Run-Time) */
typedef struct {
  volatile uint16_t      idx_in;
  volatile uint16_t      idx_out;
  volatile bool          empty;
           uint16_t      threshold;
           uint16_t      transfer_len;
} UART_DATA_INFO;

/* UART Information (Run-Time) */
typedef struct {
  ARM_UART_SignalEvent_t cb_event;
  uint32_t               event_mask;
  osSemaphoreId          semaphore;
  osMutexId              mutex_write;
  osMutexId              mutex_read;
  UART_DATA_INFO         rx;
  UART_DATA_INFO         tx;
  bool                   com_break;
  bool                   rts_cts_flow;
} UART_INFO;

/* UART Resources definition */
typedef struct {
  const ARM_UART_CAPABILITIES  *capabilities;         // Capabilities
        GPIO_PIN_ID             pin_tx;               // TX  Pin identifier
        GPIO_PIN_ID             pin_rx;               // RX  Pin identifier
        GPIO_PIN_ID             pin_cts;              // CTS Pin identifier
        GPIO_PIN_ID             pin_rts;              // RTS Pin identifier
        GPIO_AF_ID              pin_af;               // Pin AF identifier
        USART_TypeDef          *uart;                 // Pointer to UART peripheral
  enum  IRQn                    uart_irq;             // UART IRQ Number
  __IO  uint32_t               *clock_reg;            // Pointer to Clock register
        uint32_t                clock_mask;           // Clock register mask
  __IO  uint32_t               *reset_reg;            // Pointer to Reset register
        uint32_t                reset_mask;           // Reset register mask
        uint32_t                clock;                // Peripheral clock
        bool                    hw_flow;              // Hardware flow control
        bool                    manual_rts_cts;       // Manual RTS CTS control
        uint8_t                *tx_buffer;            // Transmit buffer
        uint32_t                tx_buffer_size;       // Transmit buffer size
        uint8_t                *rx_buffer;            // Receive buffer
        uint32_t                rx_buffer_size;       // Receive buffer size
        uint32_t                flow_ctrl_threshold;  // Flow control threshold
  const DMA_INFO               *dma_tx;               // Transmit DMA configuration
  const osSemaphoreDef_t       *semaphore;            // Tx Semaphore definition
  const osMutexDef_t           *mutex_write;          // Write Mutex definition
  const osMutexDef_t           *mutex_read;           // Write Mutex definition
        UART_INFO              *info;                 // Run-Time information
} const UART_RESOURCES;


/* UART1 */
#if (RTE_USART1 != 0)

#ifndef __UART1_TX_BUFFER_SIZE
#define __UART1_TX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART1_RX_BUFFER_SIZE
#define __UART1_RX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART1_FLOW_CONTROL_THRESHOLD
#define __UART1_FLOW_CONTROL_THRESHOLD  16
#endif

/* UART1 Capabilities */
static const ARM_UART_CAPABILITIES UART1_Capabilities = {
  __UART1_TX_BUFFER_SIZE,           // Transmit Buffer Size
  __UART1_RX_BUFFER_SIZE,           // Receive Buffer Size
  0,                                // Receive character time-out event
#if (RTE_USART1_HW_FLOW != 0)
  1,                                // CTS Line available
  1,                                // Signal CTS change event
#else
  0,                                // CTS Line available
  0,                                // Signal CTS change event
#endif
  0,                                // DSR Line available
  0,                                // Signal DSR change event
  0,                                // DCD Line available
  0,                                // Signal DCD change event
  0,                                // RI Line available
  0,                                // Signal RI change event
  0
};

/* UART1 DMA */
#if (RTE_USART1_TX_DMA != 0)
static const DMA_INFO UART1_DMA_TX = {
  DMAx_STREAMy(RTE_USART1_TX_DMA_NUMBER, RTE_USART1_TX_DMA_STREAM),
  RTE_USART1_TX_DMA_NUMBER,
  RTE_USART1_TX_DMA_STREAM,
  RTE_USART1_TX_DMA_CHANNEL,
  RTE_USART1_TX_DMA_PRIORITY
};
#endif

/* UART1 Semaphores */
osSemaphoreDef(UART1_Semaphore);

/* UART1 Mutex */
osMutexDef(UART1_Mutex_write);
osMutexDef(UART1_Mutex_read);


/* UART1 Data Info */
// UART Buffer size must be 2^n
static struct {
  uint8_t  tx[__UART1_TX_BUFFER_SIZE];
  uint8_t  rx[__UART1_RX_BUFFER_SIZE];
} UART1_Buffer;

static UART_INFO UART1_Info;

/* UART1 Resources */
static UART_RESOURCES UART1_Resources = {
  &UART1_Capabilities,
  { RTE_USART1_TX_PORT,  RTE_USART1_TX_BIT  },
  { RTE_USART1_RX_PORT,  RTE_USART1_RX_BIT  },
  { RTE_USART1_CTS_PORT, RTE_USART1_CTS_BIT },
  { RTE_USART1_RTS_PORT, RTE_USART1_RTS_BIT },
  GPIO_AF_USART1,
  USART1,
  USART1_IRQn,
  &RCC->APB2ENR,
  RCC_APB2ENR_USART1EN,
  &RCC->APB2RSTR,
  RCC_APB2RSTR_USART1RST,
  RTE_PCLK2,
  RTE_USART1_HW_FLOW,
  RTE_USART1_MANUAL_FLOW,
  UART1_Buffer.tx,
  __UART1_TX_BUFFER_SIZE,
  UART1_Buffer.rx,
  __UART1_RX_BUFFER_SIZE,
  __UART1_FLOW_CONTROL_THRESHOLD,
#if (RTE_USART1_TX_DMA != 0)
  &UART1_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(UART1_Semaphore),
  osMutex(UART1_Mutex_write),
  osMutex(UART1_Mutex_read),
  &UART1_Info
};
#endif

/* UART2 */
#if (RTE_USART2 != 0)

#ifndef __UART2_TX_BUFFER_SIZE
#define __UART2_TX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART2_RX_BUFFER_SIZE
#define __UART2_RX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART2_FLOW_CONTROL_THRESHOLD
#define __UART2_FLOW_CONTROL_THRESHOLD  16
#endif

/* UART2 Capabilities */
static const ARM_UART_CAPABILITIES UART2_Capabilities = {
  __UART2_TX_BUFFER_SIZE,           // Transmit Buffer Size
  __UART2_RX_BUFFER_SIZE,           // Receive Buffer Size
  0,                                // Receive character time-out event
#if (RTE_USART1_HW_FLOW != 0)
  1,                                // CTS Line available
  1,                                // Signal CTS change event
#else
  0,                                // CTS Line available
  0,                                // Signal CTS change event
#endif
  0,                                // DSR Line available
  0,                                // Signal DSR change event
  0,                                // DCD Line available
  0,                                // Signal DCD change event
  0,                                // RI Line available
  0,                                // Signal RI change event
  0
};

/* UART2 DMA */
#if (RTE_USART2_TX_DMA != 0)
static const DMA_INFO UART2_DMA_TX = {
  DMAx_STREAMy(RTE_USART2_TX_DMA_NUMBER, RTE_USART2_TX_DMA_STREAM),
  RTE_USART2_TX_DMA_NUMBER,
  RTE_USART2_TX_DMA_STREAM,
  RTE_USART2_TX_DMA_CHANNEL,
  RTE_USART2_TX_DMA_PRIORITY
};
#endif

/* UART2 Semaphores */
osSemaphoreDef(UART2_Semaphore);

/* UART2 Mutex */
osMutexDef(UART2_Mutex_write);
osMutexDef(UART2_Mutex_read);


/* UART2 Data Info */
// UART Buffer size must be 2^n
static struct {
  uint8_t  tx[__UART2_TX_BUFFER_SIZE];
  uint8_t  rx[__UART2_RX_BUFFER_SIZE];
} UART2_Buffer;

static UART_INFO UART2_Info;

/* UART2 Resources */
static UART_RESOURCES UART2_Resources = {
  &UART2_Capabilities,
  { RTE_USART2_TX_PORT,  RTE_USART2_TX_BIT  },
  { RTE_USART2_RX_PORT,  RTE_USART2_RX_BIT  },
  { RTE_USART2_CTS_PORT, RTE_USART2_CTS_BIT },
  { RTE_USART2_RTS_PORT, RTE_USART2_RTS_BIT },
  GPIO_AF_USART2,
  USART2,
  USART2_IRQn,
  &RCC->APB1ENR,
  RCC_APB1ENR_USART2EN,
  &RCC->APB1RSTR,
  RCC_APB1RSTR_USART2RST,
  RTE_PCLK1,
  RTE_USART2_HW_FLOW,
  RTE_USART2_MANUAL_FLOW,
  UART2_Buffer.tx,
  __UART2_TX_BUFFER_SIZE,
  UART2_Buffer.rx,
  __UART2_RX_BUFFER_SIZE,
  __UART2_FLOW_CONTROL_THRESHOLD,
#if (RTE_USART2_TX_DMA != 0)
  &UART2_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(UART2_Semaphore),
  osMutex(UART2_Mutex_write),
  osMutex(UART2_Mutex_read),
  &UART2_Info
};
#endif

/* UART3 */
#if (RTE_USART3 != 0)

#ifndef __UART3_TX_BUFFER_SIZE
#define __UART3_TX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART3_RX_BUFFER_SIZE
#define __UART3_RX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART3_FLOW_CONTROL_THRESHOLD
#define __UART3_FLOW_CONTROL_THRESHOLD  16
#endif

/* UART3 Capabilities */
static const ARM_UART_CAPABILITIES UART3_Capabilities = {
  __UART3_TX_BUFFER_SIZE,           // Transmit Buffer Size
  __UART3_RX_BUFFER_SIZE,           // Receive Buffer Size
  0,                                // Receive character time-out event
#if (RTE_USART3_HW_FLOW != 0)
  1,                                // CTS Line available
  1,                                // Signal CTS change event
#else
  0,                                // CTS Line available
  0,                                // Signal CTS change event
#endif
  0,                                // DSR Line available
  0,                                // Signal DSR change event
  0,                                // DCD Line available
  0,                                // Signal DCD change event
  0,                                // RI Line available
  0,                                // Signal RI change event
  0
};

/* UART3 DMA */
#if (RTE_USART3_TX_DMA != 0)
static const DMA_INFO UART3_DMA_TX = {
  DMAx_STREAMy(RTE_USART3_TX_DMA_NUMBER, RTE_USART3_TX_DMA_STREAM),
  RTE_USART3_TX_DMA_NUMBER,
  RTE_USART3_TX_DMA_STREAM,
  RTE_USART3_TX_DMA_CHANNEL,
  RTE_USART3_TX_DMA_PRIORITY
};
#endif

/* UART3 Semaphores */
osSemaphoreDef(UART3_Semaphore);

/* UART3 Mutex */
osMutexDef(UART3_Mutex_write);
osMutexDef(UART3_Mutex_read);


/* UART3 Data Info */
// UART Buffer size must be 2^n
static struct {
  uint8_t  tx[__UART3_TX_BUFFER_SIZE];
  uint8_t  rx[__UART3_RX_BUFFER_SIZE];
} UART3_Buffer;

static UART_INFO UART3_Info;

/* UART3 Resources */
static UART_RESOURCES UART3_Resources = {
  &UART3_Capabilities,
  { RTE_USART3_TX_PORT,  RTE_USART3_TX_BIT  },
  { RTE_USART3_RX_PORT,  RTE_USART3_RX_BIT  },
  { RTE_USART3_CTS_PORT, RTE_USART3_CTS_BIT },
  { RTE_USART3_RTS_PORT, RTE_USART3_RTS_BIT },
  GPIO_AF_USART3,
  USART3,
  USART3_IRQn,
  &RCC->APB1ENR,
  RCC_APB1ENR_USART3EN,
  &RCC->APB1RSTR,
  RCC_APB1RSTR_USART3RST,
  RTE_PCLK1,
  RTE_USART3_HW_FLOW,
  RTE_USART3_MANUAL_FLOW,
  UART3_Buffer.tx,
  __UART3_TX_BUFFER_SIZE,
  UART3_Buffer.rx,
  __UART3_RX_BUFFER_SIZE,
  __UART3_FLOW_CONTROL_THRESHOLD,
#if (RTE_USART3_TX_DMA != 0)
  &UART3_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(UART3_Semaphore),
  osMutex(UART3_Mutex_write),
  osMutex(UART3_Mutex_read),
  &UART3_Info
};
#endif


/* UART4 */
#if (RTE_UART4 != 0)

#ifndef __UART4_TX_BUFFER_SIZE
#define __UART4_TX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART4_RX_BUFFER_SIZE
#define __UART4_RX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART4_FLOW_CONTROL_THRESHOLD
#define __UART4_FLOW_CONTROL_THRESHOLD  16
#endif

/* UART4 Capabilities */
static const ARM_UART_CAPABILITIES UART4_Capabilities = {
  __UART4_TX_BUFFER_SIZE,           // Transmit Buffer Size
  __UART4_RX_BUFFER_SIZE,           // Receive Buffer Size
  0,                                // Receive character time-out event
  0,
  0,
  0,                                // DSR Line available
  0,                                // Signal DSR change event
  0,                                // DCD Line available
  0,                                // Signal DCD change event
  0,                                // RI Line available
  0,                                // Signal RI change event
  0
};

/* UART4 DMA */
#if (RTE_UART4_TX_DMA != 0)
static const DMA_INFO UART4_DMA_TX = {
  DMAx_STREAMy(RTE_UART4_TX_DMA_NUMBER, RTE_UART4_TX_DMA_STREAM),
  RTE_UART4_TX_DMA_NUMBER,
  RTE_UART4_TX_DMA_STREAM,
  RTE_UART4_TX_DMA_CHANNEL,
  RTE_UART4_TX_DMA_PRIORITY
};
#endif

/* UART4 Semaphores */
osSemaphoreDef(UART4_Semaphore);

/* UART4 Mutex */
osMutexDef(UART4_Mutex_write);
osMutexDef(UART4_Mutex_read);


/* UART4 Data Info */
// UART Buffer size must be 2^n
static struct {
  uint8_t  tx[__UART4_TX_BUFFER_SIZE];
  uint8_t  rx[__UART4_RX_BUFFER_SIZE];
} UART4_Buffer;

static UART_INFO UART4_Info;

/* UART4 Resources */
static UART_RESOURCES UART4_Resources = {
  &UART4_Capabilities,
  { RTE_UART4_TX_PORT,  RTE_UART4_TX_BIT  },
  { RTE_UART4_RX_PORT,  RTE_UART4_RX_BIT  },
  NULL, NULL,
  NULL, NULL,
  GPIO_AF_USART4,
  UART4,
  UART4_IRQn,
  &RCC->APB1ENR,
  RCC_APB1ENR_UART4EN,
  &RCC->APB1RSTR,
  RCC_APB1RSTR_UART4RST,
  RTE_PCLK1,
  0,
  0,
  UART4_Buffer.tx,
  __UART4_TX_BUFFER_SIZE,
  UART4_Buffer.rx,
  __UART4_RX_BUFFER_SIZE,
  __UART4_FLOW_CONTROL_THRESHOLD,
#if (RTE_UART4_TX_DMA != 0)
  &UART4_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(UART4_Semaphore),
  osMutex(UART4_Mutex_write),
  osMutex(UART4_Mutex_read),
  &UART4_Info
};
#endif


/* UART5 */
#if (RTE_UART5 != 0)

#ifndef __UART5_TX_BUFFER_SIZE
#define __UART5_TX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART5_RX_BUFFER_SIZE
#define __UART5_RX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART5_FLOW_CONTROL_THRESHOLD
#define __UART5_FLOW_CONTROL_THRESHOLD  16
#endif

/* UART5 Capabilities */
static const ARM_UART_CAPABILITIES UART5_Capabilities = {
  __UART5_TX_BUFFER_SIZE,           // Transmit Buffer Size
  __UART5_RX_BUFFER_SIZE,           // Receive Buffer Size
  0,                                // Receive character time-out event
  0,
  0,
  0,                                // DSR Line available
  0,                                // Signal DSR change event
  0,                                // DCD Line available
  0,                                // Signal DCD change event
  0,                                // RI Line available
  0,                                // Signal RI change event
  0
};

/* UART5 DMA */
#if (RTE_UART5_TX_DMA != 0)
static const DMA_INFO UART5_DMA_TX = {
  DMAx_STREAMy(RTE_UART5_TX_DMA_NUMBER, RTE_UART5_TX_DMA_STREAM),
  RTE_UART5_TX_DMA_NUMBER,
  RTE_UART5_TX_DMA_STREAM,
  RTE_UART5_TX_DMA_CHANNEL,
  RTE_UART5_TX_DMA_PRIORITY
};
#endif

/* UART5 Semaphores */
osSemaphoreDef(UART5_Semaphore);

/* UART4 Mutex */
osMutexDef(UART5_Mutex_write);
osMutexDef(UART5_Mutex_read);


/* UART5 Data Info */
// UART Buffer size must be 2^n
static struct {
  uint8_t  tx[__UART5_TX_BUFFER_SIZE];
  uint8_t  rx[__UART5_RX_BUFFER_SIZE];
} UART5_Buffer;

static UART_INFO UART5_Info;

/* UART5 Resources */
static UART_RESOURCES UART5_Resources = {
  &UART5_Capabilities,
  { RTE_UART5_TX_PORT,  RTE_UART5_TX_BIT  },
  { RTE_UART5_RX_PORT,  RTE_UART5_RX_BIT  },
  { NULL, NULL },
  { NULL, NULL },
  GPIO_AF_USART5,
  UART5,
  UART5_IRQn,
  &RCC->APB1ENR,
  RCC_APB1ENR_UART5EN,
  &RCC->APB1RSTR,
  RCC_APB1RSTR_UART5RST,
  RTE_PCLK1,
  NULL,
  NULL,
  UART5_Buffer.tx,
  __UART5_TX_BUFFER_SIZE,
  UART5_Buffer.rx,
  __UART5_RX_BUFFER_SIZE,
  __UART5_FLOW_CONTROL_THRESHOLD,
#if (RTE_UART5_TX_DMA != 0)
  &UART5_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(UART5_Semaphore),
  osMutex(UART5_Mutex_write),
  osMutex(UART5_Mutex_read),
  &UART5_Info
};
#endif

/* UART6 */
#if (RTE_USART6 != 0)

#ifndef __UART6_TX_BUFFER_SIZE
#define __UART6_TX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART6_RX_BUFFER_SIZE
#define __UART6_RX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART6_FLOW_CONTROL_THRESHOLD
#define __UART6_FLOW_CONTROL_THRESHOLD  16
#endif

/* UART6 Capabilities */
static const ARM_UART_CAPABILITIES UART6_Capabilities = {
  __UART6_TX_BUFFER_SIZE,           // Transmit Buffer Size
  __UART6_RX_BUFFER_SIZE,           // Receive Buffer Size
  0,                                // Receive character time-out event
#if (RTE_USART6_HW_FLOW != 0)
  1,                                // CTS Line available
  1,                                // Signal CTS change event
#else
  0,                                // CTS Line available
  0,                                // Signal CTS change event
#endif
  0,                                // DSR Line available
  0,                                // Signal DSR change event
  0,                                // DCD Line available
  0,                                // Signal DCD change event
  0,                                // RI Line available
  0,                                // Signal RI change event
  0
};

/* UART6 DMA */
#if (RTE_USART6_TX_DMA != 0)
static const DMA_INFO UART6_DMA_TX = {
  DMAx_STREAMy(RTE_USART6_TX_DMA_NUMBER, RTE_USART6_TX_DMA_STREAM),
  RTE_USART6_TX_DMA_NUMBER,
  RTE_USART6_TX_DMA_STREAM,
  RTE_USART6_TX_DMA_CHANNEL,
  RTE_USART6_TX_DMA_PRIORITY
};
#endif

/* UART6 Semaphores */
osSemaphoreDef(UART6_Semaphore);

/* UART6 Mutex */
osMutexDef(UART6_Mutex_write);
osMutexDef(UART6_Mutex_read);

/* UART6 Data Info */
// UART Buffer size must be 2^n
static struct {
  uint8_t  tx[__UART6_TX_BUFFER_SIZE];
  uint8_t  rx[__UART6_RX_BUFFER_SIZE];
} UART6_Buffer;

static UART_INFO UART6_Info;

/* UART6 Resources */
static UART_RESOURCES UART6_Resources = {
  &UART6_Capabilities,
  { RTE_USART6_TX_PORT,  RTE_USART6_TX_BIT  },
  { RTE_USART6_RX_PORT,  RTE_USART6_RX_BIT  },
  { RTE_USART6_CTS_PORT, RTE_USART6_CTS_BIT },
  { RTE_USART6_RTS_PORT, RTE_USART6_RTS_BIT },
  GPIO_AF_USART6,
  USART6,
  USART6_IRQn,
  &RCC->APB2ENR,
  RCC_APB2ENR_USART6EN,
  &RCC->APB2RSTR,
  RCC_APB2RSTR_USART6RST,
  RTE_PCLK2,
  RTE_USART6_HW_FLOW,
  RTE_USART6_MANUAL_FLOW,
  UART6_Buffer.tx,
  __UART6_TX_BUFFER_SIZE,
  UART6_Buffer.rx,
  __UART6_RX_BUFFER_SIZE,
  __UART6_FLOW_CONTROL_THRESHOLD,
#if (RTE_USART6_TX_DMA != 0)
  &UART6_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(UART6_Semaphore),
  osMutex(UART6_Mutex_write),
  osMutex(UART6_Mutex_read),
  &UART6_Info
};
#endif

/* UART7 */
#if (RTE_UART7 != 0)

#ifndef __UART7_TX_BUFFER_SIZE
#define __UART7_TX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART7_RX_BUFFER_SIZE
#define __UART7_RX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART7_FLOW_CONTROL_THRESHOLD
#define __UART7_FLOW_CONTROL_THRESHOLD  16
#endif

/* UART7 Capabilities */
static const ARM_UART_CAPABILITIES UART7_Capabilities = {
  __UART7_TX_BUFFER_SIZE,           // Transmit Buffer Size
  __UART7_RX_BUFFER_SIZE,           // Receive Buffer Size
  0,                                // Receive character time-out event
  0,
  0,
  0,                                // DSR Line available
  0,                                // Signal DSR change event
  0,                                // DCD Line available
  0,                                // Signal DCD change event
  0,                                // RI Line available
  0,                                // Signal RI change event
  0
};

/* UART7 DMA */
#if (RTE_UART7_TX_DMA != 0)
static const DMA_INFO UART7_DMA_TX = {
  DMAx_STREAMy(RTE_UART7_TX_DMA_NUMBER, RTE_UART7_TX_DMA_STREAM),
  RTE_UART7_TX_DMA_NUMBER,
  RTE_UART7_TX_DMA_STREAM,
  RTE_UART7_TX_DMA_CHANNEL,
  RTE_UART7_TX_DMA_PRIORITY
};
#endif

/* UART7 Semaphores */
osSemaphoreDef(UART7_Semaphore);

/* UART4 Mutex */
osMutexDef(UART7_Mutex_write);
osMutexDef(UART7_Mutex_read);


/* UART7 Data Info */
// UART Buffer size must be 2^n
static struct {
  uint8_t  tx[__UART7_TX_BUFFER_SIZE];
  uint8_t  rx[__UART7_RX_BUFFER_SIZE];
} UART7_Buffer;

static UART_INFO UART7_Info;

/* UART7 Resources */
static UART_RESOURCES UART7_Resources = {
  &UART7_Capabilities,
  { RTE_UART7_TX_PORT,  RTE_UART7_TX_BIT  },
  { RTE_UART7_RX_PORT,  RTE_UART7_RX_BIT  },
  { NULL, NULL },
  { NULL, NULL },
  GPIO_AF_USART7,
  UART7,
  UART7_IRQn,
  &RCC->APB1ENR,
  RCC_APB1ENR_UART7EN,
  &RCC->APB1RSTR,
  RCC_APB1RSTR_UART7RST,
  RTE_PCLK1,
  NULL,
  NULL,
  UART7_Buffer.tx,
  __UART7_TX_BUFFER_SIZE,
  UART7_Buffer.rx,
  __UART7_RX_BUFFER_SIZE,
  __UART7_FLOW_CONTROL_THRESHOLD,
#if (RTE_UART7_TX_DMA != 0)
  &UART7_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(UART7_Semaphore),
  osMutex(UART7_Mutex_write),
  osMutex(UART7_Mutex_read),
  &UART7_Info
};
#endif


/* UART8 */
#if (RTE_UART8 != 0)

#ifndef __UART8_TX_BUFFER_SIZE
#define __UART8_TX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART8_RX_BUFFER_SIZE
#define __UART8_RX_BUFFER_SIZE         128  // Buffer size must be 2^n
#endif

#ifndef __UART8_FLOW_CONTROL_THRESHOLD
#define __UART8_FLOW_CONTROL_THRESHOLD  16
#endif

/* UART7 Capabilities */
static const ARM_UART_CAPABILITIES UART8_Capabilities = {
  __UART8_TX_BUFFER_SIZE,           // Transmit Buffer Size
  __UART8_RX_BUFFER_SIZE,           // Receive Buffer Size
  0,                                // Receive character time-out event
  0,
  0,
  0,                                // DSR Line available
  0,                                // Signal DSR change event
  0,                                // DCD Line available
  0,                                // Signal DCD change event
  0,                                // RI Line available
  0,                                // Signal RI change event
  0
};

/* UART8 DMA */
#if (RTE_UART8_TX_DMA != 0)
static const DMA_INFO UART8_DMA_TX = {
  DMAx_STREAMy(RTE_UART8_TX_DMA_NUMBER, RTE_UART8_TX_DMA_STREAM),
  RTE_UART8_TX_DMA_NUMBER,
  RTE_UART8_TX_DMA_STREAM,
  RTE_UART8_TX_DMA_CHANNEL,
  RTE_UART8_TX_DMA_PRIORITY
};
#endif

/* UART8 Semaphores */
osSemaphoreDef(UART8_Semaphore);

/* UART8 Mutex */
osMutexDef(UART8_Mutex_write);
osMutexDef(UART8_Mutex_read);


/* UART8 Data Info */
// UART Buffer size must be 2^n
static struct {
  uint8_t  tx[__UART8_TX_BUFFER_SIZE];
  uint8_t  rx[__UART8_RX_BUFFER_SIZE];
} UART8_Buffer;

static UART_INFO UART8_Info;

/* UART8 Resources */
static UART_RESOURCES UART8_Resources = {
  &UART8_Capabilities,
  { RTE_UART8_TX_PORT,  RTE_UART8_TX_BIT  },
  { RTE_UART8_RX_PORT,  RTE_UART8_RX_BIT  },
  { NULL, NULL },
  { NULL, NULL },
  GPIO_AF_USART8,
  UART8,
  UART8_IRQn,
  &RCC->APB1ENR,
  RCC_APB1ENR_UART8EN,
  &RCC->APB1RSTR,
  RCC_APB1RSTR_UART8RST,
  RTE_PCLK1,
  NULL,
  NULL,
  UART8_Buffer.tx,
  __UART8_TX_BUFFER_SIZE,
  UART8_Buffer.rx,
  __UART8_RX_BUFFER_SIZE,
  __UART8_FLOW_CONTROL_THRESHOLD,
#if (RTE_UART8_TX_DMA != 0)
  &UART8_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(UART8_Semaphore),
  osMutex(UART8_Mutex_write),
  osMutex(UART8_Mutex_read),
  &UART8_Info
};
#endif

/* UART Resources */
static UART_RESOURCES *ptr_UART[8] = {
#if (RTE_USART1 != 0)
  &UART1_Resources,
#else
  NULL,
#endif
#if (RTE_USART2 != 0)
  &UART2_Resources,
#else
  NULL,
#endif
#if (RTE_USART3 != 0)
  &UART3_Resources,
#else
  NULL,
#endif
#if (RTE_UART4 != 0)
  &UART4_Resources,
#else
  NULL,
#endif
#if (RTE_UART5 != 0)
  &UART5_Resources,
#else
  NULL,
#endif
#if (RTE_USART6 != 0)
  &UART6_Resources,
#else
  NULL,
#endif
#if (RTE_UART7 != 0)
  &UART7_Resources,
#else
  NULL,
#endif
#if (RTE_UART8 != 0)
  &UART8_Resources,
#else
  NULL,
#endif
};


// Define Baud rate setting for USART
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))


/**
  \fn          ARM_DRV_VERSION UARTx_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRV_VERSION UARTX_GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          ARM_UART_CAPABILITIES UART_GetCapabilities (UART_RESOURCES *ptr_uart)
  \brief       Get driver capabilities.
  \param[in]   ptr_uart    Pointer to UART resources
  \return      \ref UART_CAPABILITIES
*/
static ARM_UART_CAPABILITIES UART_GetCapabilities (UART_RESOURCES *ptr_uart) {
  return *ptr_uart->capabilities;
}

#if (RTE_USART1 != 0)
  static ARM_UART_CAPABILITIES UART1_GetCapabilities (void) {
    return UART_GetCapabilities (ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_CAPABILITIES UART2_GetCapabilities (void) {
    return UART_GetCapabilities (ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_CAPABILITIES UART3_GetCapabilities (void) {
    return UART_GetCapabilities (ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_CAPABILITIES UART4_GetCapabilities (void) {
    return UART_GetCapabilities (ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static ARM_UART_CAPABILITIES UART5_GetCapabilities (void) {
    return UART_GetCapabilities (ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_CAPABILITIES UART6_GetCapabilities (void) {
    return UART_GetCapabilities (ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_CAPABILITIES UART7_GetCapabilities (void) {
    return UART_GetCapabilities (ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_CAPABILITIES UART8_GetCapabilities (void) {
    return UART_GetCapabilities (ptr_UART[7]);
  }
#endif

/**
  \fn          ARM_UART_STATUS UART_Initialize (ARM_UART_SignalEvent_t cb_event,
                                                uint32_t               event_mask
                                                UART_RESOURCES        *ptr_uart)
  \brief       Initialize UART Interface.
  \param[in]   cb_event    Pointer to \ref ARM_UART_SignalEvent
  \param[in]   event_mask
  \param[in]   ptr_uart    Pointer to UART resources
  \return      \ref ARM_UART_STATUS
*/
static ARM_UART_STATUS UART_Initialize (ARM_UART_SignalEvent_t cb_event,
                                        uint32_t               event_mask,
                                        UART_RESOURCES        *ptr_uart) {
  bool status;

  // Enable UART clock
  *ptr_uart->clock_reg |=  ptr_uart->clock_mask;

  // Reset UART peripheral
  *ptr_uart->reset_reg |=  ptr_uart->reset_mask;
  __NOP(); __NOP(); __NOP(); __NOP();
  *ptr_uart->reset_reg &= ~ptr_uart->reset_mask;

  // Clear and Enable UART IRQ
  NVIC_ClearPendingIRQ(ptr_uart->uart_irq);
  NVIC_EnableIRQ(ptr_uart->uart_irq);

  // Initialize DMA Streams
#if DMA_TX
  if (ptr_uart->dma_tx) {
    DMA_StreamInitialize(ptr_uart->dma_tx->dma, ptr_uart->dma_tx->stream);
  }
#endif

  // Initialize UART Run-Time Resources
  ptr_uart->info->cb_event        = cb_event;
  ptr_uart->info->event_mask      = event_mask;

  ptr_uart->info->tx.idx_in       = 0;
  ptr_uart->info->tx.idx_out      = 0;
  ptr_uart->info->tx.empty        = true;
  ptr_uart->info->tx.transfer_len = 0;
  ptr_uart->info->rx.idx_in       = 0;
  ptr_uart->info->rx.idx_out      = 0;
  ptr_uart->info->rx.empty        = true;
  ptr_uart->info->rx.transfer_len = 0;
  ptr_uart->info->com_break       = false;

  if (ptr_uart->info->semaphore   == NULL)
    ptr_uart->info->semaphore     = osSemaphoreCreate(ptr_uart->semaphore, 1);
  if (ptr_uart->info->mutex_write == NULL)
    ptr_uart->info->mutex_write   = osMutexCreate(ptr_uart->mutex_write);
  if (ptr_uart->info->mutex_read  == NULL)
    ptr_uart->info->mutex_read    = osMutexCreate(ptr_uart->mutex_read);

  if (ptr_uart->info->semaphore   == NULL) return ARM_UART_ERROR;
  if (ptr_uart->info->mutex_write == NULL) return ARM_UART_ERROR;
  if (ptr_uart->info->mutex_read  == NULL) return ARM_UART_ERROR;

  // Configure TX pin
  GPIO_PortClock                (ptr_uart->pin_tx.port, true);
  status = GPIO_PinAF           (ptr_uart->pin_tx.port,
                                 ptr_uart->pin_tx.num,
                                 ptr_uart->pin_af);
  if (!status) return ARM_UART_ERROR;
  status = GPIO_PinConfigure    (ptr_uart->pin_tx.port,
                                 ptr_uart->pin_tx.num,
                                 GPIO_MODE_AF,
                                 GPIO_OUTPUT_PUSH_PULL,
                                 GPIO_OUTPUT_SPEED_100MHz,
                                 GPIO_NO_PULL_UP_DOWN);
  if (!status) return ARM_UART_ERROR;

  // Configure RX pin
  GPIO_PortClock                (ptr_uart->pin_rx.port, true);
  status = GPIO_PinAF           (ptr_uart->pin_rx.port,
                                 ptr_uart->pin_rx.num,
                                 ptr_uart->pin_af);
  if (!status) return ARM_UART_ERROR;
  status = GPIO_PinConfigure    (ptr_uart->pin_rx.port,
                                 ptr_uart->pin_rx.num,
                                 GPIO_MODE_AF,
                                 GPIO_OUTPUT_PUSH_PULL,
                                 GPIO_OUTPUT_SPEED_100MHz,
                                 GPIO_NO_PULL_UP_DOWN);
  if (!status) return ARM_UART_ERROR;

  if (ptr_uart->hw_flow == true) {
    // Configure CTS pin
    GPIO_PortClock              (ptr_uart->pin_cts.port, true);
    // Hardware flow control
    if (ptr_uart->manual_rts_cts == false) {
      status = GPIO_PinAF       (ptr_uart->pin_cts.port,
                                 ptr_uart->pin_cts.num,
                                 ptr_uart->pin_af);
      if (!status) return ARM_UART_ERROR;
      status = GPIO_PinConfigure(ptr_uart->pin_cts.port,
                                 ptr_uart->pin_cts.num,
                                 GPIO_MODE_AF,
                                 GPIO_OUTPUT_PUSH_PULL,
                                 GPIO_OUTPUT_SPEED_100MHz,
                                 GPIO_NO_PULL_UP_DOWN);
    // GPIO Control of CTS pin
    } else {
      status = GPIO_PinAF       (ptr_uart->pin_cts.port,
                                 ptr_uart->pin_cts.num,
                                 GPIO_AF_SYSTEM);
      if (!status) return ARM_UART_ERROR;
      status = GPIO_PinConfigure(ptr_uart->pin_cts.port,
                                 ptr_uart->pin_cts.num,
                                 GPIO_MODE_INPUT,
                                 GPIO_OUTPUT_PUSH_PULL,
                                 GPIO_OUTPUT_SPEED_100MHz,
                                 GPIO_NO_PULL_UP_DOWN);
    }
    if (!status) return ARM_UART_ERROR;
  }

  if (ptr_uart->hw_flow == true) {
    // Configure RTS pin
    GPIO_PortClock              (ptr_uart->pin_rts.port, true);
    status = GPIO_PinAF         (ptr_uart->pin_rts.port,
                                 ptr_uart->pin_rts.num,
                                 GPIO_AF_SYSTEM);
    if (!status) return ARM_UART_ERROR;
    status = GPIO_PinConfigure  (ptr_uart->pin_rts.port,
                                 ptr_uart->pin_rts.num,
                                 GPIO_MODE_OUTPUT,
                                 GPIO_OUTPUT_PUSH_PULL,
                                 GPIO_OUTPUT_SPEED_100MHz,
                                 GPIO_NO_PULL_UP_DOWN);
    GPIO_PinWrite(ptr_uart->pin_rts.port, ptr_uart->pin_rts.num, 1);
    if (!status) return ARM_UART_ERROR;
  }

  return ARM_UART_OK;
}

#if (RTE_USART1 != 0)
  static ARM_UART_STATUS UART1_Initialize (ARM_UART_SignalEvent_t cb_event,
                                           uint32_t               event_mask) {
    return UART_Initialize (cb_event, event_mask, ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_STATUS UART2_Initialize (ARM_UART_SignalEvent_t cb_event,
                                           uint32_t               event_mask) {
    return UART_Initialize (cb_event, event_mask, ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_STATUS UART3_Initialize (ARM_UART_SignalEvent_t cb_event,
                                           uint32_t               event_mask) {
    return UART_Initialize (cb_event, event_mask, ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_STATUS UART4_Initialize (ARM_UART_SignalEvent_t cb_event,
                                           uint32_t               event_mask) {
    return UART_Initialize (cb_event, event_mask, ptr_UART[3]);
  }
#endif

  #if (RTE_UART5 != 0)
  static ARM_UART_STATUS UART5_Initialize (ARM_UART_SignalEvent_t cb_event,
                                           uint32_t               event_mask) {
    return UART_Initialize (cb_event, event_mask, ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_STATUS UART6_Initialize (ARM_UART_SignalEvent_t cb_event,
                                           uint32_t               event_mask) {
    return UART_Initialize (cb_event, event_mask, ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_STATUS UART7_Initialize (ARM_UART_SignalEvent_t cb_event,
                                           uint32_t               event_mask) {
    return UART_Initialize (cb_event, event_mask, ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_STATUS UART8_Initialize (ARM_UART_SignalEvent_t cb_event,
                                           uint32_t               event_mask) {
    return UART_Initialize (cb_event, event_mask, ptr_UART[7]);
  }
#endif

/**
  \fn          ARM_UART_STATUS UART_Uninitialize (UART_RESOURCES *ptr_uart)
  \brief       De-initialize UART Interface.
  \param[in]   ptr_uart  Pointer to UART resources
  \return      \ref ARM_UART_STATUS
*/
static ARM_UART_STATUS UART_Uninitialize (UART_RESOURCES *ptr_uart) {
  bool status;

  // Enable UART clock
  *ptr_uart->clock_reg |=  ptr_uart->clock_mask;

  // Reset UART peripheral
  *ptr_uart->reset_reg |=  ptr_uart->reset_mask;
  __NOP(); __NOP(); __NOP(); __NOP();
  *ptr_uart->reset_reg &= ~ptr_uart->reset_mask;

  // Disable UART clock
  *ptr_uart->clock_reg &= ~ptr_uart->clock_mask;

  // Clear and Disable UART IRQ
  NVIC_ClearPendingIRQ(ptr_uart->uart_irq);
  NVIC_DisableIRQ(ptr_uart->uart_irq);

  // Uninitialize SPI Run-Time Resources
  osSemaphoreDelete(ptr_uart->info->semaphore);
  osMutexDelete(ptr_uart->info->mutex_write);
  osMutexDelete(ptr_uart->info->mutex_read);

  // Uninitialize DMA Streams
  if (ptr_uart->dma_tx) {
    DMA_StreamUninitialize(ptr_uart->dma_tx->dma, ptr_uart->dma_tx->stream);
  }

  // Unconfigure TX pin
  status = GPIO_PinAF         (ptr_uart->pin_tx.port,
                               ptr_uart->pin_tx.num,
                               GPIO_AF_SYSTEM);
  if (!status) return ARM_UART_ERROR;
  status = GPIO_PinConfigure  (ptr_uart->pin_tx.port,
                               ptr_uart->pin_tx.num,
                               GPIO_MODE_INPUT,
                               GPIO_OUTPUT_PUSH_PULL,
                               GPIO_OUTPUT_SPEED_2MHz,
                               GPIO_NO_PULL_UP_DOWN);
  if (!status) return ARM_UART_ERROR;

  // Unconfigure RX pin
  status = GPIO_PinAF         (ptr_uart->pin_rx.port,
                               ptr_uart->pin_rx.num,
                               GPIO_AF_SYSTEM);
  if (!status) return ARM_UART_ERROR;
  status = GPIO_PinConfigure  (ptr_uart->pin_rx.port,
                               ptr_uart->pin_rx.num,
                               GPIO_MODE_INPUT,
                               GPIO_OUTPUT_PUSH_PULL,
                               GPIO_OUTPUT_SPEED_2MHz,
                               GPIO_NO_PULL_UP_DOWN);
  if (!status) return ARM_UART_ERROR;

  if (ptr_uart->hw_flow == true) {
    // Unconfigure CTS pin
    status = GPIO_PinAF       (ptr_uart->pin_cts.port,
                               ptr_uart->pin_cts.num,
                               GPIO_AF_SYSTEM);
    if (!status) return ARM_UART_ERROR;
    status = GPIO_PinConfigure(ptr_uart->pin_cts.port,
                               ptr_uart->pin_cts.num,
                               GPIO_MODE_INPUT,
                               GPIO_OUTPUT_PUSH_PULL,
                               GPIO_OUTPUT_SPEED_2MHz,
                               GPIO_NO_PULL_UP_DOWN);
    if (!status) return ARM_UART_ERROR;
  }

  if (ptr_uart->hw_flow == true) {
    // Unconfigure RTS pin
    status = GPIO_PinAF       (ptr_uart->pin_rts.port,
                               ptr_uart->pin_rts.num,
                               GPIO_AF_SYSTEM);
    if (!status) return ARM_UART_ERROR;
    status = GPIO_PinConfigure(ptr_uart->pin_rts.port,
                               ptr_uart->pin_rts.num,
                               GPIO_MODE_INPUT,
                               GPIO_OUTPUT_PUSH_PULL,
                               GPIO_OUTPUT_SPEED_2MHz,
                               GPIO_NO_PULL_UP_DOWN);
    if (!status) return ARM_UART_ERROR;
  }

  return ARM_UART_OK;
}

#if (RTE_USART1 != 0)
  static ARM_UART_STATUS UART1_Uninitialize (void) {
    return UART_Uninitialize (ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_STATUS UART2_Uninitialize (void) {
    return UART_Uninitialize (ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_STATUS UART3_Uninitialize (void) {
    return UART_Uninitialize (ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_STATUS UART4_Uninitialize (void) {
    return UART_Uninitialize (ptr_UART[3]);
  }
#endif

  #if (RTE_UART5 != 0)
  static ARM_UART_STATUS UART5_Uninitialize (void) {
    return UART_Uninitialize (ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_STATUS UART6_Uninitialize (void) {
    return UART_Uninitialize (ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_STATUS UART7_Uninitialize (void) {
    return UART_Uninitialize (ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_STATUS UART8_Uninitialize (void) {
    return UART_Uninitialize (ptr_UART[7]);
  }
#endif

/**
  \fn          ARM_UART_STATUS UART_PowerControl (ARM_POWER_STATE  state,
                                                  UART_RESOURCES  *ptr_uart)
  \brief       Controls UART Interface Power.
  \param[in]   state    Power state
  \param[in]   ptr_uart Pointer to UART resources
  \return      \ref UART_STATUS
*/
static ARM_UART_STATUS UART_PowerControl (ARM_POWER_STATE state, UART_RESOURCES *ptr_uart) {

  if (state == ARM_POWER_OFF) {
    // Enable UART Clock
    *ptr_uart->clock_reg |= ptr_uart->clock_mask;

    ptr_uart->uart->CR1 = 0;
    ptr_uart->uart->CR2 = 0;
    ptr_uart->uart->CR3 = 0;

    // Disable UART Clock
    *ptr_uart->clock_reg &= ~ptr_uart->clock_mask;

    // Disable DMA Streams
    if (ptr_uart->dma_tx) {
      DMA_StreamDisable(ptr_uart->dma_tx->ptr_stream);
    }

  } else if (state == ARM_POWER_FULL) {
    // Enable UART Clock
    *ptr_uart->clock_reg |= ptr_uart->clock_mask;
    // Clear status register
    ptr_uart->uart->SR = 0;

    ptr_uart->uart->CR2   |= USART_CR2_LINEN;

    if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_RX_PARITY_ERROR))
      ptr_uart->uart->CR1 |= USART_CR1_PEIE;

    if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_RX_BREAK))
      ptr_uart->uart->CR2 |= USART_CR2_LBDIE;

    if (ptr_uart->info->event_mask & ((1 << ARM_UART_EVENT_RX_FRAMING_ERROR) | (1 << ARM_UART_EVENT_RX_OVERRUN)))
      ptr_uart->uart->CR3 |= USART_CR3_EIE;

    // Enable UART
    ptr_uart->uart->CR1   |= USART_CR1_UE | // UART Enable
                             USART_CR1_RE | // Receiver Enable
                             USART_CR1_TE;  // Transmitter Enable

    // Enable interrupts
    ptr_uart->uart->CR1   |= USART_CR1_RXNEIE;

    if ((ptr_uart->hw_flow == true) && (ptr_uart->manual_rts_cts == false)) {
      GPIO_PinWrite(ptr_uart->pin_rts.port, ptr_uart->pin_rts.num, 0);
    }

  } else return ARM_UART_ERROR;

  return ARM_UART_OK;
}

#if (RTE_USART1 != 0)
  static ARM_UART_STATUS UART1_PowerControl (ARM_POWER_STATE state) {
    return UART_PowerControl (state, ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_STATUS UART2_PowerControl (ARM_POWER_STATE state) {
    return UART_PowerControl (state, ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_STATUS UART3_PowerControl (ARM_POWER_STATE state) {
    return UART_PowerControl (state, ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_STATUS UART4_PowerControl (ARM_POWER_STATE state) {
    return UART_PowerControl (state, ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static ARM_UART_STATUS UART5_PowerControl (ARM_POWER_STATE state) {
    return UART_PowerControl (state, ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_STATUS UART6_PowerControl (ARM_POWER_STATE state) {
    return UART_PowerControl (state, ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_STATUS UART7_PowerControl (ARM_POWER_STATE state) {
    return UART_PowerControl (state, ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_STATUS UART8_PowerControl (ARM_POWER_STATE state) {
    return UART_PowerControl (state, ptr_UART[7]);
  }
#endif

/**
  \fn          ARM_UART_STATUS UART_Configure (uint32_t               baudrate,
                                               uint8_t                data_bits,
                                               ARM_UART_PARITY        parity,
                                               ARM_UART_STOP_BITS     stop_bits,
                                               ARM_UART_FLOW_CONTROL  flow_control,
                                               UART_RESOURCES        *ptr_uart)
  \brief       Configure UART Interface.
  \param[in]   baudrate      Requested baudrate in bits/s
  \param[in]   data_bits     Number of data bits
  \param[in]   parity        \ref ARM_UART_PARITY
  \param[in]   stop_bits     \ref ARM_UART_STOP_BITS
  \param[in]   flow_control  \ref ARM_UART_FLOW_CONTROL
  \param[in]   ptr_uart Pointer to UART resources
  \return      \ref ARM_UART_STATUS
*/
static ARM_UART_STATUS UART_Configure (uint32_t               baudrate,
                                       uint8_t                data_bits,
                                       ARM_UART_PARITY        parity,
                                       ARM_UART_STOP_BITS     stop_bits,
                                       ARM_UART_FLOW_CONTROL  flow_control,
                                       UART_RESOURCES        *ptr_uart) {
  uint32_t br, cr1, cr2;
  uint32_t rts_pin = 1;

  if ((ptr_uart->hw_flow == true) && (ptr_uart->manual_rts_cts == false)) {
    rts_pin = GPIO_PinRead(ptr_uart->pin_rts.port, ptr_uart->pin_rts.num);
    if (rts_pin == 0)
      GPIO_PinWrite(ptr_uart->pin_rts.port, ptr_uart->pin_rts.num, 1);
  }

  // Disable UART transmitter and receiver
  cr1 = ptr_uart->uart->CR1 & (USART_CR1_RE | USART_CR1_TE);
  ptr_uart->uart->CR1 &= ~(USART_CR1_RE | USART_CR1_TE);

  // Baud rate
  ptr_uart->uart->BRR = __USART_BRR(ptr_uart->clock, baudrate);
  br = ((ptr_uart->clock << 4) / (ptr_uart->uart->BRR & 0xFFFF)) >> 4;
  // If inside +/- 2% tolerance, baud rate configured correctly
  if (!(((br * 100) < (baudrate * 102)) && ((br * 100) > (baudrate * 98))))
    return ARM_UART_ERROR_BAUDRATE;
  
  // Data Bits
  if (data_bits != 8) return ARM_UART_ERROR_DATA_BITS;

  // Parity
  switch (parity) {
    case ARM_UART_PARITY_NONE: cr1 |= 0;                                          break;
    case ARM_UART_PARITY_EVEN: cr1 |= USART_CR1_M | USART_CR1_PCE;                break;
    case ARM_UART_PARITY_ODD:  cr1 |= USART_CR1_M | USART_CR1_PCE | USART_CR1_PS; break;
    default: return ARM_UART_ERROR_PARITY;
  }

  // Stop Bits
  switch (stop_bits) {
    case ARM_UART_STOP_BITS_1:    cr2 = 0;                break;
    case ARM_UART_STOP_BITS_2:    cr2 = USART_CR2_STOP_1; break;
    case ARM_UART_STOP_BITS_1_5:  cr2 = USART_CR2_STOP;   break;
    default: return ARM_UART_ERROR_STOP_BITS;
  }

  // Flow Control
  ptr_uart->info->rts_cts_flow = false;
  switch (flow_control) {
    case ARM_UART_FLOW_CONTROL_NONE: 
      ptr_uart->uart->CR3 = 0; 
      break;
    case ARM_UART_FLOW_CONTROL_RTS_CTS:
      if ((ptr_uart->hw_flow == true) && (ptr_uart->manual_rts_cts == false)) {
        ptr_uart->info->rts_cts_flow = true;
        ptr_uart->uart->CR3 = USART_CR3_CTSE;
        if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_CTS))
          ptr_uart->uart->CR3 |= USART_CR3_CTSIE;
      } else {
        return ARM_UART_ERROR_FLOW_CONTROL;
      }
      break;
    default: return ARM_UART_ERROR_FLOW_CONTROL;
  }

  ptr_uart->uart->CR2 = cr2;
  osDelay(1);                   // Avoid unwanted output
  cr1 |= ptr_uart->uart->CR1 & (USART_CR1_UE | USART_CR1_RXNEIE);
  ptr_uart->uart->CR1 = cr1;

  if (rts_pin == 0)
   GPIO_PinWrite(ptr_uart->pin_rts.port, ptr_uart->pin_rts.num, 0);

  return ARM_UART_OK;
}

#if (RTE_USART1 != 0)
  static ARM_UART_STATUS UART1_Configure (uint32_t               baudrate,
                                          uint8_t                data_bits,
                                          ARM_UART_PARITY        parity,
                                          ARM_UART_STOP_BITS     stop_bits,
                                          ARM_UART_FLOW_CONTROL  flow_control) {
    return UART_Configure (baudrate, data_bits, parity,
                           stop_bits, flow_control, ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_STATUS UART2_Configure (uint32_t               baudrate,
                                          uint8_t                data_bits,
                                          ARM_UART_PARITY        parity,
                                          ARM_UART_STOP_BITS     stop_bits,
                                          ARM_UART_FLOW_CONTROL  flow_control) {
    return UART_Configure (baudrate, data_bits, parity,
                           stop_bits, flow_control, ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_STATUS UART3_Configure (uint32_t               baudrate,
                                          uint8_t                data_bits,
                                          ARM_UART_PARITY        parity,
                                          ARM_UART_STOP_BITS     stop_bits,
                                          ARM_UART_FLOW_CONTROL  flow_control) {
    return UART_Configure (baudrate, data_bits, parity,
                           stop_bits, flow_control, ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_STATUS UART4_Configure (uint32_t               baudrate,
                                          uint8_t                data_bits,
                                          ARM_UART_PARITY        parity,
                                          ARM_UART_STOP_BITS     stop_bits,
                                          ARM_UART_FLOW_CONTROL  flow_control) {
    return UART_Configure (baudrate, data_bits, parity,
                           stop_bits, flow_control, ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static ARM_UART_STATUS UART5_Configure (uint32_t               baudrate,
                                          uint8_t                data_bits,
                                          ARM_UART_PARITY        parity,
                                          ARM_UART_STOP_BITS     stop_bits,
                                          ARM_UART_FLOW_CONTROL  flow_control) {
    return UART_Configure (baudrate, data_bits, parity,
                           stop_bits, flow_control, ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_STATUS UART6_Configure (uint32_t               baudrate,
                                          uint8_t                data_bits,
                                          ARM_UART_PARITY        parity,
                                          ARM_UART_STOP_BITS     stop_bits,
                                          ARM_UART_FLOW_CONTROL  flow_control) {
    return UART_Configure (baudrate, data_bits, parity,
                           stop_bits, flow_control, ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_STATUS UART7_Configure (uint32_t               baudrate,
                                          uint8_t                data_bits,
                                          ARM_UART_PARITY        parity,
                                          ARM_UART_STOP_BITS     stop_bits,
                                          ARM_UART_FLOW_CONTROL  flow_control) {
    return UART_Configure (baudrate, data_bits, parity,
                           stop_bits, flow_control, ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_STATUS UART8_Configure (uint32_t               baudrate,
                                          uint8_t                data_bits,
                                          ARM_UART_PARITY        parity,
                                          ARM_UART_STOP_BITS     stop_bits,
                                          ARM_UART_FLOW_CONTROL  flow_control) {
    return UART_Configure (baudrate, data_bits, parity,
                           stop_bits, flow_control, ptr_UART[7]);
  }
#endif

/**
  \fn          int32_t UART_WriteData (const uint8_t   *data,
                                       uint32_t         size,
                                       UART_RESOURCES  *ptr_uart)
  \brief       Write data to UART transmitter.
  \param[in]   data  Pointer to buffer with data to write to UART transmitter
  \param[in]   size  Data buffer size in bytes
  \param[in]   ptr_uart Pointer to UART resources
  \return      number of data bytes written, error code if negative
*/
static int32_t UART_WriteData (const uint8_t   *data, 
                                     uint32_t   size,
                               UART_RESOURCES  *ptr_uart) {
  volatile uint32_t val, cnt;
#if DMA_TX
           uint32_t tick;
#endif

  osMutexWait(ptr_uart->info->mutex_write, osWaitForever);

  // Calculate available space in circular buffer
  val = (ptr_uart->info->tx.idx_in - ptr_uart->info->tx.idx_out) & (ptr_uart->tx_buffer_size - 1);

  // if size is 0 or buffer is full
  if ((size == 0) || ((val == 0) && (ptr_uart->info->tx.empty == false))) {
    osMutexRelease(ptr_uart->info->mutex_write);
    return 0;
  }

  val = ptr_uart->tx_buffer_size - val;

  if (size > val) size = val;

  val = ptr_uart->tx_buffer_size - ptr_uart->info->tx.idx_in;
  if (size > val) {
    memcpy(ptr_uart->tx_buffer + ptr_uart->info->tx.idx_in, data, val);
    memcpy(ptr_uart->tx_buffer, data + val, size - val);
    ptr_uart->info->tx.idx_in = size - val;
    ptr_uart->info->tx.empty = false;
  } else {
    memcpy(ptr_uart->tx_buffer + ptr_uart->info->tx.idx_in, data, size);
    val = (ptr_uart->info->tx.idx_in + size) & (ptr_uart->tx_buffer_size - 1);
    ptr_uart->info->tx.idx_in = val;
    ptr_uart->info->tx.empty = false;
  }

  osMutexRelease(ptr_uart->info->mutex_write);

  if (ptr_uart->dma_tx == NULL) {
    ptr_uart->uart->CR1 |= USART_CR1_TXEIE;
  }

#if DMA_TX
  else {
    if (osSemaphoreWait(ptr_uart->info->semaphore, 0) > 0) {
      val = (ptr_uart->info->tx.idx_in - ptr_uart->info->tx.idx_out) & (ptr_uart->tx_buffer_size - 1);
      if ((ptr_uart->info->tx.empty == false) || ((ptr_uart->info->tx.empty == true) && val != 0)) {
        if (val == 0) val = ptr_uart->tx_buffer_size;

        if (ptr_uart->info->tx.threshold != 0) {
          if(val == ptr_uart->info->tx.threshold) {
            if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_TX_THRESHOLD))
              ptr_uart->info->cb_event(ARM_UART_EVENT_TX_THRESHOLD);
          }
          if (val > ptr_uart->info->tx.threshold) val -= ptr_uart->info->tx.threshold;
        }

        cnt = ptr_uart->tx_buffer_size - ptr_uart->info->tx.idx_out;
        if (val > cnt) val = cnt;

        DMA_StreamDisable(ptr_uart->dma_tx->ptr_stream);
        tick = osKernelSysTick();
        do {
          if (DMA_StreamStatus(ptr_uart->dma_tx->ptr_stream) == false) break;
        } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(1000));

        if ((DMA_StreamStatus(ptr_uart->dma_tx->ptr_stream) == false)) {
          DMA_StreamConfigure(
            ptr_uart->dma_tx->ptr_stream,
            DMA_TRANSFER_COMPLETE_INTERRUPT |
            DMA_MEMORY_TO_PERIPHERAL        |
            DMA_MEMORY_INCREMENT            |
            DMA_PERIPHERAL_DATA_8BIT        |
            DMA_MEMORY_DATA_8BIT            |
            DMA_PERIPHERAL_SINGLE_TRANSFER  |
            DMA_MEMORY_SINGLE_TRANSFER      |
            ((ptr_uart->dma_tx->priority << DMA_PRIORITY_POS) & DMA_PRIORITY_MASK) |
            ((ptr_uart->dma_tx->channel  << DMA_CHANNEL_POS)  & DMA_CHANNEL_MASK),
            DMA_FIFO_DIRECT_MODE_ENABLE     |
            DMA_FIFO_ERROR_INTERRUPT,
            (uint32_t)(&ptr_uart->uart->DR),
            (uint32_t)ptr_uart->tx_buffer + ptr_uart->info->tx.idx_out,
            val
          );
          ptr_uart->info->tx.transfer_len = val;
          ptr_uart->uart->SR  &= ~USART_SR_TC;
          DMA_StreamEnable (ptr_uart->dma_tx->ptr_stream);
          ptr_uart->uart->CR3 |=  USART_CR3_DMAT;
        }
      }
    }
  }
#endif

  return size;
}

#if (RTE_USART1 != 0)
  static int32_t UART1_WriteData (const uint8_t *data, uint32_t size) {
    return UART_WriteData (data, size, ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static int32_t UART2_WriteData (const uint8_t *data, uint32_t size) {
    return UART_WriteData (data, size, ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static int32_t UART3_WriteData (const uint8_t *data, uint32_t size) {
    return UART_WriteData (data, size, ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static int32_t UART4_WriteData (const uint8_t *data, uint32_t size) {
    return UART_WriteData (data, size, ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static int32_t UART5_WriteData (const uint8_t *data, uint32_t size) {
    return UART_WriteData (data, size, ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static int32_t UART6_WriteData (const uint8_t *data, uint32_t size) {
    return UART_WriteData (data, size, ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static int32_t UART7_WriteData (const uint8_t *data, uint32_t size) {
    return UART_WriteData (data, size, ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static int32_t UART8_WriteData (const uint8_t *data, uint32_t size) {
    return UART_WriteData (data, size, ptr_UART[7]);
  }
#endif

/**
  \fn          int32_t UART_ReadData (uint8_t *data,
                                      uint32_t         size,
                                      UART_RESOURCES  *ptr_uart)
  \brief       Read data from UART receiver.
  \param[out]  data  Pointer to buffer for data read from UART receiver
  \param[in]   size  Data buffer size in bytes
  \param[in]   ptr_uart Pointer to UART resources
  \return      number of data bytes read, error code if negative
*/
static int32_t UART_ReadData (uint8_t         *data,
                              uint32_t         size,
                              UART_RESOURCES  *ptr_uart) {
  volatile uint32_t val;

  osMutexWait(ptr_uart->info->mutex_read, osWaitForever);

  if ((size == 0) || (ptr_uart->info->rx.empty == true)) {
    osMutexRelease(ptr_uart->info->mutex_read);
    return 0;
  }

  val = (ptr_uart->info->rx.idx_in - ptr_uart->info->rx.idx_out) & (ptr_uart->rx_buffer_size - 1);
  if (val == 0) val = ptr_uart->rx_buffer_size;

  if (size > val) size = val;

  if ((ptr_uart->hw_flow == true) && (ptr_uart->manual_rts_cts == false)) {
    if (ptr_uart->flow_ctrl_threshold < val)
      GPIO_PinWrite(ptr_uart->pin_rts.port, ptr_uart->pin_rts.num, 0);
  }

  if (size > (ptr_uart->rx_buffer_size - ptr_uart->info->rx.idx_out)) {
    memcpy(data, ptr_uart->rx_buffer + ptr_uart->info->rx.idx_out, val);
    memcpy(data + val, ptr_uart->rx_buffer, size - val);

    if (ptr_uart->info->rx.idx_in == (size - val))
      ptr_uart->info->rx.empty = true;

    ptr_uart->info->rx.idx_out = size - val;
  } else {
    memcpy(data, ptr_uart->rx_buffer + ptr_uart->info->rx.idx_out, size);
    val = (ptr_uart->info->rx.idx_out + size) & (ptr_uart->rx_buffer_size - 1);
    if (ptr_uart->info->rx.idx_in == val)
      ptr_uart->info->rx.empty = true;

    ptr_uart->info->rx.idx_out = val;
  }

  osMutexRelease(ptr_uart->info->mutex_read);

  ptr_uart->uart->CR1 |= USART_CR1_RXNEIE;
  if ((ptr_uart->hw_flow == true) && (ptr_uart->manual_rts_cts == false)) {
    if (ptr_uart->flow_ctrl_threshold < val)
      GPIO_PinWrite(ptr_uart->pin_rts.port, ptr_uart->pin_rts.num, 1);
  }

  return size;
}

#if (RTE_USART1 != 0)
  static int32_t UART1_ReadData (uint8_t *data, uint32_t size) {
    return UART_ReadData (data, size, ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static int32_t UART2_ReadData (uint8_t *data, uint32_t size) {
    return UART_ReadData (data, size, ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static int32_t UART3_ReadData (uint8_t *data, uint32_t size) {
    return UART_ReadData (data, size, ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static int32_t UART4_ReadData (uint8_t *data, uint32_t size) {
    return UART_ReadData (data, size, ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static int32_t UART5_ReadData (uint8_t *data, uint32_t size) {
    return UART_ReadData (data, size, ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static int32_t UART6_ReadData (uint8_t *data, uint32_t size) {
    return UART_ReadData (data, size, ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static int32_t UART7_ReadData (uint8_t *data, uint32_t size) {
    return UART_ReadData (data, size, ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static int32_t UART8_ReadData (uint8_t *data, uint32_t size) {
    return UART_ReadData (data, size, ptr_UART[7]);
  }
#endif

/**
  \fn          int32_t UART_DataAvailable (UART_RESOURCES  *ptr_uart)
  \brief       Check available data in UART receiver.
  \param[in]   ptr_uart Pointer to UART resources
  \return      number of data bytes in receive buffer, error code if negative
*/
static int32_t UART_DataAvailable (UART_RESOURCES  *ptr_uart) {
  uint32_t val;

  val = (ptr_uart->info->rx.idx_in - ptr_uart->info->rx.idx_out) & (ptr_uart->rx_buffer_size - 1);
  if ((val == 0) && (ptr_uart->info->rx.empty == false)) val = ptr_uart->rx_buffer_size;

  return (val);
}

#if (RTE_USART1 != 0)
  static int32_t UART1_DataAvailable (void) {
    return UART_DataAvailable (ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static int32_t UART2_DataAvailable (void) {
    return UART_DataAvailable (ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static int32_t UART3_DataAvailable (void) {
    return UART_DataAvailable (ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static int32_t UART4_DataAvailable (void) {
    return UART_DataAvailable (ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static int32_t UART5_DataAvailable (void) {
    return UART_DataAvailable (ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static int32_t UART6_DataAvailable (void) {
    return UART_DataAvailable (ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static int32_t UART7_DataAvailable (void) {
    return UART_DataAvailable (ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static int32_t UART8_DataAvailable (void) {
    return UART_DataAvailable (ptr_UART[7]);
  }
#endif

/**
  \fn          bool UART_TxDone (UART_RESOURCES  *ptr_uart)
  \brief       Check if UART transmission is completed.
  \param[in]   ptr_uart Pointer to UART resources
  \return      transmitter completion status
                 - \b true transmission done
                 - \b false transmission in progress
*/
static bool UART_TxDone (UART_RESOURCES  *ptr_uart) {
  uint32_t val;

  val = (ptr_uart->info->tx.idx_in - ptr_uart->info->tx.idx_out) & (ptr_uart->tx_buffer_size - 1);
  if ((val == 0) && (ptr_uart->uart->SR & USART_SR_TC)) return true;
  else return false;
}

#if (RTE_USART1 != 0)
  static bool UART1_TxDone (void) {
    return UART_TxDone (ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static bool UART2_TxDone (void) {
    return UART_TxDone (ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static bool UART3_TxDone (void) {
    return UART_TxDone (ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static bool UART4_TxDone (void) {
    return UART_TxDone (ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static bool UART5_TxDone (void) {
    return UART_TxDone (ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static bool UART6_TxDone (void) {
    return UART_TxDone (ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static bool UART7_TxDone (void) {
    return UART_TxDone (ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static bool UART8_TxDone (void) {
    return UART_TxDone (ptr_UART[7]);
  }
#endif

/**
  \fn          ARM_UART_STATUS UART_SetTxThreshold (uint32_t         level,
                                                    UART_RESOURCES  *ptr_uart) {
  \brief       Set transmit threshold for UART_TX_THRESHOLD event.
  \param[in]   level  Number of character in transmit buffer
  \param[in]   ptr_uart Pointer to UART resources
  \return      \ref ARM_UART_STATUS
*/
static ARM_UART_STATUS UART_SetTxThreshold (uint32_t         level,
                                            UART_RESOURCES  *ptr_uart) {

  if (level > ptr_uart->tx_buffer_size) return ARM_UART_ERROR;
  ptr_uart->info->tx.threshold = level;

  return ARM_UART_OK;
}

#if (RTE_USART1 != 0)
  static ARM_UART_STATUS UART1_SetTxThreshold (uint32_t level) {
    return UART_SetTxThreshold (level, ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_STATUS UART2_SetTxThreshold (uint32_t level) {
    return UART_SetTxThreshold (level, ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_STATUS UART3_SetTxThreshold (uint32_t level) {
    return UART_SetTxThreshold (level, ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_STATUS UART4_SetTxThreshold (uint32_t level) {
    return UART_SetTxThreshold (level, ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static ARM_UART_STATUS UART5_SetTxThreshold (uint32_t level) {
    return UART_SetTxThreshold (level, ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_STATUS UART6_SetTxThreshold (uint32_t level) {
    return UART_SetTxThreshold (level, ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_STATUS UART7_SetTxThreshold (uint32_t level) {
    return UART_SetTxThreshold (level, ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_STATUS UART8_SetTxThreshold (uint32_t level) {
    return UART_SetTxThreshold (level, ptr_UART[7]);
  }
#endif

/**
  \fn          ARM_UART_STATUS UART_SetRxThreshold (uint32_t         level,
                                                    UART_RESOURCES  *ptr_uart) {
  \brief       Set receive threshold for UART_RX_THRESHOLD event.
  \param[in]   level  Number of character in transmit buffer
  \param[in]   ptr_uart Pointer to UART resources
  \return      \ref ARM_UART_STATUS
*/
static ARM_UART_STATUS UART_SetRxThreshold (uint32_t         level,
                                            UART_RESOURCES  *ptr_uart) {

  if (level > ptr_uart->rx_buffer_size) return ARM_UART_ERROR;
  ptr_uart->info->rx.threshold = level;
  
  return ARM_UART_OK;
}

#if (RTE_USART1 != 0)
  static ARM_UART_STATUS UART1_SetRxThreshold (uint32_t level) {
    return UART_SetRxThreshold (level, ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_STATUS UART2_SetRxThreshold (uint32_t level) {
    return UART_SetRxThreshold (level, ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_STATUS UART3_SetRxThreshold (uint32_t level) {
    return UART_SetRxThreshold (level, ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_STATUS UART4_SetRxThreshold (uint32_t level) {
    return UART_SetRxThreshold (level, ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static ARM_UART_STATUS UART5_SetRxThreshold (uint32_t level) {
    return UART_SetRxThreshold (level, ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_STATUS UART6_SetRxThreshold (uint32_t level) {
    return UART_SetRxThreshold (level, ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_STATUS UART7_SetRxThreshold (uint32_t level) {
    return UART_SetRxThreshold (level, ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_STATUS UART8_SetRxThreshold (uint32_t level) {
    return UART_SetRxThreshold (level, ptr_UART[7]);
  }
#endif


/**
  \fn          ARM_UART_STATUS UART_FlushTxBuffer (UART_RESOURCES  *ptr_uart)
  \brief       Flush UART transmit buffer.
  \param[in]   ptr_uart Pointer to UART resources
  \return      \ref UART_STATUS
*/
static ARM_UART_STATUS UART_FlushTxBuffer (UART_RESOURCES  *ptr_uart) {
  ptr_uart->uart->CR1 &= ~USART_CR1_TXEIE;

  osMutexWait(ptr_uart->info->mutex_write, osWaitForever);

  ptr_uart->info->tx.idx_in  = 0;
  ptr_uart->info->tx.idx_out = 0;
  ptr_uart->info->tx.empty   = true;

  osMutexRelease(ptr_uart->info->mutex_write);
  
  return ARM_UART_OK;
}

#if (RTE_USART1 != 0)
  static ARM_UART_STATUS UART1_FlushTxBuffer (void) {
    return UART_FlushTxBuffer (ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_STATUS UART2_FlushTxBuffer (void) {
    return UART_FlushTxBuffer (ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_STATUS UART3_FlushTxBuffer (void) {
    return UART_FlushTxBuffer (ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_STATUS UART4_FlushTxBuffer (void) {
    return UART_FlushTxBuffer (ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static ARM_UART_STATUS UART5_FlushTxBuffer (void) {
    return UART_FlushTxBuffer (ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_STATUS UART6_FlushTxBuffer (void) {
    return UART_FlushTxBuffer (ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_STATUS UART7_FlushTxBuffer (void) {
    return UART_FlushTxBuffer (ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_STATUS UART8_FlushTxBuffer (void) {
    return UART_FlushTxBuffer (ptr_UART[7]);
  }
#endif


/**
  \fn          ARM_UART_STATUS UART_FlushRxBuffer (UART_RESOURCES  *ptr_uart)
  \brief       Flush UART receive buffer.
  \param[in]   ptr_uart Pointer to UART resources
  \return      \ref UART_STATUS
*/
static ARM_UART_STATUS UART_FlushRxBuffer (UART_RESOURCES  *ptr_uart) {
  uint32_t rts_pin = 1;

  ptr_uart->uart->CR1 &= ~USART_CR1_TXEIE;
  if ((ptr_uart->hw_flow == true) && (ptr_uart->manual_rts_cts == false)) {
    rts_pin = GPIO_PinRead(ptr_uart->pin_rts.port, ptr_uart->pin_rts.num);
    if (rts_pin == 0)
      GPIO_PinWrite(ptr_uart->pin_rts.port, ptr_uart->pin_rts.num, 1);
  }

  if (ptr_uart->dma_tx != NULL) {
    DMA_StreamDisable(ptr_uart->dma_tx->ptr_stream);
  }

  osMutexWait(ptr_uart->info->mutex_read, osWaitForever);
  
  ptr_uart->info->rx.idx_in  = 0;
  ptr_uart->info->rx.idx_out = 0;
  ptr_uart->info->rx.empty   = true;

  osMutexRelease(ptr_uart->info->mutex_read);

  if (rts_pin == 0)
    GPIO_PinWrite(ptr_uart->pin_rts.port, ptr_uart->pin_rts.num, 1);

  return ARM_UART_OK;
}

#if (RTE_USART1 != 0)
  static ARM_UART_STATUS UART1_FlushRxBuffer (void) {
    return UART_FlushRxBuffer (ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_STATUS UART2_FlushRxBuffer (void) {
    return UART_FlushRxBuffer (ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_STATUS UART3_FlushRxBuffer (void) {
    return UART_FlushRxBuffer (ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_STATUS UART4_FlushRxBuffer (void) {
    return UART_FlushRxBuffer (ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static ARM_UART_STATUS UART5_FlushRxBuffer (void) {
    return UART_FlushRxBuffer (ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_STATUS UART6_FlushRxBuffer (void) {
    return UART_FlushRxBuffer (ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_STATUS UART7_FlushRxBuffer (void) {
    return UART_FlushRxBuffer (ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_STATUS UART8_FlushRxBuffer (void) {
    return UART_FlushRxBuffer (ptr_UART[7]);
  }
#endif

/**
  \fn          ARM_UART_COM_ERROR UART_GetComError (UART_RESOURCES  *ptr_uart)
  \brief       Get UART Communication errors.
  \param[in]   ptr_uart Pointer to UART resources
  \return      \ref ARM_UART_COM_ERROR
*/
static ARM_UART_COM_ERROR UART_GetComError (UART_RESOURCES  *ptr_uart) {
  ARM_UART_COM_ERROR com_error = {0, 0, 0, 0};

  if (ptr_uart->uart->SR & USART_SR_ORE) com_error.rx_overrun       = 1;
  if (ptr_uart->uart->SR & USART_SR_LBD) com_error.rx_break         = 1;
  if (ptr_uart->uart->SR & USART_SR_PE)  com_error.rx_parity_error  = 1;
  if (ptr_uart->uart->SR & USART_SR_FE)  com_error.rx_framing_error = 1;

  return com_error;
}

#if (RTE_USART1 != 0)
  static ARM_UART_COM_ERROR UART1_GetComError (void) {
    return UART_GetComError (ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_COM_ERROR UART2_GetComError (void) {
    return UART_GetComError (ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_COM_ERROR UART3_GetComError (void) {
    return UART_GetComError (ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_COM_ERROR UART4_GetComError (void) {
    return UART_GetComError (ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static ARM_UART_COM_ERROR UART5_GetComError (void) {
    return UART_GetComError (ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_COM_ERROR UART6_GetComError (void) {
    return UART_GetComError (ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_COM_ERROR UART7_GetComError (void) {
    return UART_GetComError (ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_COM_ERROR UART8_GetComError (void) {
    return UART_GetComError (ptr_UART[7]);
  }
#endif

/**
  \fn          ARM_UART_STATUS UART_SetComBreak (UART_RESOURCES  *ptr_uart)
  \brief       Suspend transmission and put UART Transmission line in a break state.
  \param[in]   ptr_uart Pointer to UART resources
  \return      \ref ARM_UART_STATUS
*/
static ARM_UART_STATUS UART_SetComBreak (UART_RESOURCES  *ptr_uart) {
  ptr_uart->info->com_break = true;
  ptr_uart->uart->CR1 |= USART_CR1_TXEIE;
  return ARM_UART_OK;
}

#if (RTE_USART1 != 0)
  static ARM_UART_STATUS UART1_SetComBreak (void) {
    return UART_SetComBreak (ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_STATUS UART2_SetComBreak (void) {
    return UART_SetComBreak (ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_STATUS UART3_SetComBreak (void) {
    return UART_SetComBreak (ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_STATUS UART4_SetComBreak (void) {
    return UART_SetComBreak (ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static ARM_UART_STATUS UART5_SetComBreak (void) {
    return UART_SetComBreak (ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_STATUS UART6_SetComBreak (void) {
    return UART_SetComBreak (ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_STATUS UART7_SetComBreak (void) {
    return UART_SetComBreak (ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_STATUS UART8_SetComBreak (void) {
    return UART_SetComBreak (ptr_UART[7]);
  }
#endif


/**
  \fn          ARM_UART_STATUS UART_ClearComBreak (UART_RESOURCES  *ptr_uart)
  \brief       Suspend transmission and put UART Transmission line in a break state.
  \param[in]   ptr_uart Pointer to UART resources
  \return      \ref ARM_UART_STATUS
*/
static ARM_UART_STATUS UART_ClearComBreak (UART_RESOURCES  *ptr_uart) {
  ptr_uart->info->com_break = false;
  ptr_uart->uart->CR1 |= USART_CR1_TXEIE;
  return ARM_UART_OK;
}

#if (RTE_USART1 != 0)
  static ARM_UART_STATUS UART1_ClearComBreak (void) {
    return UART_ClearComBreak (ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_STATUS UART2_ClearComBreak (void) {
    return UART_ClearComBreak (ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_STATUS UART3_ClearComBreak (void) {
    return UART_ClearComBreak (ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_STATUS UART4_ClearComBreak (void) {
    return UART_ClearComBreak (ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static ARM_UART_STATUS UART5_ClearComBreak (void) {
    return UART_ClearComBreak (ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_STATUS UART6_ClearComBreak (void) {
    return UART_ClearComBreak (ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_STATUS UART7_ClearComBreak (void) {
    return UART_ClearComBreak (ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_STATUS UART8_ClearComBreak (void) {
    return UART_ClearComBreak (ptr_UART[7]);
  }
#endif

/**
  \fn          ARM_UART_STATUS UART_SetModemControl (ARM_UART_MODEM_CONTROL control,
                                                     UART_RESOURCES        *ptr_uart)
  \brief       Set UART Modem Control line state.
  \param[in]   control  \ref ARM_UART_MODEM_CONTROL
  \param[in]   ptr_uart Pointer to UART resources
  \return      \ref ARM_UART_STATUS
*/
static ARM_UART_STATUS UART_SetModemControl (ARM_UART_MODEM_CONTROL control,
                                             UART_RESOURCES        *ptr_uart) {

  if (ptr_uart->manual_rts_cts == false) return ARM_UART_ERROR;
  if (ptr_uart->hw_flow == false)        return ARM_UART_ERROR;

  switch (control) {
    case ARM_UART_RTS_CLEAR:
      GPIO_PinWrite(ptr_uart->pin_rts.port, ptr_uart->pin_rts.num, 1);
      break;
    case ARM_UART_RTS_SET:
      GPIO_PinWrite(ptr_uart->pin_rts.port, ptr_uart->pin_rts.num, 0);
      break;
    default: return ARM_UART_ERROR;
  }
  return ARM_UART_OK;
}

#if (RTE_USART1 != 0)
  static ARM_UART_STATUS UART1_SetModemControl (ARM_UART_MODEM_CONTROL control) {
    return UART_SetModemControl (control, ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_STATUS UART2_SetModemControl (ARM_UART_MODEM_CONTROL control) {
    return UART_SetModemControl (control, ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_STATUS UART3_SetModemControl (ARM_UART_MODEM_CONTROL control) {
    return UART_SetModemControl (control, ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_STATUS UART4_SetModemControl (ARM_UART_MODEM_CONTROL control) {
    return UART_SetModemControl (control, ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static ARM_UART_STATUS UART5_SetModemControl (ARM_UART_MODEM_CONTROL control) {
    return UART_SetModemControl (control, ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_STATUS UART6_SetModemControl (ARM_UART_MODEM_CONTROL control) {
    return UART_SetModemControl (control, ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_STATUS UART7_SetModemControl (ARM_UART_MODEM_CONTROL control) {
    return UART_SetModemControl (control, ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_STATUS UART8_SetModemControl (ARM_UART_MODEM_CONTROL control) {
    return UART_SetModemControl (control, ptr_UART[7]);
  }
#endif

/**
  \fn          ARM_UART_MODEM_STATUS UART_GetModemStatus (UART_RESOURCES *ptr_uart)
  \brief       Get UART Modem Status lines state.
  \param[in]   ptr_uart Pointer to UART resources
  \return      \ref ARM_UART_MODEM_STATUS
*/
static ARM_UART_MODEM_STATUS UART_GetModemStatus (UART_RESOURCES *ptr_uart) {
  ARM_UART_MODEM_STATUS mst = { 0, 0, 0, 0,};
  return mst;
}

#if (RTE_USART1 != 0)
  static ARM_UART_MODEM_STATUS UART1_GetModemStatus (void) {
    return UART_GetModemStatus (ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  static ARM_UART_MODEM_STATUS UART2_GetModemStatus (void) {
    return UART_GetModemStatus (ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  static ARM_UART_MODEM_STATUS UART3_GetModemStatus (void) {
    return UART_GetModemStatus (ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  static ARM_UART_MODEM_STATUS UART4_GetModemStatus (void) {
    return UART_GetModemStatus (ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  static ARM_UART_MODEM_STATUS UART5_GetModemStatus (void) {
    return UART_GetModemStatus (ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  static ARM_UART_MODEM_STATUS UART6_GetModemStatus (void) {
    return UART_GetModemStatus (ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  static ARM_UART_MODEM_STATUS UART7_GetModemStatus (void) {
    return UART_GetModemStatus (ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  static ARM_UART_MODEM_STATUS UART8_GetModemStatus (void) {
    return UART_GetModemStatus (ptr_UART[7]);
  }
#endif

/**
  \fn          void UART_IRQHandler (UART_RESOURCES *ptr_uart)
  \brief       UART Interrupt handler.
  \param[in]   ptr_uart  Pointer to UART resources
*/
static void UART_IRQHandler (UART_RESOURCES *ptr_uart) {
           uint32_t sr;
  volatile uint32_t val;
  
  sr = ptr_uart->uart->SR;

  // Parity Error
  if ((sr & USART_SR_PE) && (ptr_uart->uart->CR1 & USART_CR1_PEIE)) {
    if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_RX_PARITY_ERROR))
      ptr_uart->info->cb_event(ARM_UART_EVENT_RX_PARITY_ERROR);
  }

  // Framing Error
  if (sr & USART_SR_FE) {
    if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_RX_FRAMING_ERROR))
      ptr_uart->info->cb_event(ARM_UART_EVENT_RX_FRAMING_ERROR);
  }

  // Noise detect flag
  if (sr & USART_SR_NE) {
  }

  // Overrun error
  if (sr & USART_SR_ORE) {
    if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_RX_OVERRUN))
      ptr_uart->info->cb_event(ARM_UART_EVENT_RX_OVERRUN);
  }

  // Idle line detect
  if (sr & USART_SR_IDLE) {
  }

  // Read data register not empty
  if (sr & USART_SR_RXNE) {
    if (ptr_uart->uart->CR1 & USART_CR1_RXNEIE) {
      if ((ptr_uart->info->rx.idx_in != ptr_uart->info->rx.idx_out) ||
          ((ptr_uart->info->rx.idx_in == ptr_uart->info->rx.idx_out) && (ptr_uart->info->rx.empty == true))) {
        ptr_uart->rx_buffer[ptr_uart->info->rx.idx_in] = ptr_uart->uart->DR;
        ptr_uart->info->rx.empty = false;
        ptr_uart->info->rx.idx_in = (ptr_uart->info->rx.idx_in + 1) & (ptr_uart->rx_buffer_size - 1);

        val = (ptr_uart->info->rx.idx_in - ptr_uart->info->rx.idx_out) & (ptr_uart->rx_buffer_size - 1);
        val = (ptr_uart->rx_buffer_size - 1) - val;

        if ((ptr_uart->hw_flow == true) && (ptr_uart->manual_rts_cts == false)) {
          if (ptr_uart->flow_ctrl_threshold == val)
            GPIO_PinWrite(ptr_uart->pin_rts.port, ptr_uart->pin_rts.num, 1);
        }

        if (val == ptr_uart->info->rx.threshold) {
          if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_RX_THRESHOLD))
            ptr_uart->info->cb_event(ARM_UART_EVENT_RX_THRESHOLD);
        }
      } else {
        ptr_uart->uart->CR1 &= ~USART_CR1_RXNEIE;
      }
    }
  }

  // Transmission complete
  if (sr & USART_SR_TC) {
    if (ptr_uart->info->tx.empty == true) {
      if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_TX_DONE))
        ptr_uart->info->cb_event(ARM_UART_EVENT_TX_DONE);
    }
  }

  // Transmit data register empty
  if (sr & USART_SR_TXE) {
    if (ptr_uart->uart->CR1 & USART_CR1_TXEIE) {
      if ((ptr_uart->info->tx.empty == true) && (ptr_uart->info->com_break == true)) {
        ptr_uart->uart->CR1 |= USART_CR1_SBK;           // Send break
      } else {
        val = (ptr_uart->info->tx.idx_in - ptr_uart->info->tx.idx_out) & (ptr_uart->tx_buffer_size - 1);
        if ((val != 0) || ((val == 0) && (ptr_uart->info->tx.empty == false))) {
          if (ptr_uart->info->tx.threshold == val) {
            if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_TX_THRESHOLD))
              ptr_uart->info->cb_event(ARM_UART_EVENT_TX_THRESHOLD);
          }
          ptr_uart->uart->DR = ptr_uart->tx_buffer[ptr_uart->info->tx.idx_out];
          ptr_uart->info->tx.idx_out = (ptr_uart->info->tx.idx_out + 1) & (ptr_uart->tx_buffer_size - 1);
          val = (ptr_uart->info->tx.idx_in - ptr_uart->info->tx.idx_out) & (ptr_uart->tx_buffer_size - 1);
          if ((val == 0) && (ptr_uart->info->tx.empty == false)) {
            ptr_uart->info->tx.empty = true;
            if (ptr_uart->info->com_break == true)
              ptr_uart->uart->CR1 |= USART_CR1_SBK;     // Send break
            else
              ptr_uart->uart->CR1 &= ~USART_CR1_TXEIE;  // Disable Transmit data register empty interrupt
            if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_TX_EMPTY))
              ptr_uart->info->cb_event(ARM_UART_EVENT_TX_EMPTY);
          }
        } else {
          ptr_uart->uart->CR1 &= ~USART_CR1_TXEIE;
        }
      }
    }
  }

  // LIN break detection
  if ((sr & USART_SR_LBD) && (ptr_uart->uart->CR2 & USART_CR2_LBDIE)) {
    if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_RX_BREAK))
      ptr_uart->info->cb_event(ARM_UART_EVENT_RX_BREAK);
    ptr_uart->uart->SR &= ~USART_SR_LBD;
  }

  // CTS input change
  if ((sr & USART_SR_CTS) && (ptr_uart->uart->CR3 & USART_CR3_CTSIE)) {
  
  }
}

#if (RTE_USART1 != 0)
  void USART1_IRQHandler (void) {
    UART_IRQHandler (ptr_UART[0]);
  }
#endif

#if (RTE_USART2 != 0)
  void USART2_IRQHandler (void) {
    UART_IRQHandler (ptr_UART[1]);
  }
#endif

#if (RTE_USART3 != 0)
  void USART3_IRQHandler (void) {
    UART_IRQHandler (ptr_UART[2]);
  }
#endif

#if (RTE_UART4 != 0)
  void UART4_IRQHandler (void) {
    UART_IRQHandler (ptr_UART[3]);
  }
#endif

#if (RTE_UART5 != 0)
  void UART5_IRQHandler (void) {
    UART_IRQHandler (ptr_UART[4]);
  }
#endif

#if (RTE_USART6 != 0)
  void USART6_IRQHandler (void) {
    UART_IRQHandler (ptr_UART[5]);
  }
#endif

#if (RTE_UART7 != 0)
  void UART7_IRQHandler (void) {
    UART_IRQHandler (ptr_UART[6]);
  }
#endif

#if (RTE_UART8 != 0)
  void UART8_IRQHandler (void) {
    UART_IRQHandler (ptr_UART[7]);
  }
#endif

#if DMA_TX
/**
  \fn          void UART_DMA_TxStreamEvent (uint32_t event, UART_RESOURCES *ptr_uart)
  \brief       UART transmit DMA Stream Event.
  \param[in]   event     DMA Stream event
  \param[in]   ptr_uart  Pointer to UART resources
*/
static void UART_DMA_TxStreamEvent (uint32_t event, UART_RESOURCES *ptr_uart) {
  volatile uint32_t cnt;
  volatile uint32_t val;
           uint32_t tick;

  if (event & DMA_STREAM_TRANSFER_COMPLETE) {
    val = (ptr_uart->info->tx.idx_out + ptr_uart->info->tx.transfer_len) & (ptr_uart->tx_buffer_size - 1);
    if (val == ptr_uart->info->tx.idx_in) {
      ptr_uart->info->tx.empty   = true;
      ptr_uart->info->tx.idx_out = val;
      if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_TX_EMPTY))
        ptr_uart->info->cb_event(ARM_UART_EVENT_TX_EMPTY);
      osSemaphoreRelease(ptr_uart->info->semaphore);
    } else {
      ptr_uart->info->tx.idx_out = val;
      val = (ptr_uart->info->tx.idx_in - ptr_uart->info->tx.idx_out) & (ptr_uart->tx_buffer_size - 1);

      if (ptr_uart->info->tx.threshold != 0) {
        if(val == ptr_uart->info->tx.threshold) {
          if (ptr_uart->info->event_mask & (1 << ARM_UART_EVENT_TX_THRESHOLD))
            ptr_uart->info->cb_event(ARM_UART_EVENT_TX_THRESHOLD);
        }
        if (val > ptr_uart->info->tx.threshold) val -= ptr_uart->info->tx.threshold;
      }

      cnt = ptr_uart->tx_buffer_size - ptr_uart->info->tx.idx_out;
      if (val > cnt) val = cnt;

      DMA_StreamDisable(ptr_uart->dma_tx->ptr_stream);
      tick = osKernelSysTick();
      do {
        if (DMA_StreamStatus(ptr_uart->dma_tx->ptr_stream) == false) break;
      } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(1000));

      if (DMA_StreamStatus(ptr_uart->dma_tx->ptr_stream) == false) {
        DMA_StreamConfigure(
          ptr_uart->dma_tx->ptr_stream,
          DMA_TRANSFER_COMPLETE_INTERRUPT |
          DMA_MEMORY_TO_PERIPHERAL        |
          DMA_MEMORY_INCREMENT            |
          DMA_PERIPHERAL_DATA_8BIT        |
          DMA_MEMORY_DATA_8BIT            |
          DMA_PERIPHERAL_SINGLE_TRANSFER  |
          DMA_MEMORY_SINGLE_TRANSFER      |
          ((ptr_uart->dma_tx->priority << DMA_PRIORITY_POS) & DMA_PRIORITY_MASK) |
          ((ptr_uart->dma_tx->channel  << DMA_CHANNEL_POS)  & DMA_CHANNEL_MASK),
          DMA_FIFO_DIRECT_MODE_ENABLE     |
          DMA_FIFO_ERROR_INTERRUPT,
          (uint32_t)(&ptr_uart->uart->DR),
          (uint32_t)ptr_uart->tx_buffer + ptr_uart->info->tx.idx_out,
          val
        );
        ptr_uart->info->tx.transfer_len = val;
        ptr_uart->uart->SR  &= ~USART_SR_TC;
        DMA_StreamEnable (ptr_uart->dma_tx->ptr_stream);
        ptr_uart->uart->CR3 |=  USART_CR3_DMAT;
      }
    }
  }
}
#endif

#if (RTE_USART1 != 0)
#if (RTE_USART1_TX_DMA != 0)
  void DMAx_STREAMy_EVENT(RTE_USART1_TX_DMA_NUMBER, RTE_USART1_TX_DMA_STREAM) (uint32_t event) {
    UART_DMA_TxStreamEvent(event, ptr_UART[0]);
  }
#endif

#endif
#if (RTE_USART2 != 0)
#if (RTE_USART2_TX_DMA != 0)
  void DMAx_STREAMy_EVENT(RTE_USART2_TX_DMA_NUMBER, RTE_USART2_TX_DMA_STREAM) (uint32_t event) {
    UART_DMA_TxStreamEvent(event, ptr_UART[1]);
  }
#endif
#endif

#if (RTE_USART3 != 0)
#if (RTE_USART3_TX_DMA != 0)
  void DMAx_STREAMy_EVENT(RTE_USART3_TX_DMA_NUMBER, RTE_USART3_TX_DMA_STREAM) (uint32_t event) {
    UART_DMA_TxStreamEvent(event, ptr_UART[2]);
  }
#endif
#endif

#if (RTE_UART4 != 0)
#if (RTE_UART4_TX_DMA != 0)
  void DMAx_STREAMy_EVENT(RTE_UART4_TX_DMA_NUMBER, RTE_UART4_TX_DMA_STREAM) (uint32_t event) {
    UART_DMA_TxStreamEvent(event, ptr_UART[3]);
  }
#endif
#endif

#if (RTE_UART5 != 0)
#if (RTE_UART5_TX_DMA != 0)
  void DMAx_STREAMy_EVENT(RTE_UART5_TX_DMA_NUMBER, RTE_UART5_TX_DMA_STREAM) (uint32_t event) {
    UART_DMA_TxStreamEvent(event, ptr_UART[4]);
  }
#endif
#endif

#if (RTE_USART6 != 0)
#if (RTE_USART6_TX_DMA != 0)
  void DMAx_STREAMy_EVENT(RTE_USART6_TX_DMA_NUMBER, RTE_USART6_TX_DMA_STREAM) (uint32_t event) {
    UART_DMA_TxStreamEvent(event, ptr_UART[5]);
  }
#endif
#endif

#if (RTE_UART7 != 0)
#if (RTE_UART7_TX_DMA != 0)
  void DMAx_STREAMy_EVENT(RTE_UART7_TX_DMA_NUMBER, RTE_UART7_TX_DMA_STREAM) (uint32_t event) {
    UART_DMA_TxStreamEvent(event, ptr_UART[6]);
  }
#endif
#endif

#if (RTE_UART8 != 0)
#if (RTE_UART8_TX_DMA != 0)
  void DMAx_STREAMy_EVENT(RTE_UART8_TX_DMA_NUMBER, RTE_UART8_TX_DMA_STREAM) (uint32_t event) {
    UART_DMA_TxStreamEvent(event, ptr_UART[7]);
  }
#endif
#endif
	
#if (RTE_USART1 != 0)
  ARM_DRIVER_UART Driver_UART1 = {
    UARTX_GetVersion,
    UART1_GetCapabilities,
    UART1_Initialize,
    UART1_Uninitialize,
    UART1_PowerControl,
    UART1_Configure,
    UART1_WriteData,
    UART1_ReadData,
    UART1_DataAvailable,
    UART1_TxDone,
    UART1_SetTxThreshold,
    UART1_SetRxThreshold,
    UART1_FlushTxBuffer,
    UART1_FlushRxBuffer,
    UART1_GetComError,
    UART1_SetComBreak,
    UART1_ClearComBreak,
    UART1_SetModemControl,
    UART1_GetModemStatus
  };
#endif

#if (RTE_USART2 != 0)
  ARM_DRIVER_UART Driver_UART2 = {
    UARTX_GetVersion,
    UART2_GetCapabilities,
    UART2_Initialize,
    UART2_Uninitialize,
    UART2_PowerControl,
    UART2_Configure,
    UART2_WriteData,
    UART2_ReadData,
    UART2_DataAvailable,
    UART2_TxDone,
    UART2_SetTxThreshold,
    UART2_SetRxThreshold,
    UART2_FlushTxBuffer,
    UART2_FlushRxBuffer,
    UART2_GetComError,
    UART2_SetComBreak,
    UART2_ClearComBreak,
    UART2_SetModemControl,
    UART2_GetModemStatus
  };
#endif

#if (RTE_USART3 != 0)
  ARM_DRIVER_UART Driver_UART3 = {
    UARTX_GetVersion,
    UART3_GetCapabilities,
    UART3_Initialize,
    UART3_Uninitialize,
    UART3_PowerControl,
    UART3_Configure,
    UART3_WriteData,
    UART3_ReadData,
    UART3_DataAvailable,
    UART3_TxDone,
    UART3_SetTxThreshold,
    UART3_SetRxThreshold,
    UART3_FlushTxBuffer,
    UART3_FlushRxBuffer,
    UART3_GetComError,
    UART3_SetComBreak,
    UART3_ClearComBreak,
    UART3_SetModemControl,
    UART3_GetModemStatus
  };
#endif

#if (RTE_UART4 != 0)
  ARM_DRIVER_UART Driver_UART4 = {
    UARTX_GetVersion,
    UART4_GetCapabilities,
    UART4_Initialize,
    UART4_Uninitialize,
    UART4_PowerControl,
    UART4_Configure,
    UART4_WriteData,
    UART4_ReadData,
    UART4_DataAvailable,
    UART4_TxDone,
    UART4_SetTxThreshold,
    UART4_SetRxThreshold,
    UART4_FlushTxBuffer,
    UART4_FlushRxBuffer,
    UART4_GetComError,
    UART4_SetComBreak,
    UART4_ClearComBreak,
    UART4_SetModemControl,
    UART4_GetModemStatus
  };
#endif

#if (RTE_UART5 != 0)
  ARM_DRIVER_UART Driver_UART5 = {
    UARTX_GetVersion,
    UART5_GetCapabilities,
    UART5_Initialize,
    UART5_Uninitialize,
    UART5_PowerControl,
    UART5_Configure,
    UART5_WriteData,
    UART5_ReadData,
    UART5_DataAvailable,
    UART5_TxDone,
    UART5_SetTxThreshold,
    UART5_SetRxThreshold,
    UART5_FlushTxBuffer,
    UART5_FlushRxBuffer,
    UART5_GetComError,
    UART5_SetComBreak,
    UART5_ClearComBreak,
    UART5_SetModemControl,
    UART5_GetModemStatus
  };
#endif

#if (RTE_USART6 != 0)
  ARM_DRIVER_UART Driver_UART6 = {
    UARTX_GetVersion,
    UART6_GetCapabilities,
    UART6_Initialize,
    UART6_Uninitialize,
    UART6_PowerControl,
    UART6_Configure,
    UART6_WriteData,
    UART6_ReadData,
    UART6_DataAvailable,
    UART6_TxDone,
    UART6_SetTxThreshold,
    UART6_SetRxThreshold,
    UART6_FlushTxBuffer,
    UART6_FlushRxBuffer,
    UART6_GetComError,
    UART6_SetComBreak,
    UART6_ClearComBreak,
    UART6_SetModemControl,
    UART6_GetModemStatus
  };
#endif

#if (RTE_UART7 != 0)
  ARM_DRIVER_UART Driver_UART7 = {
    UARTX_GetVersion,
    UART7_GetCapabilities,
    UART7_Initialize,
    UART7_Uninitialize,
    UART7_PowerControl,
    UART7_Configure,
    UART7_WriteData,
    UART7_ReadData,
    UART7_DataAvailable,
    UART7_TxDone,
    UART7_SetTxThreshold,
    UART7_SetRxThreshold,
    UART7_FlushTxBuffer,
    UART7_FlushRxBuffer,
    UART7_GetComError,
    UART7_SetComBreak,
    UART7_ClearComBreak,
    UART7_SetModemControl,
    UART7_GetModemStatus
  };
#endif

#if (RTE_UART8 != 0)
  ARM_DRIVER_UART Driver_UART8 = {
    UARTX_GetVersion,
    UART8_GetCapabilities,
    UART8_Initialize,
    UART8_Uninitialize,
    UART8_PowerControl,
    UART8_Configure,
    UART8_WriteData,
    UART8_ReadData,
    UART8_DataAvailable,
    UART8_TxDone,
    UART8_SetTxThreshold,
    UART8_SetRxThreshold,
    UART8_FlushTxBuffer,
    UART8_FlushRxBuffer,
    UART8_GetComError,
    UART8_SetComBreak,
    UART8_ClearComBreak,
    UART8_SetModemControl,
    UART8_GetModemStatus
  };
#endif
