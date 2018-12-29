/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        16. April 2013
 * $Revision:    V1.00
 *  
 * Project:      FSMC Driver for ST STM32F4xx
 * Configured:   via RTE_Device.h configuration file 
 * -------------------------------------------------------------------- */

#include "FSMC_STM32F4xx.h"

#include "RTE_Device.h"


/* FSMC Pins
 +------------------+-----------------+------------------+-------------------+
 | PD0 : FSMC_D2    | PE0 : FSMC_NBL0 | PF0 : FSMC_A0    | PG0 : FSMC_A10    |
 | PD1 : FSMC_D3    | PE1 : FSMC_NBL1 | PF1 : FSMC_A1    | PG1 : FSMC_A11    |
 | PD3 : FSMC_CLK   | PE2 : FSMC_A23  | PF2 : FSMC_A2    | PG2 : FSMC_A12    |
 | PD4 : FSMC_NOE   | PE3 : FSMC_A19  | PF3 : FSMC_A3    | PG3 : FSMC_A13    |
 | PD5 : FSMC_NWE   | PE4 : FSMC_A20  | PF4 : FSMC_A4    | PG4 : FSMC_A14    |
 | PD6 : FSMC_NWAIT | PE5 : FSMC_A21  | PF5 : FSMC_A5    | PG5 : FSMC_A15    |
 | PD7 : FSMC_NE1   | PE6 : FSMC_A22  | PF6 : FSMC_NIORD | PG6 : FSMC_INT2   |
 |       FSMC_NCE2  | PE7 : FSMC_D4   | PF7 : FSMC_NREG  | PG7 : FSMC_INT3   |
 | PD8 : FSMC_D13   | PE8 : FSMC_D5   | PF8 : FSMC_NIOWR | PG9 : FSMC_NE2    |
 | PD9 : FSMC_D14   | PE9 : FSMC_D6   | PF9 : FSMC_CD    |       FSMC_NCE3   |
 | PD10: FSMC_D15   | PE10: FSMC_D7   | PF10: FSMC_INTR  | PG10: FSMC_NE3    |
 | PD11: FSMC_A16   | PE11: FSMC_D8   | PF12: FSMC_A6    |       FSMC_NCE4_1 |
 | PD12: FSMC_A17   | PE12: FSMC_D9   | PF13: FSMC_A7    | PG11: FSMC_NCE4_2 |
 | PD13: FSMC_A18   | PE13: FSMC_D10  | PF14: FSMC_A8    | PG12: FSMC_NE4    |
 | PD14: FSMC_D0    | PE14: FSMC_D11  | PF15: FSMC_A9    | PG13: FSMC_A24    |
 | PD15: FSMC_D1    | PE15: FSMC_D12  | PB7 : FSMC_NL    | PG14: FSMC_A25    |
 +------------------+-----------------+------------------+-------------------+
 */

#define FSMC_PORTB_MASK \
  ((RTE_FSMC_NL_PIN    != 0) ? (1<< 7) : 0)

#define FSMC_PORTD_MASK \
                              ((1<< 0) | (1<< 1) | (1<<14) | (1<<15) | \
  ((RTE_FSMC_DBUS_PINS == 1) ? (1<< 8) | (1<< 9) | (1<<10) : 0)      | \
  ((RTE_FSMC_ABUS_PINS == 0) ? (1<<11) | (1<<12) : 0) | \
  ((RTE_FSMC_ABUS_PINS >= 3) ? (1<<11) : 0) | \
  ((RTE_FSMC_ABUS_PINS >= 4) ? (1<<12) : 0) | \
  ((RTE_FSMC_ABUS_PINS >= 5) ? (1<<13) : 0) | \
  ((RTE_FSMC_CLK_PIN   != 0) ? (1<< 3) : 0) | \
  ((RTE_FSMC_NOE_PIN   != 0) ? (1<< 4) : 0) | \
  ((RTE_FSMC_NWE_PIN   != 0) ? (1<< 5) : 0) | \
  ((RTE_FSMC_NWAIT_PIN != 0) ? (1<< 6) : 0) | \
  ((RTE_FSMC_NE1_PIN   != 0) ? (1<< 7) : 0))

#define FSMC_PORTE_MASK \
                              ((1<< 7) | (1<< 8) | (1<< 9) | (1<<10)                | \
  ((RTE_FSMC_DBUS_PINS == 1) ? (1<<11) | (1<<12) | (1<<13) | (1<<14) | (1<<15) : 0) | \
  ((RTE_FSMC_ABUS_PINS >= 6) ? (1<< 3) : 0) | \
  ((RTE_FSMC_ABUS_PINS >= 7) ? (1<< 4) : 0) | \
  ((RTE_FSMC_ABUS_PINS >= 8) ? (1<< 5) : 0) | \
  ((RTE_FSMC_ABUS_PINS >= 9) ? (1<< 6) : 0) | \
  ((RTE_FSMC_ABUS_PINS >=10) ? (1<< 2) : 0) | \
  ((RTE_FSMC_NBL0_PIN  != 0) ? (1<< 0) : 0) | \
  ((RTE_FSMC_NBL1_PIN  != 0) ? (1<< 1) : 0))

