/* ----------------------------------------------------------------------
 * Copyright (C) 2014 ARM Limited. All rights reserved.
 *  
 * $Date:        29. April 2014
 * $Revision:    V1.01
 *  
 * Project:      GPIO Driver definitions for ST STM32F4xx
 * -------------------------------------------------------------------- */

#ifndef __GPIO_STM32F4XX_H
#define __GPIO_STM32F4XX_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"


#if defined (__CC_ARM)
#define __FORCE_INLINE  static __forceinline
#else
#define __FORCE_INLINE  __STATIC_INLINE
#endif


/// GPIO Mode
typedef enum _GPIO_MODE {
  GPIO_MODE_INPUT               = 0,
  GPIO_MODE_OUTPUT              = 1,
  GPIO_MODE_AF                  = 2,
  GPIO_MODE_ANALOG              = 3
} GPIO_MODE;

/// GPIO Output Type
typedef enum _GPIO_OUTPUT_TYPE {
  GPIO_OUTPUT_PUSH_PULL         = 0,
  GPIO_OUTPUT_OPEN_DRAIN        = 1
} GPIO_OUTPUT_TYPE;

/// GPIO Output Speed
typedef enum _GPIO_OUTPUT_SPEED {
  GPIO_OUTPUT_SPEED_2MHz        = 0,
  GPIO_OUTPUT_SPEED_25MHz       = 1,
  GPIO_OUTPUT_SPEED_50MHz       = 2,
  GPIO_OUTPUT_SPEED_100MHz      = 3
} GPIO_OUTPUT_SPEED;

/// GPIO Pull-up/down
typedef enum _GPIO_PULL_UP_DOWN {
  GPIO_NO_PULL_UP_DOWN          = 0,
  GPIO_PULL_UP                  = 1,
  GPIO_PULL_DOWN                = 2
} GPIO_PULL_UP_DOWN;

/// GPIO Alternate Function identifier
typedef enum _GPIO_AF_ID {
  GPIO_AF_SYSTEM                = 0,
  GPIO_AF_TIM1                  = 1,
  GPIO_AF_TIM2                  = 1,
  GPIO_AF_TIM3                  = 2,
  GPIO_AF_TIM4                  = 2,
  GPIO_AF_TIM5                  = 2,
  GPIO_AF_TIM8                  = 3,
  GPIO_AF_TIM9                  = 3,
  GPIO_AF_TIM10                 = 3,
  GPIO_AF_TIM11                 = 3,
  GPIO_AF_I2C1                  = 4,
  GPIO_AF_I2C2                  = 4,
  GPIO_AF_I2C3                  = 4,
  GPIO_AF_SPI1                  = 5,
  GPIO_AF_SPI2                  = 5,
  GPIO_AF_SPI3                  = 6,
  GPIO_AF_SPI5                  = 5,
  GPIO_AF_USART1                = 7,
  GPIO_AF_USART2                = 7,
  GPIO_AF_USART3                = 7,
  GPIO_AF_USART4                = 8,
  GPIO_AF_USART5                = 8,
  GPIO_AF_USART6                = 8,
  GPIO_AF_USART7                = 8,
  GPIO_AF_USART8                = 8,
  GPIO_AF_CAN1                  = 9,
  GPIO_AF_CAN2                  = 9,
  GPIO_AF_TIM12                 = 9,
  GPIO_AF_TIM13                 = 9,
  GPIO_AF_TIM14                 = 9,
  GPIO_AF_OTG_FS                = 10,
  GPIO_AF_OTG_HS                = 10,
  GPIO_AF_ETH                   = 11,
  GPIO_AF_FSMC                  = 12,
  GPIO_AF_SDIO                  = 12,
  GPIO_AF_OTG_HS_FS             = 12,
  GPIO_AF_DCMI                  = 13,
  GPIO_AF_EVENT_OUT             = 15
} GPIO_AF_ID;


/// GPIO Pin identifier
typedef struct _GPIO_PIN_ID {
  GPIO_TypeDef *port;
  uint8_t       num;
} GPIO_PIN_ID;


