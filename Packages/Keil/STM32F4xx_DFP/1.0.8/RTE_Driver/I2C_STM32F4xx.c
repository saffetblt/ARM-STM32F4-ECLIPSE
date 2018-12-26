/* ----------------------------------------------------------------------
 * Copyright (C) 2014 ARM Limited. All rights reserved.
 *  
 * $Date:        14. April 2013
 * $Revision:    V1.03
 *  
 * Driver:       Driver_I2C1, Driver_I2C2, Driver_I2C3
 * Configured:   via RTE_Device.h configuration file 
 * Project:      I2C Driver for ST STM32F4xx
 * ---------------------------------------------------------------------- 
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 * 
 *   Configuration Setting               Value     I2C Interface
 *   ---------------------               -----     -------------
 *   Connect to hardware via Driver_I2C# = 1       use I2C1
 *   Connect to hardware via Driver_I2C# = 2       use I2C2
 *   Connect to hardware via Driver_I2C# = 3       use I2C3
 * -------------------------------------------------------------------- */

/* History:
 *  Version 1.03
 *    Bugfix (corrected 2 byte reception)
 *  Version 1.02
 *    Bugfix (corrected I2C register access)
 *  Version 1.01
 *    Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.00
 *    Initial release
 */ 


#include "cmsis_os.h"
#include "stm32f4xx.h"

#include "GPIO_STM32F4xx.h"
#include "DMA_STM32F4xx.h"

#include "Driver_I2C.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#define ARM_I2C_DRV_VERSION ARM_DRV_VERSION_MAJOR_MINOR(1,03)    /* driver version */

#if ((defined(RTE_Drivers_I2C1) || \
      defined(RTE_Drivers_I2C2) || \
      defined(RTE_Drivers_I2C3))   \
     && !RTE_I2C1                  \
     && !RTE_I2C2                  \
     && !RTE_I2C3)
#error "I2C not configured in RTE_Device.h!"
#endif

#if (RTE_I2C1)
#if (  (RTE_I2C1_RX_DMA          != 0)                                     && \
     ( (RTE_I2C1_RX_DMA_NUMBER   != 1)                                     || \
      ((RTE_I2C1_RX_DMA_STREAM   != 0) && (RTE_I2C1_RX_DMA_STREAM  != 5))  || \
       (RTE_I2C1_RX_DMA_CHANNEL  != 1)                                     || \
      ((RTE_I2C1_RX_DMA_PRIORITY  < 0) || (RTE_I2C1_RX_DMA_PRIORITY > 3))))
#error "I2C1 Rx DMA configuration in RTE_Device.h is invalid!"
#endif
#if (  (RTE_I2C1_TX_DMA          != 0)                                     && \
     ( (RTE_I2C1_TX_DMA_NUMBER   != 1)                                     || \
      ((RTE_I2C1_TX_DMA_STREAM   != 6) && (RTE_I2C1_TX_DMA_STREAM  != 7))  || \
       (RTE_I2C1_TX_DMA_CHANNEL  != 1)                                     || \
      ((RTE_I2C1_TX_DMA_PRIORITY  < 0) || (RTE_I2C1_TX_DMA_PRIORITY > 3))))
#error "I2C1 Tx DMA configuration in RTE_Device.h is invalid!"
#endif
#endif

#if (RTE_I2C2)
#if (  (RTE_I2C2_RX_DMA          != 0)                                     && \
     ( (RTE_I2C2_RX_DMA_NUMBER   != 1)                                     || \
      ((RTE_I2C2_RX_DMA_STREAM   != 2) && (RTE_I2C2_RX_DMA_STREAM  != 2))  || \
       (RTE_I2C2_RX_DMA_CHANNEL  != 7)                                     || \
      ((RTE_I2C2_RX_DMA_PRIORITY  < 0) || (RTE_I2C2_RX_DMA_PRIORITY > 3))))
#error "I2C2 Rx DMA configuration in RTE_Device.h is invalid!"
#endif
#if (  (RTE_I2C2_TX_DMA          != 0)                                     && \
     ( (RTE_I2C2_TX_DMA_NUMBER   != 1)                                     || \
       (RTE_I2C2_TX_DMA_STREAM   != 7)                                     || \
       (RTE_I2C2_TX_DMA_CHANNEL  != 7)                                     || \
      ((RTE_I2C2_TX_DMA_PRIORITY  < 0) || (RTE_I2C2_TX_DMA_PRIORITY > 3))))
#error "I2C2 Tx DMA configuration in RTE_Device.h is invalid!"
#endif
#endif

#if (RTE_I2C3)
#if (  (RTE_I2C3_RX_DMA          != 0)                                     && \
     ( (RTE_I2C3_RX_DMA_NUMBER   != 1)                                     || \
       (RTE_I2C3_RX_DMA_STREAM   != 2)                                     || \
       (RTE_I2C3_RX_DMA_CHANNEL  != 3)                                     || \
      ((RTE_I2C3_RX_DMA_PRIORITY  < 0) || (RTE_I2C3_RX_DMA_PRIORITY > 3))))
#error "I2C3 Rx DMA configuration in RTE_Device.h is invalid!"
#endif
#if (  (RTE_I2C3_TX_DMA          != 0)                                     && \
     ( (RTE_I2C3_TX_DMA_NUMBER   != 1)                                     || \
       (RTE_I2C3_TX_DMA_STREAM   != 4)                                     || \
       (RTE_I2C3_TX_DMA_CHANNEL  != 3)                                     || \
      ((RTE_I2C3_TX_DMA_PRIORITY  < 0) || (RTE_I2C3_TX_DMA_PRIORITY > 3))))
#error "I2C3 Tx DMA configuration in RTE_Device.h is invalid!"
#endif
#endif


#define GPIO_AF_I2C             GPIO_AF_I2C1        /* same for all I2C */
#define RCC_APB_I2C1_MASK       RCC_APB1ENR_I2C1EN  /* same for Clock/Reset */
#define RCC_APB_I2C2_MASK       RCC_APB1ENR_I2C2EN  /* same for Clock/Reset */
#define RCC_APB_I2C3_MASK       RCC_APB1ENR_I2C3EN  /* same for Clock/Reset */

#define I2C_BUSY_TIMEOUT            25  /* I2C bus busy wait timeout in us        */
#define I2C_BUS_CLEAR_CLOCK_PERIOD  10  /* I2C bus clock period in us             */

#define DMA_STREAM_DISABLE_TOUT    100  /* DMA Stream disable timeout in CPU cycles */

/* Transfer status flags definitions */
#define FLAG_TRAN_DIR_RX     (1 << 0)   /* Transfer direction: receive            */
#define FLAG_ADDR_SENT       (1 << 1)   /* Slave address sent                     */
#define FLAG_ADDR_RSTART     (1 << 2)   /* Generate repeated start and readdress  */
#define FLAG_ADDR_DONE       (1 << 3)   /* Slave addressing done                  */
#define FLAG_NO_STOP_COND    (1 << 4)   /* Don't generate stop condition          */


