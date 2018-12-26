/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional. 
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCD_X_16BitIF_STM32F4xx.c
Purpose     : Port routines for STM32F4xx 16-bit Interface
----------------------------------------------------------------------
*/

#include "GUI.h"

/*********************************************************************
*
*       Hardware configuration
*
**********************************************************************
*/

#include "stm32f4xx.h"

#ifndef FSMC_Bank1_NORSRAM4     /* defined in ST Peripheral Library */
  #define FSMC_Bank1_NORSRAM4      ((uint32_t)0x00000006)
#endif

/* LCD /CS is NE4 - Bank 4 of NOR/SRAM Bank 1~4                     */
#define LCD_BASE   (0x60000000UL | 0x0C000000UL)

#define LCD_REG16  (*((volatile U16 *)(LCD_BASE  ))) 
#define LCD_DAT16  (*((volatile U16 *)(LCD_BASE+2)))

/*********************************************************************
*
*       Exported code
*
*********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Init
*
* Purpose:
*   This routine should be called from your application program
*   to set port pins to their initial values
*/
void LCD_X_Init(void) {

/* Configure the LCD Control pins --------------------------------------------*/
  RCC->AHB1ENR    |=((1UL <<  0) |      /* Enable GPIOA clock                 */
                     (1UL <<  2) |      /* Enable GPIOC clock                 */
                     (1UL <<  3) |      /* Enable GPIOD clock                 */
                     (1UL <<  4) |      /* Enable GPIOE clock                 */
                     (1UL <<  5) |      /* Enable GPIOF clock                 */
                     (1UL <<  6));      /* Enable GPIOG clock                 */

  /* PD.00(D2),  PD.01(D3),  PD.04(NOE), PD.05(NWE)                           */
  /* PD.08(D13), PD.09(D14), PD.10(D15), PD.14(D0), PD.15(D1)                 */
  GPIOD->MODER    &= ~0xF03F0F0F;       /* Clear Bits                         */
  GPIOD->MODER    |=  0xA02A0A0A;       /* Alternate Function mode            */
  GPIOD->OSPEEDR  &= ~0xF03F0F0F;       /* Clear Bits                         */
  GPIOD->OSPEEDR  |=  0xA02A0A0A;       /* 50 MHz Fast speed                  */
  GPIOD->AFR[0]   &= ~0x00FF00FF;       /* Clear Bits                         */
  GPIOD->AFR[0]   |=  0x00CC00CC;       /* Alternate Function mode AF12       */
  GPIOD->AFR[1]   &= ~0xFF000FFF;       /* Clear Bits                         */
  GPIOD->AFR[1]   |=  0xCC000CCC;       /* Alternate Function mode AF12       */

  /* PE.07(D4), PE.08(D5),  PE.09(D6),  PE.10(D7), PE.11(D8)                  */
  /* PE.12(D9), PE.13(D10), PE.14(D11), PE.15(D12)                            */
  GPIOE->MODER    &= ~0xFFFFC000;       /* Clear Bits                         */
  GPIOE->MODER    |=  0xAAAA8000;       /* Alternate Function mode            */
  GPIOE->OSPEEDR  &= ~0xFFFFC000;       /* Clear Bits                         */
  GPIOE->OSPEEDR  |=  0xAAAA8000;       /* 50 MHz Fast speed                  */
  GPIOE->AFR[0]   &= ~0xF0000000;       /* Clear Bits                         */
  GPIOE->AFR[0]   |=  0xC0000000;       /* Alternate Function mode AF12       */
  GPIOE->AFR[1]   &= ~0xFFFFFFFF;       /* Clear Bits                         */
  GPIOE->AFR[1]   |=  0xCCCCCCCC;       /* Alternate Function mode AF12       */

  /* PF.00(A0 (RS)) */ 
  GPIOF->MODER    &= ~0x00000003;       /* Clear Bits                         */
  GPIOF->MODER    |=  0x00000002;       /* Alternate Function mode            */
  GPIOF->OSPEEDR  &= ~0x00000003;       /* Clear Bits                         */
  GPIOF->OSPEEDR  |=  0x00000002;       /* 50 MHz Fast speed                  */
  GPIOF->AFR[0]   &= ~0x0000000F;       /* Clear Bits                         */
  GPIOF->AFR[0]   |=  0x0000000C;       /* Alternate Function mode AF12       */

  /* PG.12(NE4 (LCD/CS)) - CE1(LCD /CS)                                       */
  GPIOG->MODER    &= ~0x03000000;       /* Clear Bits                         */
  GPIOG->MODER    |=  0x02000000;       /* Alternate Function mode            */
  GPIOG->OSPEEDR  &= ~0x03000000;       /* Clear Bits                         */
  GPIOG->OSPEEDR  |=  0x02000000;       /* 50 MHz Fast speed                  */
  GPIOG->AFR[1]   &= ~0x000F0000;       /* Clear Bits                         */
  GPIOG->AFR[1]   |=  0x000C0000;       /* Alternate Function mode AF12       */

  /* PC.07(LCD Backlight                                                      */
  GPIOC->BSRRH    |=  (1UL <<   7);     /* Backlight off                      */
  GPIOC->MODER    &= ~(3UL << 2*7);     /* Clear Bits                         */
  GPIOC->MODER    |=  (1UL << 2*7);     /* PC.7 is output                     */
  GPIOC->OTYPER   &= ~(1UL <<   7);     /* PC.7 is output Push-Pull           */
  GPIOC->OSPEEDR  &= ~(3UL << 2*7);     /* Clear Bits                         */
  GPIOC->OSPEEDR  |=  (2UL << 2*7);     /* PC.7 is 50MHz Fast Speed           */

/*-- FSMC Configuration ------------------------------------------------------*/
  RCC->AHB3ENR  |= (1UL << 0);          /* Enable FSMC clock                  */

  FSMC_Bank1->BTCR[(4-1)*2 + 1] =       /* Bank4 NOR/SRAM timing register     */
                                        /* configuration                      */
                          (0 << 28) |   /* FSMC AccessMode A                  */
                          (0 << 24) |   /* Data Latency                       */
                          (0 << 20) |   /* CLK Division                       */
                          (0 << 16) |   /* Bus Turnaround Duration            */
                         (11 <<  8) |   /* Data SetUp Time                    */
                          (0 <<  4) |   /* Address Hold Time                  */
                          (1 <<  0);    /* Address SetUp Time                 */

  FSMC_Bank1->BTCR[(4-1)*2 + 0] =       /* Control register                   */
                          (0 << 19) |   /* Write burst disabled               */
                          (0 << 15) |   /* Async wait disabled                */
                          (0 << 14) |   /* Extended mode disabled             */
                          (0 << 13) |   /* NWAIT signal is disabled           */ 
                          (1 << 12) |   /* Write operation enabled            */
                          (0 << 11) |   /* NWAIT signal is active one data    */
                                        /* cycle before wait state            */
                          (0 << 10) |   /* Wrapped burst mode disabled        */
                          (0 <<  9) |   /* Wait signal polarity active low    */
                          (0 <<  8) |   /* Burst access mode disabled         */
                          (1 <<  4) |   /* Memory data  bus width is 16 bits  */
                          (0 <<  2) |   /* Memory type is SRAM                */
                          (0 <<  1) |   /* Address/Data Multiplexing disable  */
                          (1 <<  0);    /* Memory Bank enable                 */

  GPIOC->BSRRL |= (1UL << 7);           /* Backlight on                       */
}