/**
  \fn          void GPIO_PortClock (GPIO_TypeDef *GPIOx, bool clock)
  \brief       Port Clock Control
  \param[in]   GPIOx  Pointer to GPIO peripheral
  \param[in]   clock  Enable or disable clock
*/
extern void GPIO_PortClock (GPIO_TypeDef *GPIOx, bool clock);

/**
  \fn          bool GPIO_PinConfigure (GPIO_TypeDef      *GPIOx,
                                       uint32_t           num,
                                       GPIO_MODE          mode,
                                       GPIO_OUTPUT_TYPE   output_type,
                                       GPIO_OUTPUT_SPEED  output_speed,
                                       GPIO_PULL_UP_DOWN  pull_up_down)
  \brief       Configure port pin
  \param[in]   GPIOx         Pointer to GPIO peripheral
  \param[in]   num           Port pin number
  \param[in]   mode          \ref GPIO_MODE
  \param[in]   output_type   \ref GPIO_OUTPUT_TYPE
  \param[in]   output_speed  \ref GPIO_OUTPUT_SPEED
  \param[in]   pull_up_down  \ref GPIO_PULL_UP_DOWN
  \return      true  - success
               false - error
*/
extern bool GPIO_PinConfigure(GPIO_TypeDef      *GPIOx,
                              uint32_t           num,
                              GPIO_MODE          mode,
                              GPIO_OUTPUT_TYPE   output_type,
                              GPIO_OUTPUT_SPEED  output_speed,
                              GPIO_PULL_UP_DOWN  pull_up_down);

/**
  \fn          bool GPIO_PinAF (GPIO_TypeDef *GPIOx,
                                uint32_t      num,
                                uint32_t      af)
  \brief       Setup port pin alternate function
  \param[in]   GPIOx  Pointer to GPIO peripheral
  \param[in]   num    Port pin number
  \param[in]   af     \ref GPIO_AF_ID
  \return      true  - success
               false - error
*/
extern bool GPIO_PinAF (GPIO_TypeDef *GPIOx,
                        uint32_t      num,
                        GPIO_AF_ID    af);

/**
  \fn          void GPIO_PinWrite (GPIO_TypeDef *GPIOx, uint32_t num, uint32_t val)
  \brief       Write port pin
  \param[in]   GPIOx  Pointer to GPIO peripheral
  \param[in]   num    Port pin number
  \param[in]   val    Port pin value (0 or 1)
*/
__FORCE_INLINE void GPIO_PinWrite (GPIO_TypeDef *GPIOx, uint32_t num, uint32_t val) {
  if (val & 1) {
    GPIOx->BSRRL = (1UL << num);
  } else {
    GPIOx->BSRRH = (1UL << num);
  }
}

/**
  \fn          uint32_t GPIO_PinRead (GPIO_TypeDef *GPIOx, uint32_t num)
  \brief       Read port pin
  \param[in]   GPIOx  Pointer to GPIO peripheral
  \param[in]   num    Port pin number
  \return      pin value (0 or 1)
*/
__FORCE_INLINE uint32_t GPIO_PinRead (GPIO_TypeDef *GPIOx, uint32_t num) {
  return ((GPIOx->IDR >> num) & 1);
}

/**
  \fn          void GPIO_PortWrite (GPIO_TypeDef *GPIOx, uint16_t mask, uint16_t val)
  \brief       Write port pins
  \param[in]   GPIOx  Pointer to GPIO peripheral
  \param[in]   mask   Selected pins
  \param[in]   val    Pin values
*/
__FORCE_INLINE void GPIO_PortWrite (GPIO_TypeDef *GPIOx, uint16_t mask, uint16_t val) {
  GPIOx->ODR = (GPIOx->ODR & ~mask) | val;
}

/**
  \fn          uint16_t GPIO_PortRead (GPIO_TypeDef *GPIOx)
  \brief       Read port pins
  \param[in]   GPIOx  Pointer to GPIO peripheral
  \return      port pin inputs
*/
__FORCE_INLINE uint16_t GPIO_PortRead (GPIO_TypeDef *GPIOx) {
  return (GPIOx->IDR);
}


#endif /* __GPIO_STM32F4XX_H */
