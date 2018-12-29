/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        16. April 2013
 * $Revision:    V1.00
 *  
 * Project:      EXTI Driver for ST STM32F4xx
 * Configured:   via RTE_Device.h configuration file 
 * -------------------------------------------------------------------- */

#include "EXTI_STM32F4xx.h"

#include "RTE_Device.h"

#if (RTE_EXTI)

#define EXTI_PORT_MASK \
  ((RTE_EXTI0  ? 1 << RTE_EXTI0_PIN  : 0) | \
   (RTE_EXTI1  ? 1 << RTE_EXTI1_PIN  : 0) | \
   (RTE_EXTI2  ? 1 << RTE_EXTI2_PIN  : 0) | \
   (RTE_EXTI3  ? 1 << RTE_EXTI3_PIN  : 0) | \
   (RTE_EXTI4  ? 1 << RTE_EXTI4_PIN  : 0) | \
   (RTE_EXTI5  ? 1 << RTE_EXTI5_PIN  : 0) | \
   (RTE_EXTI6  ? 1 << RTE_EXTI6_PIN  : 0) | \
   (RTE_EXTI7  ? 1 << RTE_EXTI7_PIN  : 0) | \
   (RTE_EXTI8  ? 1 << RTE_EXTI8_PIN  : 0) | \
   (RTE_EXTI9  ? 1 << RTE_EXTI9_PIN  : 0) | \
   (RTE_EXTI10 ? 1 << RTE_EXTI10_PIN : 0) | \
   (RTE_EXTI11 ? 1 << RTE_EXTI11_PIN : 0) | \
   (RTE_EXTI12 ? 1 << RTE_EXTI12_PIN : 0) | \
   (RTE_EXTI13 ? 1 << RTE_EXTI13_PIN : 0) | \
   (RTE_EXTI14 ? 1 << RTE_EXTI14_PIN : 0) | \
   (RTE_EXTI15 ? 1 << RTE_EXTI15_PIN : 0))

