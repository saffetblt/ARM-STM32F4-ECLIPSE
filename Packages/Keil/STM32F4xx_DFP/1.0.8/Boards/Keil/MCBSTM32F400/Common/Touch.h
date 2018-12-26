/*-----------------------------------------------------------------------------
 * Name:    Touch.h
 * Purpose: Touchscreen controller definitions
 *-----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef __TOUCH_H
#define __TOUCH_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  int16_t x;
  int16_t y;
  uint8_t pressed;
} TSC_STATE;

extern bool Touch_Initialize   (void);
extern bool Touch_Uninitialize (void);
extern bool Touch_GetState     (TSC_STATE *pState);

#endif /* __TOUCH_H */
