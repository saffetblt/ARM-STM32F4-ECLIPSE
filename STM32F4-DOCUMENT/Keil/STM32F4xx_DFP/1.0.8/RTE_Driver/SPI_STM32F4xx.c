/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        11. December 2013
 * $Revision:    V1.04
 *  
 * Driver:       Driver_SPI1, Driver_SPI2, Driver_SPI3
 * Configured:   via RTE_Device.h configuration file 
 * Project:      SPI Driver for ST STM32F4xx
 * ---------------------------------------------------------------------- 
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 * 
 *   Configuration Setting               Value     SPI Interface
 *   ---------------------               -----     -------------
 *   Connect to hardware via Driver_SPI# = 1       use SPI1
 *   Connect to hardware via Driver_SPI# = 2       use SPI2
 *   Connect to hardware via Driver_SPI# = 3       use SPI3
 *   Connect to hardware via Driver_SPI# = 5       use SPI5
 * -------------------------------------------------------------------- */

/* History:
 *  Version 1.04
 *    Added SPI5
 *  Version 1.03
 *    Event send_data_event added to capabilities
 *    SPI IRQ handling corrected
 *  Version 1.02
 *    Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.01
 *    Corrections for configuration without DMA
 *  Version 1.00
 *    Initial release
 */ 

#include "cmsis_os.h"
#include "stm32f4xx.h"

#include "GPIO_STM32F4xx.h"
#include "DMA_STM32F4xx.h"

#include "Driver_SPI.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#define ARM_SPI_DRV_VERSION ARM_DRV_VERSION_MAJOR_MINOR(1,03)   /* driver version */

#if ((defined(RTE_Drivers_SPI1) || \
      defined(RTE_Drivers_SPI2) || \
      defined(RTE_Drivers_SPI3) || \
      defined(RTE_Drivers_SPI5))   \
     && !RTE_SPI1                  \
     && !RTE_SPI2                  \
     && !RTE_SPI3                  \
     && !RTE_SPI5)
#error "SPI not configured in RTE_Device.h!"
#endif

#if (RTE_SPI1)
#if (  (RTE_SPI1_RX_DMA          != 0)                                     && \
     ( (RTE_SPI1_RX_DMA_NUMBER   != 2)                                     || \
      ((RTE_SPI1_RX_DMA_STREAM   != 0) && (RTE_SPI1_RX_DMA_STREAM  != 2))  || \
       (RTE_SPI1_RX_DMA_CHANNEL  != 3)                                     || \
      ((RTE_SPI1_RX_DMA_PRIORITY  < 0) || (RTE_SPI1_RX_DMA_PRIORITY > 3))))
#error "SPI1 Rx DMA configuration in RTE_Device.h is invalid!"
#endif
#if (  (RTE_SPI1_TX_DMA          != 0)                                     && \
     ( (RTE_SPI1_TX_DMA_NUMBER   != 2)                                     || \
      ((RTE_SPI1_TX_DMA_STREAM   != 3) && (RTE_SPI1_TX_DMA_STREAM  != 5))  || \
       (RTE_SPI1_TX_DMA_CHANNEL  != 3)                                     || \
      ((RTE_SPI1_TX_DMA_PRIORITY  < 0) || (RTE_SPI1_TX_DMA_PRIORITY > 3))))
#error "SPI1 Tx DMA configuration in RTE_Device.h is invalid!"
#endif
#endif

#if (RTE_SPI2)
#if (  (RTE_SPI2_RX_DMA          != 0)                                     && \
     ( (RTE_SPI2_RX_DMA_NUMBER   != 1)                                     || \
       (RTE_SPI2_RX_DMA_STREAM   != 2)                                     || \
       (RTE_SPI2_RX_DMA_CHANNEL  != 0)                                     || \
      ((RTE_SPI2_RX_DMA_PRIORITY  < 0) || (RTE_SPI2_RX_DMA_PRIORITY > 3))))
#error "SPI2 Rx DMA configuration in RTE_Device.h is invalid!"
#endif
#if (  (RTE_SPI2_TX_DMA          != 0)                                     && \
     ( (RTE_SPI2_TX_DMA_NUMBER   != 1)                                     || \
       (RTE_SPI2_TX_DMA_STREAM   != 3)                                     || \
       (RTE_SPI2_TX_DMA_CHANNEL  != 0)                                     || \
      ((RTE_SPI2_TX_DMA_PRIORITY  < 0) || (RTE_SPI2_TX_DMA_PRIORITY > 3))))
#error "SPI2 Tx DMA configuration in RTE_Device.h is invalid!"
#endif
#endif

#if (RTE_SPI3)
#if (  (RTE_SPI3_RX_DMA          != 0)                                     && \
     ( (RTE_SPI3_RX_DMA_NUMBER   != 1)                                     || \
      ((RTE_SPI3_RX_DMA_STREAM   != 0) && (RTE_SPI3_RX_DMA_STREAM  != 2))  || \
       (RTE_SPI3_RX_DMA_CHANNEL  != 0)                                     || \
      ((RTE_SPI3_RX_DMA_PRIORITY  < 0) || (RTE_SPI3_RX_DMA_PRIORITY > 3))))
#error "SPI3 Rx DMA configuration in RTE_Device.h is invalid!"
#endif
#if (  (RTE_SPI3_TX_DMA          != 0)                                     && \
     ( (RTE_SPI3_TX_DMA_NUMBER   != 1)                                     || \
      ((RTE_SPI3_TX_DMA_STREAM   != 5) && (RTE_SPI3_TX_DMA_STREAM  != 7))  || \
       (RTE_SPI3_TX_DMA_CHANNEL  != 0)                                     || \
      ((RTE_SPI3_TX_DMA_PRIORITY  < 0) || (RTE_SPI3_TX_DMA_PRIORITY > 3))))
#error "SPI3 Tx DMA configuration in RTE_Device.h is invalid!"
#endif
#endif

#if (RTE_SPI5)
#if (  (RTE_SPI5_RX_DMA          != 0)                                     && \
     ( (RTE_SPI5_RX_DMA_NUMBER   != 2)                                     || \
      ((RTE_SPI5_RX_DMA_STREAM   != 3) && (RTE_SPI5_RX_DMA_CHANNEL != 2))  || \
      ((RTE_SPI5_RX_DMA_STREAM   != 5) && (RTE_SPI5_RX_DMA_CHANNEL != 7))  || \
      ((RTE_SPI5_RX_DMA_PRIORITY  < 0) || (RTE_SPI5_RX_DMA_PRIORITY > 3))))
#error "SPI5 Rx DMA configuration in RTE_Device.h is invalid!"
#endif
#if (  (RTE_SPI5_TX_DMA          != 0)                                     && \
     ( (RTE_SPI5_TX_DMA_NUMBER   != 2)                                     || \
      ((RTE_SPI5_TX_DMA_STREAM   != 4) && (RTE_SPI5_TX_DMA_CHANNEL != 2))  || \
      ((RTE_SPI5_TX_DMA_STREAM   != 6) && (RTE_SPI5_TX_DMA_CHANNEL != 7))  || \
      ((RTE_SPI5_TX_DMA_PRIORITY  < 0) || (RTE_SPI5_TX_DMA_PRIORITY > 3))))
#error "SPI5 Tx DMA configuration in RTE_Device.h is invalid!"
#endif
#endif


