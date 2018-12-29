/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        19. April 2013
 * $Revision:    V1.01
 *  
 * Project:      DMA Driver for ST STM32F4xx
 * -------------------------------------------------------------------- */

/* History:
 *  Version 1.01
 *    Changed order of operations in IRQ functions:
 *      IRQ flag cleared first then callback is executed
 *    Added functions: DMA_StreamStatus, DMA_StreamTransferItemCount  
 *  Version 1.00
 *    Initial release
 */ 

#include "DMA_STM32F4xx.h"


/// DMA Variables
static uint8_t DMA1_Stream;             // Used DMA1 Streams
static uint8_t DMA2_Stream;             // Used DMA2 Streams


/// DMA Functions

/**
  \fn          void DMA_StreamInitialize (uint32_t dma, uint32_t stream)
  \brief       Initialize DMA Stream
  \param[in]   dma     DMA number
  \param[in]   stream  DMA stream number
*/
void DMA_StreamInitialize (uint32_t dma, uint32_t stream) {
  
  switch (dma) {
    case 1:
      RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
      switch (stream) {
        case 0:
          DMA1_Stream0->CR = 0;
          DMA1->LIFCR = DMA_LIFCR_CFEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CTEIF0 |
                        DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTCIF0;
          NVIC_ClearPendingIRQ(DMA1_Stream0_IRQn);
          NVIC_EnableIRQ(DMA1_Stream0_IRQn);
          break;
        case 1:
          DMA1_Stream1->CR = 0;
          DMA1->LIFCR = DMA_LIFCR_CFEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CTEIF1 |
                        DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTCIF1;
          NVIC_ClearPendingIRQ(DMA1_Stream1_IRQn);
          NVIC_EnableIRQ(DMA1_Stream1_IRQn);
          break;
        case 2:
          DMA1_Stream2->CR = 0;
          DMA1->LIFCR = DMA_LIFCR_CFEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CTEIF2 |
                        DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTCIF2;
          NVIC_ClearPendingIRQ(DMA1_Stream2_IRQn);
          NVIC_EnableIRQ(DMA1_Stream2_IRQn);
          break;
        case 3:
          DMA1_Stream3->CR = 0;
          DMA1->LIFCR = DMA_LIFCR_CFEIF3 | DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CTEIF3 |
                        DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTCIF3;
          NVIC_ClearPendingIRQ(DMA1_Stream3_IRQn);
          NVIC_EnableIRQ(DMA1_Stream3_IRQn);
          break;
        case 4:
          DMA1_Stream4->CR = 0;
          DMA1->LIFCR = DMA_HIFCR_CFEIF4 | DMA_HIFCR_CDMEIF4 | DMA_HIFCR_CTEIF4 |
                        DMA_HIFCR_CHTIF4 | DMA_HIFCR_CTCIF4;
          NVIC_ClearPendingIRQ(DMA1_Stream4_IRQn);
          NVIC_EnableIRQ(DMA1_Stream4_IRQn);
          break;
        case 5:
          DMA1_Stream5->CR = 0;
          DMA1->LIFCR = DMA_HIFCR_CFEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CTEIF5 |
                        DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTCIF5;
          NVIC_ClearPendingIRQ(DMA1_Stream5_IRQn);
          NVIC_EnableIRQ(DMA1_Stream5_IRQn);
          break;
        case 6:
          DMA1_Stream6->CR = 0;
          DMA1->LIFCR = DMA_HIFCR_CFEIF6 | DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CTEIF5 |
                        DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTCIF6;
          NVIC_ClearPendingIRQ(DMA1_Stream6_IRQn);
          NVIC_EnableIRQ(DMA1_Stream6_IRQn);
          break;
        case 7:
          DMA1_Stream7->CR = 0;
          DMA1->LIFCR = DMA_HIFCR_CFEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CTEIF6 |
                        DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTCIF7;
          NVIC_ClearPendingIRQ(DMA1_Stream7_IRQn);
          NVIC_EnableIRQ(DMA1_Stream7_IRQn);
          break;
      }
      DMA1_Stream |= 1 << stream;
      break;
    case 2:
      RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
      switch (stream) {
        case 0:
          DMA2_Stream0->CR = 0;
          DMA2->LIFCR = DMA_LIFCR_CFEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CTEIF0 |
                        DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTCIF0;
          NVIC_ClearPendingIRQ(DMA2_Stream0_IRQn);
          NVIC_EnableIRQ(DMA2_Stream0_IRQn);
          break;
        case 1:
          DMA2_Stream1->CR = 0;
          DMA2->LIFCR = DMA_LIFCR_CFEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CTEIF1 |
                        DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTCIF1;
          NVIC_ClearPendingIRQ(DMA2_Stream1_IRQn);
          NVIC_EnableIRQ(DMA2_Stream1_IRQn);
          break;
        case 2:
          DMA2_Stream2->CR = 0;
          DMA2->LIFCR = DMA_LIFCR_CFEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CTEIF2 |
                        DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTCIF2;
          NVIC_ClearPendingIRQ(DMA2_Stream2_IRQn);
          NVIC_EnableIRQ(DMA2_Stream2_IRQn);
          break;
        case 3:
          DMA2_Stream3->CR = 0;
          DMA2->LIFCR = DMA_LIFCR_CFEIF3 | DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CTEIF3 |
                        DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTCIF3;
          NVIC_EnableIRQ(DMA2_Stream3_IRQn);
          break;
        case 4:
          DMA2_Stream4->CR = 0;
          DMA2->LIFCR = DMA_HIFCR_CFEIF4 | DMA_HIFCR_CDMEIF4 | DMA_HIFCR_CTEIF4 |
                        DMA_HIFCR_CHTIF4 | DMA_HIFCR_CTCIF4;
          NVIC_ClearPendingIRQ(DMA2_Stream4_IRQn);
          NVIC_EnableIRQ(DMA2_Stream4_IRQn);
          break;
        case 5:
          DMA2_Stream5->CR = 0;
          DMA2->LIFCR = DMA_HIFCR_CFEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CTEIF5 |
                        DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTCIF5;
          NVIC_ClearPendingIRQ(DMA2_Stream5_IRQn);
          NVIC_EnableIRQ(DMA2_Stream5_IRQn);
          break;
        case 6:
          DMA2_Stream6->CR = 0;
          DMA2->LIFCR = DMA_HIFCR_CFEIF6 | DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CTEIF5 |
                        DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTCIF6;
          NVIC_ClearPendingIRQ(DMA2_Stream6_IRQn);
          NVIC_EnableIRQ(DMA2_Stream6_IRQn);
          break;
        case 7:
          DMA2_Stream7->CR = 0;
          DMA2->LIFCR = DMA_HIFCR_CFEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CTEIF6 |
                        DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTCIF7;
          NVIC_ClearPendingIRQ(DMA2_Stream7_IRQn);
          NVIC_EnableIRQ(DMA2_Stream7_IRQn);
          break;
      }
      DMA2_Stream |= 1 << stream;
      break;
  }
}

