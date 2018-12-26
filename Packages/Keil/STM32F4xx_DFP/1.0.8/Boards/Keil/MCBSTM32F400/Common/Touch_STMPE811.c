/*-----------------------------------------------------------------------------
 * Name:    Touch_STMPE811.c
 * Purpose: Touchscreen controller functions for STMPE811
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
#include "Touch.h"
#include "Driver_I2C.h"

#ifndef TSC_I2C_PORT
#define TSC_I2C_PORT    1               /* I2C Port number                    */
#endif

#define TSC_I2C_ADDR    0x41            /* 7-bit I2C address                  */

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
 *      Touch_Initialize:  Initialize touchscreen controller
 *
 *  Parameter:  (none)
 *  Return:     true on success, false on error
 *----------------------------------------------------------------------------*/
bool Touch_Initialize (void) {
  uint32_t tick;
  
  ptrI2C->Initialize   (NULL);
  ptrI2C->PowerControl (ARM_POWER_FULL);
  ptrI2C->BusSpeed (ARM_I2C_BUS_SPEED_STANDARD);
  ptrI2C->BusClear();

  Touch_Write(STMPE811_SYS_CTRL1,      0x02); /* Reset Touch-screen controller */
  tick = osKernelSysTick();                   /* Wait 10ms                     */
  while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(10000));

  Touch_Write(STMPE811_SYS_CTRL2,      0x0C); /* Enable TSC and ADC            */
  Touch_Write(STMPE811_INT_EN,         0x07); /* Enable Touch detect, FIFO     */
  Touch_Write(STMPE811_ADC_CTRL1,      0x68); /* Set sample time , 12-bit mode */
  tick = osKernelSysTick();                   /* Wait 500us                    */
  while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(500));

  Touch_Write(STMPE811_ADC_CTRL2,      0x01); /* ADC frequency 3.25 MHz        */
  Touch_Write(STMPE811_TSC_CFG,        0xC2); /* Detect delay 10us,
                                                 Settle time 500us             */
  Touch_Write(STMPE811_FIFO_TH,        0x01); /* Threshold for FIFO            */
  Touch_Write(STMPE811_FIFO_STA,       0x01); /* FIFO reset                    */
  Touch_Write(STMPE811_FIFO_STA,       0x00); /* FIFO not reset                */
  Touch_Write(STMPE811_TSC_FRACTION_Z, 0x07); /* Fraction z                    */
  Touch_Write(STMPE811_TSC_I_DRIVE,    0x01); /* Drive 50 mA typical           */
  Touch_Write(STMPE811_GPIO_AF,        0x00); /* Pins are used for touchscreen */
  Touch_Write(STMPE811_TSC_CTRL,       0x01); /* Enable TSC                    */
  Touch_Write(STMPE811_INT_STA,        0xFF); /* Clear interrupt status        */

  return true;
}

/*-----------------------------------------------------------------------------
 *      Touch_Uninitialize:  De-initialize touchscreen controller
 *
 *  Parameter:  (none)
 *  Return:     true on success, false on error
 *----------------------------------------------------------------------------*/
bool Touch_Uninitialize (void) {

  Touch_Write(STMPE811_SYS_CTRL1, 0x02);  /* Reset Touch-screen controller    */

  return true;
}

/*-----------------------------------------------------------------------------
 *      Touch_GetState: Read touchscreen state (coordinates and pressed)
 *                      and write into TSC_STATE structure.
 *
 *  Parameters: pState - pointer to TSC_STATE structure
 *  Return:     true on success, false on error
 *----------------------------------------------------------------------------*/
bool Touch_GetState (TSC_STATE *pState) {
  uint8_t val;
  uint8_t num;
  uint8_t xyz[4];
  int32_t n;
  bool    res;

  /* Read touch status */
  res = Touch_Read(STMPE811_TSC_CTRL, &val);
  if (!res) return false;
  pState->pressed = (val & (1 << 7)) ? 1 : 0;
  
  if (pState->pressed) {
    val = STMPE811_TSC_DATA;

    /* If FIFO overflow, discard all samples except the last one */
    res = Touch_Read(STMPE811_FIFO_SIZE, &num);
    if (!res || num == 0) return false;

    while (num--) {
      n = ptrI2C->SendData   (TSC_I2C_ADDR, &val, 1, true);
      if (n != 1) return false;
      n = ptrI2C->ReceiveData(TSC_I2C_ADDR,  xyz, 4, false);
      if (n != 4) return false;
    }
    pState->x =  (xyz[0] << 4) | ((xyz[1] & 0xF0) >> 4);
    pState->y =   xyz[2]       | ((xyz[1] & 0x0F) << 8);
  }

  /* Clear interrupt flags */
  Touch_Write(STMPE811_INT_STA, 0x1F);
  return true;
}