#define FSMC_PORTF_MASK \
 (((RTE_FSMC_ABUS_PINS != 0) ? (1<< 0) | (1<< 1) | (1<< 2) | (1<< 3) | (1<< 4) | (1<< 5) | \
                               (1<<12) | (1<<13) | (1<<14) | (1<<15) : 0)                | \
  ((RTE_FSMC_NIORD_PIN != 0) ? (1<< 6) : 0) | \
  ((RTE_FSMC_NREG_PIN  != 0) ? (1<< 7) : 0) | \
  ((RTE_FSMC_NIOWR_PIN != 0) ? (1<< 8) : 0) | \
  ((RTE_FSMC_CD_PIN    != 0) ? (1<< 9) : 0) | \
  ((RTE_FSMC_INTR_PIN  != 0) ? (1<<10) : 0))

#define FSMC_PORTG_MASK \
 (((RTE_FSMC_ABUS_PINS >= 1) ? (1<< 0) : 0) | \
  ((RTE_FSMC_ABUS_PINS >= 2) ? (1<< 1) | (1<< 2) | (1<< 3) | (1<< 4) | (1<< 5) : 0) | \
  ((RTE_FSMC_ABUS_PINS >=11) ? (1<<13) : 0) | \
  ((RTE_FSMC_ABUS_PINS ==12) ? (1<<14) : 0) | \
  ((RTE_FSMC_INT2_PIN  != 0) ? (1<< 6) : 0) | \
  ((RTE_FSMC_INT3_PIN  != 0) ? (1<< 7) : 0) | \
  ((RTE_FSMC_NE2_PIN   != 0) ? (1<< 9) : 0) | \
  ((RTE_FSMC_NE3_PIN   != 0) ? (1<<10) : 0) | \
  ((RTE_FSMC_NCE42_PIN != 0) ? (1<<11) : 0) | \
  ((RTE_FSMC_NE4_PIN   != 0) ? (1<<12) : 0))

#define FSMC_PORT_MASK \
 (((FSMC_PORTB_MASK    != 0) ? (1<< 1) : 0) | \
  ((FSMC_PORTD_MASK    != 0) ? (1<< 3) : 0) | \
  ((FSMC_PORTE_MASK    != 0) ? (1<< 4) : 0) | \
  ((FSMC_PORTF_MASK    != 0) ? (1<< 5) : 0) | \
  ((FSMC_PORTG_MASK    != 0) ? (1<< 6) : 0))

#define PORT_MASK2(x) \
 (((x & (1<< 0)) ? (0x3UL<< 0) : 0) | \
  ((x & (1<< 1)) ? (0x3UL<< 2) : 0) | \
  ((x & (1<< 2)) ? (0x3UL<< 4) : 0) | \
  ((x & (1<< 3)) ? (0x3UL<< 6) : 0) | \
  ((x & (1<< 4)) ? (0x3UL<< 8) : 0) | \
  ((x & (1<< 5)) ? (0x3UL<<10) : 0) | \
  ((x & (1<< 6)) ? (0x3UL<<12) : 0) | \
  ((x & (1<< 7)) ? (0x3UL<<14) : 0) | \
  ((x & (1<< 8)) ? (0x3UL<<16) : 0) | \
  ((x & (1<< 9)) ? (0x3UL<<18) : 0) | \
  ((x & (1<<10)) ? (0x3UL<<20) : 0) | \
  ((x & (1<<11)) ? (0x3UL<<22) : 0) | \
  ((x & (1<<12)) ? (0x3UL<<24) : 0) | \
  ((x & (1<<13)) ? (0x3UL<<26) : 0) | \
  ((x & (1<<14)) ? (0x3UL<<28) : 0) | \
  ((x & (1<<15)) ? (0x3UL<<30) : 0))

#define PORT_MASK4(x) \
 (((x & (1<< 0)) ? (0xFUL<< 0) : 0) | \
  ((x & (1<< 1)) ? (0xFUL<< 4) : 0) | \
  ((x & (1<< 2)) ? (0xFUL<< 8) : 0) | \
  ((x & (1<< 3)) ? (0xFUL<<12) : 0) | \
  ((x & (1<< 4)) ? (0xFUL<<16) : 0) | \
  ((x & (1<< 5)) ? (0xFUL<<20) : 0) | \
  ((x & (1<< 6)) ? (0xFUL<<24) : 0) | \
  ((x & (1<< 7)) ? (0xFUL<<28) : 0))

#define PORT_MASK4L(x) PORT_MASK4((x>>0)&0xFF)
#define PORT_MASK4H(x) PORT_MASK4((x>>8)&0xFF)


#if (RTE_FSMC_NE1 && RTE_FSMC_NCE2)
#error "NOR Flash/PSRAM @NE1 and NAND Flash @NCE2 cannot be used simultaneously! Check settings in RTE_Device.h!"
#endif
#if (RTE_FSMC_NE2 && RTE_FSMC_NCE3)
#error "NOR Flash/PSRAM @NE2 and NAND Flash @NCE3 cannot be used simultaneously! Check settings in RTE_Device.h!"
#endif
#if (RTE_FSMC_NE3 && RTE_FSMC_NCE4)
#error "NOR Flash/PSRAM @NE3 and PC Card @NCE4 cannot be used simultaneously! Check settings in RTE_Device.h!"
#endif

