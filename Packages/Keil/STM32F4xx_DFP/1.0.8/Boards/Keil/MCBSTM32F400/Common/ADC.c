/*-----------------------------------------------------------------------------
 * Name:    ADC.c
 * Purpose: Low level ADC functions
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

#include "stm32f4xx.h"                  /* STM32F4xx Definitions              */
#include "ADC.h"

#define ADC_BITS      12                /* Number of A/D converter bits       */
#define ADC_TOUT  200000                /* Approx. A/D conversion timeout     */

/*-----------------------------------------------------------------------------
 *      ADC_Initialize: Initialize Analog to Digital Converter
 *
 * Parameters:  (none)
 * Return:      (none)
 *----------------------------------------------------------------------------*/
void ADC_Initialize (void) {
  /* Setup potentiometer pin PF9 (ADC3_7) and A/D converter ADC3 */

  RCC->APB2ENR |= (1UL <<  10);         /* Enable ADC3 clock                  */
  RCC->AHB1ENR |= (1UL <<   5);         /* Enable GPIOF clock                 */
  GPIOF->MODER |= (3UL << 2*9);         /* PF9 is Analog mode                 */

  ADC3->SQR1   =   0;
  ADC3->SQR2   =   0;
  ADC3->SQR3   =  (7UL <<  0);          /* SQ1 = channel 7                    */
  ADC3->SMPR1  =  (7UL <<  6);          /* Channel 7 sample time is 480 cyc.  */
  ADC3->SMPR2  =   0;                   /* Clear register                     */
  ADC3->CR1    =  (1UL <<  8);          /* Scan mode on                       */

  ADC3->CR2   |=  ( 1UL <<  3);         /* Initialize calibration registers   */
  while (ADC3->CR2 & (1UL << 3));       /* Wait for initialization to finish  */
  ADC3->CR2   |=  ( 1UL <<  2);         /* Start calibration                  */
  while (ADC3->CR2 & (1UL << 2));       /* Wait for calibration to finish     */

  ADC3->CR1   |=  ( 1UL <<  5);         /* enable EOC interrupt               */
  ADC3->CR2   |=  ( 1UL <<  0);         /* ADC enable                         */
}


/*-----------------------------------------------------------------------------
 *      ADC_Uninitialize: Uninitialize Analog to Digital Converter
 *                        (set hardware to reset state)
 *
 * Parameters:  (none)
 * Return:      (none)
 *----------------------------------------------------------------------------*/
void ADC_Uninitialize (void) {
  RCC->APB2ENR &= ~(1UL <<  10);        /* Disable ADC3 clock                 */
  GPIOF->MODER &= ~(3UL << 2*9);        /* PF9 input (reset state)            */
}


/*-----------------------------------------------------------------------------
 *      ADC_StartConversion:  Start A/D conversion 
 *
 * Parameters:  (none)
 * Return:      (none)
 *----------------------------------------------------------------------------*/
void ADC_StartConversion (void) {
  ADC3->CR2 |= (1 << 30);               /* Start conversion                   */  
}


/*-----------------------------------------------------------------------------
 *      ADC_ConversionDone:  Check if A/D conversion is completed
 *
 * Parameters:  (none)
 * Return:      true when conversion is completed, false otherwise
 *----------------------------------------------------------------------------*/
bool ADC_ConversionDone (void) {
  return (ADC3->SR & 2) ? true : false;
}


/*-----------------------------------------------------------------------------
 *      ADC_GetValue:  Get converted value from the ADC
 *
 * Parameters:  (none)
 * Return:      converted value or -1 if conversion in progress/failed
 *----------------------------------------------------------------------------*/
int32_t ADC_GetValue (void) {
  int32_t i;

  for (i = ADC_TOUT; i; i--) {
    if (ADC3->SR & 2) {                 /* Wait until conversion completed    */
      ADC3->SR &= ~2;                   /* Clear EOC interrupt flag           */
      return (ADC3->DR);                /* Return converted value             */
    }
  }
  return -1;                            /* Conversion timeout expired         */
}


/*-----------------------------------------------------------------------------
 *      ADC_NumBits: Get number of ADC bits
 *
 * Parameters:  (none)
 * Return:      number of ADC bits
 *----------------------------------------------------------------------------*/
int32_t ADC_NumBits (void) {
  return (ADC_BITS);
}
