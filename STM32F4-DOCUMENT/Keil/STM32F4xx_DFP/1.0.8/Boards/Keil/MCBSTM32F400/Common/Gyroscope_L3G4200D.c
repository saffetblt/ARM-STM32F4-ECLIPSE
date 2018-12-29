/*----------------------------------------------------------------------------
 * Name:    Gyroscope_L3G4200D.c
 * Purpose: L3G4200D & L3GD20 gyroscope functions
 * Note(s):
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

#include "cmsis_os.h"
#include "Gyroscope.h"
#include "Driver_I2C.h"

#ifndef GYROSCOPE_I2C_PORT
#define GYROSCOPE_I2C_PORT      1       /* I2C Port number                    */
#endif

#define GYROSCOPE_I2C_ADDR      0x68    /* Gyroscope 7-bit I2C address        */
#define GYROSCOPE_ID            0xD3    /* Gyroscope ID (Who am I value)      */

#define GYROSCOPE_I2C_ADDR_2    0x6A    /* Gyroscope 7-bit I2C address 2      */
#define GYROSCOPE_ID_2          0xD4    /* This driver also works with L3GD20 */

/* I2C Driver */
#define _I2C_Driver_(n)  Driver_I2C##n
#define  I2C_Driver_(n) _I2C_Driver_(n)
extern ARM_DRIVER_I2C    I2C_Driver_(GYROSCOPE_I2C_PORT);
#define ptrI2C         (&I2C_Driver_(GYROSCOPE_I2C_PORT))

/* Register addresses */
#define WHO_AM_I        0x0F
#define CTRL_REG1       0x20
#define CTRL_REG4       0x23
#define CTRL_REG5       0x24
#define OUT_X_L         0x28
#define OUT_X_H         0x29
#define OUT_Y_L         0x2A
#define OUT_Y_H         0x2B
#define OUT_Z_L         0x2C
#define OUT_Z_H         0x2D

static uint8_t DeviceAddr;

/*-----------------------------------------------------------------------------
 *      Gyroscope_Write:  Write a value to the gyroscope register
 *
 * Parameters:  reg - register to write
 *              val - value to write
 *
 * Return:      true on success, false on error
 *----------------------------------------------------------------------------*/
static bool Gyroscope_Write (uint8_t reg, uint8_t val) {
  uint8_t data[2];
  int32_t n;

  data[0] = reg;
  data[1] = val;
  n = ptrI2C->SendData (DeviceAddr, data, 2, false);
  if (n != 2) return false;

  return true;
}


/*-----------------------------------------------------------------------------
 *      Gyroscope_Read:  Read a value from the gyroscope register
 *
 * Parameters:  reg - register to read
 *              val - variable where value will be stored
 *
 * Return:      true on success, false on error
 *----------------------------------------------------------------------------*/
static bool Gyroscope_Read (uint8_t reg, uint8_t *val) {
  uint8_t data[1];
  int32_t n;

  data[0] = reg;
  n = ptrI2C->SendData (DeviceAddr, data, 1, true);
  if (n != 1) return false;

  n = ptrI2C->ReceiveData (DeviceAddr, val, 1, false);
  if (n != 1) return false;

  return true;
}

/*-----------------------------------------------------------------------------
 *      Gyroscope_Initialize:  Initialize gyroscope
 *
 * Parameters: (none)
 * Return:     0 on success, -1 if device ID invalid
 *----------------------------------------------------------------------------*/
int32_t Gyroscope_Initialize (void) {
  uint8_t who = 0;

  ptrI2C->Initialize   (NULL);
  ptrI2C->PowerControl (ARM_POWER_FULL);
  ptrI2C->BusSpeed (ARM_I2C_BUS_SPEED_STANDARD);
  ptrI2C->BusClear();

  /* Init L3G4200D device */
  DeviceAddr = GYROSCOPE_I2C_ADDR;
  if (Gyroscope_Read (WHO_AM_I, &who) == true) {
    if (who != GYROSCOPE_ID) {
      return (-1);                      /* Wrong ID                           */
    }
  }
  else {
    /* Check if this is L3GD20 device (L3G4200D compatible) */
    DeviceAddr = GYROSCOPE_I2C_ADDR_2;
    if (Gyroscope_Read (WHO_AM_I, &who) == true) {
      if (who != GYROSCOPE_ID_2) {
        return (-1);                    /* Wrong ID                           */
      }
    }
  }

  Gyroscope_Write (CTRL_REG1, 0x0F);    /* ODR: 100Hz, Cut-Off: 12.5,         */
  Gyroscope_Write (CTRL_REG4, 0x00);    /* Continous update,full scale 250dps */
  Gyroscope_Write (CTRL_REG5, 0x40);    /* FIFO Enable                        */
  return (0);
}


/*-----------------------------------------------------------------------------
 *      Gyroscope_Uninitialize:  Uninitialize gyroscope
 *
 * Parameters: (none)
 * Return:     0 on success, negative value on error
 *----------------------------------------------------------------------------*/
int32_t Gyroscope_Uninitialize (void) {
  /* Put gyrocope into power-down mode */
  Gyroscope_Write (CTRL_REG1, 0x00);
  return (0);
}


/*-----------------------------------------------------------------------------
 *      Gyroscope_GetData: Read angular rate data from registers and write them
 *                         into ANGLE_RATE structure. Values are expressed in
 *                         two's complement.
 *
 * Parameters: ang - pointer to ANGLE_RATE structure
 *
 * Return:     0 on success, -1 on error
 *----------------------------------------------------------------------------*/
int32_t Gyroscope_GetData (ANGLE_RATE *ang) {
  bool rtv;
  uint8_t valL, valH;
  
  rtv  = Gyroscope_Read (OUT_X_L, &valL);
  rtv |= Gyroscope_Read (OUT_X_H, &valH);
  ang->x = (int16_t)((valH << 8) | valL);
  
  rtv |= Gyroscope_Read (OUT_Y_L, &valL);
  rtv |= Gyroscope_Read (OUT_Y_H, &valH);
  ang->y = (int16_t)((valH << 8) | valL);
  
  rtv |= Gyroscope_Read (OUT_Z_L, &valL);
  rtv |= Gyroscope_Read (OUT_Z_H, &valH);
  
  ang->z = (int16_t)((valH << 8) | valL);
  return (rtv ? (0) : (-1));
}
