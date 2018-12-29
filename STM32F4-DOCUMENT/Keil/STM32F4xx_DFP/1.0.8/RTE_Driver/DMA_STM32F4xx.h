/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        19. April 2013
 * $Revision:    V1.01
 *  
 * Project:      DMA Driver definitions for ST STM32F4xx
 * -------------------------------------------------------------------- */

#ifndef __DMA_STM32F4XX_H
#define __DMA_STM32F4XX_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"


#if defined (__CC_ARM)
#define __FORCE_INLINE  static __forceinline
#else
#define __FORCE_INLINE  __STATIC_INLINE
#endif


#define _DMAx_STREAMy(x, y)              DMA##x##_Stream##y
#define  DMAx_STREAMy(x, y)             _DMAx_STREAMy(x, y)

#define _DMAx_STREAMy_EVENT(x, y)        DMA##x##_Stream##y##_Event
#define  DMAx_STREAMy_EVENT(x, y)       _DMAx_STREAMy_EVENT(x, y)


// DMA Stream Interrupt Flags
#define DMA_STREAM_FIFO_ERROR           (1UL<<0)
#define DMA_STREAM_DIRECT_MODE_ERROR    (1UL<<2)
#define DMA_STREAM_TRANSFER_ERROR       (1UL<<3)
#define DMA_STREAM_HALF_TRANSFER        (1UL<<4)
#define DMA_STREAM_TRANSFER_COMPLETE    (1UL<<5)
#define DMA_STREAM_FLAGS                (DMA_STREAM_FIFO_ERROR        | \
                                         DMA_STREAM_DIRECT_MODE_ERROR | \
                                         DMA_STREAM_TRANSFER_ERROR    | \
                                         DMA_STREAM_HALF_TRANSFER     | \
                                         DMA_STREAM_TRANSFER_COMPLETE)

// DMA Stream Configuration Register definitions
#define DMA_DIRECT_MODE_INTERRUPT       DMA_SxCR_DMEIE
#define DMA_TRANSFER_ERROR_INTERRUPT    DMA_SxCR_TEIE
#define DMA_HALF_TRANSFER_INTERRUPT     DMA_SxCR_HTIE
#define DMA_TRANSFER_COMPLETE_INTERRUPT DMA_SxCR_TCIE
#define DMA_PERIPHERAL_FLOW_CONTROL     DMA_SxCR_PFCTRL
#define DMA_PERIPHERAL_TO_MEMORY        0
#define DMA_MEMORY_TO_PERIPHERAL        DMA_SxCR_DIR_0
#define DMA_MEMORY_TO_MEMORY            DMA_SxCR_DIR_1
#define DMA_CIRCULAR_MODE               DMA_SxCR_CIRC
#define DMA_PERIPHERAL_INCREMENT        DMA_SxCR_PINC
#define DMA_MEMORY_INCREMENT            DMA_SxCR_MINC
#define DMA_PERIPHERAL_DATA_8BIT        0
#define DMA_PERIPHERAL_DATA_16BIT       DMA_SxCR_PSIZE_0
#define DMA_PERIPHERAL_DATA_32BIT       DMA_SxCR_PSIZE_1
#define DMA_MEMORY_DATA_8BIT            0
#define DMA_MEMORY_DATA_16BIT           DMA_SxCR_MSIZE_0
#define DMA_MEMORY_DATA_32BIT           DMA_SxCR_MSIZE_1
#define DMA_PERIPHERAL_INCREMENT_OFFSET DMA_SxCR_PINCOS
#define DMA_PRIORITY_POS                16
#define DMA_PRIORITY_MASK               DMA_SxCR_PL
#define DMA_DOUBLE_BUFFER_MODE          DMA_SxCR_DBM
#define DMA_CURRENT_TARGET              DMA_SxCR_CT
#define DMA_PERIPHERAL_SINGLE_TRANSFER  0
#define DMA_PERIPHERAL_BURST_TRANSFER4  DMA_SxCR_PBURST_0
#define DMA_PERIPHERAL_BURST_TRANSFER8  DMA_SxCR_PBURST_1
#define DMA_PERIPHERAL_BURST_TRANSFER16 (DMA_SxCR_PBURST_0 | DMA_SxCR_PBURST_1)
#define DMA_MEMORY_SINGLE_TRANSFER      0
#define DMA_MEMORY_BURST_TRANSFER4      DMA_SxCR_MBURST_0
#define DMA_MEMORY_BURST_TRANSFER8      DMA_SxCR_MBURST_1
#define DMA_MEMORY_BURST_TRANSFER16     (DMA_SxCR_MBURST_0 | DMA_SxCR_MBURST_1)
#define DMA_CHANNEL_POS                 25
#define DMA_CHANNEL_MASK                DMA_SxCR_CHSEL

