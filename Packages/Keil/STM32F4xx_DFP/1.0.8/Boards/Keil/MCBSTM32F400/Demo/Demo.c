/*----------------------------------------------------------------------------
 * Name:    Demo.c
 * Purpose: Demo example for MCBSTM32F400
 * Note(s):
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2013 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "cmsis_os.h"
#include "Driver_I2C.h"
#include "GLCD.h"
#include "LED.h"
#include "ADC.h"
#include "Keyboard.h"
#include "Joystick.h"
#include "Camera.h"
#include "Touch.h"
#include "Accel.h"
#include "GyroScope.h"

/* Ruler position definitions */
#define RUL_2M      ( 8*6+3)
#define RUL_1M      (17*6+3)
#define RUL_0       (26*6+3)
#define RUL_1P      (35*6+3)
#define RUL_2P      (44*6+3)
#define RUL_HGT     (5)
#define RUL_LN(ln) ((ln)*8-2)

/* Bargraph position definitions */
#define BAR_ST      (8*6+3)
#define BAR_LEN     (217)
#define BAR_HGT     (20)
#define BAR_LN(ln)  ((ln)*24)

/* Joystick position definitions */
#define JOY_X       (13*16)
#define JOY_Y       ( 5*24 + 6)

/* Button position definitions */
#define BTN_X       (9)
#define BTN_Y       (190)
#define BTN_X_EVAL  (20)
#define BTN_Y_EVAL  (182)

/* Bulb position definition */
#define BULB_X       (209)
#define BULB_Y       (190)
#define BULB_X_EVAL  (9 * 16)
#define BULB_Y_EVAL  (8 * 24)

/* Keys bit masks */
#define KEY_WAKEUP  1
#define KEY_TAMPER  2
#define KEY_USER    4

/* Mode definitions */
enum {
  M_INIT = 0,
  M_STD,
  M_MOTION,
  M_CAM,
};

/* Periodic timer definition */
void Timer_Callback (void const *arg);
osTimerDef (PeriodicTimer, Timer_Callback);
osTimerId TimerId;

/* Mode switch timeout variable */
uint32_t KeyTick;

/* Extern variables */
extern unsigned char Arrows_16bpp_red[];
extern unsigned char Button_16bpp[];
extern unsigned char Bulb_16bpp[];

/*-----------------------------------------------------------------------------
  Periodic timer callback
 *----------------------------------------------------------------------------*/
void Timer_Callback (void const *arg) {
  static uint32_t ticks;

  if (KeyTick == 0) {
    if (++ticks == 50) {
      KeyTick = 1;
      ticks = 0;
    }
  }
  else ticks = 0;
}

/*-----------------------------------------------------------------------------
  Draw rectangle at specified coordinates (x,y) with specified side lengths
  (h_len, v_len) and border.
 *----------------------------------------------------------------------------*/
void DrawRectangle (uint32_t x, uint32_t y, uint32_t h_len, uint32_t v_len, uint32_t border) {
  uint32_t i, b;

  for (b = 0; b < border; b++) {
    for (i = 0; i < h_len; i++) {
      GLCD_PutPixel (x + i, y + b);
    }
    for (i = 0; i < v_len; i++) {
      GLCD_PutPixel (x + b, y + i);
    }
    for (i = 0; i < h_len; i++) {
      GLCD_PutPixel (x + i, y + v_len - b);
    }
    for (i = 0; i < v_len; i++) {
      GLCD_PutPixel (x + h_len - b, y + i);
    }
  }
}


/*-----------------------------------------------------------------------------
  Draw vertical line at specified coordinates (x,y) with specified height (h)
 *----------------------------------------------------------------------------*/
void DrawRuler (uint32_t x, uint32_t y, uint32_t h) {
  uint32_t i;

  for (i = 0; i < h; i++) {
    GLCD_PutPixel (x, y - i);
  }
}

/*-----------------------------------------------------------------------------
  Switch between modes
 *----------------------------------------------------------------------------*/
