/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    SNMP_Agent.c
 * Purpose: SNMP Agent example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "cmsis_os.h"                   /* CMSIS RTOS definitions            */
#include "rl_net.h"                     /* Network definitions               */

#include "GLCD.h"
#include "LED.h"
#include "Keyboard.h"
#include "ADC.h"

bool LCDupdate;
char lcd_text[2][16+1];

static void Display (void const *arg);

osThreadDef(Display, osPriorityNormal, 1, 0);

/// Read digital inputs
uint8_t get_button (void) {
  return (Keyboard_GetKeys ());
}

/// IP address change notification
void dhcp_client_notify (uint32_t if_num,
                         dhcpClientOption opt, const uint8_t *val, uint32_t len) {
  if (opt == dhcpClientIPaddress) {
    // IP address has changed
    sprintf (lcd_text[0],"IP address:");
    sprintf (lcd_text[1],"%d.%d.%d.%d", val[0], val[1], val[2], val[3]);
    LCDupdate = true;
  }
}

/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
static void Display (void const *arg) {

  GLCD_Initialize ();
  GLCD_Clear (Blue);
  GLCD_SetBackColor (Blue);
  GLCD_SetTextColor (White);

  GLCD_DisplayString (1, 0, 1, "       MDK-MW       ");
  GLCD_DisplayString (2, 0, 1, " SNMP Agent example ");

  sprintf (lcd_text[0], "");
  sprintf (lcd_text[1], "Waiting for DHCP");
  LCDupdate = true;

  while(1) {
    if (LCDupdate == true) {
      GLCD_ClearLn (5, 1 );
      GLCD_DisplayString (5, 2, 1, lcd_text[0]);
      GLCD_ClearLn (6, 1);
      GLCD_DisplayString (6, 2, 1, lcd_text[1]);
      LCDupdate = false;
    }
    osDelay (250);
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
int main (void) {
  LED_Initialize ();
  Keyboard_Initialize ();
  net_initialize ();

  osThreadCreate (osThread(Display), NULL);

  while (1) {
    net_main ();
    osThreadYield ();
  }
}
