/*-----------------------------------------------------------------------------
 * Name:    Camera.c
 * Purpose: Digital camera driver
 * Note(s):
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
#include "Camera.h"
#include "Driver_I2C.h"
#include "stm32f4xx.h"
#include "GPIO_STM32F4xx.h"
#include "DMA_STM32F4xx.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h> Digital Camera Configuration
//   <o0> I2C Port <1=>I2C1 <2=>I2C2 <3=>I2C3
#define CAM_I2C_PORT 1

//   <h> DMA
//     <o0> Number <2=>2
//     <i>  Selects DMA Number (only DMA2 can be used)
//     <o1> Stream <1=>1 <7=>7
//     <i>  Selects DMA Stream (only Stream 1 or 7 can be used)
//     <o2> Channel <1=>1
//     <i>  Selects DMA Channel (only Channel 1 can be used)
//     <o3> Priority <0=>Low <1=>Medium <2=>High <3=>Very High
//     <i>  Selects DMA Priority
//   </h>

#define DCMI_DMA_NUMBER                 2
#define DCMI_DMA_STREAM                 1
#define DCMI_DMA_CHANNEL                1
#define DCMI_DMA_PRIORITY               2
// </h>

/* I2C Camera address */
#define CAM_I2C_ADDR    0x21            /* 7-bit digital camera I2C address   */

/* I2C Driver */
#define _I2C_Driver_(n)  Driver_I2C##n
#define  I2C_Driver_(n) _I2C_Driver_(n)
extern ARM_DRIVER_I2C    I2C_Driver_(CAM_I2C_PORT);
#define ptrI2C         (&I2C_Driver_(CAM_I2C_PORT))

/* DMA Source and destination addresses */
#define DCMI_SRC_ADDR   0x50050028
#define LCD_DST_ADDR    0x6C000002

/* Calculate array size */
#define ARR_SZ(x) (sizeof (x) / sizeof(x[0]))

/* Register value */
typedef struct {
  uint8_t Addr;
  uint8_t Val;
} REG_VAL;


/* OmniVision recommended settings based on OVM7690 Setting V2.2              */
/* Modified for RGB QVGA settings                                             */
REG_VAL CAM_RegInit[] = {
  {0x0E, 0x00},                         /* Normal operation mode              */
  {0x0C, 0x46},
  {0x81, 0xFF},
  {0x21, 0x44},
  {0x16, 0x03},
  {0x39, 0x80},
  {0x1E, 0xB1},
  
  /* Format */
  {0x12, 0x06},                         /* Output format control: RGB         */
  {0x82, 0x03},
  {0xD0, 0x48},
  {0x80, 0x7F},
  {0x3E, 0x30},
  {0x22, 0x00},
  
  /* Resolution */
  {0x17, 0x69},                         /* Horizontal window start point      */
  {0x18, 0xA4},                         /* Horizontal senzor size             */
  {0x19, 0x0C},                         /* Vertical Window start line         */
  {0x1A, 0xF6},                         /* Vertical sensor size               */
  
  {0xC8, 0x02},
  {0xC9, 0x80},
  {0xCA, 0x01},
  {0xCB, 0xE0},  
  {0xCC, 0x02},
  {0xCD, 0x80},
  {0xCE, 0x01},
  {0xCF, 0xE0},
  
  /* Lens Correction */
  {0x85, 0x90},
  {0x86, 0x00},
  {0x87, 0x00},
  {0x88, 0x10},
  {0x89, 0x30},
  {0x8A, 0x29},
  {0x8B, 0x26},
  
  /* Color Matrix */
  {0xBB, 0x80},
  {0xBC, 0x62},
  {0xBD, 0x1E},
  {0xBE, 0x26},
  {0xBF, 0x7B},
  {0xC0, 0xAC},
  {0xC1, 0x1E},
  
  /* Edge + Denoise */
  {0xB7, 0x05},
  {0xB8, 0x09},
  {0xB9, 0x00},
  {0xBA, 0x18},
  
  /* UVAdjust */
  {0x5A, 0x4A},
  {0x5B, 0x9F},
  {0x5C, 0x48},
  {0x5D, 0x32},
  
  /* AEC/AGC target */
  {0x24, 0x78},
  {0x25, 0x68},
  {0x26, 0xB3},
  
  /* Gamma */
  {0xA3, 0x0B},
  {0xA4, 0x15},
  {0xA5, 0x2A},
  {0xA6, 0x51},
  {0xA7, 0x63},
  {0xA8, 0x74},
  {0xA9, 0x83},
  {0xAA, 0x91},
  {0xAB, 0x9E},
  {0xAC, 0xAA},
  {0xAD, 0xBE},
  {0xAE, 0xCE},
  {0xAF, 0xE5},
  {0xB0, 0xF3},
  {0xB1, 0xFB},
  {0xB2, 0x06},
  
  /* Advance */
  {0x8C, 0x5D},
  {0x8D, 0x11},
  {0x8E, 0x12},
  {0x8F, 0x11},
  {0x90, 0x50},
  {0x91, 0x22},
  {0x92, 0xD1},
  {0x93, 0xA7},
  {0x94, 0x23},
  {0x95, 0x3B},
  {0x96, 0xFF},
  {0x97, 0x00},
  {0x98, 0x4A},
  {0x99, 0x46},
  {0x9A, 0x3D},
  {0x9B, 0x3A},
  {0x9C, 0xF0},
  {0x9D, 0xF0},
  {0x9E, 0xF0},
  {0x9F, 0xFF},
  {0xA0, 0x56},
  {0xA1, 0x55},
  {0xA2, 0x13},
  
  /* General Control */
  {0x50, 0x9A},
  {0x51, 0x80},
  {0x21, 0x23},
  
  {0x14, 0x29},
  {0x13, 0xF7},
  {0x11, 0x01},
  
  {0x0E, 0x00} 
};

