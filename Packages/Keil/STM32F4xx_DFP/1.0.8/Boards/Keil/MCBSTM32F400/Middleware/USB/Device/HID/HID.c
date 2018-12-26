/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HID.c
 * Purpose: USB Device Human Interface Device example program
 *----------------------------------------------------------------------------*/

#include "cmsis_os.h"
#include "rl_usb.h"
#include "GLCD.h"
#include "Keyboard.h"
#include "LED.h"

int main (void) {
  uint8_t but;
  uint8_t buf[2];

  LED_Initialize     ();
  Keyboard_Initialize();

  GLCD_Initialize    ();
  GLCD_Clear         (Blue);
  GLCD_SetBackColor  (Blue);
  GLCD_SetTextColor  (White);
  GLCD_DisplayString (0, 0, 1, "    USB Device      ");
  GLCD_DisplayString (1, 0, 1, "    HID Class       ");
  GLCD_DisplayString (2, 0, 1, "    HID Example     ");
  GLCD_DisplayString (4, 0, 1, "USBFS: HID0         ");
  GLCD_DisplayString (5, 0, 1, "USBHS: HID1         ");
  GLCD_DisplayString (8, 0, 1, "  Keil Tools by ARM ");
  GLCD_DisplayString (9, 0, 1, "    www.keil.com    ");

  USBD_Initialize    (0);               /* USB Device 0 Initialization        */
  USBD_Connect       (0);               /* USB Device 0 Connect               */

  USBD_Initialize    (1);               /* USB Device 1 Initialization        */
  USBD_Connect       (1);               /* USB Device 1 Connect               */

  while (1) {                           /* Loop forever                       */
    but = (uint8_t)(Keyboard_GetKeys ());
    if (((but     ) ^ buf[0]) & 1) {
      buf[0] = (but     ) & 1;
      USBD_HID_GetReportTrigger(0, 0, &buf[0], 1);
    }
    if (((but >> 1) ^ buf[1]) & 3) {
      buf[1] = (but >> 1) & 3;
      USBD_HID_GetReportTrigger(1, 0, &buf[1], 1);
    }
    osDelay(100);                       /* 100 ms delay for sampling buttons  */
  }
}