/* Driver Version */
static const ARM_DRV_VERSION DriverVersion = {
  ARM_I2C_API_VERSION,
  ARM_I2C_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_I2C_CAPABILITIES DriverCapabilities = { 0 };


/* I2C Information (Run-Time) */
typedef struct {
  uint8_t                 init;         // Init counter
  bool                    power;        // Power flag
  bool                    busy;         // Busy flag
  uint16_t volatile       error;        // Error status flags
  uint8_t                 flags;        // Transfer status flags
  uint16_t                addr;         // Device address
  uint8_t                *data;         // Data pointer
  uint32_t                size;         // Data size
  ARM_I2C_SignalEvent_t   cb_event;     // Event Callback
  osSemaphoreId           semaphore;    // Semaphore Id
  osMutexId               mutex;        // Mutex Id
} I2C_INFO;

/* I2C Resources definition */
typedef struct {
        GPIO_PIN_ID       pin_scl;      // SCL Pin identifier
        GPIO_PIN_ID       pin_sda;      // SDA Pin identifier
        I2C_TypeDef      *i2c;          // Pointer to I2C peripheral
  enum  IRQn              i2c_ev_irq;   // I2C Event IRQ Number
  enum  IRQn              i2c_er_irq;   // I2C Error IRQ Number
        uint32_t          apb_mask;     // APB Clock/Reset register mask
  const DMA_INFO         *dma_rx;       // I2C Receive DMA configuration
  const DMA_INFO         *dma_tx;       // I2C Transmit DMA configuration
  const osSemaphoreDef_t *semaphore;    // Semaphore definition
  const osMutexDef_t     *mutex;        // Mutex definition
        I2C_INFO         *info;         // Run-Time information
} const I2C_RESOURCES;


#if (RTE_I2C1)

/* I2C1 DMA */
#if (RTE_I2C1_RX_DMA)
static const DMA_INFO I2C1_DMA_RX = {
  DMAx_STREAMy(RTE_I2C1_RX_DMA_NUMBER, RTE_I2C1_RX_DMA_STREAM),
  RTE_I2C1_RX_DMA_NUMBER,
  RTE_I2C1_RX_DMA_STREAM,
  RTE_I2C1_RX_DMA_CHANNEL,
  RTE_I2C1_RX_DMA_PRIORITY
};
#endif
#if (RTE_I2C1_TX_DMA)
static const DMA_INFO I2C1_DMA_TX = {
  DMAx_STREAMy(RTE_I2C1_TX_DMA_NUMBER, RTE_I2C1_TX_DMA_STREAM),
  RTE_I2C1_TX_DMA_NUMBER,
  RTE_I2C1_TX_DMA_STREAM,
  RTE_I2C1_TX_DMA_CHANNEL,
  RTE_I2C1_TX_DMA_PRIORITY
};
#endif

/* I2C1 Semaphores */
osSemaphoreDef(I2C1_Semaphore);
osSemaphoreId  I2C1_Semaphore;

/* I2C1 Mutex */
osMutexDef(I2C1_Mutex);
osMutexId  I2C1_Mutex;

/* I2C1 Information (Run-Time) */
static I2C_INFO I2C1_Info;

/* I2C1 Resources */
static I2C_RESOURCES I2C1_Resources = {
  { RTE_I2C1_SCL_PORT, RTE_I2C1_SCL_BIT },
  { RTE_I2C1_SDA_PORT, RTE_I2C1_SDA_BIT },
  I2C1,
  I2C1_EV_IRQn,
  I2C1_ER_IRQn,
  RCC_APB_I2C1_MASK,
#if (RTE_I2C1_RX_DMA)
  &I2C1_DMA_RX,
#else
  NULL,
#endif
#if (RTE_I2C1_TX_DMA)
  &I2C1_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(I2C1_Semaphore),
  osMutex(I2C1_Mutex),
  &I2C1_Info
};

#endif /* RTE_I2C1 */


#if (RTE_I2C2)

/* I2C2 DMA */
#if (RTE_I2C2_RX_DMA)
static const DMA_INFO I2C2_DMA_RX = {
  DMAx_STREAMy(RTE_I2C2_RX_DMA_NUMBER, RTE_I2C2_RX_DMA_STREAM),
  RTE_I2C2_RX_DMA_NUMBER,
  RTE_I2C2_RX_DMA_STREAM,
  RTE_I2C2_RX_DMA_CHANNEL,
  RTE_I2C2_RX_DMA_PRIORITY
};
#endif
#if (RTE_I2C2_TX_DMA)
static const DMA_INFO I2C2_DMA_TX = {
  DMAx_STREAMy(RTE_I2C2_TX_DMA_NUMBER, RTE_I2C2_TX_DMA_STREAM),
  RTE_I2C2_TX_DMA_NUMBER,
  RTE_I2C2_TX_DMA_STREAM,
  RTE_I2C2_TX_DMA_CHANNEL,
  RTE_I2C2_TX_DMA_PRIORITY
};
#endif

/* I2C2 Semaphores */
osSemaphoreDef(I2C2_Semaphore);
osSemaphoreId  I2C2_Semaphore;

/* I2C2 Mutex */
osMutexDef(I2C2_Mutex);
osMutexId  I2C2_Mutex;

/* I2C2 Information (Run-Time) */
static I2C_INFO I2C2_Info;

/* I2C2 Resources */
static I2C_RESOURCES I2C2_Resources = {
  { RTE_I2C2_SCL_PORT, RTE_I2C2_SCL_BIT },
  { RTE_I2C2_SDA_PORT, RTE_I2C2_SDA_BIT },
  I2C2,
  I2C2_EV_IRQn,
  I2C2_ER_IRQn,
  RCC_APB_I2C2_MASK,
#if (RTE_I2C2_RX_DMA)
  &I2C2_DMA_RX,
#else
  NULL,
#endif
#if (RTE_I2C2_TX_DMA)
  &I2C2_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(I2C2_Semaphore),
  osMutex(I2C2_Mutex),
  &I2C2_Info
};

#endif /* RTE_I2C2 */


#if (RTE_I2C3)

/* I2C3 DMA */
#if (RTE_I2C3_RX_DMA)
static const DMA_INFO I2C3_DMA_RX = {
  DMAx_STREAMy(RTE_I2C3_RX_DMA_NUMBER, RTE_I2C3_RX_DMA_STREAM),
  RTE_I2C3_RX_DMA_NUMBER,
  RTE_I2C3_RX_DMA_STREAM,
  RTE_I2C3_RX_DMA_CHANNEL,
  RTE_I2C3_RX_DMA_PRIORITY
};
#endif
#if (RTE_I2C3_TX_DMA)
static const DMA_INFO I2C3_DMA_TX = {
  DMAx_STREAMy(RTE_I2C3_TX_DMA_NUMBER, RTE_I2C3_TX_DMA_STREAM),
  RTE_I2C3_TX_DMA_NUMBER,
  RTE_I2C3_TX_DMA_STREAM,
  RTE_I2C3_TX_DMA_CHANNEL,
  RTE_I2C3_TX_DMA_PRIORITY
};
#endif

/* I2C3 Semaphores */
osSemaphoreDef(I2C3_Semaphore);
osSemaphoreId  I2C3_Semaphore;

/* I2C3 Mutex */
osMutexDef(I2C3_Mutex);
osMutexId  I2C3_Mutex;

/* I2C3 Information (Run-Time) */
static I2C_INFO I2C3_Info;

/* I2C3 Resources */
static I2C_RESOURCES I2C3_Resources = {
  { RTE_I2C3_SCL_PORT, RTE_I2C3_SCL_BIT },
  { RTE_I2C3_SDA_PORT, RTE_I2C3_SDA_BIT },
  I2C3,
  I2C3_EV_IRQn,
  I2C3_ER_IRQn,
  RCC_APB_I2C3_MASK,
#if (RTE_I2C3_RX_DMA)
  &I2C3_DMA_RX,
#else
  NULL,
#endif
#if (RTE_I2C3_TX_DMA)
  &I2C3_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(I2C3_Semaphore),
  osMutex(I2C3_Mutex),
  &I2C3_Info
};

#endif /* RTE_I2C3 */


/* I2C Resources */
static I2C_RESOURCES *ptr_I2C[3] = {
#if (RTE_I2C1)
  &I2C1_Resources,
#else
  NULL,
#endif
#if (RTE_I2C2)
  &I2C2_Resources,
#else
  NULL,
#endif
#if (RTE_I2C3)
  &I2C3_Resources,
#else
  NULL,
#endif
};


/**
  \fn          ARM_DRV_VERSION I2C_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRV_VERSION I2CX_GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          ARM_I2C_CAPABILITIES I2C_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_I2C_CAPABILITIES
*/
static ARM_I2C_CAPABILITIES I2CX_GetCapabilities (void) {
  return DriverCapabilities;
}


/**
  \fn          ARM_I2C_STATUS I2C_Initialize (ARM_I2C_SignalEvent_t cb_event,
                                              I2C_RESOURCES        *ptr_i2c)
  \brief       Initialize I2C Interface.
  \param[in]   cb_event  Pointer to \ref ARM_I2C_SignalEvent
  \param[in]   ptr_i2c   Pointer to I2C resources
  \return      \ref ARM_I2C_STATUS
*/
static ARM_I2C_STATUS I2C_Initialize (ARM_I2C_SignalEvent_t cb_event,
                                      I2C_RESOURCES        *ptr_i2c) {

  if (ptr_i2c->info->init++) {
    /* Already initialized */
    return ARM_I2C_OK;
  }

  /* Enable I2C Clock */
  RCC->APB1ENR  |=  ptr_i2c->apb_mask;

  /* Reset I2C Peripheral */
  RCC->APB1RSTR |=  ptr_i2c->apb_mask;
  __NOP(); __NOP(); __NOP(); __NOP(); 
  RCC->APB1RSTR &= ~ptr_i2c->apb_mask;

  /* Clear and Enable I2C IRQ */
  NVIC_ClearPendingIRQ(ptr_i2c->i2c_ev_irq);
  NVIC_ClearPendingIRQ(ptr_i2c->i2c_er_irq);
  NVIC_EnableIRQ(ptr_i2c->i2c_ev_irq);
  NVIC_EnableIRQ(ptr_i2c->i2c_er_irq);

  /* Initialize DMA Streams */
  #if ((RTE_I2C1 && RTE_I2C1_RX_DMA) || \
       (RTE_I2C2 && RTE_I2C2_RX_DMA) || \
       (RTE_I2C3 && RTE_I2C3_RX_DMA))
    if (ptr_i2c->dma_rx) {
      DMA_StreamInitialize(ptr_i2c->dma_rx->dma, ptr_i2c->dma_rx->stream);
    }
  #endif

  #if ((RTE_I2C1 && RTE_I2C1_TX_DMA) || \
       (RTE_I2C2 && RTE_I2C2_TX_DMA) || \
       (RTE_I2C3 && RTE_I2C3_TX_DMA))
    if (ptr_i2c->dma_tx) {
      DMA_StreamInitialize(ptr_i2c->dma_tx->dma, ptr_i2c->dma_tx->stream);
    }
  #endif

  /* Initialize I2C Run-Time Resources */
  ptr_i2c->info->power = false;
  ptr_i2c->info->semaphore = osSemaphoreCreate(ptr_i2c->semaphore, 0);
  if (ptr_i2c->info->semaphore == NULL) return ARM_I2C_ERROR;
  
  ptr_i2c->info->mutex = osMutexCreate (ptr_i2c->mutex);
  if (ptr_i2c->info->mutex == NULL) return ARM_I2C_ERROR;

  ptr_i2c->info->cb_event = cb_event;

  /* Initialize I2C Peripheral */
  ptr_i2c->i2c->CR1 = 0;

  /* Configure SCL Pin */
  GPIO_PortClock   (ptr_i2c->pin_scl.port, true);
  GPIO_PinAF       (ptr_i2c->pin_scl.port, ptr_i2c->pin_scl.num, GPIO_AF_I2C);
  GPIO_PinConfigure(ptr_i2c->pin_scl.port, ptr_i2c->pin_scl.num,
                    GPIO_MODE_AF,
                    GPIO_OUTPUT_OPEN_DRAIN,
                    GPIO_OUTPUT_SPEED_25MHz,
                    GPIO_NO_PULL_UP_DOWN);

  /* Configure SDA Pin */
  GPIO_PortClock   (ptr_i2c->pin_sda.port, true);
  GPIO_PinAF       (ptr_i2c->pin_sda.port, ptr_i2c->pin_sda.num, GPIO_AF_I2C);
  GPIO_PinConfigure(ptr_i2c->pin_sda.port, ptr_i2c->pin_sda.num,
                    GPIO_MODE_AF,
                    GPIO_OUTPUT_OPEN_DRAIN,
                    GPIO_OUTPUT_SPEED_25MHz,
                    GPIO_NO_PULL_UP_DOWN);

  return ARM_I2C_OK;
}

