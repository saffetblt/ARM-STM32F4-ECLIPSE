/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        12. September 2013
 * $Revision:    V1.03
 *  
 * Project:      OTG High-Speed Common Driver for ST STM32F4xx
 * Configured:   via RTE_Device.h configuration file 
 * -------------------------------------------------------------------- */

/* History:
 *  Version 1.03
 *    Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.02
 *    Removed include of rl_usb.h header
 *  Version 1.00
 *    Initial release
 */ 

#include <stdint.h>
#include <string.h>
#include "cmsis_os.h"
#include "stm32f4xx.h"

#include "GPIO_STM32F4xx.h"
#include "OTG_HS_STM32F4xx.h"

#include "Driver_USBH.h"
#include "Driver_USBD.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#if (RTE_USB_OTG_HS_HOST)
extern void USBH_HS_IRQ (uint32_t gintsts);
#endif
#if (RTE_USB_OTG_HS_DEV)
extern void USBD_HS_IRQ (uint32_t gintsts);
#endif

static bool    pins_configured_mask = 0;
       uint8_t otg_hs_role          = ARM_USB_ROLE_NONE;


/* Common IRQ Routine *********************************************************/

/**
  \fn          void OTG_HS_IRQHandler (void)
  \brief       USB Interrupt Routine (IRQ).
*/
void OTG_HS_IRQHandler (void) {
  uint32_t gintsts;

  gintsts = OTG_HS->GINTSTS & OTG_HS->GINTMSK;

  switch (otg_hs_role) {
    case ARM_USB_ROLE_HOST:
#if (RTE_USB_OTG_HS_HOST)
      USBH_HS_IRQ (gintsts);
#endif
      break;
    case ARM_USB_ROLE_DEVICE:
#if (RTE_USB_OTG_HS_DEV)
      USBD_HS_IRQ (gintsts);
#endif
      break;
    case ARM_USB_ROLE_NONE:
      break;
  }
}


/* Auxiliary Functions ********************************************************/

/* Pin Functions ---------------------*/

