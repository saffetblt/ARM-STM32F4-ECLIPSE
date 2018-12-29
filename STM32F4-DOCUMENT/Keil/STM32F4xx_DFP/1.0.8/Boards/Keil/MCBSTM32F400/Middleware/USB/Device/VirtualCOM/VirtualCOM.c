/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    VirtualCOM.c
 * Purpose: USB Device virtual COM port demonstration
 *----------------------------------------------------------------------------*/

#include "cmsis_os.h"
#include "rl_usb.h"
#include "GLCD.h"

#include "Driver_UART.h"

extern ARM_DRIVER_UART Driver_UART1;
extern ARM_DRIVER_UART Driver_UART3;

int main (void) {
  int32_t usb_rx_ch = -1;
  int32_t usb_tx_ch = -1;
  uint8_t ch;

  GLCD_Initialize    ();
  GLCD_Clear         (Blue);
  GLCD_SetBackColor  (Blue);
  GLCD_SetTextColor  (White);
  GLCD_DisplayString (0, 0, 1, "    USB Device      ");
  GLCD_DisplayString (1, 0, 1, "  CDC ACM Class     ");
  GLCD_DisplayString (2, 0, 1, "Virtual COM Example ");
  GLCD_DisplayString (4, 0, 1, "USBFS: VCOMx        ");
  GLCD_DisplayString (5, 0, 1, "USBHS: VCOMx        ");
  GLCD_DisplayString (8, 0, 1, "  Keil Tools by ARM ");
  GLCD_DisplayString (9, 0, 1, "    www.keil.com    ");

  USBD_Initialize    (0);               /* USB Device 0 Initialization        */
  USBD_Connect       (0);               /* USB Device 0 Connect               */

  USBD_Initialize    (1);               /* USB Device 1 Initialization        */
  USBD_Connect       (1);               /* USB Device 1 Connect               */

  while (1) {                           /* Loop forever                       */
    if (USBD_Configured (0)) {
                                        /* USB -> UART                        */
      if (usb_rx_ch == -1) {
        usb_rx_ch = USBD_CDC_ACM_GetChar (0);
      }
      if (usb_rx_ch != -1) {
        ch = usb_rx_ch;
        if (Driver_UART1.WriteData (&ch, 1) == 1) {
          usb_rx_ch = -1;
        }
      }

                                        /* UART -> USB                        */
      if (usb_tx_ch == -1) {
        if (Driver_UART1.ReadData(&ch, 1) == 1) usb_tx_ch = ch;
        else                                    usb_tx_ch = -1;
      }
      if (usb_tx_ch != -1) {
        if (USBD_CDC_ACM_PutChar (0, usb_tx_ch) == usb_tx_ch) {
          usb_tx_ch = -1;
        }
      }
    }
    if (USBD_Configured (1)) {
                                        /* USB -> UART                        */
      if (usb_rx_ch == -1) {
        usb_rx_ch = USBD_CDC_ACM_GetChar (1);
      }
      if (usb_rx_ch != -1) {
        ch = usb_rx_ch;
        if (Driver_UART3.WriteData (&ch, 1) == 1) {
          usb_rx_ch = -1;
        }
      }

                                        /* UART -> USB                        */
      if (usb_tx_ch == -1) {
        if (Driver_UART3.ReadData(&ch, 1) == 1) usb_tx_ch = ch;
        else                                    usb_tx_ch = -1;
      }
      if (usb_tx_ch != -1) {
        if (USBD_CDC_ACM_PutChar (1, usb_tx_ch) == usb_tx_ch) {
          usb_tx_ch = -1;
        }
      }
    }
  }
}