#if ((RTE_FSMC_NE1 || RTE_FSMC_NCE2) && !RTE_FSMC_NE1_PIN)
#error "FSMC_NE1/NCE2 pin is not configured in RTE_Device.h!"
#endif
#if ((RTE_FSMC_NE2 || RTE_FSMC_NCE3) && !RTE_FSMC_NE2_PIN)
#error "FSMC_NE2/NCE3 pin is not configured in RTE_Device.h!"
#endif
#if ((RTE_FSMC_NE3 || RTE_FSMC_NCE4) && !RTE_FSMC_NE3_PIN)
#error "FSMC_NE3/NCE4_1 pin is not configured in RTE_Device.h!"
#endif
#if (RTE_FSMC_NE4  && !RTE_FSMC_NE4_PIN)
#error "FSMC_NE4 pin is not configured in RTE_Device.h!"
#endif
#if (RTE_FSMC_NCE4 && !RTE_FSMC_NCE42_PIN)
#error "FSMC_NCE4_2 pin is not configured in RTE_Device.h!"
#endif


/**
  \fn          void FSMC_Setup (void)
  \brief       Setup FSMC
*/
void FSMC_Setup (void) {

  /* Configure FSMC pins */
  #if (RTE_FSMC_PINS)
    RCC->AHB1ENR |= FSMC_PORT_MASK;

    /* Port B */
    #if (FSMC_PORTB_MASK != 0)
      /* Alternate function: AF12 */
      #if (FSMC_PORTB_MASK & 0x00FF)
        GPIOB->AFR[0] = (GPIOB->AFR[0] & ~PORT_MASK4L(FSMC_PORTB_MASK)) |
                        (0xCCCCCCCC    &  PORT_MASK4L(FSMC_PORTB_MASK));
      #endif
      #if (FSMC_PORTB_MASK & 0xFF00)
        GPIOB->AFR[1] = (GPIOB->AFR[1] & ~PORT_MASK4H(FSMC_PORTB_MASK)) |
                        (0xCCCCCCCC    &  PORT_MASK4H(FSMC_PORTB_MASK));
      #endif
      /* Port Mode: Alternate function */
      GPIOB->MODER = (GPIOB->MODER & ~PORT_MASK2(FSMC_PORTB_MASK)) |
                     (0xAAAAAAAA   &  PORT_MASK2(FSMC_PORTB_MASK));
      /* Port Output Type: Push-pull */
      GPIOB->OTYPER  &= ~FSMC_PORTB_MASK;
      /* Port Output Speed: 100 MHz High Speed */
      GPIOB->OSPEEDR |=  PORT_MASK2(FSMC_PORTB_MASK);
      /* Port Pull-up/down: No Pull-up/down */
      GPIOB->PUPDR   &= ~PORT_MASK2(FSMC_PORTB_MASK);
    #endif

    /* Port D */
    #if (FSMC_PORTD_MASK != 0)
      /* Alternate function: AF12 */
      #if (FSMC_PORTD_MASK & 0x00FF)
        GPIOD->AFR[0] = (GPIOD->AFR[0] & ~PORT_MASK4L(FSMC_PORTD_MASK)) |
                        (0xCCCCCCCC    &  PORT_MASK4L(FSMC_PORTD_MASK));
      #endif
      #if (FSMC_PORTD_MASK & 0xFF00)
        GPIOD->AFR[1] = (GPIOD->AFR[1] & ~PORT_MASK4H(FSMC_PORTD_MASK)) |
                        (0xCCCCCCCC    &  PORT_MASK4H(FSMC_PORTD_MASK));
      #endif
      /* Port Mode: Alternate function */
      GPIOD->MODER = (GPIOD->MODER & ~PORT_MASK2(FSMC_PORTD_MASK)) |
                     (0xAAAAAAAA   &  PORT_MASK2(FSMC_PORTD_MASK));
      /* Port Output Type: Push-pull */
      GPIOD->OTYPER  &= ~FSMC_PORTD_MASK;
      /* Port Output Speed: 100 MHz High Speed */
      GPIOD->OSPEEDR |=  PORT_MASK2(FSMC_PORTD_MASK);
      /* Port Pull-up/down: No Pull-up/down */
      GPIOD->PUPDR   &= ~PORT_MASK2(FSMC_PORTD_MASK);
    #endif

    /* Port E */
    #if (FSMC_PORTE_MASK != 0)
      /* Alternate function: AF12 */
      #if (FSMC_PORTE_MASK & 0x00FF)
        GPIOE->AFR[0] = (GPIOE->AFR[0] & ~PORT_MASK4L(FSMC_PORTE_MASK)) |
                        (0xCCCCCCCC    &  PORT_MASK4L(FSMC_PORTE_MASK));
      #endif
      #if (FSMC_PORTE_MASK & 0xFF00)
        GPIOE->AFR[1] = (GPIOE->AFR[1] & ~PORT_MASK4H(FSMC_PORTE_MASK)) |
                        (0xCCCCCCCC    &  PORT_MASK4H(FSMC_PORTE_MASK));
      #endif
      /* Port Mode: Alternate function */
      GPIOE->MODER = (GPIOE->MODER & ~PORT_MASK2(FSMC_PORTE_MASK)) |
                     (0xAAAAAAAA   &  PORT_MASK2(FSMC_PORTE_MASK));
      /* Port Output Type: Push-pull */
      GPIOE->OTYPER  &= ~FSMC_PORTE_MASK;
      /* Port Output Speed: 100 MHz High Speed */
      GPIOE->OSPEEDR |=  PORT_MASK2(FSMC_PORTE_MASK);
      /* Port Pull-up/down: No Pull-up/down */
      GPIOE->PUPDR   &= ~PORT_MASK2(FSMC_PORTE_MASK);
    #endif

    /* Port F */
    #if (FSMC_PORTF_MASK != 0)
      /* Alternate function: AF12 */
      #if (FSMC_PORTF_MASK & 0x00FF)
        GPIOF->AFR[0] = (GPIOF->AFR[0] & ~PORT_MASK4L(FSMC_PORTF_MASK)) |
                        (0xCCCCCCCC    &  PORT_MASK4L(FSMC_PORTF_MASK));
      #endif
      #if (FSMC_PORTF_MASK & 0xFF00)
        GPIOF->AFR[1] = (GPIOF->AFR[1] & ~PORT_MASK4H(FSMC_PORTF_MASK)) |
                        (0xCCCCCCCC    &  PORT_MASK4H(FSMC_PORTF_MASK));
      #endif
      /* Port Mode: Alternate function */
      GPIOF->MODER = (GPIOF->MODER & ~PORT_MASK2(FSMC_PORTF_MASK)) |
                     (0xAAAAAAAA   &  PORT_MASK2(FSMC_PORTF_MASK));
      /* Port Output Type: Push-pull */
      GPIOF->OTYPER  &= ~FSMC_PORTF_MASK;
      /* Port Output Speed: 100 MHz High Speed */
      GPIOF->OSPEEDR |=  PORT_MASK2(FSMC_PORTF_MASK);
      /* Port Pull-up/down: No Pull-up/down */
      GPIOF->PUPDR   &= ~PORT_MASK2(FSMC_PORTF_MASK);
    #endif

    /* Port G */
    #if (FSMC_PORTG_MASK != 0)
      /* Alternate function: AF12 */
      #if (FSMC_PORTG_MASK & 0x00FF)
        GPIOG->AFR[0] = (GPIOG->AFR[0] & ~PORT_MASK4L(FSMC_PORTG_MASK)) |
                        (0xCCCCCCCC    &  PORT_MASK4L(FSMC_PORTG_MASK));
      #endif
      #if (FSMC_PORTG_MASK & 0xFF00)
        GPIOG->AFR[1] = (GPIOG->AFR[1] & ~PORT_MASK4H(FSMC_PORTG_MASK)) |
                        (0xCCCCCCCC    &  PORT_MASK4H(FSMC_PORTG_MASK));
      #endif
      /* Port Mode: Alternate function */
      GPIOG->MODER = (GPIOG->MODER & ~PORT_MASK2(FSMC_PORTG_MASK)) |
                     (0xAAAAAAAA   &  PORT_MASK2(FSMC_PORTG_MASK));
      /* Port Output Type: Push-pull */
      GPIOG->OTYPER  &= ~FSMC_PORTG_MASK;
      /* Port Output Speed: 100 MHz High Speed */
      GPIOG->OSPEEDR |=  PORT_MASK2(FSMC_PORTG_MASK);
      /* Port Pull-up/down: No Pull-up/down */
      GPIOG->PUPDR   &= ~PORT_MASK2(FSMC_PORTG_MASK);
    #endif

  #endif

  /* Enable FSMC peripheral clock */
  RCC->AHB3ENR |= RCC_AHB3ENR_FSMCEN;

  /* Configure NOR Flash / PSRAM Bank 1 @NE1 */
  #if (RTE_FSMC_NE1)
    FSMC_Bank1->BTCR[0] = \
      ((RTE_FSMC_BCR1_MBKEN     != 0) ? FSMC_BCR1_MBKEN     : 0) | \
      ((RTE_FSMC_BCR1_MUXEN     != 0) ? FSMC_BCR1_MUXEN     : 0) | \
      ((RTE_FSMC_BCR1_MTYP      == 1) ? FSMC_BCR1_MTYP_0    : 0) | \
      ((RTE_FSMC_BCR1_MTYP      == 2) ? FSMC_BCR1_MTYP_1    : 0) | \
      ((RTE_FSMC_BCR1_MWID      == 1) ? FSMC_BCR1_MWID_0    : 0) | \
      ((RTE_FSMC_BCR1_FACCEN    != 0) ? FSMC_BCR1_FACCEN    : 0) | \
      ((RTE_FSMC_BCR1_BURSTEN   != 0) ? FSMC_BCR1_BURSTEN   : 0) | \
      ((RTE_FSMC_BCR1_WAITPOL   != 0) ? FSMC_BCR1_WAITPOL   : 0) | \
      ((RTE_FSMC_BCR1_WRAPMOD   != 0) ? FSMC_BCR1_WRAPMOD   : 0) | \
      ((RTE_FSMC_BCR1_WAITCFG   != 0) ? FSMC_BCR1_WAITCFG   : 0) | \
      ((RTE_FSMC_BCR1_WREN      != 0) ? FSMC_BCR1_WREN      : 0) | \
      ((RTE_FSMC_BCR1_WAITEN    != 0) ? FSMC_BCR1_WAITEN    : 0) | \
      ((RTE_FSMC_BCR1_EXTMOD    != 0) ? FSMC_BCR1_EXTMOD    : 0) | \
      ((RTE_FSMC_BCR1_ASYNCWAIT != 0) ? FSMC_BCR1_ASYNCWAIT : 0) | \
      ((RTE_FSMC_BCR1_CBURSTRW  != 0) ? FSMC_BCR1_CBURSTRW  : 0);
    FSMC_Bank1->BTCR[1] = \
       (RTE_FSMC_BTR1_ADDSET    << 0) | \
       (RTE_FSMC_BTR1_ADDHLD    << 4) | \
       (RTE_FSMC_BTR1_DATAST    << 8) | \
       (RTE_FSMC_BTR1_BUSTURN   <<16) | \
       (RTE_FSMC_BTR1_CLKDIV    <<20) | \
       (RTE_FSMC_BTR1_DATLAT    <<24) | \
       ((uint32_t)
        RTE_FSMC_BTR1_ACCMOD    <<28);
    FSMC_Bank1E->BWTR[0] = \
       (RTE_FSMC_BWTR1_ADDSET   << 0) | \
       (RTE_FSMC_BWTR1_ADDHLD   << 4) | \
       (RTE_FSMC_BWTR1_DATAST   << 8) | \
       (RTE_FSMC_BWTR1_BUSTURN  <<16) | \
       (RTE_FSMC_BWTR1_CLKDIV   <<20) | \
       (RTE_FSMC_BWTR1_DATLAT   <<24) | \
       ((uint32_t)
        RTE_FSMC_BWTR1_ACCMOD   <<28);
  #endif

  /* Configure NOR Flash / PSRAM Bank 1 @NE2 */
  #if (RTE_FSMC_NE2)
    FSMC_Bank1->BTCR[2] = \
      ((RTE_FSMC_BCR2_MBKEN     != 0) ? FSMC_BCR2_MBKEN     : 0) | \
      ((RTE_FSMC_BCR2_MUXEN     != 0) ? FSMC_BCR2_MUXEN     : 0) | \
      ((RTE_FSMC_BCR2_MTYP      == 1) ? FSMC_BCR2_MTYP_0    : 0) | \
      ((RTE_FSMC_BCR2_MTYP      == 2) ? FSMC_BCR2_MTYP_1    : 0) | \
      ((RTE_FSMC_BCR2_MWID      == 1) ? FSMC_BCR2_MWID_0    : 0) | \
      ((RTE_FSMC_BCR2_FACCEN    != 0) ? FSMC_BCR2_FACCEN    : 0) | \
      ((RTE_FSMC_BCR2_BURSTEN   != 0) ? FSMC_BCR2_BURSTEN   : 0) | \
      ((RTE_FSMC_BCR2_WAITPOL   != 0) ? FSMC_BCR2_WAITPOL   : 0) | \
      ((RTE_FSMC_BCR2_WRAPMOD   != 0) ? FSMC_BCR2_WRAPMOD   : 0) | \
      ((RTE_FSMC_BCR2_WAITCFG   != 0) ? FSMC_BCR2_WAITCFG   : 0) | \
      ((RTE_FSMC_BCR2_WREN      != 0) ? FSMC_BCR2_WREN      : 0) | \
      ((RTE_FSMC_BCR2_WAITEN    != 0) ? FSMC_BCR2_WAITEN    : 0) | \
      ((RTE_FSMC_BCR2_EXTMOD    != 0) ? FSMC_BCR2_EXTMOD    : 0) | \
      ((RTE_FSMC_BCR2_ASYNCWAIT != 0) ? FSMC_BCR2_ASYNCWAIT : 0) | \
      ((RTE_FSMC_BCR2_CBURSTRW  != 0) ? FSMC_BCR2_CBURSTRW  : 0);
    FSMC_Bank1->BTCR[3] = \
       (RTE_FSMC_BTR2_ADDSET    << 0) | \
       (RTE_FSMC_BTR2_ADDHLD    << 4) | \
       (RTE_FSMC_BTR2_DATAST    << 8) | \
       (RTE_FSMC_BTR2_BUSTURN   <<16) | \
       (RTE_FSMC_BTR2_CLKDIV    <<20) | \
       (RTE_FSMC_BTR2_DATLAT    <<24) | \
       ((uint32_t)
        RTE_FSMC_BTR2_ACCMOD    <<28);
    FSMC_Bank1E->BWTR[2] = \
       (RTE_FSMC_BWTR2_ADDSET   << 0) | \
       (RTE_FSMC_BWTR2_ADDHLD   << 4) | \
       (RTE_FSMC_BWTR2_DATAST   << 8) | \
       (RTE_FSMC_BWTR2_BUSTURN  <<16) | \
       (RTE_FSMC_BWTR2_CLKDIV   <<20) | \
       (RTE_FSMC_BWTR2_DATLAT   <<24) | \
       ((uint32_t)
        RTE_FSMC_BWTR2_ACCMOD   <<28);
  #endif

  /* Configure NOR Flash / PSRAM Bank 1 @NE3 */
  #if (RTE_FSMC_NE3)
    FSMC_Bank1->BTCR[4] = \
      ((RTE_FSMC_BCR3_MBKEN     != 0) ? FSMC_BCR3_MBKEN     : 0) | \
      ((RTE_FSMC_BCR3_MUXEN     != 0) ? FSMC_BCR3_MUXEN     : 0) | \
      ((RTE_FSMC_BCR3_MTYP      == 1) ? FSMC_BCR3_MTYP_0    : 0) | \
      ((RTE_FSMC_BCR3_MTYP      == 2) ? FSMC_BCR3_MTYP_1    : 0) | \
      ((RTE_FSMC_BCR3_MWID      == 1) ? FSMC_BCR3_MWID_0    : 0) | \
      ((RTE_FSMC_BCR3_FACCEN    != 0) ? FSMC_BCR3_FACCEN    : 0) | \
      ((RTE_FSMC_BCR3_BURSTEN   != 0) ? FSMC_BCR3_BURSTEN   : 0) | \
      ((RTE_FSMC_BCR3_WAITPOL   != 0) ? FSMC_BCR3_WAITPOL   : 0) | \
      ((RTE_FSMC_BCR3_WRAPMOD   != 0) ? FSMC_BCR3_WRAPMOD   : 0) | \
      ((RTE_FSMC_BCR3_WAITCFG   != 0) ? FSMC_BCR3_WAITCFG   : 0) | \
      ((RTE_FSMC_BCR3_WREN      != 0) ? FSMC_BCR3_WREN      : 0) | \
      ((RTE_FSMC_BCR3_WAITEN    != 0) ? FSMC_BCR3_WAITEN    : 0) | \
      ((RTE_FSMC_BCR3_EXTMOD    != 0) ? FSMC_BCR3_EXTMOD    : 0) | \
      ((RTE_FSMC_BCR3_ASYNCWAIT != 0) ? FSMC_BCR3_ASYNCWAIT : 0) | \
      ((RTE_FSMC_BCR3_CBURSTRW  != 0) ? FSMC_BCR3_CBURSTRW  : 0);
    FSMC_Bank1->BTCR[5] = \
       (RTE_FSMC_BTR3_ADDSET    << 0) | \
       (RTE_FSMC_BTR3_ADDHLD    << 4) | \
       (RTE_FSMC_BTR3_DATAST    << 8) | \
       (RTE_FSMC_BTR3_BUSTURN   <<16) | \
       (RTE_FSMC_BTR3_CLKDIV    <<20) | \
       (RTE_FSMC_BTR3_DATLAT    <<24) | \
       ((uint32_t)
        RTE_FSMC_BTR3_ACCMOD    <<28);
    FSMC_Bank1E->BWTR[4] = \
       (RTE_FSMC_BWTR3_ADDSET   << 0) | \
       (RTE_FSMC_BWTR3_ADDHLD   << 4) | \
       (RTE_FSMC_BWTR3_DATAST   << 8) | \
       (RTE_FSMC_BWTR3_BUSTURN  <<16) | \
       (RTE_FSMC_BWTR3_CLKDIV   <<20) | \
       (RTE_FSMC_BWTR3_DATLAT   <<24) | \
       ((uint32_t)
        RTE_FSMC_BWTR3_ACCMOD   <<28);
  #endif

  /* Configure NOR Flash / PSRAM Bank 1 @NE4 */
  #if (RTE_FSMC_NE4)
    FSMC_Bank1->BTCR[6] = \
      ((RTE_FSMC_BCR4_MBKEN     != 0) ? FSMC_BCR4_MBKEN     : 0) | \
      ((RTE_FSMC_BCR4_MUXEN     != 0) ? FSMC_BCR4_MUXEN     : 0) | \
      ((RTE_FSMC_BCR4_MTYP      == 1) ? FSMC_BCR4_MTYP_0    : 0) | \
      ((RTE_FSMC_BCR4_MTYP      == 2) ? FSMC_BCR4_MTYP_1    : 0) | \
      ((RTE_FSMC_BCR4_MWID      == 1) ? FSMC_BCR4_MWID_0    : 0) | \
      ((RTE_FSMC_BCR4_FACCEN    != 0) ? FSMC_BCR4_FACCEN    : 0) | \
      ((RTE_FSMC_BCR4_BURSTEN   != 0) ? FSMC_BCR4_BURSTEN   : 0) | \
      ((RTE_FSMC_BCR4_WAITPOL   != 0) ? FSMC_BCR4_WAITPOL   : 0) | \
      ((RTE_FSMC_BCR4_WRAPMOD   != 0) ? FSMC_BCR4_WRAPMOD   : 0) | \
      ((RTE_FSMC_BCR4_WAITCFG   != 0) ? FSMC_BCR4_WAITCFG   : 0) | \
      ((RTE_FSMC_BCR4_WREN      != 0) ? FSMC_BCR4_WREN      : 0) | \
      ((RTE_FSMC_BCR4_WAITEN    != 0) ? FSMC_BCR4_WAITEN    : 0) | \
      ((RTE_FSMC_BCR4_EXTMOD    != 0) ? FSMC_BCR4_EXTMOD    : 0) | \
      ((RTE_FSMC_BCR4_ASYNCWAIT != 0) ? FSMC_BCR4_ASYNCWAIT : 0) | \
      ((RTE_FSMC_BCR4_CBURSTRW  != 0) ? FSMC_BCR4_CBURSTRW  : 0);
    FSMC_Bank1->BTCR[7] = \
       (RTE_FSMC_BTR4_ADDSET    << 0) | \
       (RTE_FSMC_BTR4_ADDHLD    << 4) | \
       (RTE_FSMC_BTR4_DATAST    << 8) | \
       (RTE_FSMC_BTR4_BUSTURN   <<16) | \
       (RTE_FSMC_BTR4_CLKDIV    <<20) | \
       (RTE_FSMC_BTR4_DATLAT    <<24) | \
       ((uint32_t)
        RTE_FSMC_BTR4_ACCMOD    <<28);
    FSMC_Bank1E->BWTR[6] = \
       (RTE_FSMC_BWTR4_ADDSET   << 0) | \
       (RTE_FSMC_BWTR4_ADDHLD   << 4) | \
       (RTE_FSMC_BWTR4_DATAST   << 8) | \
       (RTE_FSMC_BWTR4_BUSTURN  <<16) | \
       (RTE_FSMC_BWTR4_CLKDIV   <<20) | \
       (RTE_FSMC_BWTR4_DATLAT   <<24) | \
       ((uint32_t)
        RTE_FSMC_BWTR4_ACCMOD   <<28);
  #endif

  /* Configure NAND Flash @NCE2 */
  #if (RTE_FSMC_NCE2)
    FSMC_Bank2->PCR2 = \
      ((RTE_FSMC_PCR2_PWAITEN   != 0) ? FSMC_PCR2_PWAITEN : 0) | \
      ((RTE_FSMC_PCR2_PBKEN     != 0) ? FSMC_PCR2_PBKEN   : 0) | \
      ((RTE_FSMC_PCR2_PTYP      != 0) ? FSMC_PCR2_PTYP    : 0) | \
      ((RTE_FSMC_PCR2_PWID      == 1) ? FSMC_PCR2_PWID_0  : 0) | \
      ((RTE_FSMC_PCR2_ECCEN     != 0) ? FSMC_PCR2_ECCEN   : 0) | \
       (RTE_FSMC_PCR2_TCLR      << 9) | \
       (RTE_FSMC_PCR2_TAR       <<13) | \
       (RTE_FSMC_PCR2_ECCPS     <<17);
    FSMC_Bank2->SR2 = \
      ((RTE_FSMC_SR2_IREN       != 0) ? FSMC_SR2_IREN : 0) | \
      ((RTE_FSMC_SR2_ILEN       != 0) ? FSMC_SR2_ILEN : 0) | \
      ((RTE_FSMC_SR2_IFEN       != 0) ? FSMC_SR2_IFEN : 0);
    FSMC_Bank2->PMEM2 = \
       (RTE_FSMC_PMEM2_MEMSET   << 0) | \
       (RTE_FSMC_PMEM2_MEMWAIT  << 8) | \
       (RTE_FSMC_PMEM2_MEMHOLD  <<16) | \
       ((uint32_t)
        RTE_FSMC_PMEM2_MEMHIZ   <<24);
    FSMC_Bank2->PATT2 = \
       (RTE_FSMC_PATT2_ATTSET   << 0) | \
       (RTE_FSMC_PATT2_ATTWAIT  << 8) | \
       (RTE_FSMC_PATT2_ATTHOLD  <<16) | \
       ((uint32_t)
        RTE_FSMC_PATT2_ATTHIZ   <<24);
  #endif

  /* Configure NAND Flash @NCE3 */
  #if (RTE_FSMC_NCE3)
    FSMC_Bank3->PCR3 = \
      ((RTE_FSMC_PCR3_PWAITEN   != 0) ? FSMC_PCR3_PWAITEN : 0) | \
      ((RTE_FSMC_PCR3_PBKEN     != 0) ? FSMC_PCR3_PBKEN   : 0) | \
      ((RTE_FSMC_PCR3_PTYP      != 0) ? FSMC_PCR3_PTYP    : 0) | \
      ((RTE_FSMC_PCR3_PWID      == 1) ? FSMC_PCR3_PWID_0  : 0) | \
      ((RTE_FSMC_PCR3_ECCEN     != 0) ? FSMC_PCR3_ECCEN   : 0) | \
       (RTE_FSMC_PCR3_TCLR      << 9) | \
       (RTE_FSMC_PCR3_TAR       <<13) | \
       (RTE_FSMC_PCR3_ECCPS     <<17);
    FSMC_Bank3->SR3 = \
      ((RTE_FSMC_SR3_IREN       != 0) ? FSMC_SR3_IREN : 0) | \
      ((RTE_FSMC_SR3_ILEN       != 0) ? FSMC_SR3_ILEN : 0) | \
      ((RTE_FSMC_SR3_IFEN       != 0) ? FSMC_SR3_IFEN : 0);
    FSMC_Bank3->PMEM3 = \
       (RTE_FSMC_PMEM3_MEMSET   << 0) | \
       (RTE_FSMC_PMEM3_MEMWAIT  << 8) | \
       (RTE_FSMC_PMEM3_MEMHOLD  <<16) | \
       ((uint32_t)
        RTE_FSMC_PMEM3_MEMHIZ   <<24);
    FSMC_Bank3->PATT3 = \
       (RTE_FSMC_PATT3_ATTSET   << 0) | \
       (RTE_FSMC_PATT3_ATTWAIT  << 8) | \
       (RTE_FSMC_PATT3_ATTHOLD  <<16) | \
       ((uint32_t)
        RTE_FSMC_PATT3_ATTHIZ   <<24);
  #endif

  /* Configure PC Card @NCE4 */
  #if (RTE_FSMC_NCE4)
    FSMC_Bank4->PCR4 = \
      ((RTE_FSMC_PCR4_PWAITEN   != 0) ? FSMC_PCR4_PWAITEN : 0) | \
      ((RTE_FSMC_PCR4_PBKEN     != 0) ? FSMC_PCR4_PBKEN   : 0) | \
      ((RTE_FSMC_PCR4_PTYP      != 0) ? FSMC_PCR4_PTYP    : 0) | \
      ((RTE_FSMC_PCR4_PWID      == 1) ? FSMC_PCR4_PWID_0  : 0) | \
      ((RTE_FSMC_PCR4_ECCEN     != 0) ? FSMC_PCR4_ECCEN   : 0) | \
       (RTE_FSMC_PCR4_TCLR      << 9) | \
       (RTE_FSMC_PCR4_TAR       <<13) | \
       (RTE_FSMC_PCR4_ECCPS     <<17);
    FSMC_Bank4->SR4 = \
      ((RTE_FSMC_SR4_IREN       != 0) ? FSMC_SR4_IREN : 0) | \
      ((RTE_FSMC_SR4_ILEN       != 0) ? FSMC_SR4_ILEN : 0) | \
      ((RTE_FSMC_SR4_IFEN       != 0) ? FSMC_SR4_IFEN : 0);
    FSMC_Bank4->PMEM4 = \
       (RTE_FSMC_PMEM4_MEMSET   << 0) | \
       (RTE_FSMC_PMEM4_MEMWAIT  << 8) | \
       (RTE_FSMC_PMEM4_MEMHOLD  <<16) | \
       ((uint32_t)
        RTE_FSMC_PMEM4_MEMHIZ   <<24);
    FSMC_Bank4->PATT4 = \
       (RTE_FSMC_PATT4_ATTSET   << 0) | \
       (RTE_FSMC_PATT4_ATTWAIT  << 8) | \
       (RTE_FSMC_PATT4_ATTHOLD  <<16) | \
       ((uint32_t)
        RTE_FSMC_PATT4_ATTHIZ   <<24);
    FSMC_Bank4->PIO4 = \
       (RTE_FSMC_PIO4_IOSET     << 0) | \
       (RTE_FSMC_PIO4_IOWAIT    << 8) | \
       (RTE_FSMC_PIO4_IOHOLD    <<16) | \
       ((uint32_t)
        RTE_FSMC_PIO4_IOHIZ     <<24);
  #endif

  /* Enable FSMC interrupts */
#if ((RTE_FSMC_NCE2 && RTE_FSMC_INT2_PIN) || \
     (RTE_FSMC_NCE3 && RTE_FSMC_INT3_PIN) || \
     (RTE_FSMC_NCE4 && RTE_FSMC_INTR_PIN))
  NVIC_EnableIRQ(FSMC_IRQn);
