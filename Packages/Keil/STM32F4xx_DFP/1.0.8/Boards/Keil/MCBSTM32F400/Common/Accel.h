/*----------------------------------------------------------------------------
 * Name:    Accel.h
 * Purpose: Accelerometer definitions
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

#ifndef __ACCEL_H
#define __ACCEL_H

#include <stdint.h>

/* Axis acceleration data */
typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} AXIS_DATA;

/* Accelerometer functions */
extern int32_t Accel_Initialize   (void);
extern int32_t Accel_Uninitialize (void);
extern int32_t Accel_GetData      (AXIS_DATA *axd);

#endif /* __ACCEL_H */
