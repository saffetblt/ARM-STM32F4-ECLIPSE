/*-----------------------------------------------------------------------------
 * Name:    LED.c
 * Purpose: Low level LED functions
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

#include "LED.h"
#include "GPIO_STM32F4xx.h"


const GPIO_PIN_ID Pin_LED[] = {
  { GPIOD, 12 },
  { GPIOD, 13 },
  { GPIOD, 14 },
  { GPIOD, 15 }
};

#define NUM_LEDS (sizeof(Pin_LED)/sizeof(GPIO_PIN_ID))


/*-----------------------------------------------------------------------------
 *      LED_Initialize:  Initialize LEDs
 *
 * Parameters: (none)
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void LED_Initialize (void) {
  uint32_t n;

  /* Configure pins: Push-pull Output Mode (50 MHz) with Pull-down resistors */
  for (n = 0; n < NUM_LEDS; n++) {
    GPIO_PortClock   (Pin_LED[n].port, true);
    GPIO_PinWrite    (Pin_LED[n].port, Pin_LED[n].num, 0);
    GPIO_PinConfigure(Pin_LED[n].port, Pin_LED[n].num,
                      GPIO_MODE_OUTPUT,
                      GPIO_OUTPUT_PUSH_PULL,
                      GPIO_OUTPUT_SPEED_50MHz,
                      GPIO_PULL_DOWN);
  }
}


/*-----------------------------------------------------------------------------
 *      LED_Uninitialize:  Uninitialize LEDs
 *
 * Parameters: (none)
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void LED_Uninitialize (void) {
  uint32_t n;

  /* Configure pins: Input mode, without Pull-up/down resistors */
  for (n = 0; n < NUM_LEDS; n++) {
    GPIO_PinConfigure(Pin_LED[n].port, Pin_LED[n].num,
                      GPIO_MODE_INPUT,
                      GPIO_OUTPUT_PUSH_PULL,
                      GPIO_OUTPUT_SPEED_2MHz,
                      GPIO_NO_PULL_UP_DOWN);
  }
}

/*-----------------------------------------------------------------------------
 *      LED_On: Turns on requested LED
 *
 * Parameters:  num - LED number
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void LED_On (uint32_t num) {
  GPIO_PinWrite(Pin_LED[num].port, Pin_LED[num].num, 1);
}

/*-----------------------------------------------------------------------------
 *      LED_Off: Turns off requested LED
 *
 * Parameters:  num - LED number
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void LED_Off (uint32_t num) {
  GPIO_PinWrite(Pin_LED[num].port, Pin_LED[num].num, 0);
}

/*-----------------------------------------------------------------------------
 *      LED_Out: Write value to LEDs
 *
 * Parameters:  val - value to be displayed on LEDs
 * Return:     (none)
 *----------------------------------------------------------------------------*/
void LED_Out (uint32_t val) {
  uint32_t n;

  for (n = 0; n < NUM_LEDS; n++) {
    if (val & (1<<n)) {
      LED_On (n);
    } else {
      LED_Off(n);
    }
  }
}

/*-----------------------------------------------------------------------------
 *      LED_Num: Get number of available LEDs
 *
 * Parameters: (none)
 * Return:      number of available LEDs
 *----------------------------------------------------------------------------*/
uint32_t LED_Num (void) {
  return (NUM_LEDS);
}
