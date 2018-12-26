/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        30. October 2013
 * $Revision:    V1.04
 *  
 * Driver:       Driver_USBD1
 * Configured:   via RTE_Device.h configuration file 
 * Project:      USB High-Speed Device Driver for ST STM32F4xx
 * ---------------------------------------------------------------------- 
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 * 
 *   Configuration Setting                Value
 *   ---------------------                -----
 *   Connect to hardware via Driver_USBD# = 1
 * -------------------------------------------------------------------- */

/* History:
 *  Version 1.04
 *    Multiple packet read
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

#include "Driver_USBD.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

extern uint8_t otg_hs_role;

extern bool OTG_HS_PinsConfigure   (uint8_t pins_mask);
extern bool OTG_HS_PinsUnconfigure (uint8_t pins_mask);
extern bool OTG_HS_PinVbusOnOff    (bool state);

#define OTG                         OTG_HS


/* USBD Driver ****************************************************************/

#define ARM_USBD_DRV_VERSION ARM_DRV_VERSION_MAJOR_MINOR(1,04) /* USBD driver version */

/* Driver Version */
static const ARM_DRV_VERSION usbd_driver_version = { ARM_USBD_API_VERSION, ARM_USBD_DRV_VERSION };

/* Driver Capabilities */
static const ARM_USBD_CAPABILITIES usbd_driver_capabilities = {
  false,  /* event_power_on      */
  false,  /* event_power_off     */
  true,   /* event_connect       */
  false,  /* event_disconnect    */
  true,   /* event_reset         */
  true,   /* event_high_speed    */
  true,   /* event_suspend       */
  true,   /* event_resume        */
  false,  /* event_remote_wakeup */
  false   /* reserved            */
};

// Number of endpoints
#if   (RTE_USB_OTG_HS_DEV_EP & 0x20)
#define USBD_EP_NUM         5
#elif (RTE_USB_OTG_HS_DEV_EP & 0x10)
#define USBD_EP_NUM         4
#elif (RTE_USB_OTG_HS_DEV_EP & 0x08)
#define USBD_EP_NUM         3
#elif (RTE_USB_OTG_HS_DEV_EP & 0x04)
#define USBD_EP_NUM         2
#elif (RTE_USB_OTG_HS_DEV_EP & 0x02)
#define USBD_EP_NUM         1
#else
#define USBD_EP_NUM         0
#endif

// Interrupt IN Endpoint intermediate buffers
#define OTG_INT_IN_EP           ((RTE_USB_OTG_HS_DEV_EP_INT >> 16) & RTE_USB_OTG_HS_DEV_EP)
#if (OTG_INT_IN_EP != 0)

#define OTG_INT_IN_EP1          ((OTG_INT_IN_EP >> 1) & 1)
#define OTG_INT_IN_EP2          ((OTG_INT_IN_EP >> 2) & 1)
#define OTG_INT_IN_EP3          ((OTG_INT_IN_EP >> 3) & 1)
#define OTG_INT_IN_EP4          ((OTG_INT_IN_EP >> 4) & 1)
#define OTG_INT_IN_EP5          ((OTG_INT_IN_EP >> 5) & 1)

#define OTG_INT_IN_EP_BUF_SZ     (OTG_INT_IN_EP1 * 512 +         \
                                  OTG_INT_IN_EP2 * 512 +         \
                                  OTG_INT_IN_EP3 * 512 +         \
                                  OTG_INT_IN_EP4 * 512 +         \
                                  OTG_INT_IN_EP5 * 512)
                             
static       uint8_t   OTG_IntInEpTmpBuf[OTG_INT_IN_EP_BUF_SZ];
static const uint8_t  *ptr_OTG_IntInEpTmpBuf[] = {
  0,
  OTG_INT_IN_EP1 ?  OTG_IntInEpTmpBuf                                                   : 0,
  OTG_INT_IN_EP2 ? (OTG_IntInEpTmpBuf +  (OTG_INT_IN_EP1 * 512)                 ) : 0,
  OTG_INT_IN_EP3 ? (OTG_IntInEpTmpBuf + ((OTG_INT_IN_EP1 + OTG_INT_IN_EP2)* 512)) : 0,
  OTG_INT_IN_EP4 ? (OTG_IntInEpTmpBuf + ((OTG_INT_IN_EP1 + OTG_INT_IN_EP2 +               \
                                          OTG_INT_IN_EP3                 )* 512)) : 0,
  OTG_INT_IN_EP5 ? (OTG_IntInEpTmpBuf + ((OTG_INT_IN_EP1 + OTG_INT_IN_EP2 +               \
                                          OTG_INT_IN_EP3 + OTG_INT_IN_EP4)* 512)) : 0
};
#endif

#define OTG_RX_FIFO_SIZE        1024
#define OTG_TX0_FIFO_SIZE       64
#define OTG_TX1_FIFO_SIZE       512
#define OTG_TX2_FIFO_SIZE       512
#define OTG_TX3_FIFO_SIZE       512
#define OTG_TX4_FIFO_SIZE       512
#define OTG_TX5_FIFO_SIZE       512

#define OTG_TX_FIFO(n)          *((__packed volatile uint32_t*)(OTG_HS_BASE + 0x1000 + n*0x1000))
#define OTG_RX_FIFO             *((__packed volatile uint32_t*)(OTG_HS_BASE + 0x1000))

#define OTG_DIEPTSIZ(EPNum)     *(&OTG->DIEPTSIZ0 + EPNum * 8)
#define OTG_DIEPCTL(EPNum)      *(&OTG->DIEPCTL0  + EPNum * 8)
#define OTG_DTXFSTS(EPNum)      *(&OTG->DTXFSTS0  + EPNum * 8)
#define OTG_DOEPTSIZ(EPNum)     *(&OTG->DOEPTSIZ0 + EPNum * 8)
#define OTG_DOEPCTL(EPNum)      *(&OTG->DOEPCTL0  + EPNum * 8)
#define OTG_DIEPINT(EPNum)      *(&OTG->DIEPINT0  + EPNum * 8)
#define OTG_DOEPINT(EPNum)      *(&OTG->DOEPINT0  + EPNum * 8)

#define OTG_EP_IN_TYPE(num)      ((OTG_DIEPCTL(num) >> 18) & 3)
#define OTG_EP_OUT_TYPE(num)     ((OTG_DOEPCTL(num) >> 18) & 3)

/* Endpoint structure */
typedef struct {
  uint8_t  *buffer;
  uint32_t  bufferIndex;
  uint32_t  dataSize;
  uint32_t  maxPacketSize;
  bool      startRequired;
  uint8_t   packetCount;
} ENDPOINT;

/* Static Variables */
static ARM_USBD_SignalDeviceEvent_t   cbDeviceEvent;
static ARM_USBD_SignalEndpointEvent_t cbEndpointEvent;

static ARM_USBD_STATE UsbdState = {0, 0, 0, 0};

volatile static ENDPOINT OutEndpoint[6];
volatile static ENDPOINT InEndpoint[6];
volatile static uint32_t InEndpointNAK;


/* LOCAL FUNCTIONS */

/**
  \fn          bool USBD_FlushInEpFifo (uint8_t ep_addr)
  \brief       Flush IN endpoint fifo
  \param[in]   ep_addr specifies Endpoint Address
                ep_addr.0..3: Address
                ep_addr.7:    Direction
*/
static void USBD_FlushInEpFifo (uint8_t ep_addr) {
  ep_addr &= ARM_USB_ENDPOINT_NUMBER_MASK;

  // Flush transmit FIFO 
  OTG->GRSTCTL = (OTG->GRSTCTL & ~OTG_HS_GRSTCTL_TXFNUM_MSK) |
                  OTG_HS_GRSTCTL_TXFNUM(ep_addr)             |
                  OTG_HS_GRSTCTL_TXFFLSH;
}