#if (RTE_I2C1)
static ARM_I2C_STATUS I2C1_Initialize (ARM_I2C_SignalEvent_t cb_event) {
  return I2C_Initialize(cb_event, ptr_I2C[0]);
}
#endif

#if (RTE_I2C2)
static ARM_I2C_STATUS I2C2_Initialize (ARM_I2C_SignalEvent_t cb_event) {
  return I2C_Initialize(cb_event, ptr_I2C[1]);
}
#endif

#if (RTE_I2C3)
static ARM_I2C_STATUS I2C3_Initialize (ARM_I2C_SignalEvent_t cb_event) {
  return I2C_Initialize(cb_event, ptr_I2C[2]);
}
#endif


/**
  \fn          ARM_I2C_STATUS I2C_Uninitialize (I2C_RESOURCES *ptr_i2c)
  \brief       De-initialize I2C Interface.
  \param[in]   ptr_i2c  Pointer to I2C resources
  \return      \ref I2C_STATUS
*/
static ARM_I2C_STATUS I2C_Uninitialize (I2C_RESOURCES *ptr_i2c) {

  if (ptr_i2c->info->init) {
    if (--ptr_i2c->info->init) {
      return ARM_I2C_OK;
    }
  }

  /* Unconfigure SCL Pin */
  GPIO_PinAF       (ptr_i2c->pin_scl.port, ptr_i2c->pin_scl.num, GPIO_AF_SYSTEM);
  GPIO_PinConfigure(ptr_i2c->pin_scl.port, ptr_i2c->pin_scl.num,
                    GPIO_MODE_INPUT,
                    GPIO_OUTPUT_PUSH_PULL,
                    GPIO_OUTPUT_SPEED_2MHz,
                    GPIO_NO_PULL_UP_DOWN);

  /* Unconfigure SDA Pin */
  GPIO_PinAF       (ptr_i2c->pin_sda.port, ptr_i2c->pin_sda.num, GPIO_AF_SYSTEM);
  GPIO_PinConfigure(ptr_i2c->pin_sda.port, ptr_i2c->pin_sda.num,
                    GPIO_MODE_INPUT,
                    GPIO_OUTPUT_PUSH_PULL,
                    GPIO_OUTPUT_SPEED_2MHz,
                    GPIO_NO_PULL_UP_DOWN);

  /* Enable I2C Clock */
  RCC->APB1ENR  |=  ptr_i2c->apb_mask;

  /* Reset I2C Peripheral */
  RCC->APB1RSTR |=  ptr_i2c->apb_mask;
  __NOP(); __NOP(); __NOP(); __NOP(); 
  RCC->APB1RSTR &= ~ptr_i2c->apb_mask;

  /* Disable I2C Clock */
  RCC->APB1ENR  &= ~ptr_i2c->apb_mask;

  /* Disable I2C IRQ */
  NVIC_DisableIRQ(ptr_i2c->i2c_ev_irq);
  NVIC_DisableIRQ(ptr_i2c->i2c_er_irq);

  /* Uninitialize DMA Streams */
  #if ((RTE_I2C1 && RTE_I2C1_RX_DMA) || \
       (RTE_I2C2 && RTE_I2C2_RX_DMA) || \
       (RTE_I2C3 && RTE_I2C3_RX_DMA))
    if (ptr_i2c->dma_rx) {
      DMA_StreamUninitialize(ptr_i2c->dma_rx->dma, ptr_i2c->dma_rx->stream);
    }
  #endif
  #if ((RTE_I2C1 && RTE_I2C1_TX_DMA) || \
       (RTE_I2C2 && RTE_I2C2_TX_DMA) || \
       (RTE_I2C3 && RTE_I2C3_TX_DMA))
    if (ptr_i2c->dma_tx) {
      DMA_StreamUninitialize(ptr_i2c->dma_tx->dma, ptr_i2c->dma_tx->stream);
    }
  #endif

  /* Uninitialize I2C Run-Time Resources */
  ptr_i2c->info->power = false;
  osSemaphoreDelete(ptr_i2c->info->semaphore);
  osMutexDelete(ptr_i2c->info->mutex);

  return ARM_I2C_OK;
}