uint32_t SwitchMode (uint32_t mode) {

  GLCD_WindowMax ();
  GLCD_Clear(White);

  switch (mode) {
    case M_INIT:
      /* Prepare display for ADC, Keys, Joystick */
      GLCD_SetBackColor  (Blue);
      GLCD_SetTextColor  (White);
      GLCD_DisplayString (0, 0, 1, "    MCBSTM32F400    ");
      GLCD_DisplayString (1, 0, 1, "    Demo Example    ");
      GLCD_SetBackColor  (White);
      GLCD_SetTextColor  (Blue);
      GLCD_DisplayString (3, 0, 1, "AD value:           ");
      GLCD_DisplayString (4, 0, 1, "Buttons :           ");
      GLCD_DisplayString (6, 0, 1, "Joystick:           ");

#ifdef _EVAL_VERSION_
      GLCD_SetTextColor(Red);
      GLCD_DisplayString (8, 2, 1, "Touch");

      GLCD_SetTextColor(Green);
      DrawRectangle (BTN_X_EVAL, BTN_Y_EVAL, 100, 40, 5);
#endif
      mode = M_STD;
      break;

    case M_STD:
      /* Prepare display for accelerator and gyroscope */
      GLCD_Clear(Navy);
      GLCD_SetBackColor  (Navy);
      GLCD_SetTextColor  (White);

      /* Draw rulers */
      DrawRuler (RUL_2M, RUL_LN(6), RUL_HGT);
      DrawRuler (RUL_1M, RUL_LN(6), RUL_HGT);
      DrawRuler (RUL_0,  RUL_LN(6), RUL_HGT);
      DrawRuler (RUL_1P, RUL_LN(6), RUL_HGT);
      DrawRuler (RUL_2P, RUL_LN(6), RUL_HGT);

      DrawRuler (RUL_2M, RUL_LN(21), RUL_HGT);
      DrawRuler (RUL_1M, RUL_LN(21), RUL_HGT);
      DrawRuler (RUL_0,  RUL_LN(21), RUL_HGT);
      DrawRuler (RUL_1P, RUL_LN(21), RUL_HGT);
      DrawRuler (RUL_2P, RUL_LN(21), RUL_HGT);
      
      /* Accelerator graphics */
      GLCD_DisplayString (0, 0, 1, "  Acceleration [g]  ");
      
      GLCD_DisplayString (4,  7, 0, "-2");
      GLCD_DisplayString (4, 16, 0, "-1");
      GLCD_DisplayString (4, 26, 0, "0");
      GLCD_DisplayString (4, 35, 0, "1");
      GLCD_DisplayString (4, 44, 0, "2");
      
      GLCD_DisplayString (2, 0, 1, "X:");
      GLCD_DisplayString (3, 0, 1, "Y:");
      GLCD_DisplayString (4, 0, 1, "Z:");
      
      /* Gyroscope graphics */
      GLCD_DisplayString (5, 0, 1, " Angular rate [d/s] ");
      
      GLCD_DisplayString (19,  6, 0, "-250");
      GLCD_DisplayString (19, 15, 0, "-125");
      GLCD_DisplayString (19, 26, 0, "0");
      GLCD_DisplayString (19, 34, 0, "125");
      GLCD_DisplayString (19, 43, 0, "250");
      
      GLCD_DisplayString (7, 0, 1, "X:");
      GLCD_DisplayString (8, 0, 1, "Y:");
      GLCD_DisplayString (9, 0, 1, "Z:");

      GLCD_SetBackColor (DarkGrey);
      mode = M_MOTION;
      break;

    case M_MOTION:
      /* Prepare display for video stream from digital camera */
      GLCD_WindowMax ();
      /* Turn camera on */      
      GLCD_Clear (Blue);      
      GLCD_WrReg (0x20, 0x0000);
      GLCD_WrReg (0x21, 0x0000);
      GLCD_WrCmd (0x22);
      Camera_On ();
      mode = M_CAM;
      break;

    case M_CAM:      
      /* Turn camera off */
      Camera_Off ();
      GLCD_Clear (Blue);
      mode = M_INIT;
      break;

    default:
      mode = M_INIT;
      break;
  }
  return (mode);
}