/* Driver Version */
static const ARM_DRV_VERSION DriverVersion = {
  ARM_SPI_API_VERSION,
  ARM_SPI_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_SPI_CAPABILITIES DriverCapabilities = {
  1,  /* send_data_event */
  0
};


/* SPI Data Information (Run-Time) */
typedef struct {
  uint8_t                *buf;                  // Pointer to data buffer
  uint32_t                cnt;                  // Number of data bytes
  bool                    inc;                  // Buffer increment flag
} SPI_DATA_INFO;

/* SPI Information (Run-Time) */
typedef struct {
  bool                    power;                // Power flag
  osSemaphoreId           semaphore_xfer;       // Transfer Semaphore
  osSemaphoreId           semaphore_busy;       // Busy Semaphore
  ARM_SPI_SignalEvent_t   cb_event;             // Event Callback
  SPI_DATA_INFO           rx;                   // Receive information
  SPI_DATA_INFO           tx;                   // Transmit information
} SPI_INFO;


/* SPI Resources definition */
typedef struct {
        GPIO_PIN_ID       pin_nss;              // NSS  Pin identifier
        GPIO_PIN_ID       pin_scl;              // SCL  Pin identifier
        GPIO_PIN_ID       pin_miso;             // MISO Pin identifier
        GPIO_PIN_ID       pin_mosi;             // MISO Pin identifier
        GPIO_AF_ID        pin_af;               // Pin AF identifier
        SPI_TypeDef      *spi;                  // Pointer to SPI peripheral
  enum  IRQn              spi_irq;              // SPI IRQ Number
  __IO  uint32_t         *clock_reg;            // Pointer to Clock register
        uint32_t          clock_mask;           // Clock register mask
  __IO  uint32_t         *reset_reg;            // Pointer to Reset register
        uint32_t          reset_mask;           // Reset register mask
        uint32_t          clock;                // Peripheral clock
  const DMA_INFO         *dma_rx;               // SPI Receive DMA configuration
  const DMA_INFO         *dma_tx;               // SPI Transmit DMA configuration
  const osSemaphoreDef_t *semaphore_xfer;       // Transfer Semaphore definition
  const osSemaphoreDef_t *semaphore_busy;       // Busy Semaphore definition
        SPI_INFO         *info;                 // Run-Time information
} const SPI_RESOURCES;


#if (RTE_SPI1)

/* SPI1 DMA */
#if (RTE_SPI1_RX_DMA)
static const DMA_INFO SPI1_DMA_RX = {
  DMAx_STREAMy(RTE_SPI1_RX_DMA_NUMBER, RTE_SPI1_RX_DMA_STREAM),
  RTE_SPI1_RX_DMA_NUMBER,
  RTE_SPI1_RX_DMA_STREAM,
  RTE_SPI1_RX_DMA_CHANNEL,
  RTE_SPI1_RX_DMA_PRIORITY
};
#endif
#if (RTE_SPI1_TX_DMA)
static const DMA_INFO SPI1_DMA_TX = {
  DMAx_STREAMy(RTE_SPI1_TX_DMA_NUMBER, RTE_SPI1_TX_DMA_STREAM),
  RTE_SPI1_TX_DMA_NUMBER,
  RTE_SPI1_TX_DMA_STREAM,
  RTE_SPI1_TX_DMA_CHANNEL,
  RTE_SPI1_TX_DMA_PRIORITY
};
#endif

/* SPI1 Semaphores */
osSemaphoreDef(SPI1_Semaphore_Xfer);
osSemaphoreDef(SPI1_Semaphore_Busy);
osSemaphoreId  SPI1_Semaphore_Xfer;
osSemaphoreId  SPI1_Semaphore_Busy;

/* SPI1 Information (Run-Time) */
static SPI_INFO SPI1_Info;

/* SPI1 Resources */
static SPI_RESOURCES SPI1_Resources = {
#if (RTE_SPI1_NSS_PIN)
  { RTE_SPI1_NSS_PORT,  RTE_SPI1_NSS_BIT  },
#else
  { NULL },
#endif
  { RTE_SPI1_SCL_PORT,  RTE_SPI1_SCL_BIT  },
  { RTE_SPI1_MISO_PORT, RTE_SPI1_MISO_BIT },
  { RTE_SPI1_MOSI_PORT, RTE_SPI1_MOSI_BIT },
  GPIO_AF_SPI1,
  SPI1,
  SPI1_IRQn,
  &RCC->APB2ENR,
  RCC_APB2ENR_SPI1EN,
  &RCC->APB2RSTR,
  RCC_APB2RSTR_SPI1RST,
  RTE_PCLK2,
#if (RTE_SPI1_RX_DMA)
  &SPI1_DMA_RX,
#else
  NULL,
#endif
#if (RTE_SPI1_TX_DMA)
  &SPI1_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(SPI1_Semaphore_Xfer),
  osSemaphore(SPI1_Semaphore_Busy),
  &SPI1_Info
};

#endif /* RTE_SPI1 */


#if (RTE_SPI2)

/* SPI 2 DMA */
#if (RTE_SPI2_RX_DMA)
static const DMA_INFO SPI2_DMA_RX = {
  DMAx_STREAMy(RTE_SPI2_RX_DMA_NUMBER, RTE_SPI2_RX_DMA_STREAM),
  RTE_SPI2_RX_DMA_NUMBER,
  RTE_SPI2_RX_DMA_STREAM,
  RTE_SPI2_RX_DMA_CHANNEL,
  RTE_SPI2_RX_DMA_PRIORITY
};
#endif
#if (RTE_SPI2_TX_DMA)
static const DMA_INFO SPI2_DMA_TX = {
  DMAx_STREAMy(RTE_SPI2_TX_DMA_NUMBER, RTE_SPI2_TX_DMA_STREAM),
  RTE_SPI2_TX_DMA_NUMBER,
  RTE_SPI2_TX_DMA_STREAM,
  RTE_SPI2_TX_DMA_CHANNEL,
  RTE_SPI2_TX_DMA_PRIORITY
};
#endif

/* SPI2 Semaphores */
osSemaphoreDef(SPI2_Semaphore_Xfer);
osSemaphoreDef(SPI2_Semaphore_Busy);
osSemaphoreId  SPI2_Semaphore_Xfer;
osSemaphoreId  SPI2_Semaphore_Busy;

/* SPI2 Information (Run-Time) */
static SPI_INFO SPI2_Info;

/* SPI2 Resources */
static SPI_RESOURCES SPI2_Resources = {
#if (RTE_SPI2_NSS_PIN)
  { RTE_SPI2_NSS_PORT,  RTE_SPI2_NSS_BIT  },
#else
  { NULL },
#endif
  { RTE_SPI2_SCL_PORT,  RTE_SPI2_SCL_BIT  },
  { RTE_SPI2_MISO_PORT, RTE_SPI2_MISO_BIT },
  { RTE_SPI2_MOSI_PORT, RTE_SPI2_MOSI_BIT },
  GPIO_AF_SPI2,
  SPI2,
  SPI2_IRQn,
  &RCC->APB1ENR,
  RCC_APB1ENR_SPI2EN,
  &RCC->APB1RSTR,
  RCC_APB1RSTR_SPI2RST,
  RTE_PCLK1,
#if (RTE_SPI2_RX_DMA)
  &SPI2_DMA_RX,
#else
  NULL,
#endif
#if (RTE_SPI2_TX_DMA)
  &SPI2_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(SPI2_Semaphore_Xfer),
  osSemaphore(SPI2_Semaphore_Busy),
  &SPI2_Info
};

#endif /* RTE_SPI2 */


#if (RTE_SPI3)

/* SPI DMA */
#if (RTE_SPI3_RX_DMA)
static const DMA_INFO SPI3_DMA_RX = {
  DMAx_STREAMy(RTE_SPI3_RX_DMA_NUMBER, RTE_SPI3_RX_DMA_STREAM),
  RTE_SPI3_RX_DMA_NUMBER,
  RTE_SPI3_RX_DMA_STREAM,
  RTE_SPI3_RX_DMA_CHANNEL,
  RTE_SPI3_RX_DMA_PRIORITY
};
#endif
#if (RTE_SPI3_TX_DMA)
static const DMA_INFO SPI3_DMA_TX = {
  DMAx_STREAMy(RTE_SPI3_TX_DMA_NUMBER, RTE_SPI3_TX_DMA_STREAM),
  RTE_SPI3_TX_DMA_NUMBER,
  RTE_SPI3_TX_DMA_STREAM,
  RTE_SPI3_TX_DMA_CHANNEL,
  RTE_SPI3_TX_DMA_PRIORITY
};
#endif

/* SPI3 Semaphores */
osSemaphoreDef(SPI3_Semaphore_Xfer);
osSemaphoreDef(SPI3_Semaphore_Busy);
osSemaphoreId  SPI3_Semaphore_Xfer;
osSemaphoreId  SPI3_Semaphore_Busy;

/* SPI3 Information (Run-Time) */
static SPI_INFO SPI3_Info;

/* SPI3 Resources */
static SPI_RESOURCES SPI3_Resources = {
#if (RTE_SPI3_NSS_PIN)
  { RTE_SPI3_NSS_PORT,  RTE_SPI3_NSS_BIT  },
#else
  { NULL },
#endif
  { RTE_SPI3_SCL_PORT,  RTE_SPI3_SCL_BIT  },
  { RTE_SPI3_MISO_PORT, RTE_SPI3_MISO_BIT },
  { RTE_SPI3_MOSI_PORT, RTE_SPI3_MOSI_BIT },
  GPIO_AF_SPI3,
  SPI3,
  SPI3_IRQn,
  &RCC->APB1ENR,
  RCC_APB1ENR_SPI3EN,
  &RCC->APB1RSTR,
  RCC_APB1RSTR_SPI3RST,
  RTE_PCLK1,
#if (RTE_SPI3_RX_DMA)
  &SPI3_DMA_RX,
#else
  NULL,
#endif
#if (RTE_SPI3_TX_DMA)
  &SPI3_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(SPI3_Semaphore_Xfer),
  osSemaphore(SPI3_Semaphore_Busy),
  &SPI3_Info
};

#endif /* RTE_SPI3 */


#if (RTE_SPI5)

/* SPI DMA */
#if (RTE_SPI5_RX_DMA)
static const DMA_INFO SPI5_DMA_RX = {
  DMAx_STREAMy(RTE_SPI5_RX_DMA_NUMBER, RTE_SPI5_RX_DMA_STREAM),
  RTE_SPI5_RX_DMA_NUMBER,
  RTE_SPI5_RX_DMA_STREAM,
  RTE_SPI5_RX_DMA_CHANNEL,
  RTE_SPI5_RX_DMA_PRIORITY
};
#endif
#if (RTE_SPI5_TX_DMA)
static const DMA_INFO SPI5_DMA_TX = {
  DMAx_STREAMy(RTE_SPI5_TX_DMA_NUMBER, RTE_SPI5_TX_DMA_STREAM),
  RTE_SPI5_TX_DMA_NUMBER,
  RTE_SPI5_TX_DMA_STREAM,
  RTE_SPI5_TX_DMA_CHANNEL,
  RTE_SPI5_TX_DMA_PRIORITY
};
#endif

/* SPI5 Semaphores */
osSemaphoreDef(SPI5_Semaphore_Xfer);
osSemaphoreDef(SPI5_Semaphore_Busy);
osSemaphoreId  SPI5_Semaphore_Xfer;
osSemaphoreId  SPI5_Semaphore_Busy;

/* SPI5 Information (Run-Time) */
static SPI_INFO SPI5_Info;

/* SPI5 Resources */
static SPI_RESOURCES SPI5_Resources = {
#if (RTE_SPI5_NSS_PIN)
  { RTE_SPI5_NSS_PORT,  RTE_SPI5_NSS_BIT  },
#else
  { NULL },
#endif
  { RTE_SPI5_SCL_PORT,  RTE_SPI5_SCL_BIT  },
  { RTE_SPI5_MISO_PORT, RTE_SPI5_MISO_BIT },
  { RTE_SPI5_MOSI_PORT, RTE_SPI5_MOSI_BIT },
  GPIO_AF_SPI5,
  SPI5,
  SPI5_IRQn,
  &RCC->APB2ENR,
  RCC_APB2ENR_SPI5EN,
  &RCC->APB2RSTR,
  RCC_APB2RSTR_SPI5RST,
  RTE_PCLK1,
#if (RTE_SPI5_RX_DMA)
  &SPI5_DMA_RX,
#else
  NULL,
#endif
#if (RTE_SPI5_TX_DMA)
  &SPI5_DMA_TX,
#else
  NULL,
#endif
  osSemaphore(SPI5_Semaphore_Xfer),
  osSemaphore(SPI5_Semaphore_Busy),
  &SPI5_Info
};

#endif /* RTE_SPI5 */


/**
  \fn          ARM_DRV_VERSION SPI_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRV_VERSION SPIX_GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          ARM_SPI_CAPABILITIES SPI_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_SPI_CAPABILITIES
*/
static ARM_SPI_CAPABILITIES SPIX_GetCapabilities (void) {
  return DriverCapabilities;
}


/**
  \fn          ARM_SPI_STATUS SPI_Initialize (ARM_SPI_SignalEvent_t cb_event,
                                              SPI_RESOURCES        *ptr_spi)
  \brief       Initialize SPI Interface.
  \param[in]   cb_event  Pointer to \ref ARM_SPI_SignalEvent
  \param[in]   ptr_spi   Pointer to SPI resources
  \return      \ref ARM_SPI_STATUS
*/
static ARM_SPI_STATUS SPI_Initialize (ARM_SPI_SignalEvent_t cb_event,
                                      SPI_RESOURCES        *ptr_spi) {
  bool status;

  // Enable SPI Clock
  *ptr_spi->clock_reg |=  ptr_spi->clock_mask;

  // Reset SPI Peripheral
  *ptr_spi->reset_reg |=  ptr_spi->reset_mask;
  __NOP(); __NOP(); __NOP(); __NOP(); 
  *ptr_spi->reset_reg &= ~ptr_spi->reset_mask;

  // Clear and Enable SPI IRQ
  NVIC_ClearPendingIRQ(ptr_spi->spi_irq);
  NVIC_EnableIRQ(ptr_spi->spi_irq);

  // Initialize DMA Streams
  if (ptr_spi->dma_rx) {
    DMA_StreamInitialize(ptr_spi->dma_rx->dma, ptr_spi->dma_rx->stream);
  }
  if (ptr_spi->dma_tx) {
    DMA_StreamInitialize(ptr_spi->dma_tx->dma, ptr_spi->dma_tx->stream);
  }

  // Initialize SPI Run-Time Resources
  ptr_spi->info->power = false;

  ptr_spi->info->semaphore_xfer = osSemaphoreCreate(ptr_spi->semaphore_xfer, 0);
  ptr_spi->info->semaphore_busy = osSemaphoreCreate(ptr_spi->semaphore_busy, 0);
  if (ptr_spi->info->semaphore_xfer == NULL) return ARM_SPI_ERROR;
  if (ptr_spi->info->semaphore_busy == NULL) return ARM_SPI_ERROR;

  ptr_spi->info->cb_event = cb_event;

  // Configure SPI NSS pin (GPIO), Output set to high
  if (ptr_spi->pin_nss.port != NULL) {
    GPIO_PortClock            (ptr_spi->pin_nss.port, true);
    GPIO_PinWrite             (ptr_spi->pin_nss.port,
                               ptr_spi->pin_nss.num,
                               1);
    status = GPIO_PinAF       (ptr_spi->pin_nss.port,
                               ptr_spi->pin_nss.num,
                               GPIO_AF_SYSTEM);
    if (!status) return ARM_SPI_ERROR;
    status = GPIO_PinConfigure(ptr_spi->pin_nss.port,
                               ptr_spi->pin_nss.num,
                               GPIO_MODE_OUTPUT,
                               GPIO_OUTPUT_PUSH_PULL,
                               GPIO_OUTPUT_SPEED_100MHz,
                               GPIO_NO_PULL_UP_DOWN);
    if (!status) return ARM_SPI_ERROR;
  }

  // Configure SPI SCL pin
  GPIO_PortClock            (ptr_spi->pin_scl.port, true);
  status = GPIO_PinAF       (ptr_spi->pin_scl.port,
                             ptr_spi->pin_scl.num,
                             ptr_spi->pin_af);
  if (!status) return ARM_SPI_ERROR;
  status = GPIO_PinConfigure(ptr_spi->pin_scl.port,
                             ptr_spi->pin_scl.num,
                             GPIO_MODE_AF,
                             GPIO_OUTPUT_PUSH_PULL,
                             GPIO_OUTPUT_SPEED_100MHz,
                             GPIO_NO_PULL_UP_DOWN);
  if (!status) return ARM_SPI_ERROR;

  // Configure SPI MISO pin
  GPIO_PortClock            (ptr_spi->pin_miso.port, true);
  status = GPIO_PinAF       (ptr_spi->pin_miso.port,
                             ptr_spi->pin_miso.num,
                             ptr_spi->pin_af);
  if (!status) return ARM_SPI_ERROR;
  status = GPIO_PinConfigure(ptr_spi->pin_miso.port,
                             ptr_spi->pin_miso.num,
                             GPIO_MODE_AF,
                             GPIO_OUTPUT_PUSH_PULL,
                             GPIO_OUTPUT_SPEED_100MHz,
                             GPIO_NO_PULL_UP_DOWN);
  if (!status) return ARM_SPI_ERROR;

  // Configure SPI MOSI pin
  GPIO_PortClock            (ptr_spi->pin_mosi.port, true);
  status = GPIO_PinAF       (ptr_spi->pin_mosi.port,
                             ptr_spi->pin_mosi.num,
                             ptr_spi->pin_af);
  if (!status) return ARM_SPI_ERROR;
  status = GPIO_PinConfigure(ptr_spi->pin_mosi.port,
                             ptr_spi->pin_mosi.num,
                             GPIO_MODE_AF,
                             GPIO_OUTPUT_PUSH_PULL,
                             GPIO_OUTPUT_SPEED_100MHz,
                             GPIO_NO_PULL_UP_DOWN);
  if (!status) return ARM_SPI_ERROR;

  return ARM_SPI_OK;
}

#if (RTE_SPI1)
static ARM_SPI_STATUS SPI1_Initialize (ARM_SPI_SignalEvent_t pSignalEvent) {
  return SPI_Initialize(pSignalEvent, &SPI1_Resources);
}
#endif

#if (RTE_SPI2)
static ARM_SPI_STATUS SPI2_Initialize (ARM_SPI_SignalEvent_t pSignalEvent) {
  return SPI_Initialize(pSignalEvent, &SPI2_Resources);
}
#endif

#if (RTE_SPI3)
static ARM_SPI_STATUS SPI3_Initialize (ARM_SPI_SignalEvent_t pSignalEvent) {
  return SPI_Initialize(pSignalEvent, &SPI3_Resources);
}
#endif

#if (RTE_SPI5)
static ARM_SPI_STATUS SPI5_Initialize (ARM_SPI_SignalEvent_t pSignalEvent) {
  return SPI_Initialize(pSignalEvent, &SPI5_Resources);
}
#endif


/**
  \fn          ARM_SPI_STATUS SPI_Uninitialize (SPI_RESOURCES *ptr_spi)
  \brief       De-initialize SPI Interface.
  \param[in]   ptr_spi  Pointer to SPI resources
  \return      \ref SPI_STATUS
*/
static ARM_SPI_STATUS SPI_Uninitialize (SPI_RESOURCES *ptr_spi) {
  bool status;

  // Enable SPI Clock
  *ptr_spi->clock_reg |=  ptr_spi->clock_mask;

  // Reset SPI Peripheral
  *ptr_spi->reset_reg |=  ptr_spi->reset_mask;
  __NOP(); __NOP(); __NOP(); __NOP(); 
  *ptr_spi->reset_reg &= ~ptr_spi->reset_mask;

  // Disable SPI Clock
  *ptr_spi->clock_reg &= ~ptr_spi->clock_mask;

  // Disable SPI IRQ
  NVIC_DisableIRQ(ptr_spi->spi_irq);

  // Uninitialize DMA Streams
  if (ptr_spi->dma_rx) {
    DMA_StreamUninitialize(ptr_spi->dma_rx->dma, ptr_spi->dma_rx->stream);
  }
  if (ptr_spi->dma_tx) {
    DMA_StreamUninitialize(ptr_spi->dma_tx->dma, ptr_spi->dma_tx->stream);
  }

  // Uninitialize SPI Run-Time Resources
  ptr_spi->info->power = false;

  osSemaphoreDelete(ptr_spi->info->semaphore_xfer);
  osSemaphoreDelete(ptr_spi->info->semaphore_busy);

  // Unconfigure SPI SCL pin
  status = GPIO_PinAF       (ptr_spi->pin_scl.port,
                             ptr_spi->pin_scl.num,
                             GPIO_AF_SYSTEM);
  if (!status) return ARM_SPI_ERROR;
  status = GPIO_PinConfigure(ptr_spi->pin_scl.port,
                             ptr_spi->pin_scl.num,
                             GPIO_MODE_INPUT,
                             GPIO_OUTPUT_PUSH_PULL,
                             GPIO_OUTPUT_SPEED_2MHz,
                             GPIO_NO_PULL_UP_DOWN);
  if (!status) return ARM_SPI_ERROR;

  // Unconfigure SPI MISO pin
  status = GPIO_PinAF       (ptr_spi->pin_miso.port,
                             ptr_spi->pin_miso.num,
                             GPIO_AF_SYSTEM);
  if (!status) return ARM_SPI_ERROR;
  status = GPIO_PinConfigure(ptr_spi->pin_miso.port,
                             ptr_spi->pin_miso.num,
                             GPIO_MODE_INPUT,
                             GPIO_OUTPUT_PUSH_PULL,
                             GPIO_OUTPUT_SPEED_2MHz,
                             GPIO_NO_PULL_UP_DOWN);
  if (!status) return ARM_SPI_ERROR;

  // Unconfigure SPI MOSI pin
  status = GPIO_PinAF       (ptr_spi->pin_mosi.port,
                             ptr_spi->pin_mosi.num,
                             GPIO_AF_SYSTEM);
  if (!status) return ARM_SPI_ERROR;
  status = GPIO_PinConfigure(ptr_spi->pin_mosi.port,
                             ptr_spi->pin_mosi.num,
                             GPIO_MODE_INPUT,
                             GPIO_OUTPUT_PUSH_PULL,
                             GPIO_OUTPUT_SPEED_2MHz,
                             GPIO_NO_PULL_UP_DOWN);
  if (!status) return ARM_SPI_ERROR;

  // Unconfigure SPI NSS pin (GPIO)
  if (ptr_spi->pin_nss.port != NULL) {
    status = GPIO_PinConfigure(ptr_spi->pin_nss.port,
                               ptr_spi->pin_nss.num,
                               GPIO_MODE_INPUT,
                               GPIO_OUTPUT_PUSH_PULL,
                               GPIO_OUTPUT_SPEED_2MHz,
                               GPIO_NO_PULL_UP_DOWN);
    if (!status) return ARM_SPI_ERROR;
    GPIO_PinWrite             (ptr_spi->pin_nss.port,
                               ptr_spi->pin_nss.num,
                               0);
  }

  return ARM_SPI_OK;
}

#if (RTE_SPI1)
static ARM_SPI_STATUS SPI1_Uninitialize (void) {
  return SPI_Uninitialize(&SPI1_Resources);
}
#endif

#if (RTE_SPI2)
static ARM_SPI_STATUS SPI2_Uninitialize (void) {
  return SPI_Uninitialize(&SPI2_Resources);
}
#endif

#if (RTE_SPI3)
static ARM_SPI_STATUS SPI3_Uninitialize (void) {
  return SPI_Uninitialize(&SPI3_Resources);
}
#endif

#if (RTE_SPI5)
static ARM_SPI_STATUS SPI5_Uninitialize (void) {
  return SPI_Uninitialize(&SPI5_Resources);
}
#endif


/**
  \fn          ARM_SPI_STATUS SPI_PowerControl (ARM_POWER_STATE  state,
                                                SPI_RESOURCES   *ptr_spi)
  \brief       Controls SPI Interface Power.
  \param[in]   state    Power state
  \param[in]   ptr_spi  Pointer to SPI resources
  \return      \ref ARM_SPI_STATUS
*/
static ARM_SPI_STATUS SPI_PowerControl (ARM_POWER_STATE state, SPI_RESOURCES *ptr_spi) {

  switch (state) {
    case ARM_POWER_OFF:

      // Disable SPI
      ptr_spi->spi->CR1 &= ~SPI_CR1_SPE;
      ptr_spi->spi->CR2  =  0;

      // Disable SPI Clock
      *ptr_spi->clock_reg &= ~ptr_spi->clock_mask;

      // Disable DMA Streams
      if (ptr_spi->dma_tx) {
        DMA_StreamDisable(ptr_spi->dma_tx->ptr_stream);
      }
      if (ptr_spi->dma_rx) {
        DMA_StreamDisable(ptr_spi->dma_rx->ptr_stream);
      }

      // Update SPI Run-Time Resources
      ptr_spi->info->power = false;

      osSemaphoreRelease(ptr_spi->info->semaphore_xfer);
      osSemaphoreWait   (ptr_spi->info->semaphore_xfer, 0);
      osSemaphoreRelease(ptr_spi->info->semaphore_busy);
      osSemaphoreWait   (ptr_spi->info->semaphore_busy, 0);

      break;

    case ARM_POWER_LOW:
      return ARM_SPI_ERROR_UNSUPPORTED;
    
    case ARM_POWER_FULL:

      // Enable SPI Clock
      *ptr_spi->clock_reg |= ptr_spi->clock_mask;

      // Enable software slave management and select master mode
      ptr_spi->spi->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR;
      // Enable SPI
      ptr_spi->spi->CR1 |= SPI_CR1_SPE;

      /* Ready for operation */
      osSemaphoreRelease(ptr_spi->info->semaphore_busy);
      ptr_spi->info->power = true;

      break;
  }

  return ARM_SPI_OK;
}

#if (RTE_SPI1)
static ARM_SPI_STATUS SPI1_PowerControl (ARM_POWER_STATE state) {
  return SPI_PowerControl(state, &SPI1_Resources);
}
#endif

#if (RTE_SPI2)
static ARM_SPI_STATUS SPI2_PowerControl (ARM_POWER_STATE state) {
  return SPI_PowerControl(state, &SPI2_Resources);
}
#endif

#if (RTE_SPI3)
static ARM_SPI_STATUS SPI3_PowerControl (ARM_POWER_STATE state) {
  return SPI_PowerControl(state, &SPI3_Resources);
}
#endif

#if (RTE_SPI5)
static ARM_SPI_STATUS SPI5_PowerControl (ARM_POWER_STATE state) {
  return SPI_PowerControl(state, &SPI5_Resources);
}
#endif


/**
  \fn          ARM_SPI_STATUS SPI_Configure (ARM_SPI_FRAME_FORMAT  frame_format,
                                             ARM_SPI_BIT_ORDER     bit_order,
                                             SPI_RESOURCES        *ptr_spi)
  \brief       Configure SPI Interface.
  \param[in]   frame_format  \ref ARM_SPI_FRAME_FORMAT
  \param[in]   bit_order     \ref ARM_SPI_BIT_ORDER
  \param[in]   ptr_spi       Pointer to SPI resources
  \return      \ref ARM_SPI_STATUS
*/
static ARM_SPI_STATUS SPI_Configure (ARM_SPI_FRAME_FORMAT  frame_format,
                                     ARM_SPI_BIT_ORDER     bit_order,
                                     SPI_RESOURCES        *ptr_spi) {
  uint16_t cr1;

  if (!ptr_spi->info->power) return ARM_SPI_ERROR;

  switch (frame_format) {
    case ARM_SPI_CPOL0_CPHA0:
      cr1 = 0;
      break;
    case ARM_SPI_CPOL0_CPHA1:
      cr1 = SPI_CR1_CPHA;
      break;
    case ARM_SPI_CPOL1_CPHA0:
      cr1 = SPI_CR1_CPOL;
      break;
    case ARM_SPI_CPOL1_CPHA1:
      cr1 = SPI_CR1_CPOL | SPI_CR1_CPHA;
      break;
    default:
      return ARM_SPI_ERROR;
  }
  switch (bit_order) {
    case ARM_SPI_MSB_LSB:
      break;
    case ARM_SPI_LSB_MSB:
      cr1 |= SPI_CR1_LSBFIRST;
      break;
    default:
      return ARM_SPI_ERROR;
  }

  ptr_spi->spi->CR1 &= ~ SPI_CR1_SPE;           // Disable SPI
  ptr_spi->spi->CR1 &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL | SPI_CR1_LSBFIRST);
  ptr_spi->spi->CR1 |=   cr1;                   // Setup CPHA, CPOL, LSBFIRST
  ptr_spi->spi->CR1 |=   SPI_CR1_SPE;           // Enable SPI

  return ARM_SPI_OK;
}

