/*-----------------------------------------------------------------------------
 * Name:    Joystick_STMPE811.c
 * Purpose: Joystick functions for STMPE811
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

#include "cmsis_os.h"
#include "STMPE811.h"
#include "Joystick.h"
#include "Driver_I2C.h"

#ifndef TSC_I2C_PORT
#define TSC_I2C_PORT    1               /* I2C Port number                    */
#endif

#define TSC_I2C_ADDR    0x44            /* 7-bit I2C address                  */

/* I2C Driver */
#define _I2C_Driver_(n)  Driver_I2C##n
#define  I2C_Driver_(n) _I2C_Driver_(n)
extern ARM_DRIVER_I2C    I2C_Driver_(TSC_I2C_PORT);
#define ptrI2C         (&I2C_Driver_(TSC_I2C_PORT))


/*-----------------------------------------------------------------------------
 *      Touch_Write:  Write a value to the touchscreen controller register
 *
 *  Parameters: reg - register to write
 *              val - value to write
 *  Return:     true on success, false on error
 *----------------------------------------------------------------------------*/
static bool Touch_Write (uint8_t reg, uint8_t val) {
  uint8_t data[2];
  int32_t n;

  data[0] = reg;
  data[1] = val;
  n = ptrI2C->SendData(TSC_I2C_ADDR, data, 2, false);
  if (n != 2) return false;

  return true;
}

/*-----------------------------------------------------------------------------
 *      Touch_Read:  Read a value from the touchscreen controller register
 *
 *  Parameters: reg - register to read
 *              val - variable where value will be stored
 *  Return:     true on success, false on error
 *----------------------------------------------------------------------------*/
static bool Touch_Read (uint8_t reg, uint8_t *val) {
  uint8_t data[1];
  int32_t n;

  data[0] = reg;
  n = ptrI2C->SendData(TSC_I2C_ADDR, data, 1, true);
  if (n != 1) return false;

  n = ptrI2C->ReceiveData(TSC_I2C_ADDR, val, 1, false);
  if (n != 1) return false;

  return true;
}


/*-----------------------------------------------------------------------------
 *      Joystick_Initialize:  Initialize Joystick
 *                            connected to the STMPE811 touchscreen controller
 *                            (GPIO expander)
 *
 *  Parameter:  (none)
 *  Return:     (none)
 *----------------------------------------------------------------------------*/
void Joystick_Initialize (void) {
  uint32_t tick;

  ptrI2C->Initialize   (NULL);
  ptrI2C->PowerControl (ARM_POWER_FULL);
  ptrI2C->BusSpeed (ARM_I2C_BUS_SPEED_STANDARD);
  ptrI2C->BusClear();

  Touch_Write(STMPE811_SYS_CTRL1, 0x02);  /* Reset Touch-screen controller      */
  tick = osKernelSysTick();               /* Wait 10ms                          */
  while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(10000));

  Touch_Write(STMPE811_SYS_CTRL2, 0x03);  /* Enable only GPIO and temp. sensor  */
  Touch_Write(STMPE811_INT_CTRL,  0x03);  /* INT is active low edge interrupt   */
  Touch_Write(STMPE811_INT_EN,    0xA0);  /* GPIO and temp. sens. int enabled   */
  Touch_Write(STMPE811_GPIO_EN,   0xFF);  /* All GPIOs are generating interrupt */
  Touch_Write(STMPE811_GPIO_DIR,  0x00);  /* GPIOs direction to input state     */
  Touch_Write(STMPE811_GPIO_FE,   0xFF);  /* Detect falling edge on all GPIOs   */
  Touch_Write(STMPE811_GPIO_AF,   0xFF);  /* Configure all pins as GPIO         */
}


/*-----------------------------------------------------------------------------
 *      Joystick_Uninitialize:  De-Initialize Joystick
 *
 *  Parameter:  (none)
 *  Return:     (none)
 *----------------------------------------------------------------------------*/
void Joystick_Uninitialize (void) {
  Touch_Write(STMPE811_SYS_CTRL1, 0x02);  /* Reset Touch-screen controller      */
}


/*-----------------------------------------------------------------------------
 *      Joystick_GetButtons:  Get Joystick buttons state
 *
 *  Parameter:  (none)
 *  Return:     Joystick button state
 *----------------------------------------------------------------------------*/
uint32_t Joystick_GetButtons (void) {
  uint32_t buttons = 0;
  uint8_t  val;

  /* Read Josytick inputs */
  if (Touch_Read(STMPE811_GPIO_MP_STA, &val) == true) {
    if ((val & (1 << 3)) == 0) {
      buttons |= JOY_UP;
    }
    if ((val & (1 << 4)) == 0) {
      buttons |= JOY_RIGHT;
    }
    if ((val & (1 << 5)) == 0) {
      buttons |= JOY_LEFT;
    }
    if ((val & (1 << 6)) == 0) {
      buttons |= JOY_DOWN;
    }
    if ((val & (1 << 7)) == 0) {
      buttons |= JOY_CENTER;
    }
  }
  return buttons;
}
