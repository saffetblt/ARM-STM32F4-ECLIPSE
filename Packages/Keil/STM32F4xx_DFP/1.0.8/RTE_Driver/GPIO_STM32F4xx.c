/* ----------------------------------------------------------------------
 * Copyright (C) 2014 ARM Limited. All rights reserved.
 *  
 * $Date:        29. April 2014
 * $Revision:    V1.01
 *  
 * Project:      GPIO Driver for ST STM32F4xx
 * -------------------------------------------------------------------- */

#include "GPIO_STM32F4xx.h"


/**
  \fn          void GPIO_PortClock (GPIO_TypeDef *GPIOx, bool clock)
  \brief       Port Clock Control
  \param[in]   GPIOx  Pointer to GPIO peripheral
  \param[in]   clock  Enable or disable clock
*/
void GPIO_PortClock (GPIO_TypeDef *GPIOx, bool clock) {

  if (clock) {
    if      (GPIOx == GPIOA) RCC->AHB1ENR |=  (1UL << 0);
    else if (GPIOx == GPIOB) RCC->AHB1ENR |=  (1UL << 1);
    else if (GPIOx == GPIOC) RCC->AHB1ENR |=  (1UL << 2);
    else if (GPIOx == GPIOD) RCC->AHB1ENR |=  (1UL << 3);
    else if (GPIOx == GPIOE) RCC->AHB1ENR |=  (1UL << 4);
    else if (GPIOx == GPIOF) RCC->AHB1ENR |=  (1UL << 5);
    else if (GPIOx == GPIOG) RCC->AHB1ENR |=  (1UL << 6);
    else if (GPIOx == GPIOH) RCC->AHB1ENR |=  (1UL << 7);
    else if (GPIOx == GPIOI) RCC->AHB1ENR |=  (1UL << 8);
  } else {
    if      (GPIOx == GPIOA) RCC->AHB1ENR &= ~(1UL << 0);
    else if (GPIOx == GPIOB) RCC->AHB1ENR &= ~(1UL << 1);
    else if (GPIOx == GPIOC) RCC->AHB1ENR &= ~(1UL << 2);
    else if (GPIOx == GPIOD) RCC->AHB1ENR &= ~(1UL << 3);
    else if (GPIOx == GPIOE) RCC->AHB1ENR &= ~(1UL << 4);
    else if (GPIOx == GPIOF) RCC->AHB1ENR &= ~(1UL << 5);
    else if (GPIOx == GPIOG) RCC->AHB1ENR &= ~(1UL << 6);
    else if (GPIOx == GPIOH) RCC->AHB1ENR &= ~(1UL << 7);
    else if (GPIOx == GPIOI) RCC->AHB1ENR &= ~(1UL << 8);
  }
}

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
bool GPIO_PinConfigure(GPIO_TypeDef      *GPIOx,
                       uint32_t           num,
                       GPIO_MODE          mode,
                       GPIO_OUTPUT_TYPE   output_type,
                       GPIO_OUTPUT_SPEED  output_speed,
                       GPIO_PULL_UP_DOWN  pull_up_down) {

  if (num >= 16) return false;

  GPIOx->MODER   = (GPIOx->MODER   & ~(0x3 << (2*num))) | (mode         << (2*num));
  GPIOx->OTYPER  = (GPIOx->OTYPER  & ~(0x1 <<    num))  | (output_type  <<    num);
  GPIOx->OSPEEDR = (GPIOx->OSPEEDR & ~(0x3 << (2*num))) | (output_speed << (2*num));
  GPIOx->PUPDR   = (GPIOx->PUPDR   & ~(0x3 << (2*num))) | (pull_up_down << (2*num));

  return true;
}

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
bool GPIO_PinAF (GPIO_TypeDef *GPIOx,
                 uint32_t      num,
                 GPIO_AF_ID    af) {

  if (num >= 16) return false;

  if (num >= 8) {
    num -= 8;
    GPIOx->AFR[1] = (GPIOx->AFR[1] & ~(0xF << (4*num))) | (af << (4*num));
  } else {
    GPIOx->AFR[0] = (GPIOx->AFR[0] & ~(0xF << (4*num))) | (af << (4*num));
  }

  return true;
}