#if (RTE_I2C1)
static ARM_I2C_STATUS I2C1_Uninitialize (void) {
  return I2C_Uninitialize(ptr_I2C[0]);
}
#endif

#if (RTE_I2C2)
static ARM_I2C_STATUS I2C2_Uninitialize (void) {
  return I2C_Uninitialize(ptr_I2C[1]);
}
#endif

#if (RTE_I2C3)
static ARM_I2C_STATUS I2C3_Uninitialize (void) {
  return I2C_Uninitialize(ptr_I2C[2]);
}
#endif


/**
  \fn          ARM_I2C_STATUS I2C_PowerControl (ARM_POWER_STATE state,
                                                I2C_RESOURCES  *ptr_i2c)
  \brief       Control I2C Interface Power.
  \param[in]   state    Power state
  \param[in]   ptr_i2c  Pointer to I2C resources
  \return      \ref ARM_I2C_STATUS
*/
static ARM_I2C_STATUS I2C_PowerControl (ARM_POWER_STATE state, I2C_RESOURCES *ptr_i2c) {

  switch (state) {
    case ARM_POWER_OFF:

      /* Disable I2C */
      ptr_i2c->i2c->CR1 &= ~I2C_CR1_PE;

      /* Disable I2C Clock */
      RCC->APB1ENR &= ~ptr_i2c->apb_mask;

      /* Disable DMA Streams */
      if (ptr_i2c->dma_tx) {
        DMA_StreamDisable(ptr_i2c->dma_tx->ptr_stream);
      }
      if (ptr_i2c->dma_rx) {
        DMA_StreamDisable(ptr_i2c->dma_rx->ptr_stream);
      }

      // Update I2C Run-Time Resources
      ptr_i2c->info->power = false;
      osSemaphoreRelease(ptr_i2c->info->semaphore);
      osSemaphoreWait   (ptr_i2c->info->semaphore, 0);

      break;

    case ARM_POWER_LOW:
      return ARM_I2C_ERROR_UNSUPPORTED;
    
    case ARM_POWER_FULL:

      /* Enable I2C Clock */
      RCC->APB1ENR |= ptr_i2c->apb_mask;

      /* Enable I2C */
      ptr_i2c->i2c->CR1 |= I2C_CR1_PE;

      /* Enable error interrupts */
      ptr_i2c->i2c->CR2 |= I2C_CR2_ITERREN;
    
      /* Update I2C Run-Time Resources */
      ptr_i2c->info->power = true;

      break;
  }

  return ARM_I2C_OK;
}

#if (RTE_I2C1)
static ARM_I2C_STATUS I2C1_PowerControl (ARM_POWER_STATE state) {
  return I2C_PowerControl(state, ptr_I2C[0]);
}
#endif

#if (RTE_I2C2)
static ARM_I2C_STATUS I2C2_PowerControl (ARM_POWER_STATE state) {
  return I2C_PowerControl(state, ptr_I2C[1]);
}
#endif

#if (RTE_I2C3)
static ARM_I2C_STATUS I2C3_PowerControl (ARM_POWER_STATE state) {
  return I2C_PowerControl(state, ptr_I2C[2]);
}
#endif


/**
  \fn          ARM_I2C_STATUS I2C_BusSpeed (ARM_I2C_BUS_SPEED speed,
                                            I2C_RESOURCES    *ptr_i2c)
  \brief       Set bus speed for I2C transfers.
  \param[in]   speed    Requested bus speed
  \param[in]   ptr_i2c  Pointer to I2C resources
  \return      \ref ARM_I2C_STATUS
*/
static ARM_I2C_STATUS I2C_BusSpeed (ARM_I2C_BUS_SPEED speed, I2C_RESOURCES *ptr_i2c) {
  uint16_t ccr;
  uint16_t trise;

  if (!ptr_i2c->info->power) return ARM_I2C_ERROR;

  switch (speed) {
    case ARM_I2C_BUS_SPEED_STANDARD:
      /* Clock = 100kHz,  Rise Time = 1000ns */
      if (RTE_PCLK1 > 42000000) return ARM_I2C_ERROR_UNSUPPORTED;
      if (RTE_PCLK1 <  2000000) return ARM_I2C_ERROR_UNSUPPORTED;
      ccr   =  RTE_PCLK1 / 100000 / 2;
      trise = (RTE_PCLK1 / 1000000) + 1;
      break;
    case ARM_I2C_BUS_SPEED_FAST:
      /* Clock = 400kHz,  Rise Time = 300ns */
      if (RTE_PCLK1 > 42000000) return ARM_I2C_ERROR_UNSUPPORTED;
      if (RTE_PCLK1 <  4000000) return ARM_I2C_ERROR_UNSUPPORTED;
      if ((RTE_PCLK1 >= 10000000) && ((RTE_PCLK1 % 10000000) == 0)) {
        ccr = I2C_CCR_FS | I2C_CCR_DUTY | (RTE_PCLK1 / 400000 / 25);
      } else {
        ccr = I2C_CCR_FS |                (RTE_PCLK1 / 400000 / 3);
      }
      trise = (RTE_PCLK1 / 333333) + 1;
      break;
    default:
      return ARM_I2C_ERROR_UNSUPPORTED;
  }

  ptr_i2c->i2c->CR1   &= ~I2C_CR1_PE;           /* Disable I2C peripheral */
  ptr_i2c->i2c->CR2   &= ~I2C_CR2_FREQ;
  ptr_i2c->i2c->CR2   |=  RTE_PCLK1 / 1000000;
  ptr_i2c->i2c->CCR    =  ccr;
  ptr_i2c->i2c->TRISE  =  trise;
  ptr_i2c->i2c->CR1   |=  I2C_CR1_PE;           /* Enable I2C peripheral */

  return ARM_I2C_OK;
}

#if (RTE_I2C1)
static ARM_I2C_STATUS I2C1_BusSpeed (ARM_I2C_BUS_SPEED speed) {
  return I2C_BusSpeed(speed, ptr_I2C[0]);
}
#endif

#if (RTE_I2C2)
static ARM_I2C_STATUS I2C2_BusSpeed (ARM_I2C_BUS_SPEED speed) {
  return I2C_BusSpeed(speed, ptr_I2C[1]);
}
#endif

#if (RTE_I2C3)
static ARM_I2C_STATUS I2C3_BusSpeed (ARM_I2C_BUS_SPEED speed) {
  return I2C_BusSpeed(speed, ptr_I2C[2]);
}
#endif