#if (RTE_EXTI0 && (RTE_EXTI0_MODE == 0))
#ifndef RTE_EXT0_IRQ
#define RTE_EXT0_IRQ 1
#endif
#endif
#if (RTE_EXTI1 && (RTE_EXTI1_MODE == 0))
#ifndef RTE_EXT1_IRQ
#define RTE_EXT1_IRQ 1
#endif
#endif
#if (RTE_EXTI2 && (RTE_EXTI2_MODE == 0))
#ifndef RTE_EXT2_IRQ
#define RTE_EXT2_IRQ 1
#endif
#endif
#if (RTE_EXTI3 && (RTE_EXTI3_MODE == 0))
#ifndef RTE_EXT3_IRQ
#define RTE_EXT3_IRQ 1
#endif
#endif
#if (RTE_EXTI4 && (RTE_EXTI4_MODE == 0))
#ifndef RTE_EXT4_IRQ
#define RTE_EXT4_IRQ 1
#endif
#endif
#if (RTE_EXTI5 && (RTE_EXTI5_MODE == 0))
#ifndef RTE_EXT5_IRQ
#define RTE_EXT5_IRQ 1
#endif
#endif
#if (RTE_EXTI6 && (RTE_EXTI6_MODE == 0))
#ifndef RTE_EXT6_IRQ
#define RTE_EXT6_IRQ 1
#endif
#endif
#if (RTE_EXTI7 && (RTE_EXTI7_MODE == 0))
#ifndef RTE_EXT7_IRQ
#define RTE_EXT7_IRQ 1
#endif
#endif
#if (RTE_EXTI8 && (RTE_EXTI8_MODE == 0))
#ifndef RTE_EXT8_IRQ
#define RTE_EXT8_IRQ 1
#endif
#endif
#if (RTE_EXTI9 && (RTE_EXTI9_MODE == 0))
#ifndef RTE_EXT9_IRQ
#define RTE_EXT9_IRQ 1
#endif
#endif
#if (RTE_EXTI10 && (RTE_EXTI10_MODE == 0))
#ifndef RTE_EXT10_IRQ
#define RTE_EXT10_IRQ 1
#endif
#endif
#if (RTE_EXTI11 && (RTE_EXTI11_MODE == 0))
#ifndef RTE_EXT11_IRQ
#define RTE_EXT11_IRQ 1
#endif
#endif
#if (RTE_EXTI12 && (RTE_EXTI12_MODE == 0))
#ifndef RTE_EXT12_IRQ
#define RTE_EXT12_IRQ 1
#endif
#endif
#if (RTE_EXTI13 && (RTE_EXTI13_MODE == 0))
#ifndef RTE_EXT13_IRQ
#define RTE_EXT13_IRQ 1
#endif
#endif
#if (RTE_EXTI14 && (RTE_EXTI14_MODE == 0))
#ifndef RTE_EXT14_IRQ
#define RTE_EXT14_IRQ 1
#endif
#endif
#if (RTE_EXTI15 && (RTE_EXTI15_MODE == 0))
#ifndef RTE_EXT15_IRQ
#define RTE_EXT15_IRQ 1
#endif
#endif
#if (RTE_EXTI16 && (RTE_EXTI16_MODE == 0))
#ifndef RTE_EXT16_IRQ
#define RTE_EXT16_IRQ 1
#endif
#endif
#if (RTE_EXTI17 && (RTE_EXTI17_MODE == 0))
#ifndef RTE_EXT17_IRQ
#define RTE_EXT17_IRQ 1
#endif
#endif
#if (RTE_EXTI18 && (RTE_EXTI18_MODE == 0))
#ifndef RTE_EXT18_IRQ
#define RTE_EXT18_IRQ 1
#endif
#endif
#if (RTE_EXTI19 && (RTE_EXTI19_MODE == 0))
#ifndef RTE_EXT19_IRQ
#define RTE_EXT19_IRQ 1
#endif
#endif
#if (RTE_EXTI20 && (RTE_EXTI20_MODE == 0))
#ifndef RTE_EXT20_IRQ
#define RTE_EXT20_IRQ 1
#endif
#endif
#if (RTE_EXTI21 && (RTE_EXTI21_MODE == 0))
#ifndef RTE_EXT21_IRQ
#define RTE_EXT21_IRQ 1
#endif
#endif
#if (RTE_EXTI22 && (RTE_EXTI22_MODE == 0))
#ifndef RTE_EXT22_IRQ
#define RTE_EXT22_IRQ 1
#endif
#endif

#endif /* (RTE_EXTI != 0) */


