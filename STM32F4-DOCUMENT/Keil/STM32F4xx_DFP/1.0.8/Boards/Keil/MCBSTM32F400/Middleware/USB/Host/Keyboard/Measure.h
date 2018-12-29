/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Measure.h
 * Purpose: Measure header
 *----------------------------------------------------------------------------*/

struct clock {                           /* structure of the clock record     */
  unsigned char    hour;                 /* hour                              */
  unsigned char     min;                 /* minute                            */
  unsigned char     sec;                 /* second                            */
  unsigned short   msec;                 /* milliseconds                      */
};

struct mrec  {                           /* structure for measurement records */
  struct   clock   time;                 /* time of measurement               */
  unsigned short analog;                 /* analog voltage                    */
};

struct interval  {                       /* structure for interval record     */
  unsigned char     min;                 /* minute                            */
  unsigned char     sec;                 /* second                            */
  unsigned short   msec;                 /* milli second                      */
};

/* external functions: */
extern void measure_display (struct mrec disp);   /* display mrec             */
extern void set_time (char *buffer);              /* set current time         */
extern void set_interval (char *buffer);          /* set interval time        */

extern char   ERROR_STR [];              /* ERROR message string              */
extern struct mrec current;              /* current measurements              */
extern struct interval setinterval;      /* interval setting values           */
extern struct interval interval;         /* interval counter                  */
