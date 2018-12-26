/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        12. March 2013
 * $Revision:    V1.00
 *  
 * Project:      EXTI Driver definitions for ST STM32F4xx
 * -------------------------------------------------------------------- */

#ifndef __EXTI_STM32F4XX_H
#define __EXTI_STM32F4XX_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"


#if defined (__CC_ARM)
#define __FORCE_INLINE  static __forceinline
#else
#define __FORCE_INLINE  __STATIC_INLINE
#endif


/// EXTI Pin
typedef enum _EXTI_PIN {
  EXTI_PIN_PA,
  EXTI_PIN_PB,
  EXTI_PIN_PC,
  EXTI_PIN_PD,
  EXTI_PIN_PE,
  EXTI_PIN_PF,
  EXTI_PIN_PG,
  EXTI_PIN_PH,
  EXTI_PIN_PI,
} EXTI_PIN;

/// EXTI Mode
typedef enum _EXTI_MODE {
  EXTI_MODE_INTERRUPT,
  EXTI_MODE_EVENT
} EXTI_MODE;

/// EXTI Trigger
typedef enum _EXTI_TRIGGER {
  EXTI_TRIGGER_NONE,
  EXTI_TRIGGER_RISING,
  EXTI_TRIGGER_FALLING,
  EXTI_TRIGGER_RISING_FALLING
} EXTI_TRIGGER;


/**
  \fn          void EXTI_Setup (void)
  \brief       Setup EXTI
*/
extern void EXTI_Setup (void);

/**
  \fn          void EXTI_ConfigurePin (uint32_t line, EXTI_PIN pin)
  \brief       Configure EXTI pin
  \param[in]   line  Line number
  \param[in]   pin   ref EXTI_PIN
  \return      true  - success
               false - error
*/
extern bool EXTI_ConfigurePin (uint32_t line, EXTI_PIN pin);

/**
  \fn          void EXTI_ConfigureLine (uint32_t     line,
                                        EXTI_MODE    mode,
                                        EXTI_TRIGGER trigger)
  \brief       Configure EXTI line
  \param[in]   line     Line number
  \param[in]   mode     \ref EXTI_MODE
  \param[in]   trigger  \ref EXTI_TRIGGER
*/
extern void EXTI_ConfigureLine (uint32_t     line,
                                EXTI_MODE    mode,
                                EXTI_TRIGGER trigger);

/**
  \fn          void EXTI_ReadPendingBit (uint32_t line)
  \brief       Read pending bit
  \param[in]   line  Line number
  \return      bit value (0 or 1)
*/
__FORCE_INLINE bool EXTI_ReadPendingBit (uint32_t line) {
  return ((EXTI->PR & (1UL << line)) != 0);
}

/**
  \fn          void EXTI_ClearPendingBit (uint32_t line)
  \brief       Clear pending bit
  \param[in]   line  Line number
*/
__FORCE_INLINE void EXTI_ClearPendingBit (uint32_t line) {
  EXTI->PR = 1UL << line;
}

/**
  \fn          void EXTI_SoftwareTrigger (uint32_t line)
  \brief       Generate Software trigger
  \param[in]   line  Line number
*/
__FORCE_INLINE void EXTI_SoftwareTrigger (uint32_t line) {
  EXTI->SWIER = 1UL << line;
}


#endif /* __EXTI_STM32F4XX_H */
