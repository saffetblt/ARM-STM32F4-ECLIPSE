/*-----------------------------------------------------------------------------
 * Name:    Camera.h
 * Purpose: Digital camera definitions
 *-----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2013 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef __CAMERA_H
#define __CAMERA_H

#include <stdint.h>

#define TEST_PATTERN_OFF 0              /* Test pattern OFF                   */
#define TEST_PATTERN_BAR 1              /* Color bar test pattern             */

extern int32_t Camera_Initialize   (void);
extern int32_t Camera_Uninitialize (void);
extern int32_t Camera_On           (void);
extern int32_t Camera_Off          (void);
extern int32_t Camera_Test         (uint32_t pattern);

#endif /* __CAMERA_H */
