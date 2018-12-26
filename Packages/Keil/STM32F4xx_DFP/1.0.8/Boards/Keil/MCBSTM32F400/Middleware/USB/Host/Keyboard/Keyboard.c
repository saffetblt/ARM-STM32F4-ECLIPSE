/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Keyboard.c
 * Purpose: USB Host HID Keyboard example program
 *----------------------------------------------------------------------------*/

#include <stdio.h>                      /* standard I/O .h-file               */
#include <ctype.h>                      /* character functions                */

#include "cmsis_os.h"                   /* CMSIS RTOS definitions             */
#include "rl_usb.h"                     /* RL-USB function prototypes         */

#include "Terminal.h"
#include "ADC.h"
#include "GLCD.h"

#include "Measure.h"                    /* global project definition file     */

const char menu[] =
   "\n"
   "+***** REMOTE MEASUREMENT RECORDER ****+\n"
   "| This program is a simple Measurement |\n"
   "| Recorder.It records state of the     |\n"
   "| voltage on analog input.             |\n"
   "+ command ----+ function --------------+\n"
   "| R [n]       | read <n> records       |\n"
   "| D           | display measurement    |\n"
   "| T hh:mm:ss  | set time               |\n"
   "| I mm:ss.ttt | set interval time      |\n"
   "| C           | clear records          |\n"
   "| Q           | quit recording         |\n"
   "| S           | start recording        |\n"
   "+-------------+------------------------+\n";

osTimerId timer_id;

struct interval setinterval;                 /* interval setting values       */
struct interval interval;                    /* interval counter              */

volatile int measurement_interval = 0;       /* measurement interval over     */
volatile int mdisplay = 0;                   /* measurement display requested */
volatile int startflag = 0;                  /* start measurement recording   */

struct mrec current;                         /* current measurements          */

#define SCNT 20                              /* number of records             */

struct mrec save_record[SCNT];               /* buffer for measurements       */
int sindex;                                  /* save index                    */
int savefirst;                               /* save first index              */

char ERROR_STR [] = "\n*** ERROR: %s\n";     /* ERROR message string in code  */

#define WRONGINDEX 0xffff                    /* error signal for wrong index  */


/******************************************************************************/
/*               Save current measurements in save_record                     */
/******************************************************************************/
static void save_current_measurements (void) {
  save_record[sindex++] = current;           /* copy current measurements     */
  if (sindex == SCNT) sindex = 0;            /* check bounds of sindex        */
  if (sindex == savefirst)  {                /* check circular buffer limits  */
    if (++savefirst == SCNT)  savefirst = 0; /* check bounds of savefirst     */
  }
}


/******************************************************************************/
/*                Timer interrupt service function                            */
/*                executes each 1 ms                                          */
/******************************************************************************/
void Timer_1ms_Handler (const void *arg) {

  if (measurement_interval)  {               /* measurement done ?            */
    save_current_measurements ();            /* yes -> save measurements      */
    measurement_interval = 0;                /* Save measurements done        */
  }
                                             /* check if interval is over     */
  if (interval.min  == 0 &&
      interval.sec  == 0 &&
      interval.msec == 0     )  {
    interval = setinterval;                  /* set interval time again       */
    measurement_interval = startflag;        /* determine measurement flag    */
  }
  else  {                                    /* update interval time          */
    if (interval.msec-- == 0)  {             /* calculate millisecond         */
      interval.msec = 999;
      if (interval.sec-- == 0)  {            /* calculate second              */
        interval.sec = 59;
        interval.min--;                      /* calculate minute              */
      }
    }
  }
                                             /* update current time           */
  if (++current.time.msec == 1000)  {        /* update millisecond cnt        */
    current.time.msec = 0;

    if (++current.time.sec == 60)  {         /* update second counter         */
      current.time.sec = 0;

      if (++current.time.min == 60)  {       /* update minute counter         */
        current.time.min = 0;

        if (++current.time.hour == 24)  {    /* update hour counter           */
          current.time.hour = 0;
        }
      }
    }
  } /* end of if( ++current.time.msec... */

  if (measurement_interval || mdisplay)  {   /* process measurement           */
    if (ADC_ConversionDone()) {
      current.analog = ADC_GetValue();
      ADC_StartConversion();
    }
    mdisplay = 0;                            /* mdisplay = 0 for ready sig.   */
  }
}
osTimerDef(timer_1ms, Timer_1ms_Handler);

/******************************************************************************/
/*                       Calculate first Read Index                           */
/******************************************************************************/
static int read_index (char *buffer) {
  int index = 0;
  int args;

  if (setinterval.min == 0     &&            /* check if setinterval is       */
      setinterval.sec == 0     &&            /* below 1 second and            */
      setinterval.msec < 999   &&            /* measurements are collected    */
      startflag                   )  {
    printf (ERROR_STR, "QUIT MEASUREMENTS BEFORE READ");
    return (WRONGINDEX);                     /* no display on the fly if      */
  }                                          /* interval time < 1 second      */
  args = sscanf (buffer, "%d", &index);      /* scan input for read count     */
  if (args == 0  ||  index == 0  || args == EOF)  index = SCNT-1;
  index = sindex - index;                    /* calculate first read index    */
  if (index < 0) index += SCNT;              /* from read count               */
  return (index);
}


