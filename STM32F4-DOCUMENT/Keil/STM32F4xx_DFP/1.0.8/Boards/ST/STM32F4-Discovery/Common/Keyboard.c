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

#define NUM_KEYS        1               /* Number of available keys           */

#define USER_MASK       (1<<0)
#define USER_PORT       GPIOA
#define USER_BIT        0


/*-----------------------------------------------------------------------------
 *      Keyboard_Initialize:  Initialize keyboard/buttons
 *
 * Parameters: (none)
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void Keyboard_Initialize (void) {

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

  if (GPIO_PinRead(USER_PORT, USER_BIT) != 0) {
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