/*********************************************************************
*
*       LCD_X_Write0_16
*
* Purpose:
*   Write to controller, with A0 = 0
*/
void LCD_X_Write0_16(U16 c) {
  LCD_REG16 = c;
}

/*********************************************************************
*
*       LCD_X_Read0_16
*
* Purpose:
*   Read from controller, with A0 = 0
*/
U16 LCD_X_Read0_16(void) {
  return (LCD_REG16);
}

/*********************************************************************
*
*       LCD_X_Write1_16
*
* Purpose:
*   Write to controller, with A0 = 1
*/
void LCD_X_Write1_16(U16 c) {
  LCD_DAT16 = c;
}

/*********************************************************************
*
*       LCD_X_Read1_16
*
* Purpose:
*   Read from controller, with A0 = 1
*/
U16 LCD_X_Read1_16(void) {
  return (LCD_DAT16);
}

/*********************************************************************
*
*       LCD_X_WriteM1_16
*
* Purpose:
*   Write multiple bytes to controller, with A0 = 1
*/
void LCD_X_WriteM1_16(U16 * pData, int NumWords) {
  for (; NumWords; NumWords--) {
    LCD_DAT16 = *pData++;
  }
}

/*********************************************************************
*
*       LCD_X_ReadM1_16
*
* Purpose:
*   Read multiple bytes from controller, with A0 = 1
*/
void LCD_X_ReadM1_16(U16 * pData, int NumWords) {
  for (; NumWords; NumWords--) {
    *pData++ = LCD_DAT16;
  }
}

/*************************** End of file ****************************/
