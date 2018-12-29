/*-----------------------------------------------------------------------------
 * Name:    Joystick.h
 * Purpose: Joystick definitions
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

#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include <stdint.h>

/* Joystick buttons */
#define JOY_LEFT    (1 << 0)
#define JOY_RIGHT   (1 << 1)
#define JOY_CENTER  (1 << 2)
#define JOY_UP      (1 << 3)
#define JOY_DOWN    (1 << 4)

/* Joystick functions */
extern void     Joystick_Initialize   (void);
extern void     Joystick_Uninitialize (void);
extern uint32_t Joystick_GetButtons   (void);

#endif /* __JOYSTICK_H */