/**
  \fn          void EXTI_Setup (void)
  \brief       Setup EXTI
*/
void EXTI_Setup (void) {
#if (RTE_EXTI)

  /* Enable System configuration controller clock */
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  /* Enable GPIO clock */
#if (EXTI_PORT_MASK != 0)
  RCC->AHB1ENR |= EXTI_PORT_MASK;
#endif

  /* Configure EXTI lines */
#if (RTE_EXTI0)
  EXTI_ConfigurePin (0,  (EXTI_PIN) RTE_EXTI0_PIN);
  EXTI_ConfigureLine(0,  (EXTI_MODE)RTE_EXTI0_MODE,  (EXTI_TRIGGER)RTE_EXTI0_TRIGGER);
#endif
#if (RTE_EXTI1)
  EXTI_ConfigurePin (1,  (EXTI_PIN) RTE_EXTI1_PIN);
  EXTI_ConfigureLine(1,  (EXTI_MODE)RTE_EXTI1_MODE,  (EXTI_TRIGGER)RTE_EXTI1_TRIGGER);
#endif
#if (RTE_EXTI2)
  EXTI_ConfigurePin (2,  (EXTI_PIN) RTE_EXTI2_PIN);
  EXTI_ConfigureLine(2,  (EXTI_MODE)RTE_EXTI2_MODE,  (EXTI_TRIGGER)RTE_EXTI2_TRIGGER);
#endif
#if (RTE_EXTI3)
  EXTI_ConfigurePin (3,  (EXTI_PIN) RTE_EXTI3_PIN);
  EXTI_ConfigureLine(3,  (EXTI_MODE)RTE_EXTI3_MODE,  (EXTI_TRIGGER)RTE_EXTI3_TRIGGER);
#endif
#if (RTE_EXTI4)
  EXTI_ConfigurePin (4,  (EXTI_PIN) RTE_EXTI4_PIN);
  EXTI_ConfigureLine(4,  (EXTI_MODE)RTE_EXTI4_MODE,  (EXTI_TRIGGER)RTE_EXTI4_TRIGGER);
#endif
#if (RTE_EXTI5)
  EXTI_ConfigurePin (5,  (EXTI_PIN) RTE_EXTI5_PIN);
  EXTI_ConfigureLine(5,  (EXTI_MODE)RTE_EXTI5_MODE,  (EXTI_TRIGGER)RTE_EXTI5_TRIGGER);
#endif
#if (RTE_EXTI6)
  EXTI_ConfigurePin (6,  (EXTI_PIN) RTE_EXTI6_PIN);
  EXTI_ConfigureLine(6,  (EXTI_MODE)RTE_EXTI6_MODE,  (EXTI_TRIGGER)RTE_EXTI6_TRIGGER);
#endif
#if (RTE_EXTI7)
  EXTI_ConfigurePin (7,  (EXTI_PIN) RTE_EXTI7_PIN);
  EXTI_ConfigureLine(7,  (EXTI_MODE)RTE_EXTI7_MODE,  (EXTI_TRIGGER)RTE_EXTI7_TRIGGER);
#endif
#if (RTE_EXTI8)
  EXTI_ConfigurePin (8,  (EXTI_PIN) RTE_EXTI8_PIN);
  EXTI_ConfigureLine(8,  (EXTI_MODE)RTE_EXTI8_MODE,  (EXTI_TRIGGER)RTE_EXTI8_TRIGGER);
#endif
#if (RTE_EXTI9)
  EXTI_ConfigurePin (9,  (EXTI_PIN) RTE_EXTI9_PIN);
  EXTI_ConfigureLine(9,  (EXTI_MODE)RTE_EXTI9_MODE,  (EXTI_TRIGGER)RTE_EXTI9_TRIGGER);
#endif
#if (RTE_EXTI10)
  EXTI_ConfigurePin (10, (EXTI_PIN) RTE_EXTI10_PIN);
  EXTI_ConfigureLine(10, (EXTI_MODE)RTE_EXTI10_MODE, (EXTI_TRIGGER)RTE_EXTI10_TRIGGER);
#endif
#if (RTE_EXTI11)
  EXTI_ConfigurePin (11, (EXTI_PIN) RTE_EXTI11_PIN);
  EXTI_ConfigureLine(11, (EXTI_MODE)RTE_EXTI11_MODE, (EXTI_TRIGGER)RTE_EXTI11_TRIGGER);
#endif
#if (RTE_EXTI12)
  EXTI_ConfigurePin (12, (EXTI_PIN) RTE_EXTI12_PIN);
  EXTI_ConfigureLine(12, (EXTI_MODE)RTE_EXTI12_MODE, (EXTI_TRIGGER)RTE_EXTI12_TRIGGER);
#endif
#if (RTE_EXTI13)
  EXTI_ConfigurePin (13, (EXTI_PIN) RTE_EXTI13_PIN);
  EXTI_ConfigureLine(13, (EXTI_MODE)RTE_EXTI13_MODE, (EXTI_TRIGGER)RTE_EXTI13_TRIGGER);
#endif
#if (RTE_EXTI14)
  EXTI_ConfigurePin (14, (EXTI_PIN) RTE_EXTI14_PIN);
  EXTI_ConfigureLine(14, (EXTI_MODE)RTE_EXTI14_MODE, (EXTI_TRIGGER)RTE_EXTI14_TRIGGER);
#endif
#if (RTE_EXTI15)
  EXTI_ConfigurePin (15, (EXTI_PIN) RTE_EXTI15_PIN);
  EXTI_ConfigureLine(15, (EXTI_MODE)RTE_EXTI15_MODE, (EXTI_TRIGGER)RTE_EXTI15_TRIGGER);
#endif

#if (RTE_EXTI16)
  EXTI_ConfigureLine(16, (EXTI_MODE)RTE_EXTI16_MODE, (EXTI_TRIGGER)RTE_EXTI16_TRIGGER);
#endif
#if (RTE_EXTI17)
  EXTI_ConfigureLine(17, (EXTI_MODE)RTE_EXTI17_MODE, (EXTI_TRIGGER)RTE_EXTI17_TRIGGER);
#endif
#if (RTE_EXTI18)
  EXTI_ConfigureLine(18, (EXTI_MODE)RTE_EXTI18_MODE, (EXTI_TRIGGER)RTE_EXTI18_TRIGGER);
#endif
#if (RTE_EXTI19)
  EXTI_ConfigureLine(19, (EXTI_MODE)RTE_EXTI19_MODE, (EXTI_TRIGGER)RTE_EXTI19_TRIGGER);
#endif
#if (RTE_EXTI20)
  EXTI_ConfigureLine(20, (EXTI_MODE)RTE_EXTI20_MODE, (EXTI_TRIGGER)RTE_EXTI20_TRIGGER);
#endif
#if (RTE_EXTI21)
  EXTI_ConfigureLine(21, (EXTI_MODE)RTE_EXTI21_MODE, (EXTI_TRIGGER)RTE_EXTI21_TRIGGER);
#endif
#if (RTE_EXTI22)
  EXTI_ConfigureLine(22, (EXTI_MODE)RTE_EXTI22_MODE, (EXTI_TRIGGER)RTE_EXTI22_TRIGGER);
#endif

  /* Enable EXTI interrupts */
#if (RTE_EXT0_IRQ)
  NVIC_EnableIRQ(EXTI0_IRQn);
#endif
#if (RTE_EXT1_IRQ)
  NVIC_EnableIRQ(EXTI1_IRQn);
#endif
#if (RTE_EXT2_IRQ)
  NVIC_EnableIRQ(EXTI2_IRQn);
#endif
#if (RTE_EXT3_IRQ)
  NVIC_EnableIRQ(EXTI3_IRQn);
#endif
#if (RTE_EXT4_IRQ)
  NVIC_EnableIRQ(EXTI4_IRQn);
#endif
#if (RTE_EXT5_IRQ  || \
     RTE_EXT6_IRQ  || \
     RTE_EXT7_IRQ  || \
     RTE_EXT8_IRQ  || \
     RTE_EXT9_IRQ)
  NVIC_EnableIRQ(EXTI9_5_IRQn);