/**
  \fn          void USBD_Reset (uint8_t ep_addr)
  \brief       Called after usbd reset interrupt to reset configuration
*/
static void USBD_Reset (void) {
  uint32_t i;

  // Clear endpoint mask registers
  OTG->DOEPMSK = 0;
  OTG->DIEPMSK = 0;

  for (i = 1; i <= USBD_EP_NUM; i++) {
    if (OTG_DOEPCTL(i) & OTG_HS_DOEPCTLx_EPENA) {
      OTG_DOEPCTL(i)   = OTG_HS_DOEPCTLx_EPDIS | // Endpoint disable
                         OTG_HS_DOEPCTLx_SNAK;   // Endpoint set NAK
    }
    if (OTG_DIEPCTL(i) & OTG_HS_DIEPCTLx_EPENA) {
      OTG_DIEPCTL(i)   = OTG_HS_DIEPCTLx_EPDIS | // Endpoint disable
                         OTG_HS_DIEPCTLx_SNAK;   // Endpoint set NAK
    }
    USBD_FlushInEpFifo(i);

    // Reset endpoint resources
    OutEndpoint[i].buffer         = NULL;
    OutEndpoint[i].dataSize       = 0;
    OutEndpoint[i].maxPacketSize  = 0;
    OutEndpoint[i].startRequired  = 0;
    InEndpoint[i].buffer          = NULL;
    InEndpoint[i].dataSize        = 0;
    InEndpoint[i].maxPacketSize   = 0;
    InEndpoint[i].startRequired   = 0;
    
    // Clear IN endpoint interrupts
    OTG_DIEPINT(i) = OTG_HS_DIEPINTx_XFCR    |
                     OTG_HS_DIEPINTx_EPDISD  |
                     OTG_HS_DIEPINTx_TOC     |
                     OTG_HS_DIEPINTx_ITTXFE  |
                     OTG_HS_DIEPINTx_INEPNE  |
                     OTG_HS_DIEPINTx_TXFE;

    // Clear OUT endpoint interrupts
    OTG_DOEPINT(i) = OTG_HS_DOEPINTx_XFCR    |
                     OTG_HS_DOEPINTx_EPDISD  |
                     OTG_HS_DOEPINTx_STUP    |
                     OTG_HS_DOEPINTx_OTEPDIS |
                     OTG_HS_DOEPINTx_B2BSTUP;
  }

  // Set device address to 0
  OTG->DCFG       = (OTG->DCFG & ~OTG_HS_DCFG_DAD_MSK) | (0 << OTG_HS_DCFG_DAD_POS);
  OTG->DAINTMSK   =  OTG_HS_DAINT_IEPINT(0) | // Enable IN endpoint0 interrupt
                     OTG_HS_DAINT_OEPINT(0);  // Enable OUT endpoint0 interrupt

  // Enable Setup phase done, Out endpoint disabled and Out transfer
  // complete interrupt
  OTG->DOEPMSK    =  OTG_HS_DOEPMSK_STUPM    |
                     OTG_HS_DOEPMSK_EPDM     |
                     OTG_HS_DOEPMSK_XFRCM;

  // Enable In endpoint disable and In transfer complete interrupt
  OTG->DIEPMSK    =  OTG_HS_DIEPMSK_EPDM     |
                     OTG_HS_DIEPMSK_XFRCM;

  // Configure FIFOs
  OTG->GRXFSIZ    =  OTG_RX_FIFO_SIZE   / 4;
  OTG->TX0FSIZ    = (OTG_RX_FIFO_SIZE   / 4) |
                   ((OTG_TX0_FIFO_SIZE  / 4) << OTG_HS_DIEPTXFx_INEPTXFD_POS);

  OTG->DIEPTXF1   = ((OTG_RX_FIFO_SIZE + OTG_TX0_FIFO_SIZE) / 4) |
                    ((OTG_TX1_FIFO_SIZE / 4) << OTG_HS_DIEPTXFx_INEPTXFD_POS);

  OTG->DIEPTXF2   = ((OTG_RX_FIFO_SIZE + OTG_TX0_FIFO_SIZE + OTG_TX1_FIFO_SIZE) / 4) |
                    ((OTG_TX2_FIFO_SIZE / 4) << OTG_HS_DIEPTXFx_INEPTXFD_POS);

  OTG->DIEPTXF3   = ((OTG_RX_FIFO_SIZE + OTG_TX0_FIFO_SIZE + OTG_TX1_FIFO_SIZE + 
                      OTG_TX2_FIFO_SIZE) / 4) | ((OTG_TX3_FIFO_SIZE / 4) << OTG_HS_DIEPTXFx_INEPTXFD_POS);

  OTG->DIEPTXF4   = ((OTG_RX_FIFO_SIZE + OTG_TX0_FIFO_SIZE + OTG_TX1_FIFO_SIZE + 
                      OTG_TX2_FIFO_SIZE + OTG_TX3_FIFO_SIZE) / 4) | 
                    ((OTG_TX4_FIFO_SIZE / 4) << OTG_HS_DIEPTXFx_INEPTXFD_POS);

  OTG->DIEPTXF5   = ((OTG_RX_FIFO_SIZE + OTG_TX0_FIFO_SIZE + OTG_TX1_FIFO_SIZE + 
                      OTG_TX2_FIFO_SIZE + OTG_TX3_FIFO_SIZE + OTG_TX4_FIFO_SIZE) / 4) | 
                    ((OTG_TX5_FIFO_SIZE / 4) << OTG_HS_DIEPTXFx_INEPTXFD_POS);

  // Configure Endpoint 0
  InEndpoint[0].maxPacketSize  = 64;
  OutEndpoint[0].maxPacketSize = 64;
  InEndpoint[0].packetCount    =  1;
  
  OTG_DIEPCTL(0)  = OTG_HS_DIEPCTLx_SNAK;                // Set NAK
  OTG_DOEPCTL(0)  = OTG_HS_DOEPCTLx_SNAK;                // Set NAK
}


/* FUNCTION PROTOTYPES */
static ARM_USBD_STATUS USBD_HW_PowerControl   (ARM_POWER_STATE state);
static uint16_t        USBD_HW_GetFrameNumber (void);


/* USB DEVICE DRIVER FUNCTIONS */

/**
  \fn          ARM_DRV_VERSION USBD_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRV_VERSION USBD_HW_GetVersion (void) { return usbd_driver_version; }

/**
  \fn          ARM_USBD_CAPABILITIES USBD_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_USBD_CAPABILITIES
*/
static ARM_USBD_CAPABILITIES USBD_HW_GetCapabilities (void) { return usbd_driver_capabilities; };

/**
  \fn          ARM_USBD_STATUS USBD_Initialize (ARM_USBD_SignalDeviceEvent_t   cb_device_event,
                                                ARM_USBD_SignalEndpointEvent_t cb_endpoint_event)
  \brief       Initialize USB Device Interface.
  \param[in]   cb_device_event    Pointer to \ref USBD_SignalDeviceEvent
  \param[in]   cb_endpoint_event  Pointer to \ref USBD_SignalEndpointEvent
  \return      \ref USBD_STATUS
*/
static ARM_USBD_STATUS USBD_HW_Initialize (ARM_USBD_SignalDeviceEvent_t   cb_device_event,
                                           ARM_USBD_SignalEndpointEvent_t cb_endpoint_event) {

  cbDeviceEvent   = cb_device_event;
  cbEndpointEvent = cb_endpoint_event;

#if (!RTE_USB_OTG_HS_OTG)
  if (OTG_HS_PinsConfigure (ARM_USB_PIN_DP | ARM_USB_PIN_DM) == false) return ARM_USBD_ERROR;

  RCC->AHB1ENR  |=  RCC_AHB1ENR_OTGHSEN;        /* OTG HS clock enable        */
  RCC->AHB1RSTR |=  RCC_AHB1ENR_OTGHSEN;        /* Reset OTG HS clock         */
  osDelay(1);                                   /* Wait 1 ms                  */
  RCC->AHB1RSTR &= ~RCC_AHB1ENR_OTGHSEN;
#endif

  return ARM_USBD_OK;
}

/**
  \fn          ARM_USBD_STATUS USBD_Uninitialize (void)
  \brief       De-initialize USB Device Interface.
  \return      \ref ARM_USBD_STATUS
*/
static ARM_USBD_STATUS USBD_HW_Uninitialize (void) {

#if (!RTE_USB_OTG_HS_OTG)
  USBD_HW_PowerControl (ARM_POWER_OFF);

  RCC->AHB1RSTR |=  RCC_AHB1ENR_OTGHSEN;        /* Reset OTG HS clock         */
  osDelay(1);                                   /* Wait 1 ms                  */
  RCC->AHB1RSTR &= ~RCC_AHB1ENR_OTGHSEN;
  RCC->AHB1ENR  &= ~RCC_AHB1ENR_OTGHSEN;        /* OTG HS clock disable       */

  if (OTG_HS_PinsUnconfigure (ARM_USB_PIN_DP | ARM_USB_PIN_DM) == false) return ARM_USBD_ERROR;
#endif

  return ARM_USBD_OK;
}

