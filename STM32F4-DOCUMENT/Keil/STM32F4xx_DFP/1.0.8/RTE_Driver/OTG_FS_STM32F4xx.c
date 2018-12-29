/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        12. September 2013
 * $Revision:    V1.03
 *  
 * Project:      OTG Full/Low-Speed Common Driver for ST STM32F4xx
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
#include "OTG_FS_STM32F4xx.h"

#include "Driver_USBH.h"
#include "Driver_USBD.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#if (RTE_USB_OTG_FS_HOST)
extern void USBH_FS_IRQ (uint32_t gintsts);
#endif
#if (RTE_USB_OTG_FS_DEV)
extern void USBD_FS_IRQ (uint32_t gintsts);
#endif

static bool    pins_configured_mask = 0;
       uint8_t otg_fs_role          = ARM_USB_ROLE_NONE;


/* Common IRQ Routine *********************************************************/

/**
  \fn          void OTG_FS_IRQHandler (void)
  \brief       USB Interrupt Routine (IRQ).
*/
void OTG_FS_IRQHandler (void) {
  uint32_t gintsts;

  gintsts = OTG_FS->GINTSTS & OTG_FS->GINTMSK;

  switch (otg_fs_role) {
    case ARM_USB_ROLE_HOST:
#if (RTE_USB_OTG_FS_HOST)
      USBH_FS_IRQ (gintsts);
#endif
      break;
    case ARM_USB_ROLE_DEVICE:
#if (RTE_USB_OTG_FS_DEV)
      USBD_FS_IRQ (gintsts);
#endif
      break;
    case ARM_USB_ROLE_NONE:
      break;
  }
}


/* Auxiliary Functions ********************************************************/

/* Pin Functions ---------------------*/

