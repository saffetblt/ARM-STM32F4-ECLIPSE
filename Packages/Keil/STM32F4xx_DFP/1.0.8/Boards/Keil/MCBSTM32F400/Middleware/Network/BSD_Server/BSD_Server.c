/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    BSD_Server.c
 * Purpose: LED Server example using BSD sockets
 *----------------------------------------------------------------------------*/

#include "cmsis_os.h"                   /* CMSIS RTOS definitions             */
#include "rl_net.h"                     /* Network definitions                */

#include "GLCD.h"
#include "LED.h"

#define PORT_NUM 1001
#define BLINKLED 0x01                   /* Command for blink the leds         */

static void Server (void const *arg);
osThreadDef(Server, osPriorityNormal, 2, 0);

/// Initialize display
static void init_display () {
  GLCD_Initialize ();
  GLCD_Clear (Blue);
  GLCD_SetBackColor (Blue);
  GLCD_SetTextColor (White);
  GLCD_DisplayString (3, 0, 1, "       MDK-MW       ");
  GLCD_DisplayString (4, 0, 1, " BSD Server example ");
}

/*----------------------------------------------------------------------------
  Thread 'Server': BSD Server socket process
 *---------------------------------------------------------------------------*/
static void Server (void const *arg) {
  SOCKADDR_IN addr;
  int sock, sd, res;
  int type = (int)arg;
  char dbuf[4];

  while (1) {
    sock = socket (AF_INET, type, 0);

    addr.sin_port        = htons(PORT_NUM);
    addr.sin_family      = PF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;

    bind (sock, (SOCKADDR *)&addr, sizeof(addr));

    if (type == SOCK_STREAM) {
      listen (sock, 1);
      sd = accept (sock, NULL, NULL);
      closesocket (sock);
      sock = sd;
    }

    while (1) {
      res = recv (sock, dbuf, sizeof (dbuf), 0);
      if (res <= 0) {
        break;
      }
      if (dbuf[0] == BLINKLED) {
        LED_Out (dbuf[1]);
      }
    }
    closesocket (sock);
  }
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
int main (void) {

  LED_Initialize ();
  init_display ();
  net_initialize ();

  osThreadCreate (osThread(Server), (void *)SOCK_STREAM);
  osThreadCreate (osThread(Server), (void *)SOCK_DGRAM);
  while(1) {
    net_main ();
    osThreadYield ();
  }
}