/**
  \fn          ARM_USBD_STATUS USBD_HW_PowerControl (ARM_POWER_STATE state)
  \brief       Control USB Device Interface Power.
  \param[in]   state specifies Power state
  \return      \ref ARM_USBD_STATUS
*/
static ARM_USBD_STATUS USBD_HW_PowerControl (ARM_POWER_STATE state) {
  uint32_t tick;

  switch (state) {
    case ARM_POWER_OFF:
      NVIC_DisableIRQ(OTG_HS_IRQn);             // Disable OTG interrupt
    
      RCC->AHB1ENR  |=  RCC_AHB1ENR_OTGHSEN;    // Enable OTG HS clock
      osDelay(1);                               // Wait 1 ms

      OTG->DCTL     |=  OTG_HS_DCTL_SDIS;       // Soft disconnect enabled
      OTG->GCCFG    &= ~(OTG_HS_GCCFG_VBUSBSEN |// Disable VBUS sensing device "B"
                         OTG_HS_GCCFG_PWRDWN);  // Power down activated

      otg_hs_role = ARM_USB_ROLE_NONE;

      OTG->GAHBCFG  &= ~OTG_HS_GAHBCFG_GINT;

      // Core soft reset
      OTG->GRSTCTL  |=  OTG_HS_GRSTCTL_CSRST;

      tick = osKernelSysTick();
      do {
        if ((OTG->GRSTCTL & OTG_HS_GRSTCTL_CSRST) == 0) break;
      } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(10000));

      if (OTG->GRSTCTL & OTG_HS_GRSTCTL_CSRST) return ARM_USBD_ERROR;

#if (RTE_USB_OTG_HS_PHY)
      RCC->AHB1ENR  &= ~RCC_AHB1ENR_OTGHSULPIEN;// Disable OTG HS ULPI clock
#endif
      RCC->AHB1ENR  &= ~RCC_AHB1ENR_OTGHSEN;    // Disable OTG HS clock
      break;

    case ARM_POWER_LOW:
      return ARM_USBD_ERROR;

    case ARM_POWER_FULL:
      RCC->AHB1ENR  |=  RCC_AHB1ENR_OTGHSEN;    // Enable OTG HS clock

#if (RTE_USB_OTG_HS_PHY)
      RCC->AHB1ENR  |=  RCC_AHB1ENR_OTGHSULPIEN;// Enable OTG HS ULPI clock
#else
      OTG->GUSBCFG  |=  OTG_HS_GUSBCFG_PHSEL;   // Full Speed serial transceiver
#endif

      // Wait until AHB Master state machine is in the idle condition
      tick = osKernelSysTick();
      do {
        if (OTG->GRSTCTL & OTG_HS_GRSTCTL_AHBIDL) break;
      } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(100000));

      if ((OTG->GRSTCTL & OTG_HS_GRSTCTL_AHBIDL) == 0) return ARM_USBD_ERROR;

      // Core soft reset
      OTG->GRSTCTL  |=  OTG_HS_GRSTCTL_CSRST;

      tick = osKernelSysTick();
      do {
        if ((OTG->GRSTCTL & OTG_HS_GRSTCTL_CSRST) == 0) break;
      } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(10000));

      if (OTG->GRSTCTL & OTG_HS_GRSTCTL_CSRST) return ARM_USBD_ERROR;

      osDelay(3);                               // Wait 3 ms

      OTG->GAHBCFG  &= ~OTG_HS_GAHBCFG_GINT;    // Disable interrupts
      OTG->GCCFG    &= ~OTG_HS_GCCFG_VBUSBSEN;  // Disable VBUS sensing device "B"
      OTG->DCTL     |=  OTG_HS_DCTL_SDIS;       // Soft disconnect enabled

      // Set turnaround time and force device mode
      OTG->GUSBCFG   = (OTG->GUSBCFG & ~OTG_HS_GUSBCFG_TRDT_MSK) |
                        OTG_HS_GUSBCFG_TRDT(9)                   |
                        OTG_HS_GUSBCFG_FDMOD;
      osDelay(100);

      otg_hs_role = ARM_USB_ROLE_DEVICE;

#if RTE_USB_OTG_HS_PHY
      OTG->DCFG     &= ~OTG_HS_DCFG_DSPD_MSK;    // High speed
#else
      OTG->DCFG     |=  OTG_HS_DCFG_DSPD_MSK;    // Full Speed
#endif

      // Clear interrupts
      OTG->GINTSTS   =  OTG_HS_GINTSTS_MMIS     |
                        OTG_HS_GINTSTS_SOF      |
                        OTG_HS_GINTSTS_ESUSP    |
                        OTG_HS_GINTSTS_USBSUSP  |
                        OTG_HS_GINTSTS_USBRST   |
                        OTG_HS_GINTSTS_ENUMDNE  |
                        OTG_HS_GINTSTS_ISOODRP  |
                        OTG_HS_GINTSTS_EOPF     |
                        OTG_HS_GINTSTS_IISOIXFR |
                        OTG_HS_GINTSTS_IPXFR    |
                        OTG_HS_GINTSTS_CIDSCHG  |
                        OTG_HS_GINTSTS_DISCINT  |
                        OTG_HS_GINTSTS_SRQINT   |
                        OTG_HS_GINTSTS_WKUPINT;
                         
      // Unmask interrupts
      OTG->GINTMSK   =  OTG_HS_GINTMSK_USBSUSPM |
                        OTG_HS_GINTMSK_USBRST   |
                        OTG_HS_GINTMSK_ENUMDNEM |
                        OTG_HS_GINTMSK_RXFLVLM  |
                        OTG_HS_GINTMSK_IEPINT   |
                        OTG_HS_GINTMSK_OEPINT   |
                        OTG_HS_GINTMSK_WUIM;

      // Enable interrupts
      NVIC_EnableIRQ(OTG_HS_IRQn);
      OTG->GAHBCFG  |=  OTG_HS_GAHBCFG_GINT     |
                        OTG_HS_GAHBCFG_TXFELVL;

      break;
  }
  return ARM_USBD_OK;
}

/**
  \fn          ARM_USBD_STATUS USBD_HW_DeviceConnect (void)
  \brief       Connect USB Device.
  \return      \ref ARM_USBD_STATUS
*/
static ARM_USBD_STATUS USBD_HW_DeviceConnect (void) {
  OTG->GCCFG  |=  OTG_HS_GCCFG_VBUSBSEN |   // Enable VBUS sensing device "B"
                  OTG_HS_GCCFG_PWRDWN;      // Power down deactivated
  OTG->DCTL   &= ~OTG_HS_DCTL_SDIS;         // Soft disconnect disabled

  return ARM_USBD_OK;
}

/**
  \fn          ARM_USBD_STATUS USBD_HW_DeviceDisconnect (void)
  \brief       Disconnect USB Device.
  \return      \ref ARM_USBD_STATUS
*/
static ARM_USBD_STATUS USBD_HW_DeviceDisconnect (void) {
  OTG->DCTL   |=   OTG_HS_DCTL_SDIS;        // Soft disconnect enabled
  OTG->GCCFG  &= ~(OTG_HS_GCCFG_VBUSBSEN |  // Disable VBUS sensing device "B"
                   OTG_HS_GCCFG_PWRDWN);    // Power down activated 

  UsbdState.connected = false;
  UsbdState.active    = false;
  return ARM_USBD_OK;
}

/**
  \fn          ARM_USBD_STATE USBD_HW_DeviceGetState (void)
  \brief       Get current USB Device State.
  \return      \ref ARM_USBD_STATE
*/
static ARM_USBD_STATE USBD_HW_DeviceGetState (void) {
  return UsbdState;
}

/**
  \fn          ARM_USBD_STATUS USBD_HW_DeviceRemoteWakeup (void)
  \brief       Trigger USB Remote Wakeup.
  \return      \ref ARM_USBD_STATUS
*/
static ARM_USBD_STATUS USBD_HW_DeviceRemoteWakeup (void) {
  OTG->DCTL   |=   OTG_HS_DCTL_RWUSIG;      // Remote wakeup signalling
  osDelay(5);
  OTG->DCTL   &=  ~OTG_HS_DCTL_RWUSIG;
  return ARM_USBD_OK;
}

