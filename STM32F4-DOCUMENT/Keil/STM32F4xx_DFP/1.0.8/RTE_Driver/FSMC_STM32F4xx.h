/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        15. March 2013
 * $Revision:    V1.00
 *  
 * Project:      FSMC Driver definitions for ST STM32F4xx
 * -------------------------------------------------------------------- */

#ifndef __FSMC_STM32F4XX_H
#define __FSMC_STM32F4XX_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"


/// INTx Pin Event
#define FSMC_INT_RISING_EDGE    (1<<0)
#define FSMC_INT_FALLING_EDGE   (1<<2)
#define FSMC_INT_HIGH_LEVEL     (1<<1)

/// NAND Bank
typedef enum _FSMC_BANK {
  FSMC_BANK1,
  FSMC_BANK2,
  FSMC_BANK3,
  FSMC_BANK4
} FSMC_BANK;


/**
  \fn          void FSMC_Setup (void)
  \brief       Setup FSMC
*/
extern void FSMC_Setup (void);

/**
  \fn          bool FSMC_EnableECC (FSMC_BANK bank, bool enable)
  \brief       Enable/Disable ECC
  \param[in]   bank    Bank selection
  \param[in]   enable  Enable/Disable ECC
  \return      true  - success
               false - error
*/
extern bool FSMC_EnableECC (FSMC_BANK bank, bool enable);

  /**
  \fn          uint32_t FSMC_GetECC (FSMC_BANK bank)
  \brief       Get ECC Result
  \param[in]   bank  Bank selection
  \return      32-bit ECC value
*/
extern uint32_t FSMC_GetECC (FSMC_BANK bank);


#endif /* __FSMC_STM32F4XX_H */