#endif
#if (RTE_EXT10_IRQ || \
     RTE_EXT11_IRQ || \
     RTE_EXT12_IRQ || \
     RTE_EXT13_IRQ || \
     RTE_EXT14_IRQ || \
     RTE_EXT15_IRQ)
  NVIC_EnableIRQ(EXTI15_10_IRQn);
#endif
#if (RTE_EXT16_IRQ)
  NVIC_EnableIRQ(PVD_IRQn);
#endif
#if (RTE_EXT17_IRQ)
  NVIC_EnableIRQ(RTC_Alarm_IRQn);
#endif
#if (RTE_EXT18_IRQ)
  NVIC_EnableIRQ(OTG_FS_WKUP_IRQn);
#endif
#if (RTE_EXT19_IRQ)
  NVIC_EnableIRQ(ETH_WKUP_IRQn);
#endif
#if (RTE_EXT20_IRQ)
  NVIC_EnableIRQ(OTG_HS_WKUP_IRQn);
#endif
#if (RTE_EXT21_IRQ)
  NVIC_EnableIRQ(TAMP_STAMP_IRQn);
#endif
#if (RTE_EXT22_IRQ)
  NVIC_EnableIRQ(RTC_WKUP_IRQn);
#endif

#endif
}

/**
  \fn          void EXTI_ConfigurePin (uint32_t line, EXTI_PIN pin)
  \brief       Configure EXTI pin
  \param[in]   line  Line number
  \param[in]   pin   ref EXTI_PIN
  \return      true  - success
               false - error
*/
bool EXTI_ConfigurePin (uint32_t line, EXTI_PIN pin) {
  uint32_t n, m;

  if (line >= 16) return false;
  if (line >= 12) {
    if (pin > EXTI_PIN_PH) return false;
  } else {
    if (pin > EXTI_PIN_PI) return false;
  }
  n =  line / 4;
  m = (line % 4) * 4;
  SYSCFG->EXTICR[n] = (SYSCFG->EXTICR[n] & ~(0xF << m)) | (pin << m);

  return true;
}