/**
  \fn          ARM_USBD_STATUS USBD_HW_DeviceSetAddress (uint8_t dev_addr,
                                                         ARM_USBD_SET_ADDRESS_STAGE stage)
  \brief       Set USB Device Address.
  \param[in]   dev_addr specifies Device Address
  \param[in]   stage specifies stage in which the function is called
  \return      \ref ARM_USBD_STATUS
*/
static ARM_USBD_STATUS USBD_HW_DeviceSetAddress (uint8_t dev_addr,
                                                 ARM_USBD_SET_ADDRESS_STAGE stage) {
  OTG->DCFG = (OTG->DCFG & ~OTG_HS_DCFG_DAD_MSK) |
               OTG_HS_DCFG_DAD(dev_addr);
  return ARM_USBD_OK;
}

/**
  \fn          ARM_USBD_STATUS USBD_HW_DeviceConfigure (bool configure)
  \brief       Configure/unconfigure USB Device.
  \param[in]   configure specifies operation
                - \b false Unconfigure
                - \b true  Configure
  \return      \ref ARM_USBD_STATUS
*/
static ARM_USBD_STATUS USBD_HW_DeviceConfigure (bool configure) {
  return ARM_USBD_OK;
}

/**
  \fn          ARM_USBD_STATUS USBD_HW_EndpointConfigure (uint8_t ep_addr,
                                                          ARM_USB_ENDPOINT_TYPE ep_type,
                                                          uint16_t ep_max_packet_size)
  \brief       Configure USB Endpoint.
  \param[in]   ptr_epd specifies pointer to Endpoint descriptor
  \return      \ref ARM_USBD_STATUS
*/
static ARM_USBD_STATUS USBD_HW_EndpointConfigure (uint8_t ep_addr,
                                                  ARM_USB_ENDPOINT_TYPE ep_type,
                                                  uint16_t ep_max_packet_size) {
  uint32_t val, dir;
  uint8_t num;

  num  = ep_addr & ARM_USB_ENDPOINT_NUMBER_MASK;
  val  = ep_max_packet_size & ARM_USB_ENDPOINT_MAX_PACKET_SIZE_MASK;

  if (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) dir = 16;
  else                                           dir = 0;

  if (num == 0) return ARM_USBD_OK;

  // Check if endpoint is enabled 
  if ((RTE_USB_OTG_HS_DEV_EP & (1 << num)) == 0) return ARM_USBD_ERROR;

  // Check if endpoint supports requested type
  switch (ep_type) {
    case ARM_USB_ENDPOINT_ISOCHRONOUS:
      if ((RTE_USB_OTG_HS_DEV_EP_ISO  & (1 << (num + dir))) == 0)
        return ARM_USBD_ERROR;
      break;
    case ARM_USB_ENDPOINT_BULK:
      if ((RTE_USB_OTG_HS_DEV_EP_BULK & (1 << (num + dir))) == 0)
        return ARM_USBD_ERROR;
      break;
    case ARM_USB_ENDPOINT_INTERRUPT:
      if ((RTE_USB_OTG_HS_DEV_EP_INT  & (1 << (num + dir))) == 0)
        return ARM_USBD_ERROR;
      break;
    default:
      break;
  }

  // IN Endpoint Configuration
  if (dir) {

    // Set IN Endpoint resources
    InEndpoint[num].buffer        = NULL;
    InEndpoint[num].dataSize      = 0;
    InEndpoint[num].maxPacketSize = val;
    InEndpoint[num].startRequired = false;

    if (OTG_EP_IN_TYPE(num) == ARM_USB_ENDPOINT_ISOCHRONOUS) {
      InEndpoint[num].packetCount = (ep_max_packet_size & 
                                     ARM_USB_ENDPOINT_MICROFRAME_TRANSACTIONS_MASK) >> 11;
    } else InEndpoint[num].packetCount = 1;
     
#if (OTG_INT_IN_EP != 0)
    if (ep_type == ARM_USB_ENDPOINT_INTERRUPT) {
      InEndpoint[num].startRequired = true;
    }
#endif
    InEndpointNAK  &= ~(1 << num);

    // Configure IN endpoint
    OTG_DIEPCTL(num) = (num     <<  OTG_HS_DIEPCTLx_TXFNUM_POS) |   // Fifo Number
                       (ep_type <<  OTG_HS_DIEPCTLx_EPTYP_POS)  |   // Endpoint Type
                       (val);                                       // Max Packet Size

    // Set DATA0 PID for Interrupt or Bulk Endpoint
    if (ep_type >= ARM_USB_ENDPOINT_BULK) {
      OTG_DIEPCTL(num) |= OTG_HS_DIEPCTLx_SD0PID;
    }

    OTG_DIEPCTL(num)   |= OTG_HS_DIEPCTLx_USBAEP;     // Activate Endpoint

    if (OTG_DIEPCTL(num) & OTG_HS_DIEPCTLx_EPENA) {
      OTG_DIEPCTL(num) |= OTG_HS_DIEPCTLx_EPDIS;      // Disable endpoint
    }
    OTG_DIEPCTL(num)   |= OTG_HS_DIEPCTLx_SNAK;       // Set Endpoint NAK

    USBD_FlushInEpFifo (num | ARM_USB_ENDPOINT_DIRECTION_MASK);


    // Isochronous IN Endpoint Configuration
    if (OTG_EP_IN_TYPE(num) == ARM_USB_ENDPOINT_ISOCHRONOUS) {
      OTG->GINTMSK |= OTG_HS_GINTMSK_IISOIXFRM;       // Enable IISOIXFR

      // Regarding FrameNumber, set Frame
      if (USBD_HW_GetFrameNumber() & 1) OTG_DIEPCTL(num) |= OTG_HS_DIEPCTLx_SEVNFRM;
      else                              OTG_DIEPCTL(num) |= OTG_HS_DIEPCTLx_SODDFRM;

      // Enable Endpoint and Clear NAK
      OTG_DIEPCTL(num) |= OTG_HS_DIEPCTLx_EPENA | OTG_HS_DIEPCTLx_CNAK;
    }

    // Enable IN endpoint interrupt
    OTG->DAINTMSK  |= OTG_HS_DAINTMSK_IEPM(num);


  // OUT Endpoint Configuration
  } else {
    
    // Set OUT Endpoint resources
    OutEndpoint[num].buffer         = NULL;
    OutEndpoint[num].dataSize       = 0;
    OutEndpoint[num].maxPacketSize  = val;

    if (OTG_EP_OUT_TYPE(num) == ARM_USB_ENDPOINT_ISOCHRONOUS) {
      OutEndpoint[num].packetCount = (ep_max_packet_size & 
                                      ARM_USB_ENDPOINT_MICROFRAME_TRANSACTIONS_MASK) >> 11;
    } else OutEndpoint[num].packetCount = 1;

    // Configure OUT endpoint
    OTG_DOEPCTL(num) = (ep_type <<  OTG_HS_DOEPCTLx_EPTYP_POS)| // Endpoint Type
                        OTG_HS_DOEPCTLx_SNAK                  | // Set NAK
                        val;                                    // Max Packet Size

    // Set DATA0 PID for Interrupt or Bulk Endpoint
    if (ep_type >= ARM_USB_ENDPOINT_BULK) {
      OTG_DOEPCTL(num) |= OTG_HS_DOEPCTLx_SD0PID;
    }

    // Isochronous OUT Endpoint Configuration
    if (OTG_EP_OUT_TYPE(num) == ARM_USB_ENDPOINT_ISOCHRONOUS) {
      OTG->GINTMSK |= OTG_HS_GINTMSK_EOPFM;           // Enable End of Periodic Frame Interrupt
    }

    OTG_DOEPCTL(num) |= OTG_HS_DOEPCTLx_USBAEP;       // Activate Endpoint

   // Enable OUT endpoint interrupt
   OTG->DAINTMSK     |= OTG_HS_DAINTMSK_OEPM(num);
  }
  return ARM_USBD_OK;
}