/**
  \fn          bool OTG_FS_PinsConfigure (uint8_t pins_mask)
  \brief       Configure single or multiple USB Pin(s).
  \param[in]   Mask of pins to be configured (possible masking values:
               USB_PIN_DP, USB_PIN_DM, USB_PIN_VBUS, USB_PIN_OC, USB_PIN_ID)
  \return      true = success, false = fail
*/
bool OTG_FS_PinsConfigure (uint8_t pins_mask) {

#if (RTE_USB_OTG_FS)
#if (RTE_USB_OTG_FS_HOST && !RTE_OTG_FS_VBUS_PIN)
#error   Configure VBUS Power On/Off pin for USB OTG Full-speed in RTE_Device.h!
#endif
#else
#error   Enable USB OTG Full-speed in RTE_Device.h!
#endif

  if ((pins_configured_mask ^ pins_mask) & ARM_USB_PIN_DP) {
    GPIO_PortClock        (GPIOA, true);
    if (!GPIO_PinAF       (GPIOA, 12, GPIO_AF_OTG_FS)) return false;  /* PA12: OTG_FS_DP*/
    if (!GPIO_PinConfigure(GPIOA, 12, GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    pins_configured_mask |= ARM_USB_PIN_DP;
  }
  if ((pins_configured_mask ^ pins_mask) & ARM_USB_PIN_DM) {
    GPIO_PortClock        (GPIOA, true);
    if (!GPIO_PinAF       (GPIOA, 11, GPIO_AF_OTG_FS)) return false;  /* PA11: OTG_FS_DM*/
    if (!GPIO_PinConfigure(GPIOA, 11, GPIO_MODE_AF,    GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_100MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    pins_configured_mask |= ARM_USB_PIN_DM;
  }
  if ((pins_configured_mask ^ pins_mask) & ARM_USB_PIN_ID) {
    pins_configured_mask |= ARM_USB_PIN_ID;
  }
  if ((pins_configured_mask ^ pins_mask) & ARM_USB_PIN_VBUS) {
    GPIO_PortClock        (RTE_OTG_FS_VBUS_PORT, true);
    if (!GPIO_PinAF       (RTE_OTG_FS_VBUS_PORT, RTE_OTG_FS_VBUS_BIT, GPIO_AF_SYSTEM)) return false;  /* VBUS Pin */
    if (!GPIO_PinConfigure(RTE_OTG_FS_VBUS_PORT, RTE_OTG_FS_VBUS_BIT, GPIO_MODE_OUTPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    pins_configured_mask |= ARM_USB_PIN_VBUS;
  }
  if ((pins_configured_mask ^ pins_mask) & ARM_USB_PIN_OC) {
    GPIO_PortClock        (RTE_OTG_FS_OC_PORT, true);
    if (!GPIO_PinAF       (RTE_OTG_FS_OC_PORT,   RTE_OTG_FS_OC_BIT,   GPIO_AF_SYSTEM)) return false;  /* OC Pin */
    if (!GPIO_PinConfigure(RTE_OTG_FS_OC_PORT,   RTE_OTG_FS_OC_BIT,   GPIO_MODE_INPUT,  GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    pins_configured_mask |= ARM_USB_PIN_OC;
  }

  return true;
}

/**
  \fn          bool OTG_FS_PinsUnconfigure (uint8_t pins_mask)
  \brief       Unconfigure to reset settings single or multiple USB Pin(s).
  \param[in]   Mask of pins to be unconfigured (possible masking values:
               USB_PIN_DP, USB_PIN_DM, USB_PIN_VBUS, USB_PIN_OC, USB_PIN_ID)
  \return      true = success, false = fail
*/
bool OTG_FS_PinsUnconfigure (uint8_t pins_mask) {

  if ((pins_configured_mask & pins_mask) & ARM_USB_PIN_DP) {
    if (!GPIO_PinAF       (GPIOA, 12, GPIO_AF_SYSTEM)) return false;  /* PA12: OTG_FS_DP*/
    if (!GPIO_PinConfigure(GPIOA, 12, GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz,   GPIO_NO_PULL_UP_DOWN)) return false;
    pins_configured_mask &= ~ARM_USB_PIN_DP;
  }
  if ((pins_configured_mask & pins_mask) & ARM_USB_PIN_DM) {
    if (!GPIO_PinAF       (GPIOA, 11, GPIO_AF_SYSTEM)) return false;  /* PA11: OTG_FS_DM*/
    if (!GPIO_PinConfigure(GPIOA, 11, GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz,   GPIO_NO_PULL_UP_DOWN)) return false;
    pins_configured_mask &= ~ARM_USB_PIN_DM;
  }
  if ((pins_configured_mask & pins_mask) & ARM_USB_PIN_ID) {
    if (!GPIO_PinAF       (GPIOA, 10, GPIO_AF_SYSTEM)) return false;  /* PA10: OTG_FS_ID*/
    if (!GPIO_PinConfigure(GPIOA, 10, GPIO_MODE_INPUT, GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz,   GPIO_NO_PULL_UP_DOWN)) return false;
    pins_configured_mask &= ~ARM_USB_PIN_ID;
  }
  if ((pins_configured_mask & pins_mask) & ARM_USB_PIN_VBUS) {
    if (!GPIO_PinAF       (RTE_OTG_FS_VBUS_PORT, RTE_OTG_FS_VBUS_BIT, GPIO_AF_SYSTEM)) return false;  /* VBUS Pin */
    if (!GPIO_PinConfigure(RTE_OTG_FS_VBUS_PORT, RTE_OTG_FS_VBUS_BIT, GPIO_MODE_INPUT,  GPIO_OUTPUT_PUSH_PULL, GPIO_OUTPUT_SPEED_2MHz, GPIO_NO_PULL_UP_DOWN)) return false;
    pins_configured_mask &= ~ARM_USB_PIN_VBUS;
  }
  if ((pins_configured_mask & pins_mask) & ARM_USB_PIN_OC) {
    /* It's function is same as by reset */
    pins_configured_mask &= ~ARM_USB_PIN_OC;
  }

  return true;
}

/**
  \fn          bool OTG_FS_PinVbusOnOff (bool state)
  \brief       Drive VBUS Pin On/Off.
  \param[in]   state    State On/Off (true = On, false = Off)
  \return      true = success, false = fail
*/

bool OTG_FS_PinVbusOnOff (bool state) {

#if (RTE_OTG_FS_VBUS_PIN)
  GPIO_PinWrite (RTE_OTG_FS_VBUS_PORT, RTE_OTG_FS_VBUS_BIT, state == RTE_OTG_FS_VBUS_ACTIVE);

  return true;
#else
  return false;
#endif
}