#endif
}

/**
  \fn          bool FSMC_EnableECC (FSMC_BANK bank, bool enable)
  \brief       Enable/Disable ECC
  \param[in]   bank    Bank selection
  \param[in]   enable  Enable/Disable ECC
  \return      true  - success
               false - error
*/
bool FSMC_EnableECC (FSMC_BANK bank, bool enable) {

  switch (bank) {
    case FSMC_BANK2:
      if (enable) {
        FSMC_Bank2->PCR2 |=  FSMC_PCR2_ECCEN;
      } else {
        FSMC_Bank2->PCR2 &= ~FSMC_PCR2_ECCEN;
      }
      break;
    case FSMC_BANK3:
      if (enable) {
        FSMC_Bank3->PCR3 |=  FSMC_PCR3_ECCEN;
      } else {
        FSMC_Bank3->PCR3 &= ~FSMC_PCR3_ECCEN;
      }
      break;
    default:
      return false;
  }

  return true;
}

/**
  \fn          uint32_t FSMC_GetECC (FSMC_BANK bank)
  \brief       Get ECC Result
  \param[in]   bank  Bank selection
  \return      32-bit ECC value
*/
uint32_t FSMC_GetECC (FSMC_BANK bank) {

  switch (bank) {
    case FSMC_BANK2:
      return FSMC_Bank2->ECCR2;
    case FSMC_BANK3:
      return FSMC_Bank3->ECCR3;
    default:
      break;
  }

  return 0;
}


