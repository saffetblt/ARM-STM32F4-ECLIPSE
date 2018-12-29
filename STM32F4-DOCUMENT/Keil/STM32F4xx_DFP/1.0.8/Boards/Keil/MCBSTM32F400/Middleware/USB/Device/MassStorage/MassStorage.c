/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    MassStorage.c
 * Purpose: USB Device Mass Storage Example
 *----------------------------------------------------------------------------*/

#include "cmsis_os.h"
#include "rl_fs.h"
#include "rl_usb.h"
#include "GLCD.h"

int main (void) {

  GLCD_Initialize    ();
  GLCD_Clear         (Blue);
  GLCD_SetBackColor  (Blue);
  GLCD_SetTextColor  (White);
  GLCD_DisplayString (0, 0, 1, "    USB Device      ");
  GLCD_DisplayString (1, 0, 1, "    MSC Class       ");
  GLCD_DisplayString (2, 0, 1, "Mass Storage Example");
  GLCD_DisplayString (4, 0, 1, "USBFS: Memory Disk  ");
  GLCD_DisplayString (5, 0, 1, "USBHS: SD Card Disk ");
  GLCD_DisplayString (8, 0, 1, "  Keil Tools by ARM ");
  GLCD_DisplayString (9, 0, 1, "    www.keil.com    ");

  finit              ("M0:");           /* Initialize SD Card 0               */
  fmount             ("M0:");           /* Mount SD Card 0                    */

  USBD_Initialize    (0);               /* USB Device 0 Initialization        */
  USBD_Connect       (0);               /* USB Device 0 Connect               */

  USBD_Initialize    (1);               /* USB Device 1 Initialization        */
  USBD_Connect       (1);               /* USB Device 1 Connect               */

  osThreadTerminate  (osThreadGetId());
}
