/*----------------------------------------------------------------------------
 * Name:    Accel_LIS3DH.c
 * Purpose: LIS3DH Accelerometer functions
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
#include "Accel.h"
#include "Driver_I2C.h"

#ifndef ACCEL_I2C_PORT
#define ACCEL_I2C_PORT  1               /* I2C Port number                    */
#endif

#define ACCEL_I2C_ADDR  0x18            /* Accelerometer 7-bit I2C address    */
#define ACCEL_ID        0x33            /* Accelerometer ID (Who am I value)  */

/* I2C Driver */
#define _I2C_Driver_(n)  Driver_I2C##n
#define  I2C_Driver_(n) _I2C_Driver_(n)
extern ARM_DRIVER_I2C    I2C_Driver_(ACCEL_I2C_PORT);
#define ptrI2C         (&I2C_Driver_(ACCEL_I2C_PORT))

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


/*-----------------------------------------------------------------------------
 *      Accel_Write:  Write a value to the accelerometer register
 *
 * Parameters:  reg - register to write
 *              val - value to write
 *
 * Return:      true on success, false on error
 *----------------------------------------------------------------------------*/
static bool Accel_Write (uint8_t reg, uint8_t val) {
  uint8_t data[2];
  int32_t n;

  data[0] = reg;
  data[1] = val;
  n = ptrI2C->SendData(ACCEL_I2C_ADDR, data, 2, false);
  if (n != 2) return false;

  return true;
}


/*-----------------------------------------------------------------------------
 *      Accel_Read:  Read a value from the accelerometer register
 *
 * Parameters:  reg - register to read
 *              val - variable where value will be stored
 *
 * Return:      true on success, false on error
 *----------------------------------------------------------------------------*/
static bool Accel_Read (uint8_t reg, uint8_t *val) {
  uint8_t data[1];
  int32_t n;

  data[0] = reg;
  n = ptrI2C->SendData(ACCEL_I2C_ADDR, data, 1, true);
  if (n != 1) return false;

  n = ptrI2C->ReceiveData(ACCEL_I2C_ADDR, val, 1, false);
  if (n != 1) return false;

  return true;
}


/*-----------------------------------------------------------------------------
 *      Accel_Initialize:  Initialize accelerometer
 *
 * Parameters: (none)
 * Return:     0 on success, -1 if device ID invalid
 *----------------------------------------------------------------------------*/
int32_t Accel_Initialize (void) {
  uint8_t who = 0;

  ptrI2C->Initialize   (NULL);
  ptrI2C->PowerControl (ARM_POWER_FULL);
  ptrI2C->BusSpeed (ARM_I2C_BUS_SPEED_STANDARD);
  ptrI2C->BusClear();

  Accel_Read (WHO_AM_I, &who);
  if (who != ACCEL_ID) {
    return (-1);                        /* Wrong ID                           */
  }
  Accel_Write (CTRL_REG1, 0x77); 
  Accel_Write (CTRL_REG4, 0x00);
  Accel_Write (CTRL_REG5, 0x40);        /* Normal mode, FIFO Enable           */
  return (0);
}


/*-----------------------------------------------------------------------------
 *      Accel_Uninitialize:  Uninitialize accelerometer
 *
 * Parameters: (none)
 * Return:     0 on success, negative value on error
 *----------------------------------------------------------------------------*/
int32_t Accel_Uninitialize (void) {
  /* Put accelerometer into power-down mode */
  Accel_Write (CTRL_REG1, 0x08);
  return (0);
}


/*-----------------------------------------------------------------------------
 *      Accel_GetData:  Read acceleration data from registers and write them
 *                      into AXIS_DATA structure. Values are expressed in
 *                      two's complement.
 *
 * Parameters: axd - pointer to AXIS_DATA structure
 *
 * Return:     0 on success, -1 on error
 *----------------------------------------------------------------------------*/
int32_t Accel_GetData (AXIS_DATA *axd) {
  bool rtv;
  uint8_t valL, valH;
  
  rtv  = Accel_Read (OUT_X_L, &valL);
  rtv |= Accel_Read (OUT_X_H, &valH);
  axd->x = (int16_t)((valH << 8) | valL);
  
  rtv |= Accel_Read (OUT_Y_L, &valL);
  rtv |= Accel_Read (OUT_Y_H, &valH);
  axd->y = (int16_t)((valH << 8) | valL);
  
  rtv |= Accel_Read (OUT_Z_L, &valL);
  rtv |= Accel_Read (OUT_Z_H, &valH);
  
  axd->z = (int16_t)((valH << 8) | valL);
  return (rtv ? (0) : (-1));
}