#if (RTE_SPI1)
static ARM_SPI_STATUS SPI1_Configure (ARM_SPI_FRAME_FORMAT  frame_format,
                                      ARM_SPI_BIT_ORDER     bit_order) {
  return SPI_Configure(frame_format, bit_order, &SPI1_Resources);
}
#endif

#if (RTE_SPI2)
static ARM_SPI_STATUS SPI2_Configure (ARM_SPI_FRAME_FORMAT  frame_format,
                                      ARM_SPI_BIT_ORDER     bit_order) {
  return SPI_Configure(frame_format, bit_order, &SPI2_Resources);
}
#endif

#if (RTE_SPI3)
static ARM_SPI_STATUS SPI3_Configure (ARM_SPI_FRAME_FORMAT  frame_format,
                                      ARM_SPI_BIT_ORDER     bit_order) {
  return SPI_Configure(frame_format, bit_order, &SPI3_Resources);
}
#endif

#if (RTE_SPI5)
static ARM_SPI_STATUS SPI5_Configure (ARM_SPI_FRAME_FORMAT  frame_format,
                                      ARM_SPI_BIT_ORDER     bit_order) {
  return SPI_Configure(frame_format, bit_order, &SPI5_Resources);
}
#endif


/**
  \fn          uint32_t SPI_BusSpeed (uint32_t       bps,
                                      SPI_RESOURCES *ptr_spi)
  \brief       Set bus speed for SPI transfers.
  \param[in]   bps      Requested speed in bits/s
  \param[in]   ptr_spi  Pointer to SPI resources
  \return      Configured bus speed in bits/s
*/
static uint32_t SPI_BusSpeed (uint32_t       bps,
                              SPI_RESOURCES *ptr_spi) {
  uint32_t n;

  if (!ptr_spi->info->power) return 0;

  for (n = 1; n < 8; n++) {
    if (bps >= (ptr_spi->clock >> n)) break;
  }
  bps = ptr_spi->clock >> n;

  ptr_spi->spi->CR1 &= ~SPI_CR1_SPE;            // Disable SPI
  ptr_spi->spi->CR1 &= ~SPI_CR1_BR;             // Clear BR bits
  ptr_spi->spi->CR1 |=  (n - 1) << 3;           // Setup BR bits
  ptr_spi->spi->CR1 |=  SPI_CR1_SPE;            // Enable SPI

  return bps;
}