/**
  \fn          ARM_USBD_STATUS USBD_HW_EndpointUnconfigure (uint8_t ep_addr)
  \brief       Unconfigure USB Endpoint.
  \param[in]   ep_addr specifies Endpoint Address
                ep_addr.0..3: Address
                ep_addr.7:    Direction
  \return      \ref ARM_USBD_STATUS
*/
static ARM_USBD_STATUS USBD_HW_EndpointUnconfigure (uint8_t ep_addr) {
  uint32_t num, IsoEpEnCnt, tick;

  IsoEpEnCnt = 0;

  // Unconfigure IN Endpoint
  if (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) {
    ep_addr &= ARM_USB_ENDPOINT_NUMBER_MASK;

    OTG->DAINTMSK &= ~OTG_HS_DAINTMSK_IEPM(ep_addr);      // Disable IN EP interrupt
    // Count Active Isochronous IN Endpoints
    if (OTG_EP_IN_TYPE(ep_addr) == ARM_USB_ENDPOINT_ISOCHRONOUS) {
      for (num = 1; num <= USBD_EP_NUM; num++) {
        if (OTG_DIEPCTL(num) & OTG_HS_DIEPCTLx_USBAEP) {
          if (OTG_EP_IN_TYPE(num) == ARM_USB_ENDPOINT_ISOCHRONOUS) {
            IsoEpEnCnt++;
          }
        }
      }
      // If Last Active Isochronous IN Endpoint, Disable IISOIXFR
      if (IsoEpEnCnt == 1) OTG->GINTMSK &= ~OTG_HS_GINTMSK_IISOIXFRM;
    }

    if (OTG_DIEPCTL(ep_addr) & OTG_HS_DIEPCTLx_EPENA)
      OTG_DIEPCTL(ep_addr)  |=  OTG_HS_DIEPCTLx_EPDIS;    // Disable Endpoint
    OTG_DIEPCTL(ep_addr)    |=  OTG_HS_DIEPCTLx_SNAK;     // Set Endpoint NAK

    if (ep_addr) 
      OTG_DIEPCTL(ep_addr)  &= ~OTG_HS_DIEPCTLx_USBAEP;   // Deactivate Endpoint

    // Reset IN endpoint resources
    InEndpoint[ep_addr].buffer        = NULL;
    InEndpoint[ep_addr].dataSize      = 0;
    InEndpoint[ep_addr].maxPacketSize = 0;
    InEndpoint[ep_addr].startRequired = false;
    InEndpoint[ep_addr].packetCount   = 0;
    InEndpointNAK &= ~(1 << ep_addr);

  // Unconfigure OUT Endpoint
  } else {
    OTG->DAINTMSK &= ~OTG_HS_DAINTMSK_OEPM(ep_addr);      // Disable IN EP interrupt

    // Count Active Isochronous OUT Endpoints
    if (OTG_EP_OUT_TYPE(ep_addr) == ARM_USB_ENDPOINT_ISOCHRONOUS) {
      for (num = 1; num <= USBD_EP_NUM; num++) {
        if (OTG_DOEPCTL(num) & OTG_HS_DOEPCTLx_USBAEP) {
          if (OTG_EP_OUT_TYPE(num) == ARM_USB_ENDPOINT_ISOCHRONOUS) {
            IsoEpEnCnt++;
          }
        }
      }
      // If Last Active Isochronous OUT Endpoint, Disable EOPF
      if (IsoEpEnCnt == 1) OTG->GINTMSK &= ~OTG_HS_GINTMSK_EOPFM;
    }

    // Set Global Out Nak
    OTG->DCTL |= OTG_HS_DCTL_SGONAK;
    tick = osKernelSysTick();
    do {
      if (OTG->GINTSTS & OTG_HS_GINTSTS_GONAKEFF) break;
    } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(10000));

    if ((OTG->GINTSTS & OTG_HS_GINTSTS_GONAKEFF) == 0) return ARM_USBD_ERROR;

    OTG_DOEPCTL(ep_addr) |= OTG_HS_DOEPCTLx_SNAK;         // Set Endpoint NAK
    
    if (ep_addr) {
      // Disable OUT endpoint
      if (OTG_DOEPCTL(ep_addr) & OTG_HS_DOEPCTLx_EPENA) { // If Endpoint is Enabled
        OTG_DOEPCTL(ep_addr)  |=  OTG_HS_DOEPCTLx_EPDIS;  // Disable Endpoint

        tick = osKernelSysTick();
        do {
          if (OTG_DOEPINT(ep_addr) & OTG_HS_DOEPINTx_EPDISD) break;
        } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(10000));

        if ((OTG_DOEPINT(ep_addr) & OTG_HS_DOEPINTx_EPDISD) == 0) return ARM_USBD_ERROR;
      }

      OTG_DOEPCTL(ep_addr) &= ~OTG_HS_DOEPCTLx_USBAEP;    // Deactivate Endpoint
    }

    OTG_DOEPCTL(ep_addr) &= ~OTG_HS_DOEPCTLx_USBAEP;      // Deactivate Endpoint

    OTG->DCTL |= OTG_HS_DCTL_CGONAK;                      // Clear Global OUT NAK

    // Reset OUT endpoint resources
    OutEndpoint[ep_addr].buffer        = NULL;
    OutEndpoint[ep_addr].dataSize      = 0;
    OutEndpoint[ep_addr].maxPacketSize = 0;
    InEndpoint[ep_addr].packetCount    = 0;
  }
  return ARM_USBD_OK;
}

/**
  \fn          ARM_USBD_STATUS USBD_HW_EndpointStall (uint8_t ep_addr, bool stall)
  \brief       Set/Clear Stall for USB Endpoint.
  \param[in]   ep_addr specifies Endpoint Address
                ep_addr.0..3: Address
                ep_addr.7:    Direction
  \param[in]   stall specifies operation
                - \b false Clear
                - \b true Set
  \return      \ref ARM_USBD_STATUS
*/
static ARM_USBD_STATUS USBD_HW_EndpointStall (uint8_t ep_addr, bool stall) {
  uint32_t tick;

  if (stall) {
    // IN Endpoint stall
    if (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) {
      ep_addr &= ARM_USB_ENDPOINT_NUMBER_MASK;

      if (OTG_DIEPCTL(ep_addr) & OTG_HS_DIEPCTLx_EPENA)
        OTG_DIEPCTL(ep_addr)  |= OTG_HS_DIEPCTLx_STALL | OTG_HS_DIEPCTLx_EPDIS;
      else 
        OTG_DIEPCTL(ep_addr)  |= OTG_HS_DIEPCTLx_STALL;

      USBD_FlushInEpFifo (ep_addr | ARM_USB_ENDPOINT_DIRECTION_MASK);

    // OUT Endpoint Stall
    } else {
      OTG->DCTL |= OTG_HS_DCTL_SGONAK;                // Set Global OUT NAK
      tick = osKernelSysTick();
      do {
        if (OTG->GINTSTS & OTG_HS_GINTSTS_GONAKEFF) break;
      } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(10000));

      if ((OTG->GINTSTS & OTG_HS_GINTSTS_GONAKEFF) == 0) return ARM_USBD_ERROR;

      // Stall Out endpoint
      if (OTG_DOEPCTL(ep_addr) & OTG_HS_DOEPCTLx_EPENA)
        OTG_DOEPCTL(ep_addr)  |= OTG_HS_DOEPCTLx_STALL | OTG_HS_DOEPCTLx_EPDIS;
      else
        OTG_DOEPCTL(ep_addr)  |= OTG_HS_DOEPCTLx_STALL;

      OTG->DCTL |= OTG_HS_DCTL_CGONAK;                // Clear global NAK
    }
  } else {
    // Clear IN Endpoint stall
    if (ep_addr & 0x80) {
      ep_addr &= ~0x80;

      if (OTG_DIEPCTL(ep_addr) &  OTG_HS_DIEPCTLx_EPENA)  // If Endpoint enabled
        OTG_DIEPCTL(ep_addr)   |= OTG_HS_DIEPCTLx_EPDIS;  // Disable Endpoint

      USBD_FlushInEpFifo (ep_addr | ARM_USB_ENDPOINT_DIRECTION_MASK);

      // Set DATA0 pid for interrupt and bulk endpoint
      if (((OTG_DIEPCTL(ep_addr) & OTG_HS_DIEPCTLx_EPTYP_MSK) >> OTG_HS_DIEPCTLx_EPTYP_POS) > 1)
        OTG_DIEPCTL(ep_addr) |= OTG_HS_DIEPCTLx_SD0PID;

      OTG_DIEPCTL(ep_addr) &= ~OTG_HS_DIEPCTLx_STALL;     // Clear Stall
 
    // Clear OUT Endpoint stall
    } else {
      // Set DATA0 pid for interrupt and bulk endpoint
      if (((OTG_DOEPCTL(ep_addr) & OTG_HS_DOEPCTLx_EPTYP_MSK) >> OTG_HS_DOEPCTLx_EPTYP_POS) > 1) {
        OTG_DOEPCTL(ep_addr) |= OTG_HS_DOEPCTLx_SD0PID;
      }
      OTG_DOEPCTL(ep_addr) &= ~OTG_HS_DOEPCTLx_STALL;     // Clear stall
    }
  }
  return ARM_USBD_OK;
}