/**
  \fn          ARM_I2C_STATUS I2C_BusClear (I2C_RESOURCES *ptr_i2c)
  \brief       Send nine clock pulses to clear the bus.
  \param[in]   ptr_i2c  Pointer to I2C resources
  \return      \ref ARM_I2C_STATUS
*/
static ARM_I2C_STATUS I2C_BusClear (I2C_RESOURCES *ptr_i2c) {
  uint32_t i;
  uint32_t tick;
  uint32_t state;

  /* Configure SCL Pin as GPIO */
  GPIO_PortClock   (ptr_i2c->pin_scl.port, true);
  GPIO_PinAF       (ptr_i2c->pin_scl.port, ptr_i2c->pin_scl.num, GPIO_AF_SYSTEM);
  GPIO_PinConfigure(ptr_i2c->pin_scl.port, ptr_i2c->pin_scl.num,
                    GPIO_MODE_OUTPUT,
                    GPIO_OUTPUT_OPEN_DRAIN,
                    GPIO_OUTPUT_SPEED_25MHz,
                    GPIO_NO_PULL_UP_DOWN);

  /* Configure SDA Pin as GPIO */
  GPIO_PortClock   (ptr_i2c->pin_sda.port, true);
  GPIO_PinAF       (ptr_i2c->pin_sda.port, ptr_i2c->pin_sda.num, GPIO_AF_SYSTEM);
  GPIO_PinConfigure(ptr_i2c->pin_sda.port, ptr_i2c->pin_sda.num,
                    GPIO_MODE_OUTPUT,
                    GPIO_OUTPUT_OPEN_DRAIN,
                    GPIO_OUTPUT_SPEED_25MHz,
                    GPIO_NO_PULL_UP_DOWN);

  /* Pull SCL and SDA high */
  GPIO_PinWrite (ptr_i2c->pin_scl.port, ptr_i2c->pin_scl.num, 1);
  GPIO_PinWrite (ptr_i2c->pin_sda.port, ptr_i2c->pin_sda.num, 1);
  tick = osKernelSysTick();
  while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec (I2C_BUS_CLEAR_CLOCK_PERIOD));

  for (i = 0; i < 9; i++) {
    if (GPIO_PinRead (ptr_i2c->pin_sda.port, ptr_i2c->pin_sda.num)) {
      /* Break if slave released SDA line */
      break;
    }
    /* Clock high */
    GPIO_PinWrite (ptr_i2c->pin_scl.port, ptr_i2c->pin_scl.num, 1);
    tick = osKernelSysTick();
    while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec (I2C_BUS_CLEAR_CLOCK_PERIOD/2));

    /* Clock low */
    GPIO_PinWrite (ptr_i2c->pin_scl.port, ptr_i2c->pin_scl.num, 0);
    tick = osKernelSysTick();
    while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec (I2C_BUS_CLEAR_CLOCK_PERIOD/2));
  }
  /* Check SDA state */
  state = GPIO_PinRead (ptr_i2c->pin_sda.port, ptr_i2c->pin_sda.num);

  /* Configure SCL Pin as I2C peripheral pin */
  GPIO_PortClock   (ptr_i2c->pin_scl.port, true);
  GPIO_PinAF       (ptr_i2c->pin_scl.port, ptr_i2c->pin_scl.num, GPIO_AF_I2C);
  GPIO_PinConfigure(ptr_i2c->pin_scl.port, ptr_i2c->pin_scl.num,
                    GPIO_MODE_AF,
                    GPIO_OUTPUT_OPEN_DRAIN,
                    GPIO_OUTPUT_SPEED_25MHz,
                    GPIO_NO_PULL_UP_DOWN);

  /* Configure SDA Pin as I2C peripheral pin */
  GPIO_PortClock   (ptr_i2c->pin_sda.port, true);
  GPIO_PinAF       (ptr_i2c->pin_sda.port, ptr_i2c->pin_sda.num, GPIO_AF_I2C);
  GPIO_PinConfigure(ptr_i2c->pin_sda.port, ptr_i2c->pin_sda.num,
                    GPIO_MODE_AF,
                    GPIO_OUTPUT_OPEN_DRAIN,
                    GPIO_OUTPUT_SPEED_25MHz,
                    GPIO_NO_PULL_UP_DOWN);

  return (state) ? ARM_I2C_OK : ARM_I2C_ERROR;
}

#if (RTE_I2C1)
static ARM_I2C_STATUS I2C1_BusClear (void) {
  return I2C_BusClear(ptr_I2C[0]);
}
#endif

#if (RTE_I2C2)
static ARM_I2C_STATUS I2C2_BusClear (void) {
  return I2C_BusClear(ptr_I2C[1]);
}
#endif

#if (RTE_I2C3)
static ARM_I2C_STATUS I2C3_BusClear (void) {
  return I2C_BusClear(ptr_I2C[2]);
}
#endif


/**
  \fn          int32_t I2C_SendData (uint32_t       addr,
                                     const uint8_t *data,
                                     uint32_t       size,
                                     bool           rx_pending,
                                     I2C_RESOURCES *ptr_i2c)

  \brief       Send data to I2C Slave.
  \param[in]   addr         Slave address (7-bit or 10-bit)
  \param[in]   data         Pointer to buffer with data to send to I2C slave
  \param[in]   size         Data buffer size in bytes
  \param[in]   xfer_pending Transfer operation is pending - Stop condition will not be generated
  \param[in]   ptr_i2c      Pointer to I2C resources
  \return      number of data bytes sent and acknowledged, error code if negative
*/
static int32_t I2C_SendData (uint32_t       addr,
                             const uint8_t *data,
                             uint32_t       size,
                             bool           xfer_pending,
                             I2C_RESOURCES *ptr_i2c) {
  uint32_t i, tick;
  uint32_t s_ctrl;
  int32_t  ret_val;
  
  osMutexWait (ptr_i2c->info->mutex, osWaitForever);

  ptr_i2c->info->addr  = addr;
  ptr_i2c->info->data  = (uint8_t *)data;
  ptr_i2c->info->size  = size;
  ptr_i2c->info->flags = 0;

  if (ptr_i2c->dma_tx) {
    /* DMA mode data transfer */
    s_ctrl = (ptr_i2c->dma_tx->channel << DMA_CHANNEL_POS)  |
             DMA_MEMORY_SINGLE_TRANSFER                     |
             DMA_PERIPHERAL_SINGLE_TRANSFER                 |
             (ptr_i2c->dma_tx->priority << DMA_PRIORITY_POS)|
             DMA_MEMORY_DATA_8BIT                           |
             DMA_PERIPHERAL_DATA_8BIT                       |
             DMA_MEMORY_INCREMENT                           |
             DMA_MEMORY_TO_PERIPHERAL                       |
             DMA_TRANSFER_COMPLETE_INTERRUPT;
    
    DMA_StreamDisable (ptr_i2c->dma_tx->ptr_stream);

    for (i = DMA_STREAM_DISABLE_TOUT; i; i++) {
      if (!DMA_StreamStatus (ptr_i2c->dma_tx->ptr_stream)) break;
    }
    if (DMA_StreamStatus (ptr_i2c->dma_tx->ptr_stream) == true) {
      return (-ARM_I2C_ERROR);
    }

    DMA_StreamConfigure (ptr_i2c->dma_tx->ptr_stream,
                         s_ctrl,
                         DMA_FIFO_DIRECT_MODE_ENABLE,
                         (uint32_t)&(ptr_i2c->i2c->DR),
                         (uint32_t)data,
                         size);
    DMA_StreamEnable (ptr_i2c->dma_tx->ptr_stream);
    ptr_i2c->i2c->CR2 = (ptr_i2c->i2c->CR2 & ~I2C_CR2_ITBUFEN) | I2C_CR2_DMAEN;
  }
  else {
    /* Interrupt mode data transfer */
    ptr_i2c->i2c->CR2 = (ptr_i2c->i2c->CR2 & ~I2C_CR2_DMAEN) | I2C_CR2_ITBUFEN;
  }

  if (xfer_pending == true) {
    ptr_i2c->info->flags = FLAG_NO_STOP_COND;
  }

  /* Generate start condition */
  ptr_i2c->i2c->CR1 |= I2C_CR1_START;
  /* Enable interrupts */
  ptr_i2c->i2c->CR2 |= I2C_CR2_ITEVTEN;
  osSemaphoreWait (ptr_i2c->info->semaphore, osWaitForever);
  
  /* Set transmitted size */
  if (ptr_i2c->dma_tx) {
    ret_val = size - DMA_StreamTransferItemCount(ptr_i2c->dma_tx->ptr_stream);
  }
  else {
    ret_val = size - ptr_i2c->info->size;
  }

  /* Check for error conditions */
  if (ptr_i2c->info->error) {
    if (ptr_i2c->info->error & I2C_SR1_BERR) {
      /* Bus error (misplaced start, stop) */
      ret_val = -ARM_I2C_ERROR_BUS_BUSY;
    }
    else if (ptr_i2c->info->error & I2C_SR1_AF) {
      /* Acknowledge failure */
      if (!(ptr_i2c->info->flags & FLAG_ADDR_DONE)) {
        /* Slave does not respond */
        ret_val = -ARM_I2C_ERROR_NO_SLAVE;
        /* Generate stop condition */
        xfer_pending = false;
      }
    }
    else ret_val = -ARM_I2C_ERROR;
  }

  if (xfer_pending == false) {
    /* Generate stop condition */
    ptr_i2c->i2c->CR1 |= I2C_CR1_STOP;
    /* Wait until stop condition detected */
    tick = osKernelSysTick();
    do {
      if (!(ptr_i2c->i2c->SR2 & I2C_SR2_BUSY)) {
        /* No communication on the bus */
        break;
      }
    } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(I2C_BUSY_TIMEOUT));
    if (ptr_i2c->i2c->SR1 & I2C_SR2_BUSY) {
      ret_val = -ARM_I2C_ERROR_BUS_BUSY;
    }
  }
  osMutexRelease (ptr_i2c->info->mutex);
  return ret_val;
}

