/*----------------------------------------------------------------------------
 * Name:    Gyroscope.h
 * Purpose: Gyroscope definitions
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

#ifndef __GYROSCOPE_H
#define __GYROSCOPE_H

#include <stdint.h>

typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} ANGLE_RATE;

/* Gyroscope functions */
extern int32_t Gyroscope_Initialize   (void);
extern int32_t Gyroscope_Uninitialize (void);
extern int32_t Gyroscope_GetData      (ANGLE_RATE *ang);

#endif /* __GYROSCOPE_H */