/**
\fn          void USBD_HW_EndpointReadSet (uint8_t ep_addr, uint8_t *buf, uint32_t len)
\brief       Set Endpoint for next read.
\param[in]   ep_addr specifies Endpoint Address
              ep_addr.0..3: Address
              ep_addr.7:    Direction
*/
static void USBD_HW_EndpointReadSet (uint8_t ep_addr) {
  uint32_t sz;
  
  // Set packet count and transfer size
  if  (OutEndpoint[ep_addr].dataSize > OutEndpoint[ep_addr].maxPacketSize) sz = OutEndpoint[ep_addr].maxPacketSize;
  else                                                                     sz = OutEndpoint[ep_addr].dataSize;
  if (ep_addr != 0) {
    OTG_DOEPTSIZ(ep_addr) = (OutEndpoint[ep_addr].packetCount << OTG_HS_DOEPTSIZx_PKTCNT_POS) |
                             sz;
  } else {
    OTG_DOEPTSIZ(ep_addr) = (OutEndpoint[ep_addr].packetCount << OTG_HS_DOEPTSIZx_PKTCNT_POS) |
                            (3 << OTG_HS_DOEPTSIZ0_STUPCNT_POS)|
                             sz;
  }
  // Set correct frame for isochronous endpoint
  if (OTG_EP_OUT_TYPE(ep_addr) == ARM_USB_ENDPOINT_ISOCHRONOUS) {
    if ((USBD_HW_GetFrameNumber() & 1)) OTG_DOEPCTL(ep_addr) |= OTG_HS_DOEPCTLx_SEVNFRM;
    else                                OTG_DOEPCTL(ep_addr) |= OTG_HS_DOEPCTLx_SODDFRM;
  }
  // Clear NAK  and enable endpoint
  OTG_DOEPCTL(ep_addr) |= OTG_HS_DOEPCTLx_EPENA | OTG_HS_DOEPCTLx_CNAK;
}


/**
\fn          ARM_USBD_STATUS USBD_HW_EndpointReadStart (uint8_t ep_addr, uint8_t *buf, uint32_t len)
\brief       Start USB Endpoint Read operation.
\param[in]   ep_addr specifies Endpoint Address
              ep_addr.0..3: Address
              ep_addr.7:    Direction
\param[out]  buf specifies buffer for data read from Endpoint
\param[in]   len specifies buffer length
\return      \ref ARM_USBD_STATUS
*/
static ARM_USBD_STATUS USBD_HW_EndpointReadStart (uint8_t ep_addr, uint8_t *buf, uint32_t len) {

  if (OutEndpoint[ep_addr].maxPacketSize == 0)
    return ARM_USBD_ERROR;
  if (OutEndpoint[ep_addr].bufferIndex != 0)
    return ARM_USBD_ERROR;

  OutEndpoint[ep_addr].bufferIndex = 0;
  OutEndpoint[ep_addr].buffer      = buf;
  OutEndpoint[ep_addr].dataSize    = len;

  USBD_HW_EndpointReadSet(ep_addr);

  return ARM_USBD_OK;
}

/**
  \fn          static int32_t USBD_HW_ReadFromFifo (uint8_t ep_addr)
  \brief       Read data from USB Endpoint.
  \param[in]   ep_addr specifies Endpoint Address
                ep_addr.0..3: Address
                ep_addr.7:    Direction
  \param[in]   sz specifies data size to be read from fifo
  \return      number of data bytes read, error code if negative
*/
static int32_t USBD_HW_ReadFromFifo (uint8_t ep_addr, uint32_t sz) {
  uint32_t i, val, residue;
  uint8_t  tmpBuf[4];

  // Check if Endpoint is activated and buffer available
  if ((OTG_DOEPCTL(ep_addr) & OTG_HS_DOEPCTLx_USBAEP) == 0) return 0; 
  if (OutEndpoint[ep_addr].buffer == NULL)                  return 0;

  if (sz > OutEndpoint[ep_addr].dataSize)
    sz = OutEndpoint[ep_addr].dataSize;

  // If Isochronous Endpoint
  if (OTG_EP_OUT_TYPE(ep_addr) == ARM_USB_ENDPOINT_ISOCHRONOUS) {
    val =  OutEndpoint[ep_addr].packetCount - 
          ((OTG_DOEPTSIZ(ep_addr) & OTG_HS_DOEPTSIZx_PKTCNT_MSK) >> OTG_HS_DOEPTSIZx_PKTCNT_POS);

    switch ((OTG_DOEPTSIZ(ep_addr) & OTG_HS_DOEPTSIZx_RXDPID_MSK) >> OTG_HS_DOEPTSIZx_RXDPID_POS) {
      // Data0
      case 0:
        if (val != 1) sz = 0; break;
       // Data1
      case 2:
        if (val != 2) sz = 0; break;
      // Data2
      case 1:
        if (val != 3) sz = 0; break; 
    }
  }

  // Copy data from fifo
  for (i = 0; i < (uint32_t)(sz / 4); i++) {
    *((__packed uint32_t *)(OutEndpoint[ep_addr].buffer + OutEndpoint[ep_addr].bufferIndex)) = OTG_RX_FIFO;
    OutEndpoint[ep_addr].bufferIndex += 4;
  }
  // if data size is not equal n*4
  residue = sz % 4;
  if (residue != 0) {
    *((__packed uint32_t *)tmpBuf) = OTG_RX_FIFO;
    for (i = 0; i < residue; i++) {
      OutEndpoint[ep_addr].buffer[OutEndpoint[ep_addr].bufferIndex++] = tmpBuf[i];
    }
  }

  if (sz != OutEndpoint[ep_addr].maxPacketSize) OutEndpoint[ep_addr].dataSize  = 0;
  else                                          OutEndpoint[ep_addr].dataSize -= sz;

  return (sz);
}

/**
  \fn          int32_t USBD_HW_EndpointRead (uint8_t ep_addr, uint8_t *buf, uint32_t len)
  \brief       Read data from USB Endpoint.
  \param[in]   ep_addr specifies Endpoint Address
                ep_addr.0..3: Address
                ep_addr.7:    Direction
  \param[out]  buf specifies buffer for data read from Endpoint
  \param[in]   len specifies buffer length
  \return      number of data bytes read, error code if negative
*/
static int32_t USBD_HW_EndpointRead (uint8_t ep_addr, uint8_t *buf, uint32_t len) {
  uint32_t sz;

  sz = OutEndpoint[ep_addr].bufferIndex;
  OutEndpoint[ep_addr].buffer      = NULL;
  OutEndpoint[ep_addr].bufferIndex = 0;
  return (sz);
}