#if (RTE_I2C1)
static int32_t I2C1_SendData (uint32_t       addr,
                              const uint8_t *data,
                              uint32_t       size,
                              bool           rx_pending) {
  return I2C_SendData(addr, data, size, rx_pending, ptr_I2C[0]);
}
#endif

#if (RTE_I2C2)
static int32_t I2C2_SendData (uint32_t       addr,
                              const uint8_t *data,
                              uint32_t       size,
                              bool           rx_pending) {
  return I2C_SendData(addr, data, size, rx_pending, ptr_I2C[1]);
}
#endif

#if (RTE_I2C3)
static int32_t I2C3_SendData (uint32_t       addr,
                              const uint8_t *data,
                              uint32_t       size,
                              bool           rx_pending) {
  return I2C_SendData(addr, data, size, rx_pending, ptr_I2C[2]);
}
#endif


/**
  \fn          int32_t I2C_ReceiveData (uint32_t       addr,
                                        uint8_t       *data,
                                        uint32_t       size,
                                        bool           tx_pending,
                                        I2C_RESOURCES *ptr_i2c)
  \brief       Receive data from I2C Slave.
  \param[in]   addr         Slave address (7-bit or 10-bit)
  \param[out]  data         Pointer to buffer for data received from I2C slave
  \param[in]   size         Data buffer size in bytes
  \param[in]   xfer_pending Transfer operation is pending - Stop condition will not be generated
  \param[in]   ptr_i2c      Pointer to I2C resources
  \return      number of data bytes received, error code if negative
*/
static int32_t I2C_ReceiveData (uint32_t       addr,
                                uint8_t       *data,
                                uint32_t       size,
                                bool           xfer_pending,
                                I2C_RESOURCES *ptr_i2c) {
  uint32_t i, tick;
  uint32_t s_ctrl;
  int32_t  ret_val;

  osMutexWait (ptr_i2c->info->mutex, osWaitForever);

  ptr_i2c->info->error = 0;
  ptr_i2c->info->addr  = addr;
  ptr_i2c->info->data  = data;
  ptr_i2c->info->size  = size;
  ptr_i2c->info->flags = FLAG_TRAN_DIR_RX;

  if (ptr_i2c->dma_rx) {
    /* DMA mode data transfer */
    s_ctrl = (ptr_i2c->dma_rx->channel << DMA_CHANNEL_POS)  |
             DMA_MEMORY_SINGLE_TRANSFER                     |
             DMA_PERIPHERAL_SINGLE_TRANSFER                 |
             (ptr_i2c->dma_rx->priority << DMA_PRIORITY_POS)|
             DMA_MEMORY_DATA_8BIT                           |
             DMA_PERIPHERAL_DATA_8BIT                       |
             DMA_MEMORY_INCREMENT                           |
             DMA_PERIPHERAL_TO_MEMORY                       |
             DMA_TRANSFER_COMPLETE_INTERRUPT;

    DMA_StreamDisable (ptr_i2c->dma_tx->ptr_stream);
    
    for (i = DMA_STREAM_DISABLE_TOUT; i; i++) {
      if (!DMA_StreamStatus (ptr_i2c->dma_tx->ptr_stream)) break;
    }
    if (DMA_StreamStatus (ptr_i2c->dma_tx->ptr_stream) == true) {
      return (-ARM_I2C_ERROR);
    }

    DMA_StreamConfigure (ptr_i2c->dma_rx->ptr_stream,
                         s_ctrl,
                         DMA_FIFO_DIRECT_MODE_ENABLE,
                         (uint32_t)&(ptr_i2c->i2c->DR),
                         (uint32_t)data,
                         size);

    DMA_StreamEnable (ptr_i2c->dma_rx->ptr_stream);
    ptr_i2c->i2c->CR2 = (ptr_i2c->i2c->CR2 & ~I2C_CR2_ITBUFEN) | I2C_CR2_DMAEN |
                                                                 I2C_CR2_LAST  ;
  }
  else {
    /* Interrupt mode data transfer */
    ptr_i2c->i2c->CR2 = (ptr_i2c->i2c->CR2 & ~I2C_CR2_DMAEN) | I2C_CR2_ITBUFEN;
  }

  if (xfer_pending == true) {
    ptr_i2c->info->flags |= FLAG_NO_STOP_COND;
  }
  
  /* Generate start condition and enable acknowledge generation */
  ptr_i2c->i2c->CR1 |= I2C_CR1_START | I2C_CR1_ACK;
  /* Enable event interrupts */
  ptr_i2c->i2c->CR2 |= I2C_CR2_ITEVTEN;
  osSemaphoreWait (ptr_i2c->info->semaphore, osWaitForever);
  
  /* Set received size */
  if (ptr_i2c->dma_rx) {
    ret_val = size - DMA_StreamTransferItemCount (ptr_i2c->dma_rx->ptr_stream);
  }
  else {
    ret_val = size - ptr_i2c->info->size;
  }
  
  /* Check for error conditions */
  if (ptr_i2c->info->error) {
    if (ptr_i2c->info->error & I2C_SR1_BERR) {
      /* Bus error (misplaced start, stop) */
      ret_val = -ARM_I2C_ERROR_BUS_BUSY;
    }
    else if (ptr_i2c->info->error & I2C_SR1_AF) {
      /* Acknowledge failure */
      if (!(ptr_i2c->info->flags & FLAG_ADDR_DONE)) {
        /* Slave does not respond */
        ret_val = -ARM_I2C_ERROR_NO_SLAVE;
        /* Generate stop condition */
        xfer_pending = false;
      }
    }
    else ret_val = -ARM_I2C_ERROR;
  }
  
  if (xfer_pending == false) {
    /* Generate stop condition */
    ptr_i2c->i2c->CR1 |= I2C_CR1_STOP;
    /* Wait until stop condition detected */
    tick = osKernelSysTick();
    do {
      if (!(ptr_i2c->i2c->SR2 & I2C_SR2_BUSY)) {
        /* No communication on the bus */
        break;
      }
    } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(I2C_BUSY_TIMEOUT));
    if (ptr_i2c->i2c->SR2 & I2C_SR2_BUSY) {
      ret_val = -ARM_I2C_ERROR_BUS_BUSY;
    }
  }
  osMutexRelease (ptr_i2c->info->mutex);
  return ret_val;
}