// DMA Stream FIFO Control Register definitions
#define DMA_FIFO_THRESHOLD_25           0
#define DMA_FIFO_THRESHOLD_50           DMA_SxFCR_FTH_0
#define DMA_FIFO_THRESHOLD_75           DMA_SxFCR_FTH_1
#define DMA_FIFO_THRESHOLD_100          (DMA_SxFCR_FTH_0 | DMA_SxFCR_FTH_1)
#define DMA_FIFO_DIRECT_MODE_ENABLE     0
#define DMA_FIFO_DIRECT_MODE_DISABLE    DMA_SxFCR_DMDIS
#define DMA_FIFO_ERROR_INTERRUPT        DMA_SxFCR_FEIE


// DMA Information definitions
typedef struct _DMA_INFO {
  DMA_Stream_TypeDef *ptr_stream;
  uint8_t             dma;
  uint8_t             stream;
  uint8_t             channel;
  uint8_t             priority;
} DMA_INFO;


/// DMA Functions

/**
  \fn          void DMA_StreamInitialize (uint32_t dma, uint32_t stream)
  \brief       Initialize DMA Stream
  \param[in]   dma     DMA number
  \param[in]   stream  DMA stream number
*/
extern void DMA_StreamInitialize (uint32_t dma, uint32_t stream);

/**
  \fn          void DMA_StreamUninitialize (uint32_t dma, uint32_t stream)
  \brief       Uninitialize DMA Stream
  \param[in]   dma     DMA number
  \param[in]   stream  DMA stream number
*/
extern void DMA_StreamUninitialize (uint32_t dma, uint32_t stream);

/**
  \fn          void DMA_StreamConfigure (DMA_Stream_TypeDef *DMA_Stream,
                                         uint32_t            cr,
                                         uint32_t            fcr,
                                         uint32_t            par,
                                         uint32_t            mar,
                                         uint32_t            ndtr);
  \brief       Configure DMA Stream for next transfer
  \param[in]   DMA_Stream specifies pointer to DMA Stream peripheral
  \param[in]   cr    Configuration register value
  \param[in]   fcr   FIFO control register value
  \param[in]   par   Peripheral address register value
  \param[in]   mar   Memory address register value
  \param[in]   ndtr  Number of data transfer register value
*/
__FORCE_INLINE void DMA_StreamConfigure (DMA_Stream_TypeDef *DMA_Stream,
                                         uint32_t            cr,
                                         uint32_t            fcr,
                                         uint32_t            par,
                                         uint32_t            mar,
                                         uint32_t            ndtr) {
  DMA_Stream->CR   = cr;
  DMA_Stream->FCR  = fcr;
  DMA_Stream->PAR  = par;
  DMA_Stream->M0AR = mar;
  DMA_Stream->NDTR = ndtr;
}


/**
  \fn          void DMA_StreamEnable (DMA_Stream_TypeDef *DMA_Stream)
  \brief       Enable stream and/or start memory to memory transfer
  \param[in]   DMA_Stream  Pointer to DMA Stream peripheral
*/
__FORCE_INLINE void DMA_StreamEnable (DMA_Stream_TypeDef *DMA_Stream) {
  DMA_Stream->CR |=  DMA_SxCR_EN;
}

/**
  \fn          void DMA_StreamDisable (DMA_Stream_TypeDef *DMA_Stream)
  \brief       Disable stream and/or stop memory to memory transfer
  \param[in]   DMA_Stream  Pointer to DMA Stream peripheral
*/
__FORCE_INLINE void DMA_StreamDisable (DMA_Stream_TypeDef *DMA_Stream) {
  DMA_Stream->CR &= ~DMA_SxCR_EN;
}

/**
  \fn          bool DMA_StreamStatus (DMA_Stream_TypeDef *DMA_Stream)
  \brief       Check if stream is enabled or disabled
  \param[in]   DMA_Stream  Pointer to DMA Stream peripheral
  \return      Stream Status
               - \b true Enabled
               - \b false Disabled
*/
__FORCE_INLINE bool DMA_StreamStatus (DMA_Stream_TypeDef *DMA_Stream) {
  return (DMA_Stream->CR & DMA_SxCR_EN) ? true : false;
}

/**
  \fn          uint32_t DMA_StreamTransferItemCount (DMA_Stream_TypeDef *DMA_Stream)
  \brief       Get number of data items to transfer
  \param[in]   DMA_Stream  Pointer to DMA Stream peripheral
  \return      Number of data items to transfer
*/
__FORCE_INLINE uint32_t DMA_StreamTransferItemCount (DMA_Stream_TypeDef *DMA_Stream) {
  return DMA_Stream->NDTR;
}


#endif /* __DMA_STM32F4XX_H */