/**
  \fn          bool OTG_HS_PinsConfigure (uint8_t pins_mask)
  \brief       Configure single or multiple USB Pin(s).
  \param[in]   Mask of pins to be configured (possible masking values:
               USB_PIN_DP, USB_PIN_DM, USB_PIN_VBUS, USB_PIN_OC, USB_PIN_ID)
  \return      true = success, false = fail
*/
bool OTG_HS_PinsConfigure (uint8_t pins_mask) {

#if (RTE_USB_OTG_HS)
#if (RTE_USB_OTG_HS_HOST && !RTE_OTG_HS_VBUS_PIN)
#error   Configure VBUS Power On/Off pin for USB OTG High-speed in RTE_Device.h!
#endif
#else
#error   Enable USB OTG High-speed in RTE_Device.h!
#endif

  if ((pins_configured_mask ^ pins_mask) & (ARM_USB_PIN_DP | ARM_USB_PIN_DM)) {
#if (RTE_USB_OTG_HS_PHY)
    /* External ULPI PHY */
    GPIO_PortClock        (GPIOA, true);
    GPIO_PortClock        (GPIOB, true);
    GPIO_PortClock        (GPIOC, true);
    GPIO_PortClock        (GPIOI, true);
    GPIO_PortClock        (GPIOH, true);
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_CK_PORT,  RTE_USB_OTG_HS_ULPI_CK_PIN,  GPIO_AF_OTG_HS)) return false; /* OTG_HS_ULPI_CK  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_CK_PORT,  RTE_USB_OTG_HS_ULPI_CK_PIN,  GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_DIR_PORT, RTE_USB_OTG_HS_ULPI_DIR_PIN, GPIO_AF_OTG_HS)) return false; /* OTG_HS_ULPI_DIR */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_DIR_PORT, RTE_USB_OTG_HS_ULPI_DIR_PIN, GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_STP_PORT, RTE_USB_OTG_HS_ULPI_STP_PIN, GPIO_AF_OTG_HS)) return false; /* OTG_HS_ULPI_STP */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_STP_PORT, RTE_USB_OTG_HS_ULPI_STP_PIN, GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_NXT_PORT, RTE_USB_OTG_HS_ULPI_NXT_PIN, GPIO_AF_OTG_HS)) return false; /* OTG_HS_ULPI_NXT */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_NXT_PORT, RTE_USB_OTG_HS_ULPI_NXT_PIN, GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D0_PORT,  RTE_USB_OTG_HS_ULPI_D0_PIN,  GPIO_AF_OTG_HS)) return false; /* OTG_HS_ULPI_D0  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D0_PORT,  RTE_USB_OTG_HS_ULPI_D0_PIN,  GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D1_PORT,  RTE_USB_OTG_HS_ULPI_D1_PIN,  GPIO_AF_OTG_HS)) return false; /* OTG_HS_ULPI_D1  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D1_PORT,  RTE_USB_OTG_HS_ULPI_D1_PIN,  GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D2_PORT,  RTE_USB_OTG_HS_ULPI_D2_PIN,  GPIO_AF_OTG_HS)) return false; /* OTG_HS_ULPI_D2  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D2_PORT,  RTE_USB_OTG_HS_ULPI_D2_PIN,  GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D3_PORT,  RTE_USB_OTG_HS_ULPI_D3_PIN,  GPIO_AF_OTG_HS)) return false; /* OTG_HS_ULPI_D3  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D3_PORT,  RTE_USB_OTG_HS_ULPI_D3_PIN,  GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D4_PORT,  RTE_USB_OTG_HS_ULPI_D4_PIN,  GPIO_AF_OTG_HS)) return false; /* OTG_HS_ULPI_D4  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D4_PORT,  RTE_USB_OTG_HS_ULPI_D4_PIN,  GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D5_PORT,  RTE_USB_OTG_HS_ULPI_D5_PIN,  GPIO_AF_OTG_HS)) return false; /* OTG_HS_ULPI_D5  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D5_PORT,  RTE_USB_OTG_HS_ULPI_D5_PIN,  GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D6_PORT,  RTE_USB_OTG_HS_ULPI_D6_PIN,  GPIO_AF_OTG_HS)) return false; /* OTG_HS_ULPI_D6  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D6_PORT,  RTE_USB_OTG_HS_ULPI_D6_PIN,  GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D7_PORT,  RTE_USB_OTG_HS_ULPI_D7_PIN,  GPIO_AF_OTG_HS)) return false; /* OTG_HS_ULPI_D7  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D7_PORT,  RTE_USB_OTG_HS_ULPI_D7_PIN,  GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    pins_configured_mask |= ARM_USB_PIN_DP | ARM_USB_PIN_DM;
#else
    /* Embedded PHY */
    GPIO_PortClock        (GPIOB, true);
    if (!GPIO_PinAF       (GPIOB, 15, GPIO_AF_OTG_HS)) return false; /* PB15: OTG_HS_DP */
    if (!GPIO_PinConfigure(GPIOB, 15, GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (GPIOB, 14, GPIO_AF_OTG_HS)) return false; /* PB14: OTG_HS_DM  */
    if (!GPIO_PinConfigure(GPIOB, 14, GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
#endif
  }
  if ((pins_configured_mask ^ pins_mask) & ARM_USB_PIN_ID) {
    pins_configured_mask |= ARM_USB_PIN_ID;
  }
  if ((pins_configured_mask ^ pins_mask) & ARM_USB_PIN_VBUS) {
    GPIO_PortClock        (RTE_OTG_HS_VBUS_PORT, true);
    if (!GPIO_PinAF       (RTE_OTG_HS_VBUS_PORT, RTE_OTG_HS_VBUS_BIT, GPIO_AF_SYSTEM)) return false;  /* VBUS Pin */
    if (!GPIO_PinConfigure(RTE_OTG_HS_VBUS_PORT, RTE_OTG_HS_VBUS_BIT, GPIO_MODE_OUTPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    pins_configured_mask |= ARM_USB_PIN_VBUS;
  }
  if ((pins_configured_mask ^ pins_mask) & ARM_USB_PIN_OC) {
#if (RTE_USB_OTG_HS_PHY)
    pins_configured_mask |= ARM_USB_PIN_OC;
#endif
  }

  return true;
}

/**
  \fn          bool OTG_HS_PinsUnconfigure (uint8_t pins_mask)
  \brief       Unconfigure to reset settings single or multiple USB Pin(s).
  \param[in]   Mask of pins to be unconfigured (possible masking values:
               USB_PIN_DP, USB_PIN_DM, USB_PIN_VBUS, USB_PIN_OC, USB_PIN_ID)
  \return      true = success, false = fail
*/
bool OTG_HS_PinsUnconfigure (uint8_t pins_mask) {

  if ((pins_configured_mask & pins_mask) & (ARM_USB_PIN_DP | ARM_USB_PIN_DM)) {
#if (RTE_USB_OTG_HS_PHY)
    /* External ULPI PHY */
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_CK_PORT,  RTE_USB_OTG_HS_ULPI_CK_PIN,  GPIO_AF_SYSTEM)) return false; /* OTG_HS_ULPI_CK  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_CK_PORT,  RTE_USB_OTG_HS_ULPI_CK_PIN,  GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_DIR_PORT, RTE_USB_OTG_HS_ULPI_DIR_PIN, GPIO_AF_SYSTEM)) return false; /* OTG_HS_ULPI_DIR */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_DIR_PORT, RTE_USB_OTG_HS_ULPI_DIR_PIN, GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_STP_PORT, RTE_USB_OTG_HS_ULPI_STP_PIN, GPIO_AF_SYSTEM)) return false; /* OTG_HS_ULPI_STP */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_STP_PORT, RTE_USB_OTG_HS_ULPI_STP_PIN, GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_NXT_PORT, RTE_USB_OTG_HS_ULPI_NXT_PIN, GPIO_AF_SYSTEM)) return false; /* OTG_HS_ULPI_NXT */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_NXT_PORT, RTE_USB_OTG_HS_ULPI_NXT_PIN, GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D0_PORT,  RTE_USB_OTG_HS_ULPI_D0_PIN,  GPIO_AF_SYSTEM)) return false; /* OTG_HS_ULPI_D0  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D0_PORT,  RTE_USB_OTG_HS_ULPI_D0_PIN,  GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D1_PORT,  RTE_USB_OTG_HS_ULPI_D1_PIN,  GPIO_AF_SYSTEM)) return false; /* OTG_HS_ULPI_D1  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D1_PORT,  RTE_USB_OTG_HS_ULPI_D1_PIN,  GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D2_PORT,  RTE_USB_OTG_HS_ULPI_D2_PIN,  GPIO_AF_SYSTEM)) return false; /* OTG_HS_ULPI_D2  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D2_PORT,  RTE_USB_OTG_HS_ULPI_D2_PIN,  GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D3_PORT,  RTE_USB_OTG_HS_ULPI_D3_PIN,  GPIO_AF_SYSTEM)) return false; /* OTG_HS_ULPI_D3  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D3_PORT,  RTE_USB_OTG_HS_ULPI_D3_PIN,  GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D4_PORT,  RTE_USB_OTG_HS_ULPI_D4_PIN,  GPIO_AF_SYSTEM)) return false; /* OTG_HS_ULPI_D4  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D4_PORT,  RTE_USB_OTG_HS_ULPI_D4_PIN,  GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D5_PORT,  RTE_USB_OTG_HS_ULPI_D5_PIN,  GPIO_AF_SYSTEM)) return false; /* OTG_HS_ULPI_D5  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D5_PORT,  RTE_USB_OTG_HS_ULPI_D5_PIN,  GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D6_PORT,  RTE_USB_OTG_HS_ULPI_D6_PIN,  GPIO_AF_SYSTEM)) return false; /* OTG_HS_ULPI_D6  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D6_PORT,  RTE_USB_OTG_HS_ULPI_D6_PIN,  GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (RTE_USB_OTG_HS_ULPI_D7_PORT,  RTE_USB_OTG_HS_ULPI_D7_PIN,  GPIO_AF_SYSTEM)) return false; /* OTG_HS_ULPI_D7  */
    if (!GPIO_PinConfigure(RTE_USB_OTG_HS_ULPI_D7_PORT,  RTE_USB_OTG_HS_ULPI_D7_PIN,  GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
#else
    /* Embedded PHY */
    if (!GPIO_PinAF       (GPIOB, 15, GPIO_AF_SYSTEM)) return false; /* PB15: OTG_HS_DP */
    if (!GPIO_PinConfigure(GPIOB, 15, GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    if (!GPIO_PinAF       (GPIOB, 14, GPIO_AF_SYSTEM)) return false; /* PB14: OTG_HS_DM  */
    if (!GPIO_PinConfigure(GPIOB, 14, GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
#endif
    pins_configured_mask &= ~(ARM_USB_PIN_DP | ARM_USB_PIN_DM);
  }
  if ((pins_configured_mask & pins_mask) & ARM_USB_PIN_ID) {
    pins_configured_mask &= ~ARM_USB_PIN_ID;
  }
  if ((pins_configured_mask & pins_mask) & ARM_USB_PIN_VBUS) {
    if (!GPIO_PinAF       (RTE_OTG_FS_VBUS_PORT, RTE_OTG_FS_VBUS_BIT, GPIO_AF_SYSTEM)) return false;  /* VBUS Pin */
    if (!GPIO_PinConfigure(RTE_OTG_FS_VBUS_PORT, RTE_OTG_FS_VBUS_BIT, GPIO_MODE_INPUT,  GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    pins_configured_mask &= ~ARM_USB_PIN_VBUS;
  }
  if ((pins_configured_mask & pins_mask) & ARM_USB_PIN_OC) {
#if (RTE_USB_OTG_HS_PHY)
    pins_configured_mask &= ~ARM_USB_PIN_OC;
#endif
  }

  return true;
}

/**
  \fn          bool OTG_HS_PinVbusOnOff (bool state)
  \brief       Drive VBUS Pin On/Off.
  \param[in]   state    State On/Off (true = On, false = Off)
  \return      true = success, false = fail
*/

bool OTG_HS_PinVbusOnOff (bool state) {

#if (RTE_OTG_HS_VBUS_PIN)
  GPIO_PinWrite (RTE_OTG_HS_VBUS_PORT, RTE_OTG_HS_VBUS_BIT, state == RTE_OTG_FS_VBUS_ACTIVE);

  return true;
#else
  return false;
#endif
}