#if (RTE_SPI1)
static uint32_t SPI1_BusSpeed (uint32_t bps) {
  return SPI_BusSpeed(bps, &SPI1_Resources);
}
#endif

#if (RTE_SPI2)
static uint32_t SPI2_BusSpeed (uint32_t bps) {
  return SPI_BusSpeed(bps, &SPI2_Resources);
}
#endif

#if (RTE_SPI3)
static uint32_t SPI3_BusSpeed (uint32_t bps) {
  return SPI_BusSpeed(bps, &SPI3_Resources);
}
#endif

#if (RTE_SPI5)
static uint32_t SPI5_BusSpeed (uint32_t bps) {
  return SPI_BusSpeed(bps, &SPI5_Resources);
}
#endif


/**
  \fn          ARM_SPI_STATUS SPI_SlaveSelect (ARM_SPI_SS_SIGNAL  ss,
                                               SPI_RESOURCES     *ptr_spi)
  \brief       Control Slave Select (SS) signal.
  \param[in]   ss       \ref SPI_SS_SIGNAL
  \param[in]   ptr_spi  Pointer to SPI resources
  \return      \ref SPI_STATUS
*/
static ARM_SPI_STATUS SPI_SlaveSelect (ARM_SPI_SS_SIGNAL  ss,
                                       SPI_RESOURCES     *ptr_spi) {

  if (!ptr_spi->info->power) return ARM_SPI_ERROR;

  if (ptr_spi->pin_nss.port != NULL) {
    GPIO_PinWrite(ptr_spi->pin_nss.port,
                  ptr_spi->pin_nss.num,
                  (ss == ARM_SPI_SS_ACTIVE) ? 0 : 1);
  }

  return ARM_SPI_OK;
}