#if (RTE_I2C1)
static int32_t I2C1_ReceiveData (uint32_t       addr,
                                 uint8_t       *data,
                                 uint32_t       size,
                                 bool           tx_pending) {
  return I2C_ReceiveData(addr, data, size, tx_pending, ptr_I2C[0]);
}
#endif

#if (RTE_I2C2)
static int32_t I2C2_ReceiveData (uint32_t       addr,
                                 uint8_t       *data,
                                 uint32_t       size,
                                 bool           tx_pending) {
  return I2C_ReceiveData(addr, data, size, tx_pending, ptr_I2C[1]);
}
#endif

#if (RTE_I2C3)
static int32_t I2C3_ReceiveData (uint32_t       addr,
                                 uint8_t       *data,
                                 uint32_t       size,
                                 bool           tx_pending) {
  return I2C_ReceiveData(addr, data, size, tx_pending, ptr_I2C[2]);
}
#endif


/**
  \fn          ARM_I2C_STATUS I2C_AbortTransfer (I2C_RESOURCES *ptr_i2C)
  \brief       Abort current I2C transfer.
  \param[in]   ptr_i2c  Pointer to I2C resources
  \return      \ref ARM_I2C_STATUS
*/
static ARM_I2C_STATUS I2C_AbortTransfer (I2C_RESOURCES *ptr_i2c) {

  if (!ptr_i2c->info->power) return ARM_I2C_ERROR;
  
  /* Disable DMA requests and I2C interrupts */
  ptr_i2c->i2c->CR2 &= (I2C_CR2_DMAEN | I2C_CR2_ITBUFEN);

  /* Disable DMA Streams */
  if (ptr_i2c->dma_tx) {
    DMA_StreamDisable(ptr_i2c->dma_tx->ptr_stream);
  }
  if (ptr_i2c->dma_rx) {
    DMA_StreamDisable(ptr_i2c->dma_rx->ptr_stream);
  }

  /* Release the semaphore */
  if (osSemaphoreRelease (ptr_i2c->info->semaphore) != osOK) {
    return ARM_I2C_ERROR;
  }
  /* Release mutex */
  if (osMutexRelease (ptr_i2c->info->mutex) != osOK) {
    return ARM_I2C_ERROR;
  }

  return ARM_I2C_OK;
}

#if (RTE_I2C1)
static ARM_I2C_STATUS I2C1_AbortTransfer (void) {
  return I2C_AbortTransfer(ptr_I2C[0]);
}
#endif

#if (RTE_I2C2)
static ARM_I2C_STATUS I2C2_AbortTransfer (void) {
  return I2C_AbortTransfer(ptr_I2C[1]);
}
#endif

#if (RTE_I2C3)
static ARM_I2C_STATUS I2C3_AbortTransfer (void) {
  return I2C_AbortTransfer(ptr_I2C[2]);
}
#endif


/**
  \fn          void I2C_EV_IRQHandler (I2C_RESOURCES *ptr_i2c)
  \brief       I2C Event Interrupt handler.
  \param[in]   ptr_i2c  Pointer to I2C resources
*/

static void I2C_EV_IRQHandler (I2C_RESOURCES *ptr_i2c) {
  uint32_t addr;
  uint32_t status = ptr_i2c->i2c->SR1;

  if (status & I2C_SR1_SB) {
    /* (EV5) - start bit was generated, send address */
    addr = ptr_i2c->info->addr;

    if (ptr_i2c->info->flags & FLAG_ADDR_RSTART) {
      /* 10-bit master receiver, repeated start - send address header */
      ptr_i2c->i2c->DR = 0xF0 | ((addr >> 7) & 0x06) | 1;
      ptr_i2c->info->flags |= FLAG_ADDR_SENT;
    }
    else {
      if (addr & ARM_I2C_ADDRESS_10BIT) {
        /* 10-bit addressing mode */
        ptr_i2c->i2c->DR = 0xF0 | ((addr >> 7) & 0x06);
      }
      else {
        /* 7-bit addressing mode */
        ptr_i2c->i2c->DR = ((addr & 0x7F) << 1) | ((ptr_i2c->info->flags & FLAG_TRAN_DIR_RX) != 0);
        ptr_i2c->info->flags |= FLAG_ADDR_SENT;
      }
    }
  }
  else if (status & I2C_SR1_ADD10) {
    /* (EV9) - 10-bit address header sent, send LSB byte of the device address */
    ptr_i2c->i2c->DR = (ptr_i2c->info->addr & 0xFF);
    
    if (!(ptr_i2c->info->flags & FLAG_TRAN_DIR_RX)) {
      /* Transmit mode - 10-bit address sent */
      ptr_i2c->info->flags |= FLAG_ADDR_SENT;
    }
  }
  else if (status & I2C_SR1_ADDR) {
    /* (EV6) - addressing complete */
    if (ptr_i2c->info->flags & FLAG_ADDR_SENT) {
      if (ptr_i2c->info->size == 1) {
        /* Clear ACK flag */
        ptr_i2c->i2c->CR1 &= ~I2C_CR1_ACK;
        
      }
    }

    /* Clear ADDR flag */
    ptr_i2c->i2c->SR1;
    ptr_i2c->i2c->SR2;

    if (ptr_i2c->info->flags & FLAG_ADDR_SENT) {
      /* Address was sent and acked - addressing done */
      ptr_i2c->info->flags |= FLAG_ADDR_DONE;
    }
    else {
      if (ptr_i2c->info->flags & FLAG_TRAN_DIR_RX) {
        /* Receive mode */
        if (!(ptr_i2c->info->flags & FLAG_ADDR_RSTART)) {
          if (ptr_i2c->info->addr & ARM_I2C_ADDRESS_10BIT) {
            /* 10-bit master receiver - generate repeated start */
            ptr_i2c->i2c->CR1    |= I2C_CR1_START;
            ptr_i2c->info->flags |= FLAG_ADDR_RSTART;
          }
        }
      }
    }
  }
  else {
    /* Data transfer */
    if (ptr_i2c->info->flags & FLAG_TRAN_DIR_RX) {
      /* Receiving data */
      if (status & I2C_SR1_BTF) {
      }
      else if (status & I2C_SR1_RXNE) {
        /* (EV7) - Data register not empty (receivers) */
        if (ptr_i2c->info->size) {
          *ptr_i2c->info->data++ = ptr_i2c->i2c->DR;
          ptr_i2c->info->size--;
        }

        if (ptr_i2c->info->size == 1) {
          /* Disable acknowledge */
          ptr_i2c->i2c->CR1 &= ~I2C_CR1_ACK;
        }
        else if (ptr_i2c->info->size == 0) {
          /* End of transfer */
          ptr_i2c->i2c->CR2 &= ~I2C_CR2_ITEVTEN;
          /* Release the semaphore */
          osSemaphoreRelease (ptr_i2c->info->semaphore);
        }
      }
    }
    else {
      /* Transmitting data */
      if (status & I2C_SR1_BTF) {
        /* End of transmitt transfer */
        ptr_i2c->i2c->CR2 &= ~I2C_CR2_ITEVTEN;
        /* Release the semaphore */
        osSemaphoreRelease (ptr_i2c->info->semaphore);
      }
      else if (status & I2C_SR1_TXE) {
        /* (EV8) - Data register empty (transmitters) */
        if (ptr_i2c->info->size) {
          ptr_i2c->i2c->DR = *ptr_i2c->info->data++;
          ptr_i2c->info->size--;
        }
      }
    }
  }
}