/******************************************************************************/
/*                         Clear Measurement Records                          */
/******************************************************************************/
static void clear_records (void) {
  int idx;                                   /* index for loop                */

  startflag = 0;                             /* stop measurement collecting   */
  sindex = savefirst = 0;                    /* reset circular buffer index   */
  for (idx = 0; idx != SCNT; idx++)  {       /* mark all records unused       */
    save_record[idx].time.hour = 0xff;       /* unused flag: hour = 0xff      */
  }
}

/*------------------------------------------------------------------------------
 *        USB Host Thread
 *----------------------------------------------------------------------------*/
void USBH_Thread (void const *arg) {
  osPriority priority;                       /* Thread priority               */
  char con    =  0;                          /* Connection status of kbd      */
  char con_ex =  40;                         /* Previous connection status    
                                                + initial time in 100 ms   
                                                intervals for initial disp    */
  char out    =  1;                          /* Output to keyboard LEDs       */

  USBH_Initialize (0);                       /* Initialize USB Host 0         */
  USBH_Initialize (1);                       /* Initialize USB Host 1         */

  while (1) {
    con = USBH_HID_GetDeviceStatus(0) == usbOK;  /* Get kbd connection status */
    if ((con ^ con_ex) & 1) {                /* If connection status changed  */
      priority = osThreadGetPriority (osThreadGetId());
      osThreadSetPriority (osThreadGetId(), osPriorityAboveNormal);
      if (con) {
        USBH_HID_Write (0,(uint8_t *)&out,1);/* Turn on NUM LED               */
        printf ("\nKeyboard connected\n");
      } else {
        printf ("\nKeyboard disconnected ...\n");
      }
      osThreadSetPriority (osThreadGetId(), priority);
      con_ex = con;
    } else if (con_ex > 1) {                 /* If initial time active        */
      con_ex -= 2;                           /* Decrement initial time        */
      if ((con_ex <= 1) && (!con)) {         /* If initial time expired       */
        priority = osThreadGetPriority (osThreadGetId());
        osThreadSetPriority (osThreadGetId(), osPriorityAboveNormal);
        printf ("\nNo keyboard connected ... \n");
        osThreadSetPriority (osThreadGetId(), priority);
        con_ex = con;
      } else {
        osDelay(200);
      }
    }
    osDelay(100);
  }
}
osThreadDef(USBH_Thread, osPriorityNormal, 1, NULL);

/*------------------------------------------------------------------------------
 *        Application
 *----------------------------------------------------------------------------*/
int main (void) {
  char cmdbuf [15];                          /* command input buffer          */
  int i;                                     /* index for command buffer      */
  int idx;                                   /* index for circular buffer     */

  GLCD_Initialize    ();                     /* Initialize the grapic LCD     */
  GLCD_Clear         (White);                /* Clear the grapic LCD          */
  GLCD_SetTextColor  (Blue);                 /* Set text color to blue        */
  ADC_Initialize     ();                     /* Initialize A/D converter      */
  ADC_StartConversion();                     /* Start ADC conversion          */

  osThreadCreate (osThread(USBH_Thread), NULL);

  timer_id = osTimerCreate(osTimer(timer_1ms), osTimerPeriodic, NULL);
  osTimerStart(timer_id, 1);

  clear_records ();                          /* initialize circular buffer    */
  printf ( menu );                           /* display command menu          */

  while (1)  {                               /* loop forever                  */
    printf ("\nCommand: ");
                                             /* get command line input        */
    if (getline (&cmdbuf[0], sizeof (cmdbuf)) == false)
      continue;

    for (i = 0; cmdbuf[i] != 0; i++)  {      /* convert to upper characters   */
      cmdbuf[i] = toupper(cmdbuf[i]);
    }

    for (i = 0; cmdbuf[i] == ' '; i++);      /* skip blanks                   */

    switch (cmdbuf[i])  {                    /* proceed to command function   */

      case 'R':                              /* Read circular Buffer          */
        if ((idx = read_index (&cmdbuf[i+1])) == WRONGINDEX)  break;
        while (idx != sindex)  {             /* check end of table            */
          if (USBH_HID_GetKeyboardKey(0) == 0x1B) break;   /* ESC terminates  */
          if (save_record[idx].time.hour != 0xff)  {
            measure_display (save_record[idx]);  /* display record            */
            printf ("\n");
          }
          if (++idx == SCNT) idx = 0;        /* next circular buffer entry    */
        }
        break;

      case 'T':                              /* Enter Current Time            */
        set_time (&cmdbuf[i+1]);
        break;

      case 'I':                              /* Enter Interval Time           */
        set_interval (&cmdbuf[i+1]);
        break;

      case 'D':                              /* Display Command               */
        printf ("\nDisplay current Measurements: (ESC to abort)\n");
        while (1) {
          if (USBH_HID_GetKeyboardKey(0) == 0x1B) break;   /* ESC terminates  */
          mdisplay = 1;                      /* request measurement           */
          while (mdisplay);                  /* wait for measurement          */
          measure_display (current);         /* display values                */
        }
        printf ("\n\n");
        break;

      case 'S':                              /* Start Command                 */
        printf ("\nStart Measurement Recording\n");
        startflag = 1;
        break;

      case 'Q':                              /* Quit Command                  */
        printf ("\nQuit Measurement Recording\n");
        startflag = 0;
        break;

      case 'C':                              /* Clear Command                 */
        printf ("\nClear Measurement Records\n");
        clear_records ();
        break;

      default:                               /* Error Handling                */
        printf (ERROR_STR, "UNKNOWN COMMAND");
        printf (menu);                       /* display command menu          */
        break;
    }
  }
}