/**
  \fn          void DMA_StreamUninitialize (uint32_t dma, uint32_t stream)
  \brief       Uninitialize DMA Stream
  \param[in]   dma     DMA number
  \param[in]   stream  DMA stream number
*/
void DMA_StreamUninitialize (uint32_t dma, uint32_t stream) {

  switch (dma) {
    case 1:
      switch (stream) {
        case 0:
          NVIC_DisableIRQ(DMA1_Stream0_IRQn);
          DMA1_Stream0->CR = 0;
          break;
        case 1:
          NVIC_DisableIRQ(DMA1_Stream1_IRQn);
          DMA1_Stream1->CR = 0;
          break;
        case 2:
          NVIC_DisableIRQ(DMA1_Stream2_IRQn);
          DMA1_Stream2->CR = 0;
          break;
        case 3:
          NVIC_DisableIRQ(DMA1_Stream3_IRQn);
          DMA1_Stream3->CR = 0;
          break;
        case 4:
          NVIC_DisableIRQ(DMA1_Stream4_IRQn);
          DMA1_Stream4->CR = 0;
          break;
        case 5:
          NVIC_DisableIRQ(DMA1_Stream5_IRQn);
          DMA1_Stream5->CR = 0;
          break;
        case 6:
          NVIC_DisableIRQ(DMA1_Stream6_IRQn);
          DMA1_Stream6->CR = 0;
          break;
        case 7:
          NVIC_DisableIRQ(DMA1_Stream7_IRQn);
          DMA1_Stream7->CR = 0;
          break;
      }
      DMA1_Stream &= ~(1 << stream);
      if (DMA1_Stream == 0){
        RCC->AHB1ENR &= ~RCC_AHB1ENR_DMA1EN;
      }
      break;
    case 2:
      switch (stream) {
        case 0:
          NVIC_DisableIRQ(DMA2_Stream0_IRQn);
          DMA2_Stream0->CR = 0;
          break;
        case 1:
          NVIC_DisableIRQ(DMA2_Stream1_IRQn);
          DMA2_Stream1->CR = 0;
          break;
        case 2:
          NVIC_DisableIRQ(DMA2_Stream2_IRQn);
          DMA2_Stream2->CR = 0;
          break;
        case 3:
          NVIC_DisableIRQ(DMA2_Stream3_IRQn);
          DMA2_Stream3->CR = 0;
          break;
        case 4:
          NVIC_DisableIRQ(DMA2_Stream4_IRQn);
          DMA2_Stream4->CR = 0;
          break;
        case 5:
          NVIC_DisableIRQ(DMA2_Stream5_IRQn);
          DMA2_Stream5->CR = 0;
          break;
        case 6:
          NVIC_DisableIRQ(DMA2_Stream6_IRQn);
          DMA2_Stream6->CR = 0;
          break;
        case 7:
          NVIC_DisableIRQ(DMA2_Stream7_IRQn);
          DMA2_Stream7->CR = 0;
          break;
      }
      DMA2_Stream &= ~(1 << stream);
      if (DMA2_Stream == 0){
        RCC->AHB1ENR &= ~RCC_AHB1ENR_DMA2EN;
      }
      break;
  }
}