#if (RTE_SPI1)
static ARM_SPI_STATUS SPI1_SlaveSelect (ARM_SPI_SS_SIGNAL ss) {
  return SPI_SlaveSelect(ss, &SPI1_Resources);
}
#endif

#if (RTE_SPI2)
static ARM_SPI_STATUS SPI2_SlaveSelect (ARM_SPI_SS_SIGNAL ss) {
  return SPI_SlaveSelect(ss, &SPI2_Resources);
}
#endif

#if (RTE_SPI3)
static ARM_SPI_STATUS SPI3_SlaveSelect (ARM_SPI_SS_SIGNAL ss) {
  return SPI_SlaveSelect(ss, &SPI3_Resources);
}
#endif

#if (RTE_SPI5)
static ARM_SPI_STATUS SPI5_SlaveSelect (ARM_SPI_SS_SIGNAL ss) {
  return SPI_SlaveSelect(ss, &SPI5_Resources);
}
#endif


/**
  \fn          uint8_t SPI_TransferByte (uint8_t out, SPI_RESOURCES *ptr_spi)
  \brief       Send and receive one byte via SPI Interface.
  \param[in]   out      Byte to be sent to the SPI data output
  \param[in]   ptr_spi  Pointer to SPI resources
  \return      Byte received from the SPI data input
*/
static uint8_t SPI_TransferByte (uint8_t out, SPI_RESOURCES *ptr_spi) {

  if (!ptr_spi->info->power) return 0;

  ptr_spi->spi->DR = out;
  while (!(ptr_spi->spi->SR & SPI_SR_RXNE) && ptr_spi->info->power);

  return (ptr_spi->spi->DR);
}