/* Camera module register settings for QVGA/RGB565 capture.                   */
REG_VAL CAM_RegSz[] = {
  {0xC8, 0x02},
  {0xC9, 0x80},
  {0xCA, 0x01},
  {0xCB, 0xE0},
  {0xCC, 0x01},
  {0xCd, 0x40},
  {0xCE, 0x00},
  {0xCF, 0xF0},
};


/*-----------------------------------------------------------------------------
 *      Camera_Write:  Write a value to the camera register
 *
 *  Parameters: reg - register address
 *              val - value to write
 *  Return:     true on success, false on error
 *----------------------------------------------------------------------------*/
static bool Camera_Write (uint8_t reg, uint8_t val) {
  uint8_t data[2];
  int32_t n;

  data[0] = reg;
  data[1] = val;
  n = ptrI2C->SendData(CAM_I2C_ADDR, data, 2, false);
  if (n != 2) return false;

  return true;
}


/*-----------------------------------------------------------------------------
 *      Camera_Read:  Read a value from the camera register
 *
 *  Parameters: reg - register address
 *              val - variable where value will be stored
 *  Return:     true on success, false on error
 *----------------------------------------------------------------------------*/
static bool Camera_Read (uint8_t reg, uint8_t *val) {
  uint8_t data[1];
  int32_t n;

  data[0] = reg;
  n = ptrI2C->SendData(CAM_I2C_ADDR, data, 1, true);
  if (n != 1) return false;

  n = ptrI2C->ReceiveData(CAM_I2C_ADDR, val, 1, false);
  if (n != 1) return false;

  return true;
}


/*-----------------------------------------------------------------------------
 *      Camera_SetQVGA:  Configure display size to QVGA (240*320)
 *
 *  Parameters: (none)
 *  Return:     (none)
 *----------------------------------------------------------------------------*/
static void Camera_SetQVGA (void) {
  uint32_t i;

  for (i = 0; i < ARR_SZ(CAM_RegInit); i++) {
    Camera_Write (CAM_RegInit[i].Addr, CAM_RegInit[i].Val);
  }

  for (i = 0; i < ARR_SZ(CAM_RegSz); i++) {
    Camera_Write (CAM_RegSz[i].Addr, CAM_RegSz[i].Val);
  }
}


/*-----------------------------------------------------------------------------
 *      Camera_Initialize:  Initialize digital camera interface
 *
 *  Parameters: (none)
 *  Return:     0 on success, -1 on error
 *----------------------------------------------------------------------------*/
