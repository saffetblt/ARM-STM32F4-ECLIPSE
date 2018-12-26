/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Mcommand.c
 * Purpose: Time Set Commands for Remote Measurement Recorder
 *----------------------------------------------------------------------------*/

#include <stdio.h>                       /* standard ANSI I/O .h-file         */

#include "cmsis_os.h"                    /* CMSIS RTOS definitions            */

#include "Measure.h"                     /* global project definition file    */

#include "ADC.h"

extern osTimerId timer_id;

/******************************************************************************/
/*                           Display measurements                             */
/******************************************************************************/
void measure_display (struct mrec display)  {

  printf ("\rTime: %2d:%02d:%02d.%03d  ",
           display.time.hour,            /* display hour                      */
           display.time.min,             /* display minute                    */
           display.time.sec,             /* display second                    */
           display.time.msec);           /* display millisecond               */

  printf (" AD value:%4.2fV", (float) (display.analog * 3.3 / (1 << ADC_NumBits ())));
}


/******************************************************************************/
/*                           Set Current Time                                 */
/******************************************************************************/
void set_time (char * buffer)  {
  int hour, min, sec;                    /* temporary time values             */
  int args;                              /* number of arguments               */

  args = sscanf (buffer, "%d:%d:%d",     /* scan input line for               */
                 &hour,                  /* hour, minute and second           */
                 &min,
                 &sec);
  if (hour > 23  ||  min > 59  ||        /* check for valid inputs            */
      sec > 59   ||  args < 2  ||  args == EOF)  {
    printf (ERROR_STR, "INVALID TIME FORMAT");
  }
  else  {                                /* if inputs valid then              */
    osTimerStop (timer_id);
    current.time.hour = hour;            /* setting the new time: hours       */
    current.time.min  = min;             /* setting the new time: minutes     */
    current.time.sec  = sec;             /* setting the new time: seconds     */
    current.time.msec = 0;               /* setting the new time: miliseconds */
    interval.min = 0;                    /* force new interval                */
    interval.sec = 0;
    interval.msec = 0;
    osTimerStart(timer_id, 1);
  }
}


/******************************************************************************/
/*                            Set Interval Time                               */
/******************************************************************************/
void set_interval (char * buffer) {
  struct interval itime = {0,0,0};       /* temporary interval record         */
  int min, sec, msec;
  int args;                              /* number of arguments               */

  args = sscanf (buffer, "%d:%d.%d",     /* scan input line for               */
                 &min,                   /* minute, second and milliseconds   */
                 &sec,
                 &msec);
                                         /* check valid inputs                */
  if (sec > 59  ||  msec > 999  ||  args < 2  || args == EOF)  {
    printf (ERROR_STR, "INVALID INTERVAL FORMAT");
  }
  else  {                                /* if inputs are valid then          */
    itime.min  = min;
    itime.sec  = sec;
    itime.msec = msec;
    if (itime.min != 0 || itime.sec != 0 || itime.msec != 0)  {
      if (itime.msec-- == 0)  {          /* correct interval time for         */
        itime.msec = 999;                /* interrupt routine                 */
        if (itime.sec-- == 0)  {
          itime.sec = 59;
          itime.min--;
        }
      }
    }
    osTimerStop (timer_id);
    setinterval = itime;                 /* of the new setinterval time       */
    interval.min = 0;                    /* force new interval                */
    interval.sec = 0;
    interval.msec = 0;
    osTimerStart(timer_id, 1);
  }
}