#if (RTE_SPI1)
static uint8_t SPI1_TransferByte (uint8_t out) {
  return SPI_TransferByte(out, &SPI1_Resources);
}
#endif

#if (RTE_SPI2)
static uint8_t SPI2_TransferByte (uint8_t out) {
  return SPI_TransferByte(out, &SPI2_Resources);
}
#endif

#if (RTE_SPI3)
static uint8_t SPI3_TransferByte (uint8_t out) {
  return SPI_TransferByte(out, &SPI3_Resources);
}
#endif

#if (RTE_SPI5)
static uint8_t SPI5_TransferByte (uint8_t out) {
  return SPI_TransferByte(out, &SPI5_Resources);
}
#endif


/**
  \fn          ARM_SPI_STATUS SPI_SendData (const uint8_t  *buf,
                                                  uint32_t  len,
                                            SPI_RESOURCES  *ptr_spi)
  \brief       Send data block to SPI Slave.
  \param[in]   buf      Pointer to the buffer containing data to send
  \param[in]   len      Data buffer length in bytes
  \param[in]   ptr_spi  Pointer to SPI resources
  \return      \ref SPI_STATUS
*/
static ARM_SPI_STATUS SPI_SendData (const uint8_t  *buf,
                                          uint32_t  len,
                                    SPI_RESOURCES  *ptr_spi) {
  uint8_t ch;

  if ((buf == NULL) || (len == 0)) return ARM_SPI_ERROR;

  osSemaphoreWait(ptr_spi->info->semaphore_busy, osWaitForever);

  if (!ptr_spi->info->power) return ARM_SPI_ERROR;

  if (ptr_spi->dma_rx) {
    DMA_StreamConfigure(
      ptr_spi->dma_rx->ptr_stream,
      DMA_TRANSFER_COMPLETE_INTERRUPT |
      DMA_PERIPHERAL_FLOW_CONTROL     |
      DMA_PERIPHERAL_TO_MEMORY        |
      DMA_PERIPHERAL_DATA_8BIT        |
      DMA_MEMORY_DATA_8BIT            |
      DMA_PERIPHERAL_SINGLE_TRANSFER  |
      DMA_MEMORY_SINGLE_TRANSFER      |
      ((ptr_spi->dma_rx->priority << DMA_PRIORITY_POS) & DMA_PRIORITY_MASK) |
      ((ptr_spi->dma_rx->channel  << DMA_CHANNEL_POS)  & DMA_CHANNEL_MASK),
      DMA_FIFO_DIRECT_MODE_ENABLE,
      ptr_spi->spi->DR,                         // Peripheral Address
      (uint32_t)(&ch),                          // Memory Address
      len                                       // Number of Data items to transfer
    );
    DMA_StreamEnable(ptr_spi->dma_rx->ptr_stream);
    ptr_spi->info->rx.buf = NULL;
    ptr_spi->spi->CR2 |= SPI_CR2_RXDMAEN;       // Rx Buffer DMA enable
  } else {
    ptr_spi->info->rx.buf = NULL;
    ptr_spi->info->rx.cnt = len;
    ptr_spi->spi->CR2 |= SPI_CR2_RXNEIE;        // Rx Buffer Not Empty Interrupt enable
  }

  if (ptr_spi->dma_tx) {
    DMA_StreamConfigure(
      ptr_spi->dma_tx->ptr_stream,
      DMA_TRANSFER_COMPLETE_INTERRUPT |
      DMA_PERIPHERAL_FLOW_CONTROL     |
      DMA_MEMORY_TO_PERIPHERAL        |
      DMA_MEMORY_INCREMENT            |
      DMA_PERIPHERAL_DATA_8BIT        |
      DMA_MEMORY_DATA_8BIT            |
      DMA_PERIPHERAL_SINGLE_TRANSFER  |
      DMA_MEMORY_SINGLE_TRANSFER      |
   /* DMA_MEMORY_BURST_TRANSFER8      | */
      ((ptr_spi->dma_tx->priority << DMA_PRIORITY_POS) & DMA_PRIORITY_MASK) |
      ((ptr_spi->dma_tx->channel  << DMA_CHANNEL_POS)  & DMA_CHANNEL_MASK),
      DMA_FIFO_DIRECT_MODE_ENABLE,
   /* DMA_FIFO_DIRECT_MODE_DISABLE | DMA_FIFO_THRESHOLD_50, */
      ptr_spi->spi->DR,                         // Peripheral Address
      (uint32_t)buf,                            // Memory Address
      len                                       // Number of Data items to transfer
    );
    DMA_StreamEnable(ptr_spi->dma_rx->ptr_stream);
    ptr_spi->spi->CR2 |= SPI_CR2_TXDMAEN;       // Tx Buffer DMA enable
  } else {
    ptr_spi->info->tx.buf = (uint8_t *)buf;
    ptr_spi->info->tx.cnt = len;
    ptr_spi->info->tx.inc = true;
    ptr_spi->spi->CR2 |= SPI_CR2_TXEIE;         // Tx Buffer Empty Interrupt enable
  }

  osSemaphoreWait(ptr_spi->info->semaphore_xfer, osWaitForever);

  return ARM_SPI_OK;
}

#if (RTE_SPI1)
static ARM_SPI_STATUS SPI1_SendData (const uint8_t *buf, uint32_t len) {
  return SPI_SendData(buf, len, &SPI1_Resources);
}
#endif

#if (RTE_SPI2)
static ARM_SPI_STATUS SPI2_SendData (const uint8_t *buf, uint32_t len) {
  return SPI_SendData(buf, len, &SPI2_Resources);
}
#endif

#if (RTE_SPI3)
static ARM_SPI_STATUS SPI3_SendData (const uint8_t *buf, uint32_t len) {
  return SPI_SendData(buf, len, &SPI3_Resources);
}
#endif

#if (RTE_SPI5)
static ARM_SPI_STATUS SPI5_SendData (const uint8_t *buf, uint32_t len) {
  return SPI_SendData(buf, len, &SPI5_Resources);
}
#endif


