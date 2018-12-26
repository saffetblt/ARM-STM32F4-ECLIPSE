/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    Blinky.c
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "cmsis_os.h"
#include "STM32F4xx.h"
#include "LED.h"

osThreadId tid_ledOn;                       /* Thread id of thread: ledOn    */
osThreadId tid_ledOff;                      /* Thread id of thread: ledOff   */


/*----------------------------------------------------------------------------
  Thread 1 'ledOn': switches the LED on
 *---------------------------------------------------------------------------*/
 void ledOn (void const *argument) {
  for (;;) {
    LED_On(0);                              /* Turn LED On                   */
    osSignalSet(tid_ledOff, 0x0001);        /* set signal to ledoff thread   */
    osDelay(2000);                          /* delay 2s                      */
  }
}

/*----------------------------------------------------------------------------
  Thread 2 'ledOff': switches the LED off
 *---------------------------------------------------------------------------*/
void ledOff (void const *argument) {
  for (;;) {
    osSignalWait(0x0001, osWaitForever);    /* wait for an event flag 0x0001 */
    osDelay(500);                           /* wait for 500ms                */
    LED_Off(0);                             /* Turn LED Off                  */
  }
}


osThreadDef(ledOn,  osPriorityNormal, 1, 0);
osThreadDef(ledOff, osPriorityNormal, 1, 0);

/*----------------------------------------------------------------------------
  Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {

  SystemCoreClockUpdate();
  LED_Initialize();                         /* Initialize LEDs               */

  tid_ledOn  = osThreadCreate(osThread(ledOn) , NULL);
  tid_ledOff = osThreadCreate(osThread(ledOff), NULL);

  osSignalSet(tid_ledOn, 0x0001);          /* set signal to ledOn thread     */

  osDelay(osWaitForever);
  while(1);
}