/**
  \fn          static int32_t USBD_HW_WriteToFifo (uint8_t ep_addr)
  \brief       Write data to USB Endpoint fifo.
  \param[in]   ep_addr specifies Endpoint Address
                ep_addr.0..3: Address
                ep_addr.7:    Direction
  \return      number of data bytes written, error code if negative
*/
static int32_t USBD_HW_WriteToFifo (uint8_t ep_addr) {
  uint32_t i;

  ep_addr &= ARM_USB_ENDPOINT_NUMBER_MASK;

  // Check if enough space in fifo
  if ((OTG_DTXFSTS(ep_addr) * 4) < InEndpoint[ep_addr].dataSize) return 0;

  // Set transfer size and packet count
  OTG_DIEPTSIZ(ep_addr) = (InEndpoint[ep_addr].packetCount << OTG_HS_DIEPTSIZx_PKTCNT_POS) |
                          (InEndpoint[ep_addr].packetCount << OTG_HS_DIEPTSIZx_MCNT_POS)   |
                           InEndpoint[ep_addr].dataSize;

  // Set correct frame fo isochronous endpoint
  if (OTG_EP_IN_TYPE(ep_addr) == ARM_USB_ENDPOINT_ISOCHRONOUS) {
    if (USBD_HW_GetFrameNumber() & 1) OTG_DIEPCTL(ep_addr) |= OTG_HS_DIEPCTLx_SEVNFRM;
    else OTG_DIEPCTL(ep_addr)                              |= OTG_HS_DIEPCTLx_SODDFRM;
  }

  // Enable endpoint and clear NAK
  OTG_DIEPCTL(ep_addr) |= OTG_HS_DIEPCTLx_EPENA | OTG_HS_DIEPCTLx_CNAK;

  // Copy data to fifo
  for (i = 0; i < (uint32_t)((InEndpoint[ep_addr].dataSize+3)/4); i++) {
    OTG_TX_FIFO(ep_addr) = *(__packed uint32_t *)InEndpoint[ep_addr].buffer;
    InEndpoint[ep_addr].buffer +=4;
  }

  InEndpoint[ep_addr].buffer = NULL;
  return InEndpoint[ep_addr].dataSize;
}

/**
  \fn          int32_t USBD_HW_EndpointWrite (uint8_t ep_addr, const uint8_t *buf, uint32_t len)
  \brief       Write data to USB Endpoint.
  \param[in]   ep_addr specifies Endpoint Address
                ep_addr.0..3: Address
                ep_addr.7:    Direction
  \param[in]   buf specifies buffer with data to write to Endpoint
  \param[in]   len specifies buffer length
  \return      number of data bytes written, error code if negative
*/
static int32_t USBD_HW_EndpointWrite (uint8_t ep_addr, const uint8_t *buf, uint32_t len) {
  uint8_t num;

  num = ep_addr & ARM_USB_ENDPOINT_NUMBER_MASK;

  // If endpoint is disabled, return 0
  if ((OTG_DIEPCTL(num) & OTG_HS_DIEPCTLx_USBAEP) == 0) return (0);

  if (len > InEndpoint[num].maxPacketSize)
    len = InEndpoint[num].maxPacketSize;

  InEndpoint[num].dataSize = len;

#if (OTG_INT_IN_EP != 0)
  if (OTG_EP_IN_TYPE(num) == ARM_USB_ENDPOINT_INTERRUPT) {
    // Check if buffer is empty
    if (InEndpoint[num].buffer != NULL) return (-ARM_USBD_ERROR);

    // Copy data to intermediate buffer
    memcpy((uint8_t *)(ptr_OTG_IntInEpTmpBuf[num]), buf, len);
    InEndpoint[num].buffer = (uint8_t *)ptr_OTG_IntInEpTmpBuf[num];

    if (InEndpoint[num].startRequired == true) {
      InEndpoint[num].startRequired = false;
      InEndpointNAK       |=  1 << num;
      OTG_DIEPCTL(num) |=  OTG_HS_DIEPCTLx_SNAK;      // Set NAK
      OTG->DIEPMSK     |=  OTG_HS_DIEPMSK_INEPNEM;    // Enable NAK effective interrupt
    }
  } else
#endif
  {
    InEndpoint[num].buffer = (uint8_t *)buf;
    USBD_HW_WriteToFifo(ep_addr);
  }
  return (len);
}

/**
  \fn          ARM_USBD_STATUS USBD_HW_EndpointAbort (uint8_t ep_addr)
  \brief       Abort current USB Endpoint transfer.
  \param[in]   ep_addr specifies Endpoint Address
                ep_addr.0..3: Address
                ep_addr.7:    Direction
  \return      \ref ARM_USBD_STATUS
*/
static ARM_USBD_STATUS USBD_HW_EndpointAbort (uint8_t ep_addr) {

  if (ep_addr & ARM_USB_ENDPOINT_DIRECTION_MASK) {
    ep_addr &= ARM_USB_ENDPOINT_NUMBER_MASK;
    InEndpoint[ep_addr].buffer = NULL;
    if (OTG_DIEPCTL(ep_addr) &  OTG_HS_DIEPCTLx_EPENA)    // If endpoint enabled
      OTG_DIEPCTL(ep_addr)   |= OTG_HS_DIEPCTLx_EPDIS;    // disable endpoint
    OTG_DIEPCTL(ep_addr)     |= OTG_HS_DIEPCTLx_SNAK;     // Set NAK

    USBD_FlushInEpFifo (ep_addr | ARM_USB_ENDPOINT_DIRECTION_MASK);

  } else {
    OutEndpoint[ep_addr].buffer = NULL;
    if (OTG_DOEPCTL(ep_addr) &  OTG_HS_DOEPCTLx_EPENA)    // If endpoint enabled
      OTG_DOEPCTL(ep_addr)   |= OTG_HS_DOEPCTLx_EPDIS;    // Disable endpoint
    OTG_DOEPCTL(ep_addr)     |= OTG_HS_DOEPCTLx_SNAK;     // Set NAK
  }
  return ARM_USBD_OK;
}

/**
  \fn          uint16_t USBD_HW_GetFrameNumber (void)
  \brief       Get current USB Frame Number.
  \return      Frame Number
*/
static uint16_t USBD_HW_GetFrameNumber (void) {
  return ((OTG->DSTS & OTG_HS_DSTS_FNSOF_MSK) >> OTG_HS_DSTS_FNSOF_POS);
}