/**
  \fn          ARM_SPI_STATUS SPI_ReceiveData (uint8_t       *buf,
                                               uint32_t       len,
                                               uint8_t        out,
                                               SPI_RESOURCES *ptr_spi)
  \brief       Receive data block from SPI Slave.
  \param[out]  buf      Pointer to the buffer for receiving data
  \param[in]   len      Data buffer length in bytes
  \param[in]   out      Byte to be sent repeatedly during receive
  \param[in]   ptr_spi  Pointer to SPI resources
  \return      \ref SPI_STATUS
*/
static ARM_SPI_STATUS SPI_ReceiveData (uint8_t       *buf,
                                       uint32_t       len,
                                       uint8_t        out,
                                       SPI_RESOURCES *ptr_spi) {

  if ((buf == NULL) || (len == 0)) return ARM_SPI_ERROR;

  osSemaphoreWait(ptr_spi->info->semaphore_busy, osWaitForever);

  if (!ptr_spi->info->power) return ARM_SPI_ERROR;

  if (ptr_spi->dma_rx) {
    DMA_StreamConfigure(
      ptr_spi->dma_rx->ptr_stream,
      DMA_TRANSFER_COMPLETE_INTERRUPT |
      DMA_PERIPHERAL_FLOW_CONTROL     |
      DMA_PERIPHERAL_TO_MEMORY        |
      DMA_MEMORY_INCREMENT            |
      DMA_PERIPHERAL_DATA_8BIT        |
      DMA_MEMORY_DATA_8BIT            |
      DMA_PERIPHERAL_SINGLE_TRANSFER  |
      DMA_MEMORY_SINGLE_TRANSFER      |
      ((ptr_spi->dma_rx->priority << DMA_PRIORITY_POS) & DMA_PRIORITY_MASK) |
      ((ptr_spi->dma_rx->channel  << DMA_CHANNEL_POS)  & DMA_CHANNEL_MASK),
      DMA_FIFO_DIRECT_MODE_ENABLE,
      ptr_spi->spi->DR,                         // Peripheral Address
      (uint32_t)buf,                            // Memory Address
      len                                       // Number of Data items to transfer
    );
    DMA_StreamEnable(ptr_spi->dma_rx->ptr_stream);
    ptr_spi->info->rx.buf = buf;
    ptr_spi->spi->CR2 |= SPI_CR2_RXDMAEN;       // Rx Buffer DMA enable
  } else {
    ptr_spi->info->rx.buf = buf;
    ptr_spi->info->rx.cnt = len;
    ptr_spi->info->rx.inc = true;
    ptr_spi->spi->CR2 |= SPI_CR2_RXNEIE;        // Rx Buffer Not Empty Interrupt enable
  }

  if (ptr_spi->dma_tx) {
    DMA_StreamConfigure(
      ptr_spi->dma_tx->ptr_stream,
      DMA_TRANSFER_COMPLETE_INTERRUPT |
      DMA_PERIPHERAL_FLOW_CONTROL     |
      DMA_MEMORY_TO_PERIPHERAL        |
      DMA_PERIPHERAL_DATA_8BIT        |
      DMA_MEMORY_DATA_8BIT            |
      DMA_PERIPHERAL_SINGLE_TRANSFER  |
      DMA_MEMORY_SINGLE_TRANSFER      |
      ((ptr_spi->dma_tx->priority << DMA_PRIORITY_POS) & DMA_PRIORITY_MASK) |
      ((ptr_spi->dma_tx->channel  << DMA_CHANNEL_POS)  & DMA_CHANNEL_MASK),
      DMA_FIFO_DIRECT_MODE_ENABLE,
      ptr_spi->spi->DR,                         // Peripheral Address
      (uint32_t)(uint32_t)(&out),               // Memory Address
      len                                       // Number of Data items to transfer
    );
    DMA_StreamEnable(ptr_spi->dma_rx->ptr_stream);
    ptr_spi->spi->CR2 |= SPI_CR2_TXDMAEN;       // Tx Buffer DMA enable
  } else {
    ptr_spi->info->tx.buf = &out;
    ptr_spi->info->tx.cnt = len;
    ptr_spi->info->tx.inc = false;
    ptr_spi->spi->CR2 |= SPI_CR2_TXEIE;         // Tx Buffer Empty Interrupt enable
  }

  osSemaphoreWait(ptr_spi->info->semaphore_xfer, osWaitForever);

  return ARM_SPI_OK;
}

#if (RTE_SPI1)
static ARM_SPI_STATUS SPI1_ReceiveData (uint8_t *buf, uint32_t len, uint8_t out) {
  return SPI_ReceiveData(buf, len, out, &SPI1_Resources);
}
#endif

#if (RTE_SPI2)
static ARM_SPI_STATUS SPI2_ReceiveData (uint8_t *buf, uint32_t len, uint8_t out) {
  return SPI_ReceiveData(buf, len, out, &SPI2_Resources);
}
#endif

#if (RTE_SPI3)
static ARM_SPI_STATUS SPI3_ReceiveData (uint8_t *buf, uint32_t len, uint8_t out) {
  return SPI_ReceiveData(buf, len, out, &SPI3_Resources);
}
#endif

#if (RTE_SPI5)
static ARM_SPI_STATUS SPI5_ReceiveData (uint8_t *buf, uint32_t len, uint8_t out) {
  return SPI_ReceiveData(buf, len, out, &SPI5_Resources);
}
#endif


/**
  \fn          ARM_SPI_STATUS SPI_AbortTransfer (SPI_RESOURCES *ptr_spi)
  \brief       Abort current SPI transfer.
  \param[in]   ptr_spi  Pointer to SPI resources
  \return      \ref ARM_SPI_STATUS
*/
static ARM_SPI_STATUS SPI_AbortTransfer (SPI_RESOURCES *ptr_spi) {

  if (!ptr_spi->info->power) return ARM_SPI_ERROR;

  ptr_spi->spi->CR2 &= ~(SPI_CR2_TXEIE    |     // Tx Buffer Empty Interrupt disable
                         SPI_CR2_RXNEIE   |     // Rx Buffer Not Empty Interrupt disable
                         SPI_CR2_TXDMAEN  |     // Tx Buffer DMA disable
                         SPI_CR2_RXDMAEN);      // Rx Buffer DMA disable

  ptr_spi->spi->CR1 &= ~SPI_CR1_SPE;            // Disable SPI
  __NOP(); __NOP(); __NOP(); __NOP();
  ptr_spi->spi->CR1 |=  SPI_CR1_SPE;            // Enable SPI
  
  if (ptr_spi->dma_tx) {
    DMA_StreamDisable(ptr_spi->dma_tx->ptr_stream);
  }
  if (ptr_spi->dma_rx) {
    DMA_StreamDisable(ptr_spi->dma_rx->ptr_stream);
  }

  osSemaphoreRelease(ptr_spi->info->semaphore_xfer);
  osSemaphoreWait   (ptr_spi->info->semaphore_xfer, 0);
  osSemaphoreRelease(ptr_spi->info->semaphore_busy);
  osSemaphoreWait   (ptr_spi->info->semaphore_busy, 0);

  return ARM_SPI_OK;
}

#if (RTE_SPI1)
static ARM_SPI_STATUS SPI1_AbortTransfer (void) {
  return SPI_AbortTransfer(&SPI1_Resources);
}
#endif

#if (RTE_SPI2)
static ARM_SPI_STATUS SPI2_AbortTransfer (void) {
  return SPI_AbortTransfer(&SPI2_Resources);
}
#endif

#if (RTE_SPI3)
static ARM_SPI_STATUS SPI3_AbortTransfer (void) {
  return SPI_AbortTransfer(&SPI3_Resources);
}
#endif

#if (RTE_SPI5)
static ARM_SPI_STATUS SPI5_AbortTransfer (void) {
  return SPI_AbortTransfer(&SPI5_Resources);
}
#endif


/**
  \fn          void SPI_IRQHandler (SPI_RESOURCES *ptr_spi)
  \brief       SPI Interrupt handler.
  \param[in]   ptr_spi  Pointer to SPI resources
*/
static void SPI_IRQHandler (SPI_RESOURCES *ptr_spi) {

  // Transmit Buffer Empty
  if (ptr_spi->spi->CR2 & SPI_CR2_TXEIE) {
    if (ptr_spi->spi->SR & SPI_SR_TXE) {
      if (ptr_spi->info->tx.cnt) {
        ptr_spi->spi->DR = *(ptr_spi->info->tx.buf);
        if (ptr_spi->info->tx.inc == true) {
          ptr_spi->info->tx.buf++;
        }
        ptr_spi->info->tx.cnt--;
      } else {
        ptr_spi->spi->CR2 &= ~SPI_CR2_TXEIE;      // Tx Buffer Empty Interrupt disable
        NVIC_ClearPendingIRQ (ptr_spi->spi_irq);

        if (ptr_spi->info->rx.buf == NULL) {
          osSemaphoreRelease(ptr_spi->info->semaphore_xfer);
        }
      }
    }
  }
  
  // Receive Buffer Not Empty
  if(ptr_spi->spi->CR2 & SPI_CR2_RXNEIE) {
    if (ptr_spi->spi->SR & SPI_SR_RXNE) {
      if (ptr_spi->info->rx.buf) {
        *(ptr_spi->info->rx.buf) = ptr_spi->spi->DR;
        if (ptr_spi->info->rx.inc == true) {
          ptr_spi->info->rx.buf++;
        }
      } else {
        ptr_spi->spi->DR;
      }
      ptr_spi->info->rx.cnt--;
      if (ptr_spi->info->rx.cnt == 0) {
        ptr_spi->spi->CR2 &= ~SPI_CR2_RXNEIE;     // Rx Buffer Not Empty Interrupt disable
        NVIC_ClearPendingIRQ (ptr_spi->spi_irq);

        if (ptr_spi->info->rx.buf) {
          osSemaphoreRelease(ptr_spi->info->semaphore_xfer);
        } else {
          if (ptr_spi->info->cb_event) {
             (ptr_spi->info->cb_event)(ARM_SPI_EVENT_SEND_DONE);
          }
        }
        osSemaphoreRelease(ptr_spi->info->semaphore_busy);
      }
    }
  }
}

