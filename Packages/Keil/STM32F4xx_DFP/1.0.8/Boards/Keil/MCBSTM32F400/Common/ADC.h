/*-----------------------------------------------------------------------------
 * Name:    ADC.h
 * Purpose: ADC definitions
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

#ifndef __ADC_H
#define __ADC_H

#include <stdint.h>
#include <stdbool.h>

extern void     ADC_Initialize      (void);
extern void     ADC_Uninitialize    (void);
extern void     ADC_StartConversion (void);
extern bool     ADC_ConversionDone  (void);
extern int32_t  ADC_GetValue        (void);
extern int32_t  ADC_NumBits         (void);

#endif /* __ADC_H */