int32_t Camera_Initialize (void) {
  uint8_t  val;
  uint32_t s_ctrl;

  ptrI2C->Initialize   (NULL);
  ptrI2C->PowerControl (ARM_POWER_FULL);
  ptrI2C->BusSpeed (ARM_I2C_BUS_SPEED_STANDARD);
  ptrI2C->BusClear();

  /* Configure camera size */
  Camera_SetQVGA ();
  
  Camera_Read (0x6F, &val);
  val &= ~(1 << 7);
  Camera_Write (0x6F, val);

  GPIO_PortClock (GPIOA, true);
  GPIO_PortClock (GPIOH, true);
  GPIO_PortClock (GPIOI, true);

  /* Configure PA6:                                                           */
  /* Alternate function, push-pull output, 100 MHz, with pull-up, AF is DCMI  */
  GPIO_PinConfigure (GPIOA,  6, GPIO_MODE_AF,             GPIO_OUTPUT_PUSH_PULL,
                                GPIO_OUTPUT_SPEED_100MHz, GPIO_PULL_UP);
  GPIO_PinAF        (GPIOA,  6, GPIO_AF_DCMI);
  
  /* Configure PH8, PH9, PH10, PH11, PH12, PH14                               */
  /* Alternate function, push-pull outputs, 100MHz, with pull-up, AF is DCMI  */
  GPIO_PinConfigure (GPIOH,  8, GPIO_MODE_AF,             GPIO_OUTPUT_PUSH_PULL,
                                GPIO_OUTPUT_SPEED_100MHz, GPIO_PULL_UP);
  GPIO_PinAF        (GPIOH,  8, GPIO_AF_DCMI);

  GPIO_PinConfigure (GPIOH,  9, GPIO_MODE_AF,             GPIO_OUTPUT_PUSH_PULL,
                                GPIO_OUTPUT_SPEED_100MHz, GPIO_PULL_UP);
  GPIO_PinAF        (GPIOH,  9, GPIO_AF_DCMI);

  GPIO_PinConfigure (GPIOH, 10, GPIO_MODE_AF,             GPIO_OUTPUT_PUSH_PULL,
                                GPIO_OUTPUT_SPEED_100MHz, GPIO_PULL_UP);
  GPIO_PinAF        (GPIOH, 10, GPIO_AF_DCMI);

  GPIO_PinConfigure (GPIOH, 11, GPIO_MODE_AF,             GPIO_OUTPUT_PUSH_PULL,
                                GPIO_OUTPUT_SPEED_100MHz, GPIO_PULL_UP);
  GPIO_PinAF        (GPIOH, 11, GPIO_AF_DCMI);

  GPIO_PinConfigure (GPIOH, 12, GPIO_MODE_AF,             GPIO_OUTPUT_PUSH_PULL,
                                GPIO_OUTPUT_SPEED_100MHz, GPIO_PULL_UP);
  GPIO_PinAF        (GPIOH, 12, GPIO_AF_DCMI);

  GPIO_PinConfigure (GPIOH, 14, GPIO_MODE_AF,             GPIO_OUTPUT_PUSH_PULL,
                                GPIO_OUTPUT_SPEED_100MHz, GPIO_PULL_UP);
  GPIO_PinAF        (GPIOH, 14, GPIO_AF_DCMI);

  /* Configure PI4, PI5, PI6, PI7                                             */
  /* Alternate function, push-pull outputs, 100MHz, with pull-up, AF is DCMI  */
  GPIO_PinConfigure (GPIOI,  4, GPIO_MODE_AF,             GPIO_OUTPUT_PUSH_PULL,
                                GPIO_OUTPUT_SPEED_100MHz, GPIO_PULL_UP);
  GPIO_PinAF        (GPIOI,  4, GPIO_AF_DCMI);

  GPIO_PinConfigure (GPIOI,  5, GPIO_MODE_AF,             GPIO_OUTPUT_PUSH_PULL,
                                GPIO_OUTPUT_SPEED_100MHz, GPIO_PULL_UP);
  GPIO_PinAF        (GPIOI,  5, GPIO_AF_DCMI);

  GPIO_PinConfigure (GPIOI,  6, GPIO_MODE_AF,             GPIO_OUTPUT_PUSH_PULL,
                                GPIO_OUTPUT_SPEED_100MHz, GPIO_PULL_UP);
  GPIO_PinAF        (GPIOI,  6, GPIO_AF_DCMI);

  GPIO_PinConfigure (GPIOI,  7, GPIO_MODE_AF,             GPIO_OUTPUT_PUSH_PULL,
                                GPIO_OUTPUT_SPEED_100MHz, GPIO_PULL_UP);
  GPIO_PinAF        (GPIOI,  7, GPIO_AF_DCMI);

  /* Enable DCMI peripheral clock */
  RCC->AHB2ENR |= RCC_AHB2ENR_DCMIEN;

  /* Configure DCMI peripheral */
  DCMI->CR &= ~(DCMI_CR_ENABLE | DCMI_CR_CAPTURE);
  
  DCMI->CR  =  DCMI_CR_PCKPOL |         /* Rising polarity                    */
               DCMI_CR_VSPOL;           /* VS Polarity high                   */
                                        /* HS Polarity low                    */
                                        /* Continuous mode                    */
                                        /* Hardware sync                      */
                                        /* Capture rate all frames            */
                                        /* Extended data mode 8b              */
  /* Configure DMA stream */
  DMA_StreamInitialize (DCMI_DMA_NUMBER, DCMI_DMA_STREAM);
  
  s_ctrl = (DCMI_DMA_CHANNEL  << DMA_CHANNEL_POS)  |
           DMA_MEMORY_DATA_16BIT                   |
           DMA_PERIPHERAL_DATA_32BIT               |
           (DCMI_DMA_PRIORITY << DMA_PRIORITY_POS) |
           DMA_PERIPHERAL_TO_MEMORY                |
           DMA_CIRCULAR_MODE;

  DMA_StreamConfigure (DMAx_STREAMy(DCMI_DMA_NUMBER, DCMI_DMA_STREAM),
                       s_ctrl,
                       DMA_FIFO_DIRECT_MODE_DISABLE |
                       DMA_FIFO_THRESHOLD_100,
                       DCMI_SRC_ADDR,
                       LCD_DST_ADDR,
                       1);
  return (0);
}


