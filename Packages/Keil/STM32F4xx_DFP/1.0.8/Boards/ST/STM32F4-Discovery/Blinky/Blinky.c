/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2013 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "STM32F4xx.h"
#include "LED.h"
#include "Keyboard.h"


volatile uint32_t msTicks;                      /* counts 1ms timeTicks       */

/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;
}

/*----------------------------------------------------------------------------
  delays number of tick Systicks (happens every 1 ms)
 *----------------------------------------------------------------------------*/
void Delay (uint32_t dlyTicks) {                                              
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}


/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
  int32_t led_num = LED_Num();
  int32_t num = -1; 
  int32_t dir =  1;
 uint32_t btns = 0;

  SystemCoreClockUpdate();                      /* Get Core Clock Frequency   */
  if (SysTick_Config(SystemCoreClock / 1000)) { /* SysTick 1 msec interrupts  */
    while (1);                                  /* Capture error              */
  }

  LED_Initialize();
  Keyboard_Initialize();

  while(1) {                                    /* Loop forever               */
    btns = Keyboard_GetKeys();                  /* Read button states         */

    if (btns != (1UL << 0)) {
      /* Calculate 'num': 0,1,...,LED_NUM-1,LED_NUM-1,...,1,0,0,...  */
      num += dir;
      if (num == led_num) { dir = -1; num =  led_num-1; } 
      else if   (num < 0) { dir =  1; num =  0;         }

      LED_On (num);
      Delay( 50);                               /* Delay 50ms                 */
      LED_Off(num);
      Delay(200);                               /* Delay 200ms                */
    }
    else {
      LED_Out (0x0F);
      Delay(10);                                /* Delay 10ms                 */
    }
  }
}
