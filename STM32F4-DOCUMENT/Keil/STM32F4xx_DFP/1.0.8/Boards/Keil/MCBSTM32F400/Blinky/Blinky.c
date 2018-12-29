/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: LED Flasher for MCBSTM32F400
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

#include "stm32f4xx.h"                  /* STM32F4xx Definitions              */
#include "LED.h"

uint32_t LEDOn, LEDOff; 

/*----------------------------------------------------------------------------
  SysTick IRQ Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler (void) {
  static uint32_t ticks;
  
  switch (ticks++) {
    case  0: LEDOn  = 1; break;
    case  5: LEDOff = 1; break;
    case  9: ticks  = 0; break;
    
    default:
      if (ticks > 10) {
        ticks = 0;
      }
  }
}

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
  int32_t max_num = LED_Num() - 1;
  int32_t num = 0;
  int32_t dir = 1;

  SystemCoreClockUpdate();
  LED_Initialize();                         /* LED Initialization             */

  SysTick_Config(SystemCoreClock/100);      /* Generate interrupt each 10 ms  */

  while (1) {
    if (LEDOn) {
      LEDOn = 0;
      LED_On (num);                         /* Turn specified LED on          */
    }

    if (LEDOff) {
      LEDOff = 0;
      LED_Off (num);                        /* Turn specified LED off         */

      num += dir;                           /* Change LED number              */
      
      if (dir == 1 && num == max_num) {
        dir = -1;                           /* Change direction to down       */
      }
      else if (num == 0) {
        dir =  1;                           /* Change direction to up         */
      }
    }
  }
}