#if (RTE_SPI1)
void SPI1_IRQHandler (void) {
  SPI_IRQHandler(&SPI1_Resources);
}
#endif

#if (RTE_SPI2)
void SPI2_IRQHandler (void) {
  SPI_IRQHandler(&SPI2_Resources);
}
#endif

#if (RTE_SPI3)
void SPI3_IRQHandler (void) {
  SPI_IRQHandler(&SPI3_Resources);
 }
#endif

#if (RTE_SPI5)
void SPI5_IRQHandler (void) {
  SPI_IRQHandler(&SPI5_Resources);
 }
#endif


/* DMA Callbacks */

#if ((RTE_SPI1 && RTE_SPI1_TX_DMA) || \
     (RTE_SPI2 && RTE_SPI2_TX_DMA) || \
     (RTE_SPI3 && RTE_SPI3_TX_DMA) || \
     (RTE_SPI5 && RTE_SPI5_TX_DMA))

/**
  \fn          void SPI_DMA_TxComplete (SPI_RESOURCES *ptr_spi)
  \brief       DMA transmit complete callback.
  \param[in]   ptr_spi  Pointer to SPI resources
*/
static void SPI_DMA_TxComplete (SPI_RESOURCES *ptr_spi) {

  DMA_StreamDisable(ptr_spi->dma_tx->ptr_stream);
  ptr_spi->spi->CR2 &= ~SPI_CR2_TXDMAEN;        // Tx Buffer DMA disable
  
  if (ptr_spi->info->rx.buf == NULL) {
    osSemaphoreRelease(ptr_spi->info->semaphore_xfer);
  }
}

#endif

#if ((RTE_SPI1 && RTE_SPI1_RX_DMA) || \
     (RTE_SPI2 && RTE_SPI2_RX_DMA) || \
     (RTE_SPI3 && RTE_SPI3_RX_DMA) || \
     (RTE_SPI5 && RTE_SPI5_RX_DMA))

/**
  \fn          void SPI_DMA_RxComplete (SPI_RESOURCES *ptr_spi)
  \brief       DMA receive complete callback.
  \param[in]   ptr_spi  Pointer to SPI resources
*/
static void SPI_DMA_RxComplete (SPI_RESOURCES *ptr_spi) {

  DMA_StreamDisable(ptr_spi->dma_rx->ptr_stream);
  ptr_spi->spi->CR2 &= ~SPI_CR2_RXDMAEN;        // Rx Buffer DMA disable

  osSemaphoreRelease(ptr_spi->info->semaphore_busy);
  
  if (ptr_spi->info->rx.buf) {
    osSemaphoreRelease(ptr_spi->info->semaphore_xfer);
  } else {
    if (ptr_spi->info->cb_event) {
       (ptr_spi->info->cb_event)(ARM_SPI_EVENT_SEND_DONE);
    }
  }
}

#endif

#if (RTE_SPI1)
#if (RTE_SPI1_TX_DMA)
void DMAx_STREAMy_EVENT(RTE_SPI1_TX_DMA_NUMBER, RTE_SPI1_TX_DMA_STREAM) (uint32_t events) {
  SPI_DMA_TxComplete(&SPI1_Resources);
}
#endif
#if (RTE_SPI1_RX_DMA)
void DMAx_STREAMy_EVENT(RTE_SPI1_RX_DMA_NUMBER, RTE_SPI1_RX_DMA_STREAM) (uint32_t events) {
  SPI_DMA_RxComplete(&SPI1_Resources);
}
#endif
#endif

#if (RTE_SPI2)
#if (RTE_SPI2_TX_DMA)
void DMAx_STREAMy_EVENT(RTE_SPI2_TX_DMA_NUMBER, RTE_SPI2_TX_DMA_STREAM) (uint32_t events) {
  SPI_DMA_TxComplete(&SPI2_Resources);
}
#endif
#if (RTE_SPI2_RX_DMA)
void DMAx_STREAMy_EVENT(RTE_SPI2_RX_DMA_NUMBER, RTE_SPI2_RX_DMA_STREAM) (uint32_t events) {
  SPI_DMA_RxComplete(&SPI2_Resources);
}
#endif
#endif

#if (RTE_SPI3)
#if (RTE_SPI3_TX_DMA)
void DMAx_STREAMy_EVENT(RTE_SPI3_TX_DMA_NUMBER, RTE_SPI3_TX_DMA_STREAM) (uint32_t events) {
  SPI_DMA_TxComplete(&SPI3_Resources);
}
#endif
#if (RTE_SPI3_RX_DMA)
void DMAx_STREAMy_EVENT(RTE_SPI3_RX_DMA_NUMBER, RTE_SPI3_RX_DMA_STREAM) (uint32_t events) {
  SPI_DMA_RxComplete(&SPI3_Resources);
}
#endif
#endif

#if (RTE_SPI5)
#if (RTE_SPI5_TX_DMA)
void DMAx_STREAMy_EVENT(RTE_SPI5_TX_DMA_NUMBER, RTE_SPI5_TX_DMA_STREAM) (uint32_t events) {
  SPI_DMA_TxComplete(&SPI5_Resources);
}
#endif
#if (RTE_SPI5_RX_DMA)
void DMAx_STREAMy_EVENT(RTE_SPI5_RX_DMA_NUMBER, RTE_SPI5_RX_DMA_STREAM) (uint32_t events) {
  SPI_DMA_RxComplete(&SPI5_Resources);
}
#endif
#endif


/* SPI1 Driver Control Block */
#if (RTE_SPI1)
ARM_DRIVER_SPI Driver_SPI1 = {
  SPIX_GetVersion,
  SPIX_GetCapabilities,
  SPI1_Initialize,
  SPI1_Uninitialize,
  SPI1_PowerControl,
  SPI1_Configure,
  SPI1_BusSpeed,
  SPI1_SlaveSelect,
  SPI1_TransferByte,
  SPI1_SendData,
  SPI1_ReceiveData,
  SPI1_AbortTransfer
};
#endif

/* SPI2 Driver Control Block */
#if (RTE_SPI2)
ARM_DRIVER_SPI Driver_SPI2 = {
  SPIX_GetVersion,
  SPIX_GetCapabilities,
  SPI2_Initialize,
  SPI2_Uninitialize,
  SPI2_PowerControl,
  SPI2_Configure,
  SPI2_BusSpeed,
  SPI2_SlaveSelect,
  SPI2_TransferByte,
  SPI2_SendData,
  SPI2_ReceiveData,
  SPI2_AbortTransfer
};
#endif

/* SPI3 Driver Control Block */
#if (RTE_SPI3)
ARM_DRIVER_SPI Driver_SPI3 = {
  SPIX_GetVersion,
  SPIX_GetCapabilities,
  SPI3_Initialize,
  SPI3_Uninitialize,
  SPI3_PowerControl,
  SPI3_Configure,
  SPI3_BusSpeed,
  SPI3_SlaveSelect,
  SPI3_TransferByte,
  SPI3_SendData,
  SPI3_ReceiveData,
  SPI3_AbortTransfer
};
#endif

/* SPI5 Driver Control Block */
#if (RTE_SPI5)
ARM_DRIVER_SPI Driver_SPI5 = {
  SPIX_GetVersion,
  SPIX_GetCapabilities,
  SPI5_Initialize,
  SPI5_Uninitialize,
  SPI5_PowerControl,
  SPI5_Configure,
  SPI5_BusSpeed,
  SPI5_SlaveSelect,
  SPI5_TransferByte,
  SPI5_SendData,
  SPI5_ReceiveData,
  SPI5_AbortTransfer
};
#endif