/**
  \fn          void EXTI_ConfigureLine (uint32_t     line,
                                        EXTI_MODE    mode,
                                        EXTI_TRIGGER trigger)
  \brief       Configure EXTI line
  \param[in]   line     Line number
  \param[in]   mode     \ref EXTI_MODE
  \param[in]   trigger  \ref EXTI_TRIGGER
*/
void EXTI_ConfigureLine (uint32_t     line,
                         EXTI_MODE    mode,
                         EXTI_TRIGGER trigger) {
  uint32_t mask;

  mask = 1UL << line;

  switch (mode) {
    case EXTI_MODE_INTERRUPT:
      EXTI->IMR |=  mask;
      EXTI->EMR &= ~mask;
      break;
    case EXTI_MODE_EVENT:
      EXTI->IMR &= ~mask;
      EXTI->EMR |=  mask;
      break;
  }

  switch (trigger) {
    case EXTI_TRIGGER_NONE:
      EXTI->RTSR &= ~mask;
      EXTI->FTSR &=  mask;
      break;
    case EXTI_TRIGGER_RISING:
      EXTI->RTSR |=  mask;
      EXTI->FTSR &= ~mask;
      break;
    case EXTI_TRIGGER_FALLING:
      EXTI->RTSR &= ~mask;
      EXTI->FTSR |=  mask;
      break;
    case EXTI_TRIGGER_RISING_FALLING:
      EXTI->RTSR |=  mask;
      EXTI->FTSR |=  mask;
      break;
  }
}


#if (RTE_EXT0_IRQ)
__weak void EXTI0_Event (void) {};