/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
  TSC_STATE  tsc_state;
  AXIS_DATA  acc;
  ANGLE_RATE ang;
  uint32_t mode, ofs;
  uint32_t keyMsk, joyMsk, adcVal;
  int32_t key  = -1;
  int32_t joy  = -1;
  int32_t adc  = -1;

  TimerId = osTimerCreate (osTimer(PeriodicTimer), osTimerPeriodic, NULL);
  
  if (TimerId) {
    osTimerStart (TimerId, 10);
  }

  LED_Initialize();                     /* LED Init                           */
  ADC_Initialize();                     /* A/D Converter Init                 */
  Keyboard_Initialize();                /* Keyboard Init                      */
  Joystick_Initialize();                /* Joystick Init                      */
  Camera_Initialize();                  /* Camera Init                        */
  Touch_Initialize();                   /* Touchscreen Controller Init        */
  Accel_Initialize();                   /* Accelerator Init                   */
  Gyroscope_Initialize();               /* Gyroscope Init                     */
  GLCD_Initialize();                    /* Graphical Display Init             */

  GLCD_Clear (White);
  GLCD_SetBackColor  (Blue);
  GLCD_SetTextColor  (White);
  GLCD_DisplayString (0, 0, 1, "    MCBSTM32F400    ");
  GLCD_DisplayString (1, 0, 1, "    Demo Example    ");
  GLCD_SetBackColor  (White);
  GLCD_SetTextColor  (Blue);

  GLCD_DisplayString (3, 0, 1, " Use WAKEUP key to  ");
  GLCD_DisplayString (4, 0, 1, "   switch example   ");
  GLCD_DisplayString (5, 0, 1, "        mode        ");

  GLCD_DisplayString (8, 0, 1, "  Press any key to  ");
  GLCD_DisplayString (9, 0, 1, "      continue      ");

  /* Wait for user to press the key */
  while (!Keyboard_GetKeys() && !Joystick_GetButtons());
  mode = SwitchMode (M_INIT);
  KeyTick = 0;

  while (1) {    
    if (KeyTick && (Keyboard_GetKeys() & KEY_WAKEUP)) {
      mode = SwitchMode (mode);
      KeyTick = 0;
      /* Force refresh */
      key = -1;
      joy = -1;
      adc = -1;
    }

    switch (mode) {

      /* Show potentiometer, keys, joystick */
      case M_STD:
        keyMsk = Keyboard_GetKeys();    /* Show buttons state                 */
        if (key ^ keyMsk) {          
          GLCD_SetTextColor(Black);
          if (keyMsk & KEY_TAMPER)    { GLCD_DisplayString (4, 9, 1, "TAMPER"); }
          if (keyMsk & KEY_USER  )    { GLCD_DisplayString (5, 9, 1, "USER");   }
          
          GLCD_SetTextColor(LightGrey);
          if (!(keyMsk & KEY_TAMPER)) { GLCD_DisplayString (4, 9, 1, "TAMPER"); }
          if (!(keyMsk & KEY_USER  )) { GLCD_DisplayString (5, 9, 1, "USER");   }
        }

        joyMsk = Joystick_GetButtons(); /* Show joystick arrows               */
        if (joy ^ joyMsk) {
          joy = joyMsk;

          /* Left arrow white/red? */
          ofs = (joy & JOY_DOWN)   ? (2416) : (0);
          GLCD_Bitmap(JOY_X+ 0, JOY_Y+20, 19, 14, &Arrows_16bpp_red[ofs + 0*532]);

          /* Right arrow white/red? */
          ofs = (joy & JOY_UP)     ? (2416) : (0);
          GLCD_Bitmap(JOY_X+35, JOY_Y+20, 19, 14, &Arrows_16bpp_red[ofs + 1*532]);

          /* Center dot white/red */
          ofs = (joy & JOY_CENTER) ? (2416) : (0);
          GLCD_Bitmap(JOY_X+21, JOY_Y+21, 12, 12, &Arrows_16bpp_red[ofs + 4*532]);

          /* Up arrow white/red? */
          ofs = (joy & JOY_LEFT)   ? (2416) : (0);
          GLCD_Bitmap(JOY_X+20, JOY_Y+ 0, 14, 19, &Arrows_16bpp_red[ofs + 2*532]);

          /* Down arrow white/red? */
          ofs = (joy & JOY_RIGHT)  ? (2416) : (0);
          GLCD_Bitmap(JOY_X+20, JOY_Y+35, 14, 19, &Arrows_16bpp_red[ofs + 3*532]);
        }

        ADC_StartConversion();          /* Show A/D conversion bargraph       */
        adcVal = ADC_GetValue();
        if (adc ^ adcVal) {
          GLCD_SetTextColor(Green);
          GLCD_Bargraph (144, 3*24, 160, 20, adcVal >> 2);
        }
        
        Touch_GetState (&tsc_state);    /* Get touch screen state             */

        if (!tsc_state.pressed) {
          tsc_state.x = 0;
          tsc_state.y = 0;
        }

        if (tsc_state.x >= 200 && tsc_state.x <= 1000 &&
            tsc_state.y >= 260 && tsc_state.y <= 1650) {
#ifdef _EVAL_VERSION_
          /* Draw a square dot instead of bitmap */
          GLCD_SetTextColor(Red);
          DrawRectangle (BULB_X_EVAL, BULB_Y_EVAL, 20, 20, 15);
#else
          GLCD_Bitmap(BTN_X,  BTN_Y,  120, 48, &Button_16bpp[1*11520]);
          GLCD_Bitmap(BULB_X, BULB_Y,  26, 40, &Bulb_16bpp[1*2080]);
#endif
        }
        else {
#ifdef _EVAL_VERSION_
          /* Draw a square dot instead of bitmap */
          GLCD_SetTextColor(Yellow);
          DrawRectangle (BULB_X_EVAL, BULB_Y_EVAL, 20, 20, 15);
#else
          GLCD_Bitmap(BTN_X,  BTN_Y,  120, 48, &Button_16bpp[0*11520]);
          GLCD_Bitmap(BULB_X, BULB_Y,  26, 40, &Bulb_16bpp[0*2080]);
#endif
        }
        break;

      /* Show accelerator and gyroscope outputs */
      case M_MOTION:
        if (Accel_GetData (&acc)) {     /* Get acceleration data              */
          acc.x = 0; acc.y = 0; acc.z = 0;
        }
        GLCD_SetTextColor (Red);
        GLCD_Bargraph (BAR_ST, BAR_LN(2), BAR_LEN, BAR_HGT, 512 + (acc.x >> 6));
        GLCD_Bargraph (BAR_ST, BAR_LN(3), BAR_LEN, BAR_HGT, 512 + (acc.y >> 6));
        GLCD_Bargraph (BAR_ST, BAR_LN(4), BAR_LEN, BAR_HGT, 512 + (acc.z >> 6));

        if (Gyroscope_GetData (&ang)) { /* Get angular rate data              */
          ang.x = 0; ang.y = 0; ang.z = 0;
        }
        GLCD_SetTextColor (Yellow);
        GLCD_Bargraph (BAR_ST, BAR_LN(7), BAR_LEN, BAR_HGT, 512 + (ang.x >> 6));
        GLCD_Bargraph (BAR_ST, BAR_LN(8), BAR_LEN, BAR_HGT, 512 + (ang.y >> 6));
        GLCD_Bargraph (BAR_ST, BAR_LN(9), BAR_LEN, BAR_HGT, 512 + (ang.z >> 6));
        break;

      /* Show digital camera output on graphical LCD */
      case M_CAM:
        break;

      default:
        mode = SwitchMode (mode);
        break;
    }
  }
}