/// DMA1 Stream Events
__weak void DMA1_Stream0_Event (uint32_t event) {};
__weak void DMA1_Stream1_Event (uint32_t event) {};
__weak void DMA1_Stream2_Event (uint32_t event) {};
__weak void DMA1_Stream3_Event (uint32_t event) {};
__weak void DMA1_Stream4_Event (uint32_t event) {};
__weak void DMA1_Stream5_Event (uint32_t event) {};
__weak void DMA1_Stream6_Event (uint32_t event) {};
__weak void DMA1_Stream7_Event (uint32_t event) {};

/// DMA2 Stream Events
__weak void DMA2_Stream0_Event (uint32_t event) {};
__weak void DMA2_Stream1_Event (uint32_t event) {};
__weak void DMA2_Stream2_Event (uint32_t event) {};
__weak void DMA2_Stream3_Event (uint32_t event) {};
__weak void DMA2_Stream4_Event (uint32_t event) {};
__weak void DMA2_Stream5_Event (uint32_t event) {};
__weak void DMA2_Stream6_Event (uint32_t event) {};
__weak void DMA2_Stream7_Event (uint32_t event) {};


/**
  \fn          void DMA1_Stream0_IRQHandler (void)
  \brief       DMA1 Stream0 interrupt handler
*/
void DMA1_Stream0_IRQHandler (void) {
  uint32_t events;

  events = (DMA1->LISR >>  0) & DMA_STREAM_FLAGS;
  DMA1->LIFCR = events <<  0;
  DMA1_Stream0_Event(events);
}

/**
  \fn          void DMA1_Stream1_IRQHandler (void)
  \brief       DMA1 Stream1 interrupt handler
*/
void DMA1_Stream1_IRQHandler (void) {
  uint32_t events;

  events = (DMA1->LISR >>  6) & DMA_STREAM_FLAGS;
  DMA1->LIFCR = events <<  6;
  DMA1_Stream1_Event(events);
}

/**
  \fn          void DMA1_Stream2_IRQHandler (void)
  \brief       DMA1 Stream2 interrupt handler
*/
void DMA1_Stream2_IRQHandler (void) {
  uint32_t events;

  events = (DMA1->LISR >> 16) & DMA_STREAM_FLAGS;
  DMA1->LIFCR = events << 16;
  DMA1_Stream2_Event(events);
}

/**
  \fn          void DMA1_Stream3_IRQHandler (void)
  \brief       DMA1 Stream3 interrupt handler
*/
void DMA1_Stream3_IRQHandler (void) {
  uint32_t events;

  events = (DMA1->LISR >> 22) & DMA_STREAM_FLAGS;
  DMA1->LIFCR = events << 22;
  DMA1_Stream3_Event(events);
}

/**
  \fn          void DMA1_Stream4_IRQHandler (void)
  \brief       DMA1 Stream4 interrupt handler
*/
void DMA1_Stream4_IRQHandler (void) {
  uint32_t events;

  events = (DMA1->HISR >>  0) & DMA_STREAM_FLAGS;
  DMA1->HIFCR = events <<  0;
  DMA1_Stream4_Event(events);
}

