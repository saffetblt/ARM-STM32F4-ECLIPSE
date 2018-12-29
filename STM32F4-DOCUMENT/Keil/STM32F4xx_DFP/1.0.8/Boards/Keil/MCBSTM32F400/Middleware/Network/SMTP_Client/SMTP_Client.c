/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    SMTP_Client.c
 * Purpose: SMTP Client example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"                   /* CMSIS RTOS definitions             */
#include "rl_net.h"                     /* Network definitions                */

#include "GLCD.h"
#include "LED.h"
#include "ADC.h"

// Enter your valid SMTP Server IP address here
const uint8_t srv_ip[4] = { 192,168,0,253 };

uint8_t smtp_event;
osThreadId send_id;

static void BlinkLed (void const *arg);
static void SendMail (void const *arg);

osThreadDef(BlinkLed, osPriorityNormal, 1, 0);
osThreadDef(SendMail, osPriorityNormal, 1, 0);

/// Read analog inputs
uint16_t AD_in (uint32_t ch) {
  ADC_StartConversion();
  return (ADC_GetValue());
}

/// Inform application about SMTP status
void smtp_inform (smtpClientEvent event) {
  smtp_event = event;
  // Notify send thread of SMTP status
  osSignalSet (send_id, 0x0001);
}

/*----------------------------------------------------------------------------
  Thread 'SendMail': Send an email
 *---------------------------------------------------------------------------*/
static void SendMail (void const *arg) {
  netStatus retv;

  GLCD_Initialize ();
  GLCD_Clear (Blue);
  GLCD_SetBackColor (Blue);
  GLCD_SetTextColor (White);
  GLCD_DisplayString (1, 0, 1, "       MDK-MW       ");
  GLCD_DisplayString (2, 0, 1, "SMTP Client example ");
  send_id = osThreadGetId ();

  while(1) {
    // After 5 seconds start sending email
    osDelay (5000);
    GLCD_DisplayString (5, 0, 1, "Sending email...");

    // Start SMTP client, use default SMTP port
    retv = smtp_client_connect (srv_ip, 0);
    if (retv != netOK) {
      // Failed to start SMTP process
      GLCD_DisplayString (5, 0, 1, "Connect failed     ");
      continue;
    }
    osSignalWait (0x0001, osWaitForever);
    // Check the completion code
    switch (smtp_event) {
      case smtpClientSuccess:
        // Success, terminate thread
        GLCD_DisplayString (6, 0, 1, "Success, email sent");
        osThreadTerminate (send_id);
        break;
      case smtpClientTimeout:
        // Timeout, try again
        GLCD_DisplayString (5, 0, 1, "Server timeout      ");
      break;
      case smtpClientError:
        // Error, try again
        GLCD_DisplayString (5, 0, 1, "Operation failed    ");
        break;
    }
  }
}

/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static void BlinkLed (void const *arg) {
  const uint8_t led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                                0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
  int cnt = 0;

  while(1) {
    // Every 100 ms
    LED_Out (led_val[cnt]);
    if (++cnt >= sizeof(led_val)) {
      cnt = 0;
    }
    osDelay (100);
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
int main (void) {
  LED_Initialize ();
  ADC_Initialize ();
  net_initialize ();

  osThreadCreate (osThread(BlinkLed), NULL);
  osThreadCreate (osThread(SendMail), NULL);

  while(1) {
    net_main ();
    osThreadYield ();
  }
}
