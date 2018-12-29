/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Retarget.c
 * Purpose: Retarget Input to the USB HID Keyboard and Output to the LCD 
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <rt_sys.h>
#include "rl_usb.h"
#include "GLCD.h"

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdin;
FILE __stdout;

/*--------------------------- fputc ------------------------------------------*/

int fputc (int ch, FILE *f) {
  static int ln  = 0;
  static int col = 0;

  if (ch == 0x0A) {           /* Line Feed */
    col = 0;
    ln ++;
    if (ln >= 40) {
      ln = 39;
      GLCD_ScrollVertical (8);
    }
  } else if (ch == 0x0D) {    /* Carriage Return */
    col = 0;
  } else if (ch == 0x08) {    /* Backspace */
    if (col) 
      col --;
  } else {
    GLCD_DisplayChar (ln, col++, 0, (unsigned char) ch);
  }

  return (ch);
}

/*--------------------------- fgetc ------------------------------------------*/

int fgetc (FILE *f) {
  int ch;

  do {
    ch = USBH_HID_GetKeyboardKey (0);
  } while (ch < 0);
  return (ch);
}

/*--------------------------- ferror -----------------------------------------*/

int ferror (FILE *f) {
  return EOF;
}