/*-----------------------------------------------------------------------------
 *      Camera_Uninitialize:  Uninitialize digital camera interface
 *
 *  Parameters: (none)
 *  Return:     0 on success, -1 on error
 *----------------------------------------------------------------------------*/
int32_t Camera_Uninitialize (void) {
  /* Turn off the camera */
  Camera_Off();

  /* Disable DCMI clock */
  RCC->AHB2ENR &= ~RCC_AHB2ENR_DCMIEN;
  return (0);
}


/*-----------------------------------------------------------------------------
 *      Camera_On:  Turn camera ON
 *
 *  Parameters: (none)
 *  Return:     0 on success, -1 on error
 *----------------------------------------------------------------------------*/
int32_t Camera_On (void) {
  uint8_t val;

  if (Camera_Read (0x0E, &val)) {
    val &= ~(1 << 3);

    if (Camera_Write (0x0E, val)) {     /* Put camera into normal mode        */
      DMA_StreamEnable (DMAx_STREAMy(DCMI_DMA_NUMBER, DCMI_DMA_STREAM));
      DCMI->CR |= DCMI_CR_ENABLE;       /* Enable DCMI                        */
      DCMI->CR |= DCMI_CR_CAPTURE;      /* Start image capture                */
      return (0);
    }
  }
  return (-1);
}


/*-----------------------------------------------------------------------------
 *      Camera_Off:  Turn camera OFF
 *
 *  Parameters: (none)
 *  Return:     0 on success, -1 on error
 *----------------------------------------------------------------------------*/
int32_t Camera_Off (void) {
  uint8_t val;

  DMA_StreamDisable (DMAx_STREAMy(DCMI_DMA_NUMBER, DCMI_DMA_STREAM));
  DCMI->CR &= ~DCMI_CR_CAPTURE;         /* Disable image capture              */
  DCMI->CR &= ~DCMI_CR_ENABLE;          /* Disable DCMI                       */

  if (Camera_Read (0x0E, &val)) {
    /* Put camera into sleep mode */
    if (Camera_Write (0x0E, val | (1<<3))) {
      return (0);
    }
  }
  return (-1);
}

/*-----------------------------------------------------------------------------
 *      Camera_Test:  Enable camera test mode
 *
 *  Parameters: mode - type of test pattern
 *  Return:     0 on success, -1 on error
 *----------------------------------------------------------------------------*/
int32_t Camera_Test (uint32_t mode) {
  uint8_t val;

  if (Camera_Read (0x0C, &val)) {
    if (mode == TEST_PATTERN_OFF) {
      val &= ~1;                        /* Color bar OFF                      */
    }
    else {    
      val |= 1;                         /* Color bar enable                   */
    }
    
    if (Camera_Write (0x0C, val)) {
      return (0);
    }
  }
  return (-1);
}