/**
  \fn          void USBD_HS_IRQ (uint32_t gintsts)
  \brief       USB Device Interrupt Routine (IRQ).
*/
void USBD_HS_IRQ (uint32_t gintsts) {
  uint32_t val, num, msk, sz, ep_int;
  static uint32_t IsoInIncomplete = 0;

// Reset interrupt
  if (gintsts & OTG_HS_GINTSTS_USBRST) {
    UsbdState.active = false;
    USBD_Reset();
    cbDeviceEvent(ARM_USBD_EVENT_RESET);
    OTG->GINTSTS = OTG_HS_GINTSTS_USBRST;
  }

// Suspend interrupt
  if (gintsts & OTG_HS_GINTSTS_USBSUSP) {
    UsbdState.active = false;
    cbDeviceEvent(ARM_USBD_EVENT_SUSPEND);
    OTG->GINTSTS = OTG_HS_GINTSTS_USBSUSP;
  }

// Resume interrupt
  if (gintsts & OTG_HS_GINTSTS_WKUPINT) {
    UsbdState.active = true;
    cbDeviceEvent(ARM_USBD_EVENT_RESUME);
    OTG->GINTSTS = OTG_HS_GINTSTS_WKUPINT;
  }

// Speed enumeration completed
  if (gintsts & OTG_HS_GINTSTS_ENUMDNE) {
    switch ((OTG->DSTS & OTG_HS_DSTS_ENUMSPD_MSK) >> OTG_HS_DSTS_ENUMSPD_POS) {
      case 0:
        UsbdState.speed     = ARM_USB_SPEED_HIGH;
        UsbdState.connected = true;
        cbDeviceEvent(ARM_USBD_EVENT_CONNECT);
        cbDeviceEvent(ARM_USBD_EVENT_HIGH_SPEED);
        break;
      case 3:
        UsbdState.speed     = ARM_USB_SPEED_FULL;
        UsbdState.connected = true;
        cbDeviceEvent(ARM_USBD_EVENT_CONNECT);
        break;
      default:
        UsbdState.connected = false;
    }

    OTG->DCTL    |= OTG_HS_DCTL_CGINAK;     // Clear global IN NAK
    OTG->DCTL    |= OTG_HS_DCTL_CGONAK ;    // clear global OUT NAK
    OTG->GINTSTS  = OTG_HS_GINTSTS_ENUMDNE;
  }

  if (gintsts & OTG_HS_GINTSTS_RXFLVL) {
    OTG->GINTMSK &= ~OTG_HS_GINTMSK_RXFLVLM;

    val =  OTG->GRXSTSP;
    num =  val & 0x0F;
    sz  = (val >> 4) & 0x7FF;

    switch ((val >> 17) & 0x0F) {
      // Setup packet
      case 6:
        USBD_HW_ReadFromFifo(0, sz);
      break;

      // OUT packet
      case 2:
        USBD_HW_ReadFromFifo(num, sz);
        break;

      // Global OUT NAK
      case 1:
        break;

      // OUT transfer completed
      case 3:
        break;
      
      // SETUP transaction completed
      case 4:
        break;

      default:
        break;
    }
    OTG->GINTMSK |= OTG_HS_GINTMSK_RXFLVLM;
  }

// OUT Packet
  if (gintsts & OTG_HS_GINTSTS_OEPINT) {
    msk = (((OTG->DAINT & OTG->DAINTMSK) >> 16) & 0xFFFF);
    num = 0;

    do {
      if ((msk >> num) & 1) {
        ep_int = OTG_DOEPINT(num) & OTG->DOEPMSK;
        // Endpoint disabled
        if (ep_int & OTG_HS_DOEPINTx_EPDISD) {
          if (OTG_EP_OUT_TYPE(num) == ARM_USB_ENDPOINT_ISOCHRONOUS) {
            // Set packet count and transfer size
            OTG_DOEPTSIZ(num) = (OutEndpoint[num].packetCount << OTG_HS_DOEPTSIZx_PKTCNT_POS) | 
                                (OutEndpoint[num].maxPacketSize);

            // Set correct frame
            if ((USBD_HW_GetFrameNumber() & 1)) OTG_DOEPCTL(num) |= OTG_HS_DOEPCTLx_SEVNFRM;
            else                                OTG_DOEPCTL(num) |= OTG_HS_DOEPCTLx_SODDFRM;

            OTG_DOEPCTL(num) |= OTG_HS_DOEPCTLx_EPENA | OTG_HS_DOEPCTLx_CNAK;
          }
          OTG_DOEPINT(num) = OTG_HS_DOEPINTx_EPDISD;
        }

        // Setup phase done interrupt
        if (ep_int & OTG_HS_DOEPINTx_STUP) {
          OutEndpoint[num].dataSize = 0;
          OTG_DOEPINT(num) = OTG_HS_DOEPINTx_STUP;
          cbEndpointEvent(num, ARM_USBD_EP_EVENT_SETUP);
        }

        // Transfer complete interrupt
        if (ep_int & OTG_HS_DOEPINTx_XFCR) {
          OTG_DOEPINT(num) = OTG_HS_DOEPINTx_XFCR;
          if (OutEndpoint[num].dataSize != 0) {
            USBD_HW_EndpointReadSet(num);
          } else {
            if (OTG_EP_OUT_TYPE(num) != ARM_USB_ENDPOINT_ISOCHRONOUS) {
              cbEndpointEvent(num, ARM_USBD_EP_EVENT_OUT);
            }
          }
        }
      }
      num++;
    } while (msk >> num);
  }

// IN Packet
  if (gintsts & OTG_HS_GINTSTS_IEPINT) {
    msk = (OTG->DAINT & OTG->DAINTMSK & 0xFFFF);
    num = 0;

    do {
      if ((msk >> num) & 1) {
        ep_int = OTG_DIEPINT(num) & OTG->DIEPMSK;
        // Endpoint Disabled
        if (ep_int & OTG_HS_DIEPINTx_EPDISD) {
          OTG_DIEPINT(num) = OTG_HS_DIEPINTx_EPDISD;

          if (OTG_EP_IN_TYPE(num) == ARM_USB_ENDPOINT_ISOCHRONOUS) {
            if ((IsoInIncomplete & (1 << num)) != 0) {
              // Flush IN endpoint fifo and write write new data if available
              USBD_FlushInEpFifo(num | ARM_USB_ENDPOINT_DIRECTION_MASK);
              if (InEndpoint[num].buffer) USBD_HW_WriteToFifo(num);
              IsoInIncomplete &= ~(1 << num);
            }
          }
        }

        // IN endpoint NAK effective
#if (OTG_INT_IN_EP)
        if (ep_int & OTG_HS_DIEPINTx_INEPNE) {
          if (OTG_EP_IN_TYPE(num) == ARM_USB_ENDPOINT_INTERRUPT) {
            if (InEndpointNAK & (1 << num)) {
              InEndpointNAK &= ~ (1 << num);
              // if no more forced NAKs, disable IN NAK effective interrupt
              if (InEndpointNAK == 0) OTG->DIEPMSK &= ~OTG_HS_DIEPMSK_INEPNEM;

              // If Data available, write Data
              if (InEndpoint[num].buffer) USBD_HW_WriteToFifo(num);
            }
          }
          OTG_DIEPINT(num) = OTG_HS_DIEPINTx_INEPNE;
        }
#endif

        // Transmit completed
        if (ep_int & OTG_HS_DIEPINTx_XFCR) {
          OTG_DIEPINT(num) = OTG_HS_DIEPINTx_XFCR;

#if (OTG_INT_IN_EP)
          if (OTG_EP_IN_TYPE(num) == ARM_USB_ENDPOINT_INTERRUPT) {
            // If Data available, write Data
            if (InEndpoint[num].buffer) USBD_HW_WriteToFifo(num);

            // If no data available next interrupt IN transfer must
            // synchronized on NAK
            else InEndpoint[num].startRequired = true;
          }
#endif
          cbEndpointEvent(num, ARM_USBD_EP_EVENT_IN);
        }
      }
      num++;
    } while (msk >> num);
  }

// End of periodic frame
  if (gintsts & OTG_HS_GINTSTS_EOPF) {
    for (num = 1; num <= USBD_EP_NUM; num++) {

      if (OTG_EP_OUT_TYPE(num) != ARM_USB_ENDPOINT_ISOCHRONOUS) continue;
      if ((OTG_DOEPCTL(num) & OTG_HS_DOEPCTLx_USBAEP) == 0) continue;

      // Incomplete Isochronous out transfer
      if (OTG->GINTSTS & OTG_HS_GINTSTS_IPXFR) {
        if ((USBD_HW_GetFrameNumber() & 1) == ((OTG_DOEPCTL(num) >> OTG_HS_DOEPCTLx_EONUM_POS) & 1)) {
          if (OTG_DOEPCTL(num) & OTG_HS_DOEPCTLx_EPENA) {
            OTG_DOEPCTL(num) |= OTG_HS_DOEPCTLx_EPDIS;
          }
        }

      // Isochronous out transfer completed
      } else {
        cbEndpointEvent(num, ARM_USBD_EP_EVENT_OUT);
      }
    }
    OTG->GINTSTS = OTG_HS_GINTSTS_EOPF | OTG_HS_GINTSTS_IPXFR;
  }

// Incomplete isochronous IN transfer
  if (gintsts & OTG_HS_GINTSTS_IISOIXFR) {
    OTG->GINTSTS = OTG_HS_GINTSTS_IISOIXFR;
    for (num = 1; num <= USBD_EP_NUM; num++) {

      if (OTG_EP_IN_TYPE(num) != ARM_USB_ENDPOINT_ISOCHRONOUS)     continue;
      if ((OTG_DIEPCTL(num)   &  OTG_HS_DIEPCTLx_USBAEP) == 0) continue;

      if (OTG_DIEPCTL(num) & OTG_HS_DIEPCTLx_EPENA) {
        if ((USBD_HW_GetFrameNumber() & 1) == ((OTG_DIEPCTL(num) >> OTG_HS_DIEPCTLx_EONUM_POS) & 1)) {

          IsoInIncomplete |= (1 << num);
          OTG_DIEPCTL(num)    |= OTG_HS_DIEPCTLx_EPDIS | OTG_HS_DIEPCTLx_SNAK;
        }
      }
    }
  }
}

ARM_DRIVER_USBD Driver_USBD1 = {
  USBD_HW_GetVersion,
  USBD_HW_GetCapabilities,
  USBD_HW_Initialize,
  USBD_HW_Uninitialize,
  USBD_HW_PowerControl,
  USBD_HW_DeviceConnect,
  USBD_HW_DeviceDisconnect,
  USBD_HW_DeviceGetState,
  USBD_HW_DeviceRemoteWakeup,
  USBD_HW_DeviceSetAddress,
  USBD_HW_DeviceConfigure,
  USBD_HW_EndpointConfigure,
  USBD_HW_EndpointUnconfigure,
  USBD_HW_EndpointStall,
  USBD_HW_EndpointReadStart,
  USBD_HW_EndpointRead,
  USBD_HW_EndpointWrite,
  USBD_HW_EndpointAbort,
  USBD_HW_GetFrameNumber
};