#if (RTE_FSMC_NCE2 && RTE_FSMC_INT2_PIN)
__weak void FSMC_INT2_Event (uint32_t event) {};
#endif
#if (RTE_FSMC_NCE3 && RTE_FSMC_INT3_PIN)
__weak void FSMC_INT3_Event (uint32_t event) {};
#endif
#if (RTE_FSMC_NCE4 && RTE_FSMC_INTR_PIN)
__weak void FSMC_INTR_Event (uint32_t event) {};
#endif

#if ((RTE_FSMC_NCE2 && RTE_FSMC_INT2_PIN) || \
     (RTE_FSMC_NCE3 && RTE_FSMC_INT3_PIN) || \
     (RTE_FSMC_NCE4 && RTE_FSMC_INTR_PIN))

/**
  \fn          void FSMC_IRQHandler (void)
  \brief       FSMC interrupt handler
*/
void FSMC_IRQHandler (void) {
  uint32_t events;

#if (RTE_FSMC_NCE2 && RTE_FSMC_INT2_PIN)
  events = FSMC_Bank2->SR2 & (FSMC_SR2_IRS | FSMC_SR2_IFS | FSMC_SR2_ILS);
  FSMC_Bank2->SR2 &= ~events;
  FSMC_INT2_Event(events);
#endif

#if (RTE_FSMC_NCE3 && RTE_FSMC_INT3_PIN)
  events = FSMC_Bank3->SR3 & (FSMC_SR3_IRS | FSMC_SR3_IFS | FSMC_SR3_ILS);
  FSMC_Bank3->SR3 &= ~events;
  FSMC_INT3_Event(events);
#endif

#if (RTE_FSMC_NCE4 && RTE_FSMC_INTR_PIN)
  events = FSMC_Bank4->SR4 & (FSMC_SR4_IRS | FSMC_SR4_IFS | FSMC_SR4_ILS);
  FSMC_Bank4->SR4 &= ~events;
  FSMC_INTR_Event(events);
#endif
}

#endif