/**
  \fn          void DMA1_Stream5_IRQHandler (void)
  \brief       DMA1 Stream5 interrupt handler
*/
void DMA1_Stream5_IRQHandler (void) {
  uint32_t events;

  events = (DMA1->HISR >>  6) & DMA_STREAM_FLAGS;
  DMA1->HIFCR = events <<  6;
  DMA1_Stream5_Event(events);
}

/**
  \fn          void DMA1_Stream6_IRQHandler (void)
  \brief       DMA1 Stream6 interrupt handler
*/
void DMA1_Stream6_IRQHandler (void) {
  uint32_t events;

  events = (DMA1->HISR >> 16) & DMA_STREAM_FLAGS;
  DMA1->HIFCR = events << 16;
  DMA1_Stream6_Event(events);
}

/**
  \fn          void DMA1_Stream7_IRQHandler (void)
  \brief       DMA1 Stream7 interrupt handler
*/
void DMA1_Stream7_IRQHandler (void) {
  uint32_t events;

  events = (DMA1->HISR >> 22) & DMA_STREAM_FLAGS;
  DMA1->HIFCR = events << 22;
  DMA1_Stream7_Event(events);
}

/**
  \fn          void DMA2_Stream0_IRQHandler (void)
  \brief       DMA2 Stream0 interrupt handler
*/
void DMA2_Stream0_IRQHandler (void) {
  uint32_t events;

  events = (DMA2->LISR >>  0) & DMA_STREAM_FLAGS;
  DMA2->LIFCR = events <<  0;
  DMA2_Stream0_Event(events);
}

/**
  \fn          void DMA2_Stream1_IRQHandler (void)
  \brief       DMA2 Stream1 interrupt handler
*/
void DMA2_Stream1_IRQHandler (void) {
  uint32_t events;

  events = (DMA2->LISR >>  6) & DMA_STREAM_FLAGS;
  DMA2->LIFCR = events <<  6;
  DMA2_Stream1_Event(events);
}

/**
  \fn          void DMA2_Stream2_IRQHandler (void)
  \brief       DMA2 Stream2 interrupt handler
*/
void DMA2_Stream2_IRQHandler (void) {
  uint32_t events;

  events = (DMA2->LISR >> 16) & DMA_STREAM_FLAGS;
  DMA2->LIFCR = events << 16;
  DMA2_Stream2_Event(events);
}

/**
  \fn          void DMA2_Stream3_IRQHandler (void)
  \brief       DMA2 Stream3 interrupt handler
*/
void DMA2_Stream3_IRQHandler (void) {
  uint32_t events;

  events = (DMA2->LISR >> 22) & DMA_STREAM_FLAGS;
  DMA2->LIFCR = events << 22;
  DMA2_Stream3_Event(events);
}

/**
  \fn          void DMA2_Stream4_IRQHandler (void)
  \brief       DMA2 Stream4 interrupt handler
*/
void DMA2_Stream4_IRQHandler (void) {
  uint32_t events;

  events = (DMA2->HISR >>  0) & DMA_STREAM_FLAGS;
  DMA2->HIFCR = events <<  0;
  DMA2_Stream4_Event(events);
}

/**
  \fn          void DMA2_Stream5_IRQHandler (void)
  \brief       DMA2 Stream5 interrupt handler
*/
void DMA2_Stream5_IRQHandler (void) {
  uint32_t events;

  events = (DMA2->HISR >>  6) & DMA_STREAM_FLAGS;
  DMA2->HIFCR = events <<  6;
  DMA2_Stream5_Event(events);
}

/**
  \fn          void DMA2_Stream6_IRQHandler (void)
  \brief       DMA2 Stream6 interrupt handler
*/
void DMA2_Stream6_IRQHandler (void) {
  uint32_t events;

  events = (DMA2->HISR >> 16) & DMA_STREAM_FLAGS;
  DMA2->HIFCR = events << 16;  
  DMA2_Stream6_Event(events);
}

/**
  \fn          void DMA2_Stream7_IRQHandler (void)
  \brief       DMA2 Stream7 interrupt handler
*/
void DMA2_Stream7_IRQHandler (void) {
  uint32_t events;

  events = (DMA2->HISR >> 22) & DMA_STREAM_FLAGS;
  DMA2->HIFCR = events << 22;
  DMA2_Stream7_Event(events);
}