#if (RTE_I2C1)
void I2C1_EV_IRQHandler (void) {
  I2C_EV_IRQHandler(ptr_I2C[0]);
}
#endif

#if (RTE_I2C2)
void I2C2_EV_IRQHandler (void) {
  I2C_EV_IRQHandler(ptr_I2C[1]);
}
#endif

#if (RTE_I2C3)
void I2C3_EV_IRQHandler (void) {
  I2C_EV_IRQHandler(ptr_I2C[2]);
}
#endif


/**
  \fn          void I2C_ER_IRQHandler (I2C_RESOURCES *ptr_i2c)
  \brief       I2C Error Interrupt handler.
  \param[in]   ptr_i2c  Pointer to I2C resources
*/
static void I2C_ER_IRQHandler (I2C_RESOURCES *ptr_i2c) {
  uint32_t status = ptr_i2c->i2c->SR1;
  uint16_t err    = 0;
  
  if (status & I2C_SR1_SMBALERT) {
    /* SMBus alert */
    err |= I2C_SR1_SMBALERT;
  }
  if (status & I2C_SR1_TIMEOUT) {
    /* Timeout - SCL remained LOW for 25ms */
    err |= I2C_SR1_TIMEOUT;
  }
  if (status & I2C_SR1_PECERR) {
    /* PEC Error in reception */
    err |= I2C_SR1_PECERR;
  }
  if (status & I2C_SR1_OVR) {
    /* Overrun/Underrun */
    err |= I2C_SR1_OVR;
  }
  if (status & I2C_SR1_AF) {
    /* Acknowledge failure */
    err |= I2C_SR1_AF;
  }
  if (status & I2C_SR1_ARLO) {
    /* Arbitration lost */
    err |= I2C_SR1_ARLO;
  }
  if (status & I2C_SR1_BERR) {
    /* Bus error - misplaced start/stop */
    err |= I2C_SR1_BERR;
  }
  /* Save and clear error flags */
  ptr_i2c->info->error |=  err;
  ptr_i2c->i2c->SR1    &= ~err;
  
  /* Release the semaphore */
  osSemaphoreRelease (ptr_i2c->info->semaphore);
}

#if (RTE_I2C1)
void I2C1_ER_IRQHandler (void) {
  I2C_ER_IRQHandler(ptr_I2C[0]);
}
#endif

#if (RTE_I2C2)
void I2C2_ER_IRQHandler (void) {
  I2C_ER_IRQHandler(ptr_I2C[1]);
}
#endif

#if (RTE_I2C3)
void I2C3_ER_IRQHandler (void) {
  I2C_ER_IRQHandler(ptr_I2C[2]);
}
#endif


/* DMA Stream Event */
#if (RTE_I2C1)
#if (RTE_I2C1_TX_DMA)
void DMAx_STREAMy_EVENT (RTE_I2C1_TX_DMA_NUMBER, RTE_I2C1_TX_DMA_STREAM) (uint32_t event) {
  ptr_I2C[0]->i2c->CR2 &= ~I2C_CR2_DMAEN;
  /* BTF event will be generated after EOT - BTF will release the semaphore */
}
#endif
#if (RTE_I2C1_RX_DMA)
void DMAx_STREAMy_EVENT (RTE_I2C1_RX_DMA_NUMBER, RTE_I2C1_RX_DMA_STREAM) (uint32_t event) {
  ptr_I2C[0]->i2c->CR2 &= ~I2C_CR2_DMAEN;
  /* No I2C events after this point - release the semaphore */
  osSemaphoreRelease (ptr_I2C[0]->info->semaphore);
}
#endif
#endif

#if (RTE_I2C2)
#if (RTE_I2C2_TX_DMA)
void DMAx_STREAMy_EVENT (RTE_I2C2_TX_DMA_NUMBER, RTE_I2C2_TX_DMA_STREAM) (uint32_t event) {
  ptr_I2C[1]->i2c->CR2 &= ~I2C_CR2_DMAEN;
  /* BTF event will be generated after EOT - BTF will release the semaphore */
}
#endif
#if (RTE_I2C2_RX_DMA)
void DMAx_STREAMy_EVENT (RTE_I2C2_RX_DMA_NUMBER, RTE_I2C2_RX_DMA_STREAM) (uint32_t event) {
  ptr_I2C[1]->i2c->CR2 &= ~I2C_CR2_DMAEN;
  /* No I2C events after this point - release the semaphore */
  osSemaphoreRelease (ptr_I2C[1]->info->semaphore);
}
#endif
#endif

#if (RTE_I2C3)
#if (RTE_I2C3_TX_DMA)
void DMAx_STREAMy_EVENT (RTE_I2C3_TX_DMA_NUMBER, RTE_I2C3_TX_DMA_STREAM) (uint32_t event) {
  ptr_I2C[1]->i2c->CR2 &= ~I2C_CR2_DMAEN;
  /* BTF event will be generated after EOT - BTF will release the semaphore */
}
#endif
#if (RTE_I2C3_RX_DMA)
void DMAx_STREAMy_EVENT (RTE_I2C3_RX_DMA_NUMBER, RTE_I2C3_RX_DMA_STREAM) (uint32_t event) {
  ptr_I2C[2]->i2c->CR2 &= ~I2C_CR2_DMAEN;
  /* No I2C events after this point - release the semaphore */
  osSemaphoreRelease (ptr_I2C[2]->info->semaphore);
}
#endif
#endif


/* I2C1 Driver Control Block */
#if (RTE_I2C1)
ARM_DRIVER_I2C Driver_I2C1 = {
  I2CX_GetVersion,
  I2CX_GetCapabilities,
  I2C1_Initialize,
  I2C1_Uninitialize,
  I2C1_PowerControl,
  I2C1_BusSpeed,
  I2C1_BusClear,
  I2C1_SendData,
  I2C1_ReceiveData,
  I2C1_AbortTransfer
};
#endif

/* I2C2 Driver Control Block */
#if (RTE_I2C2)
ARM_DRIVER_I2C Driver_I2C2 = {
  I2CX_GetVersion,
  I2CX_GetCapabilities,
  I2C2_Initialize,
  I2C2_Uninitialize,
  I2C2_PowerControl,
  I2C2_BusSpeed,
  I2C2_BusClear,
  I2C2_SendData,
  I2C2_ReceiveData,
  I2C2_AbortTransfer
};
#endif

/* I2C3 Driver Control Block */
#if (RTE_I2C3)
ARM_DRIVER_I2C Driver_I2C3 = {
  I2CX_GetVersion,
  I2CX_GetCapabilities,
  I2C3_Initialize,
  I2C3_Uninitialize,
  I2C3_PowerControl,
  I2C3_BusSpeed,
  I2C3_BusClear,
  I2C3_SendData,
  I2C3_ReceiveData,
  I2C3_AbortTransfer
};
#endif