/**
  \fn          void EXTI0_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void EXTI0_IRQHandler (void) {
  EXTI->PR = 1UL << 0;
  EXTI0_Event();
}

#endif


#if (RTE_EXT1_IRQ)
__weak void EXTI1_Event (void) {};

/**
  \fn          void EXTI1_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void EXTI1_IRQHandler (void) {
  EXTI->PR = 1UL << 1;
  EXTI1_Event();
}

#endif


#if (RTE_EXT2_IRQ)
__weak void EXTI2_Event (void) {};

/**
  \fn          void EXTI2_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void EXTI2_IRQHandler (void) {
  EXTI->PR = 1UL << 2;
  EXTI2_Event();
}

#endif


#if (RTE_EXT3_IRQ)
__weak void EXTI3_Event (void) {};

/**
  \fn          void EXTI3_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void EXTI3_IRQHandler (void) {
  EXTI->PR = 1UL << 3;
  EXTI3_Event();
}

#endif


#if (RTE_EXT4_IRQ)
__weak void EXTI4_Event (void) {};

/**
  \fn          void EXTI4_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void EXTI4_IRQHandler (void) {
  EXTI->PR = 1UL << 4;
  EXTI4_Event();
}

#endif


#if (RTE_EXT5_IRQ || \
     RTE_EXT6_IRQ || \
     RTE_EXT7_IRQ || \
     RTE_EXT8_IRQ || \
     RTE_EXT9_IRQ)

#if (RTE_EXT5_IRQ)
__weak void EXTI5_Event (void) {};
#endif
#if (RTE_EXT6_IRQ)
__weak void EXTI6_Event (void) {};
#endif
#if (RTE_EXT7_IRQ)
__weak void EXTI7_Event (void) {};
#endif
#if (RTE_EXT8_IRQ)
__weak void EXTI8_Event (void) {};
#endif
#if (RTE_EXT9_IRQ)
__weak void EXTI9_Event (void) {};
#endif

/**
  \fn          void EXTI9_5_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void EXTI9_5_IRQHandler (void) {
  uint32_t events;
  
  events = EXTI->PR & (0x1F << 5);
  EXTI->PR = events;

#if (RTE_EXT5_IRQ)
  if (events & (1UL << 5)) EXTI5_Event();
#endif
#if (RTE_EXT6_IRQ)
  if (events & (1UL << 6)) EXTI6_Event();
#endif
#if (RTE_EXT7_IRQ)
  if (events & (1UL << 7)) EXTI7_Event();
#endif
#if (RTE_EXT8_IRQ)
  if (events & (1UL << 8)) EXTI8_Event();
#endif
#if (RTE_EXT9_IRQ)
  if (events & (1UL << 9)) EXTI9_Event();
#endif
}

#endif


#if (RTE_EXT10_IRQ || \
     RTE_EXT11_IRQ || \
     RTE_EXT12_IRQ || \
     RTE_EXT13_IRQ || \
     RTE_EXT14_IRQ || \
     RTE_EXT15_IRQ)

#if (RTE_EXT10_IRQ)
__weak void EXTI10_Event (void) {};
#endif
#if (RTE_EXT11_IRQ)
__weak void EXTI11_Event (void) {};
#endif
#if (RTE_EXT12_IRQ)
__weak void EXTI12_Event (void) {};
#endif
#if (RTE_EXT13_IRQ)
__weak void EXTI13_Event (void) {};
#endif
#if (RTE_EXT14_IRQ)
__weak void EXTI14_Event (void) {};
#endif
#if (RTE_EXT15_IRQ)
__weak void EXTI15_Event (void) {};
#endif

/**
  \fn          void EXTI15_10_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void EXTI15_10_IRQHandler (void) {
  uint32_t events;
  
  events = EXTI->PR & (0x3F << 10);
  EXTI->PR = events;

#if (RTE_EXT10_IRQ)
  if (events & (1UL << 10)) EXTI10_Event();
#endif
#if (RTE_EXT11_IRQ)
  if (events & (1UL << 11)) EXTI11_Event();
#endif
#if (RTE_EXT12_IRQ)
  if (events & (1UL << 12)) EXTI12_Event();
#endif
#if (RTE_EXT13_IRQ)
  if (events & (1UL << 13)) EXTI13_Event();
#endif
#if (RTE_EXT14_IRQ)
  if (events & (1UL << 14)) EXTI14_Event();
#endif
#if (RTE_EXT15_IRQ)
  if (events & (1UL << 15)) EXTI15_Event();
#endif
}

#endif


#if (RTE_EXT16_IRQ)
__weak void EXTI16_Event (void) {};

/**
  \fn          void PVD_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void PVD_IRQHandler (void) {
  EXTI->PR = 1UL << 16;
  EXTI16_Event();
}

#endif


#if (RTE_EXT17_IRQ)
__weak void EXTI17_Event (void) {};

/**
  \fn          void RTC_Alarm_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void RTC_Alarm_IRQHandler (void) {
  EXTI->PR = 1UL << 17;
  EXTI17_Event();
}

#endif


#if (RTE_EXT18_IRQ)
__weak void EXTI18_Event (void) {};

/**
  \fn          void OTG_FS_WKUP_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void OTG_FS_WKUP_IRQHandler (void) {
  EXTI->PR = 1UL << 18;
  EXTI18_Event();
}

#endif


#if (RTE_EXT19_IRQ)
__weak void EXTI19_Event (void) {};

/**
  \fn          void ETH_WKUP_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void ETH_WKUP_IRQHandler (void) {
  EXTI->PR = 1UL << 19;
  EXTI19_Event();
}

#endif


#if (RTE_EXT20_IRQ)
__weak void EXTI20_Event (void) {};

/**
  \fn          void PVD_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void OTG_HS_WKUP_IRQHandler (void) {
  EXTI->PR = 1UL << 20;
  EXTI20_Event();
}

#endif


#if (RTE_EXT21_IRQ)
__weak void EXTI21_Event (void) {};

/**
  \fn          void TAMP_STAMP_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void TAMP_STAMP_IRQHandler (void) {
  EXTI->PR = 1UL << 21;
  EXTI21_Event();
}

#endif


#if (RTE_EXT22_IRQ)
__weak void EXTI22_Event (void) {};

/**
  \fn          void RTC_WKUP_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void RTC_WKUP_IRQHandler (void) {
  EXTI->PR = 1UL << 22;
  EXTI22_Event();
}

#endif
