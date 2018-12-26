/*-----------------------------------------------------------------------------
 * Name:    Keyboard.c
 * Purpose: Low level Keyboard functions
 *-----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "Keyboard.h"
#include "GPIO_STM32F4xx.h"

#define NUM_KEYS        3               /* Number of available keys           */

/* Keys for MCBSTM32F200/400 */
#define WAKEUP_MASK     (1<<0)
#define WAKEUP_PORT     GPIOA
#define WAKEUP_BIT      0
#define TAMPER_MASK     (1<<1)
#define TAMPER_PORT     GPIOC
#define TAMPER_BIT      13
#define USER_MASK       (1<<2)
#define USER_PORT       GPIOG
#define USER_BIT        15


/*-----------------------------------------------------------------------------
 *      Keyboard_Initialize:  Initialize keyboard/buttons
 *
 * Parameters: (none)
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void Keyboard_Initialize (void) {

  /* Configure WAKEUP pin */
  GPIO_PortClock   (WAKEUP_PORT, true);
  GPIO_PinConfigure(WAKEUP_PORT, WAKEUP_BIT,
                    GPIO_MODE_INPUT,
                    GPIO_OUTPUT_PUSH_PULL,
                    GPIO_OUTPUT_SPEED_25MHz,
                    GPIO_NO_PULL_UP_DOWN);

  /* Configure TAMPER pin */
  GPIO_PortClock   (TAMPER_PORT, true);
  GPIO_PinConfigure(TAMPER_PORT, TAMPER_BIT,
                    GPIO_MODE_INPUT,
                    GPIO_OUTPUT_PUSH_PULL,
                    GPIO_OUTPUT_SPEED_25MHz,
                    GPIO_NO_PULL_UP_DOWN);

  /* Configure USER pin */
  GPIO_PortClock   (USER_PORT, true);
  GPIO_PinConfigure(USER_PORT, USER_BIT,
                    GPIO_MODE_INPUT,
                    GPIO_OUTPUT_PUSH_PULL,
                    GPIO_OUTPUT_SPEED_25MHz,
                    GPIO_NO_PULL_UP_DOWN);
}


/*-----------------------------------------------------------------------------
 *      Keyboard_Uninitialize:  Uninitialize keyboard/buttons
 *
 * Parameters: (none)
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void Keyboard_Uninitialize (void) {
}


/*-----------------------------------------------------------------------------
 *      Keyboard_GetKeys:  Get keyboard state
 *
 * Parameters: (none)
 * Return:      Keys bitmask
 *----------------------------------------------------------------------------*/
uint32_t Keyboard_GetKeys (void) {
  /* Read board keyboard inputs */
  uint32_t val = 0;

  if (GPIO_PinRead(WAKEUP_PORT, WAKEUP_BIT) == 1) {
    /* Wakeup button */
    val |= WAKEUP_MASK;
  }
  if (GPIO_PinRead(TAMPER_PORT, TAMPER_BIT) == 0) {
    /* Tamper button */
    val |= TAMPER_MASK;
  }
  if (GPIO_PinRead(USER_PORT, USER_BIT) == 0) {
    /* User button */
    val |= USER_MASK;
  }
  return (val);
}


/*-----------------------------------------------------------------------------
 *      Keyboard_NumKeys:  Get number of available keys
 *
 * Parameters: (none)
 * Return:      number of keys
 *----------------------------------------------------------------------------*/
uint32_t Keyboard_NumKeys (void) {
  return (NUM_KEYS);
}
