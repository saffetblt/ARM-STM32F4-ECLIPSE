/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        12. September 2013
 * $Revision:    V1.03
 *  
 * Driver:       Driver_USBH1
 * Configured:   via RTE_Device.h configuration file 
 * Project:      USB High-Speed Host Driver for ST STM32F4xx
 * ---------------------------------------------------------------------- 
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 * 
 *   Configuration Setting                Value
 *   ---------------------                -----
 *   Connect to hardware via Driver_USBH# = 1
 *   USB Host controller interface        = Custom
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

#include "RTE_Device.h"
#include "RTE_Components.h"

extern uint8_t otg_hs_role;

extern bool OTG_HS_PinsConfigure   (uint8_t pins_mask);
extern bool OTG_HS_PinsUnconfigure (uint8_t pins_mask);
extern bool OTG_HS_PinVbusOnOff    (bool state);

#define OTG                         OTG_HS

static bool port_reset;

/* USBH Driver ****************************************************************/

#define ARM_USBH_DRV_VERSION ARM_DRV_VERSION_MAJOR_MINOR(1,03) /* USBH driver version */

/* Driver Version */
static const ARM_DRV_VERSION usbh_driver_version = { ARM_USBH_API_VERSION, ARM_USBH_DRV_VERSION };

/* Driver Capabilities */
static const ARM_USBH_CAPABILITIES usbh_driver_capabilities = {
  0x0001, /* available Root HUB ports mask */
  true,   /* connect event                 */
  true,   /* disconnect event              */
  false,  /* remote wakeup event           */
  true,   /* overcurrent event             */
  0       /* reserved                      */ 
};


static ARM_USBH_STATUS USBH_HW_PowerControl (ARM_POWER_STATE state);
static ARM_USBH_STATUS USBH_HW_URB_Abort    (ARM_USBH_EP_HANDLE ep_hndl, ARM_USBH_URB *ptr_urb);

#define OTG_MAX_CH                  16

static uint32_t  *OTG_DFIFO[OTG_MAX_CH] = { OTG_HS_DFIFO0,  OTG_HS_DFIFO1,  
                                            OTG_HS_DFIFO2,  OTG_HS_DFIFO3,   
                                            OTG_HS_DFIFO4,  OTG_HS_DFIFO5,   
                                            OTG_HS_DFIFO6,  OTG_HS_DFIFO7,   
                                            OTG_HS_DFIFO8,  OTG_HS_DFIFO9,  
                                            OTG_HS_DFIFO10, OTG_HS_DFIFO11, 
                                            OTG_HS_DFIFO12, OTG_HS_DFIFO13, 
                                            OTG_HS_DFIFO14, OTG_HS_DFIFO15  
                                          };

static ARM_USBH_SignalPortEvent_t       signal_port_event;
static ARM_USBH_SignalEndpointEvent_t   signal_endpoint_event;

static bool           HW_Accessing                = false;
static ARM_USBH_URB  *CHURB          [OTG_MAX_CH] = { 0 };
static uint16_t       cntInterval    [OTG_MAX_CH] = { 0 };
static uint8_t        cntIntervalMax [OTG_MAX_CH] = { 0 };
static ARM_USBH_STATE port_state                  = { 0 };


/* USBH Channel Functions ------------*/

/**
  \fn          uint32_t USBH_HW_CH_GetIndexFromCh (OTG_HS_HC *ptr_ch)
  \brief       Get the Index of Channel from it's Address.
  \param[in]   ptr_ch Pointer to the Channel
  \return      Index of the Channel
*/
static uint32_t USBH_HW_CH_GetIndexFromCh (OTG_HS_HC *ptr_ch) {
  return (ptr_ch - (OTG_HS_HC *)(&(OTG->HCCHAR0)));
}

/**
  \fn          OTG_HS_HC USBH_HW_CH_GetChFromIndex (uint32_t index)
  \brief       Get the Channel Address from it's Index.
  \param[in]   index  Index of the Channel
  \return      Address of the Channel
*/
static OTG_HS_HC *USBH_HW_CH_GetChFromIndex (uint32_t index) {
  return ((OTG_HS_HC *)(&(OTG->HCCHAR0) + index));
}

/**
  \fn          void *USBH_HW_CH_FindFree (void)
  \brief       Find a free Channel.
  \return      Pointer to the first free Channel 
              (0 = no free Channel is available)
*/
static void *USBH_HW_CH_FindFree (void) {
  OTG_HS_HC *ptr_ch;
  uint32_t   i;

  ptr_ch = (OTG_HS_HC *)(&(OTG->HCCHAR0));

  for (i = 0; i < OTG_MAX_CH; i++) {
    if (!(ptr_ch->HCCHAR & 0x3FFFFFFF)) return ptr_ch;
    ptr_ch++;
  }

  return 0;
}

/**
  \fn          bool USBH_HW_CH_Disable (OTG_HS_HC *ptr_ch)
  \brief       Disable the Channel.
  \param[in]   ptr_ch Pointer to the Channel
  \return      true = success, false = fail
*/
static bool USBH_HW_CH_Disable (OTG_HS_HC *ptr_ch) {

  if (!ptr_ch) return false;
  ptr_ch->HCINTMSK  =  0;
  osDelay (10);
  if (ptr_ch->HCCHAR & OTG_HS_HCCHARx_CHENA) {
    ptr_ch->HCINT   =  ~OTG_HS_HCINTx_CHH;
    ptr_ch->HCCHAR  =  (ptr_ch->HCCHAR  |  OTG_HS_HCCHARx_CHENA);
    osDelay (10);
    ptr_ch->HCCHAR  =  (ptr_ch->HCCHAR  & ~OTG_HS_HCCHARx_CHENA) | OTG_HS_HCCHARx_CHDIS;
    if (ptr_ch->HCINT & OTG_HS_HCINTx_CHH) return true;
    if ((ptr_ch->HCCHAR & (OTG_HS_HCCHARx_CHENA | OTG_HS_HCCHARx_CHDIS)) == (OTG_HS_HCCHARx_CHENA | OTG_HS_HCCHARx_CHDIS)) return true;
    osDelay (50);
    if (ptr_ch->HCINT & OTG_HS_HCINTx_CHH) return true;
    if ((ptr_ch->HCCHAR & (OTG_HS_HCCHARx_CHENA | OTG_HS_HCCHARx_CHDIS)) == (OTG_HS_HCCHARx_CHENA | OTG_HS_HCCHARx_CHDIS)) return true;
    osDelay (50);
    if (ptr_ch->HCINT & OTG_HS_HCINTx_CHH) return true;
    if ((ptr_ch->HCCHAR & (OTG_HS_HCCHARx_CHENA | OTG_HS_HCCHARx_CHDIS)) == (OTG_HS_HCCHARx_CHENA | OTG_HS_HCCHARx_CHDIS)) return true;
    return false;
  }

  return true;
}

/**
  \fn          bool USBH_HW_CH_TransferEnqueue (OTG_HS_HC *ptr_ch, 
                                                uint32_t   tgl_typ, 
                                                uint8_t   *buf, 
                                                uint32_t   len)
  \brief       Enqueue the Transfer.
  \param[in]   ptr_ch             Pointer to the Channel
  \param[in]   tgl_typ:           Toggle      (bit 5..4: bit 5 - force toggle, bit 4 - value) and 
                                  Packet type (bit 3..0: USB_PID_IN, USB_PID_OUT, USB_PID_PING or USB_PID_SETUP)
  \param[in]   buf:               Start of the buffer for receive or transmit
  \param[in]   len:               Length of the data to be received or sent
  \return      true = success, false = fail
*/
static bool USBH_HW_CH_TransferEnqueue (OTG_HS_HC *ptr_ch, uint32_t tgl_typ, uint8_t *buf, uint32_t len) {
  uint32_t  hcchar;
  uint32_t  hctsiz;
  uint32_t  hcintmsk;
  uint32_t  mpsiz;
  uint32_t  ch_idx;
  uint32_t *ptr_src;
  uint32_t *ptr_dest;
  uint32_t  i;
  bool      load_data;

  if (!ptr_ch)
    return false;

  if (!(OTG->HPRT & OTG_HS_HPRT_PCSTS)) return false;

  hcchar   = ptr_ch->HCCHAR;                      /* Read channel characterist*/
  hctsiz   = ptr_ch->HCTSIZ;                      /* Read channel size info   */
  hcintmsk = 0;

  /* Prepare transfer                                                         */
                                                  /* Prepare HCCHAR register  */
  hcchar &=        OTG_HS_HCCHARx_ODDFRM   |      /* Keep ODDFRM              */
                   OTG_HS_HCCHARx_DAD_MSK  |      /* Keep DAD                 */
                   OTG_HS_HCCHARx_MC_MSK   |      /* Keep MC                  */
                   OTG_HS_HCCHARx_EPTYP_MSK|      /* Keep EPTYP               */
                   OTG_HS_HCCHARx_LSDEV    |      /* Keep LSDEV               */
                   OTG_HS_HCCHARx_EPNUM_MSK|      /* Keep EPNUM               */
                   OTG_HS_HCCHARx_MPSIZ_MSK;      /* Keep MPSIZ               */
  switch (tgl_typ & 0x0F) {
    case ARM_USB_PID_IN:
      hcchar   |=  OTG_HS_HCCHARx_EPDIR;
      hcintmsk  =  OTG_HS_HCINTMSKx_DTERRM | 
                   OTG_HS_HCINTMSKx_BBERRM | 
                   OTG_HS_HCINTMSKx_TXERRM | 
                   OTG_HS_HCINTMSKx_ACKM   | 
                   OTG_HS_HCINTMSKx_NAKM   | 
                   OTG_HS_HCINTMSKx_STALLM | 
                   OTG_HS_HCINTMSKx_XFRCM  ;
      break;
    case ARM_USB_PID_OUT:
      hcchar   &= ~OTG_HS_HCCHARx_EPDIR;
      hcintmsk  =  OTG_HS_HCINTMSKx_TXERRM | 
                   OTG_HS_HCINTMSKx_NYET   | 
                   OTG_HS_HCINTMSKx_NAKM   | 
                   OTG_HS_HCINTMSKx_STALLM | 
                   OTG_HS_HCINTMSKx_XFRCM  ;
      break;
    case ARM_USB_PID_SETUP:
      hcchar   &= ~OTG_HS_HCCHARx_EPDIR;
      hcintmsk  =  OTG_HS_HCINTMSKx_TXERRM | 
                   OTG_HS_HCINTMSKx_NAKM   | 
                   OTG_HS_HCINTMSKx_STALLM | 
                   OTG_HS_HCINTMSKx_XFRCM  ;
      break;
    case ARM_USB_PID_PING:
      hcchar   &= ~OTG_HS_HCCHARx_EPDIR;
      hcintmsk  =  OTG_HS_HCINTMSKx_TXERRM | 
                   OTG_HS_HCINTMSKx_ACKM   | 
                   OTG_HS_HCINTMSKx_NAKM   | 
                   OTG_HS_HCINTMSKx_STALLM | 
                   OTG_HS_HCINTMSKx_XFRCM  ;
      break;
  }
  hcchar       &= ~OTG_HS_HCCHARx_CHDIS;
  hcchar       |=  OTG_HS_HCCHARx_CHENA;

                                                  /* Prepare HCTSIZ register  */
  hctsiz       &=  OTG_HS_HCTSIZx_DPID_MSK;       /* Keep DPID                */
  if ((tgl_typ & 0x0F) == ARM_USB_PID_SETUP) {    /* If setup pckt DPID=MDATA */
    hctsiz     &= ~OTG_HS_HCTSIZx_DPID_MSK;
    hctsiz     |=  OTG_HS_HCTSIZx_DPID_MDATA;
  } else if ((tgl_typ >> 5) & 1) {                /* If toggle force bit activ*/
    if ((tgl_typ >> 4) & 1) {                     /* Toggle bit value         */
      hctsiz   &= ~OTG_HS_HCTSIZx_DPID_MSK;
      hctsiz   |=  OTG_HS_HCTSIZx_DPID_DATA1;
    } else {
      hctsiz   &= ~OTG_HS_HCTSIZx_DPID_MSK;
      hctsiz   |=  OTG_HS_HCTSIZx_DPID_DATA0;
    }
  }
  if ((tgl_typ & 0x0F) == ARM_USB_PID_PING) {     /* If OUT pckt DOPING       */
    hctsiz     |=  OTG_HS_HCISIZx_DOPING;
  }

  mpsiz = hcchar & 0x7FF;                         /* Maximum packet size      */
  if (len) {                                      /* Normal packet            */
    hctsiz |= ((len+mpsiz-1) / mpsiz) << 19;      /* Prepare PKTCNT field     */
    hctsiz |= ( len                 ) <<  0;      /* Prepare XFRSIZ field     */
  } else {                                        /* Zero length packet       */
    hctsiz |= ( 1                   ) << 19;      /* Prepare PKTCNT field     */
    hctsiz |= ( 0                   ) <<  0;      /* Prepare XFRSIZ field     */
  }

  ch_idx  = USBH_HW_CH_GetIndexFromCh (ptr_ch);

  ptr_ch->HCINTMSK = hcintmsk;                    /* Enable channel interrupts*/
  ptr_ch->HCTSIZ   = hctsiz;                      /* Write ch transfer size   */

  /* load_data == true if there is data to be loaded to FIFO 
    (If packet is OUT or SETUP and len > 0)                                   */
  load_data = (((tgl_typ & 0x0F) == ARM_USB_PID_OUT)    || 
               ((tgl_typ & 0x0F) == ARM_USB_PID_SETUP)) && 
                 len; 

  ptr_ch->HCCHAR = hcchar;                        /* Write ch characteristics */

  if (load_data) {
    ptr_src  = (uint32_t *)(buf);
    ptr_dest = OTG_DFIFO[ch_idx];
    for (i = 0; i < (len + 3)/4; i++)
      *ptr_dest = *ptr_src++;
  }

  return true;
}

/* USBH Controller Functions ---------*/

/**
  \fn          bool USBH_HW_ControllerReset (void)
  \brief       Reset the USB Controller.
  \return      true = success, false = fail
*/
static bool USBH_HW_ControllerReset (void) {
  int32_t tout;

  OTG->GAHBCFG   &= ~OTG_HS_GAHBCFG_GINT;           /* Disable interrupts     */

  /* Wait until AHB Master state machine is in the idle condition             */
  for (tout = 1000; tout >= 0; tout--) {            /* Wait max 1 second      */
    if (OTG->GRSTCTL & OTG_HS_GRSTCTL_AHBIDL) break;
    if (!tout) return false;
    osDelay (1);
  }
  OTG->GRSTCTL |=  OTG_HS_GRSTCTL_CSRST;            /* Core soft reset        */
  for (tout = 1000; tout >= 0; tout--) {            /* Wait max 1 second      */
    if (!(OTG->GRSTCTL & OTG_HS_GRSTCTL_CSRST)) break;
    if (!tout) return false;
    osDelay (1);
  }
  osDelay (3);

  return true;
}

/**
  \fn          bool USBH_HW_ControllerRunSetup (void)
  \brief       Setup USB Controller for running.
  \return      true = success, false = fail
*/
static bool USBH_HW_ControllerRunSetup (void) {

#if (RTE_USB_OTG_HS_PHY)
  /* External ULPI PHY */
  OTG->GUSBCFG   &= ~OTG_HS_GUSBCFG_PHSEL;          /* High-spd trnscvr       */
  OTG->GUSBCFG   |=  OTG_HS_GUSBCFG_PTCI      |     /* Ind. pass through      */
                     OTG_HS_GUSBCFG_PCCI      |     /* Ind. complement        */
                     OTG_HS_GUSBCFG_ULPIEVBUSI|     /* ULPI ext Vbus ind      */
                     OTG_HS_GUSBCFG_ULPIEVBUSD;     /* ULPI ext Vbus drv      */
#else
  /* Embedded PHY */
  OTG->GUSBCFG   |=  OTG_HS_GUSBCFG_PHSEL;          /* Full-spd trnscvr       */
  OTG->GCCFG     &= ~OTG_HS_GCCFG_VBUSBSEN;         /* Disable VBUS sens of B */
  OTG->GCCFG     &= ~OTG_HS_GCCFG_VBUSASEN;         /* Disable VBUS sens of A */
  OTG->GCCFG     |=  OTG_HS_GCCFG_NOVBUSSENS;       /* No VBUS sensing        */
#endif

  OTG->GCCFG     |=  OTG_HS_GCCFG_PWRDWN  ;         /* Disable power down     */

  osDelay (20);

#if (!RTE_USB_OTG_HS_OTG)
  if (!(OTG->GUSBCFG & OTG_HS_GUSBCFG_FHMOD)) {
    OTG->GUSBCFG |=  OTG_HS_GUSBCFG_FHMOD;          /* Force host mode        */
    osDelay (50);
  }

  otg_hs_role = ARM_USB_ROLE_HOST;
#endif

  /* Core initialization                                                      */
  OTG->GRXFSIZ    = (512/4) +                       /* RxFIFO depth is 512 by */
                         2  +                       /* 8 bytes for Int EP     */
                         4  ;                       /* Packet info and status */
  OTG->GNPTXFSIZ  = ((512/4)<<16) | ((512/4)+6);    /* Non-peri TxFIFO        */
  OTG->HPTXFSIZ   = (16<<16) |  (((512/4)*2)+6);    /* Peri TxFIFO mem        */

  OTG->GINTMSK   |=  OTG_HS_GINTMSK_DISCINT|        /* En disconn int         */
                     OTG_HS_GINTMSK_HCIM   |        /* En host ch int         */
                     OTG_HS_GINTMSK_PRTIM  |        /* En host prt int        */
                     OTG_HS_GINTMSK_RXFLVLM|        /* Enable RXFIFO int      */
                     OTG_HS_GINTMSK_SOFM   ;        /* Enable SOF int         */
  OTG->HAINTMSK   =  0xFFFF;                        /* En all ch ints         */
  OTG->GINTSTS    =  0xFFFFFFFF;                    /* Clear interrupts       */

  return true;
}

/* USBH Module Functions -------------*/

/**
  \fn          ARM_DRV_VERSION USBH_HW_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRV_VERSION USBH_HW_GetVersion (void) { return usbh_driver_version; }

/**
  \fn          ARM_USBH_CAPABILITIES USBH_HW_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_USBH_CAPABILITIES
*/
static ARM_USBH_CAPABILITIES USBH_HW_GetCapabilities (void) { return usbh_driver_capabilities; }

/**
  \fn          ARM_USBH_STATUS USBH_HW_Initialize (ARM_USBH_SignalPortEvent_t     cb_port_event,
                                                   ARM_USBH_SignalEndpointEvent_t cb_endpoint_event)
  \brief       Initialize USB Host Interface.
  \param[in]   cb_port_event      Pointer to \ref ARM_USBH_SignalPortEvent
  \param[in]   cb_endpoint_event  Pointer to \ref ARM_USBH_SignalEndpointEvent
  \return      \ref ARM_USBH_STATUS
*/
static ARM_USBH_STATUS USBH_HW_Initialize (ARM_USBH_SignalPortEvent_t cb_port_event, ARM_USBH_SignalEndpointEvent_t cb_endpoint_event) {
  uint8_t i;

  signal_port_event     = cb_port_event;
  signal_endpoint_event = cb_endpoint_event;

  HW_Accessing          = false;
  for (i = 0; i < OTG_MAX_CH; i++) {
    CHURB[i] = 0;
    cntInterval[i] = 0;
    cntIntervalMax[i] = 0;
  }
  port_state.active      = 0;
  port_state.connected   = 0;
  port_state.overcurrent = 0;
  port_state.speed       = 0;
  port_reset             = false;

#if (!RTE_USB_OTG_HS_OTG)
  if (OTG_HS_PinsConfigure (ARM_USB_PIN_DP | ARM_USB_PIN_DM | ARM_USB_PIN_OC | ARM_USB_PIN_VBUS) == false) return ARM_USBH_ERROR;

  RCC->AHB1ENR  |=  RCC_AHB1ENR_OTGHSEN;        /* OTG HS clock enable        */
  RCC->AHB1RSTR |=  RCC_AHB1ENR_OTGHSEN;        /* Reset OTG HS clock         */
  osDelay(1);                                   /* Wait 1 ms                  */
  RCC->AHB1RSTR &= ~RCC_AHB1ENR_OTGHSEN;

#if (RTE_USB_OTG_HS_PHY)
  /* External ULPI PHY */
  RCC->AHB1ENR  |=  RCC_AHB1ENR_OTGHSULPIEN;    /* OTG HS ULPI clock enable   */
#endif

  NVIC_SetPriority  (OTG_HS_IRQn, 0);           /* OTG int highest priority   */
#endif

  return ARM_USBH_OK;
}

/**
  \fn          ARM_USBH_STATUS USBH_HW_Uninitialize (void)
  \brief       De-initialize USB Host Interface.
  \return      \ref ARM_USBH_STATUS
*/
static ARM_USBH_STATUS USBH_HW_Uninitialize (void) {

#if (!RTE_USB_OTG_HS_OTG)
  USBH_HW_PowerControl (ARM_POWER_OFF);

#if (RTE_USB_OTG_HS_PHY)
  RCC->AHB1ENR  &= ~RCC_AHB1ENR_OTGHSULPIEN;    /* OTG HS ULPI clock disable  */
#endif

  RCC->AHB1RSTR |=  RCC_AHB1ENR_OTGHSEN;        /* Reset OTG HS clock         */
  osDelay(1);                                   /* Wait 1 ms                  */
  RCC->AHB1RSTR &= ~RCC_AHB1ENR_OTGHSEN;
  RCC->AHB1ENR  &= ~RCC_AHB1ENR_OTGHSEN;        /* OTG HS clock disable       */

  if (OTG_HS_PinsUnconfigure (ARM_USB_PIN_DP | ARM_USB_PIN_DM | ARM_USB_PIN_OC | ARM_USB_PIN_VBUS) == false) return ARM_USBH_ERROR;
#endif

  return ARM_USBH_OK;
}

/**
  \fn          ARM_USBH_STATUS USBH_HW_PowerControl (ARM_POWER_STATE state)
  \brief       Control USB Host Interface Power.
  \param[in]   state Power state
  \return      \ref ARM_USBH_STATUS
*/
static ARM_USBH_STATUS USBH_HW_PowerControl (ARM_POWER_STATE state) { 

  switch (state) {
    case ARM_POWER_OFF:
      NVIC_DisableIRQ  (OTG_HS_IRQn);               /* Disable OTG interrupt  */
      OTG->GAHBCFG   &= ~OTG_HS_GAHBCFG_GINT;       /* Disable interrupts     */
      OTG->GCCFG      = 0;                          /* Power down activated   */

      otg_hs_role = ARM_USB_ROLE_NONE;

      if (!USBH_HW_ControllerReset())    return ARM_USBH_ERROR;
      break;

    case ARM_POWER_LOW:
      return ARM_USBH_ERROR;

    case ARM_POWER_FULL:
      if (!USBH_HW_ControllerReset())    return ARM_USBH_ERROR;
      if (!USBH_HW_ControllerRunSetup()) return ARM_USBH_ERROR;
      NVIC_EnableIRQ    (OTG_HS_IRQn);              /* Enable OTG interrupt   */
      OTG->GAHBCFG   |=  OTG_HS_GAHBCFG_GINT;       /* Enable interrupts      */
      break;
  }

  return ARM_USBH_OK; 
}

/**
  \fn          ARM_USBH_STATUS USBH_HW_PortPowerOnOff (uint8_t port, bool power)
  \brief       Root HUB Port Power on/off.
  \param[in]   port  Root HUB Port Number
  \param[in]   power Operation
                - \b false Power off
                - \b true  Power on
  \return      \ref ARM_USBH_STATUS
*/
static ARM_USBH_STATUS USBH_HW_PortPowerOnOff (uint8_t port, bool power) {

  if (power) {
    OTG->GAHBCFG &= ~OTG_HS_GAHBCFG_GINT;           /* Disable interrupts     */
    OTG->HPRT    |=  OTG_HS_HPRT_PPWR;              /* Port power on          */
    if (OTG_HS_PinVbusOnOff (true ) == false) return ARM_USBH_ERROR;
    osDelay(200);                                   /* Allow VBUS to stabilize*/
    OTG->HAINT    =  0xFFFF;                        /* Clear port interrupts  */
    OTG->GINTSTS  =  0xFFFFFFFF;                    /* Clear interrupts       */
    OTG->GAHBCFG |=  OTG_HS_GAHBCFG_GINT;           /* Disable interrupts     */
  } else {
    if (OTG_HS_PinVbusOnOff (false) == false) return ARM_USBH_ERROR;
    OTG->HPRT    &= ~OTG_HS_HPRT_PPWR;              /* Port power off         */
  }

  return ARM_USBH_OK;
}

/**
  \fn          ARM_USBH_STATUS USBH_HW_PortReset (uint8_t port)
  \brief       Do Root HUB Port Reset.
  \param[in]   port Root HUB Port Number
  \return      \ref ARM_USBH_STATUS
*/
static ARM_USBH_STATUS USBH_HW_PortReset (uint8_t port) {
  uint32_t hprt;
  int32_t  tout;

#if (!RTE_USB_OTG_HS_PHY)
  uint32_t hcfg;

  if (!(OTG->HPRT & OTG_HS_HPRT_PCSTS)) return ARM_USBH_ERROR;

  hcfg = OTG->HCFG;
  hprt = OTG->HPRT;
  switch ((hprt >> 17) & 3) {
    case 0:                             /* High-speed detected                */
    case 1:                             /* Full-speed detected                */
      if (OTG->HFIR != 48000) OTG->HFIR = 48000;
      if ((hcfg & 3) != 1) {
        OTG->HCFG = (hcfg & ~OTG_HS_HCFG_FSLSPCS(3)) | OTG_HS_HCFG_FSLSPCS(1);
      }
      break;
    case 2:                             /* Low-speed detected                 */
      if (OTG->HFIR != 6000) OTG->HFIR = 6000;
      if ((hcfg & 3) != 2) {
        OTG->HCFG = (hcfg & ~OTG_HS_HCFG_FSLSPCS(3)) | OTG_HS_HCFG_FSLSPCS(2);
      }
      break;
    case 3:
      break;
  }
#endif

  if (!(OTG->HPRT & OTG_HS_HPRT_PCSTS)) return ARM_USBH_ERROR;

  port_reset = true;
  hprt  = OTG->HPRT;
  hprt &= ~OTG_HS_HPRT_PENA;            /* Disable port                       */
  OTG->HPRT = hprt;
  osDelay (10);
  hprt |=  OTG_HS_HPRT_PRST;            /* Port reset                         */
  OTG->HPRT = hprt;
  osDelay (50);
  hprt &= ~OTG_HS_HPRT_PRST;            /* Clear port reset                   */
  OTG->HPRT = hprt;
  osDelay (20);

  /* Wait for port to become enabled, for max 1 s                             */
  for (tout = 1000; tout >= 0; tout--) {
    if (!port_reset) break;
    if (!tout) {
      port_reset = false;
      return ARM_USBH_ERROR;
    }
    osDelay (1);
  }

  port_reset = false;
  return ARM_USBH_OK;
}

/**
  \fn          ARM_USBH_STATUS USBH_HW_PortSuspend (uint8_t port)
  \brief       Suspend Root HUB Port (stop generating SOFs).
  \param[in]   port Root HUB Port Number
  \return      \ref ARM_USBH_STATUS
*/
static ARM_USBH_STATUS USBH_HW_PortSuspend (uint8_t port) { 

  OTG->HPRT |=  OTG_HS_HPRT_PSUSP;      /* Port suspend                       */

  return ARM_USBH_OK;
}

/**
  \fn          ARM_USBH_STATUS USBH_HW_PortResume (uint8_t port)
  \brief       Resume Root HUB Port (start generating SOFs).
  \param[in]   port Root HUB Port Number
  \return      \ref ARM_USBH_STATUS
*/
static ARM_USBH_STATUS USBH_HW_PortResume (uint8_t port) { 

  OTG->HPRT |=  OTG_HS_HPRT_PRES;       /* Port resume                        */

  return ARM_USBH_OK;
}

/**
  \fn          ARM_USBH_STATE USBH_HW_PortGetState (uint8_t port)
  \brief       Get current Root HUB Port connection State.
  \param[in]   port Root HUB Port Number
  \return      \ref ARM_USBH_STATE
*/
static ARM_USBH_STATE  USBH_HW_PortGetState (uint8_t port) { 
  uint32_t hprt;

  hprt = OTG->HPRT;

  port_state.active    = (hprt & OTG_HS_HPRT_PENA ) != 0;
  port_state.connected = (hprt & OTG_HS_HPRT_PCSTS) != 0;
  switch ((hprt & OTG_HS_HPRT_PSPD_MSK) >> OTG_HS_HPRT_PSPD_POS) {
    case 0:                             /* High speed                         */
     port_state.speed = ARM_USB_SPEED_HIGH;
     break;
    case 1:                             /* Full speed                         */
     port_state.speed = ARM_USB_SPEED_FULL;
     break;
    case 2:                             /* Low speed                          */
     port_state.speed = ARM_USB_SPEED_LOW;
     break;
    default:
     break;
  }

  return port_state; 
}

/**
  \fn          ARM_USBH_EP_HANDLE USBH_HW_EndpointCreate (uint8_t ep_addr,
                                                          ARM_USB_ENDPOINT_TYPE ep_type,
                                                          uint16_t ep_max_packet_size,
                                                          uint8_t  ep_interval)
  \brief       Create Endpoint in System.
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \param[in]   ep_type  Endpoint Type
  \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
  \param[in]   ep_interval        Endpoint Polling Interval
  \return      \ref ARM_USBH_EP_HANDLE
*/
static ARM_USBH_EP_HANDLE USBH_HW_EndpointCreate (uint8_t ep_addr, ARM_USB_ENDPOINT_TYPE ep_type, uint16_t ep_max_packet_size, uint8_t ep_interval) {
  OTG_HS_HC *ptr_ch;

  ptr_ch = (OTG_HS_HC *)(USBH_HW_CH_FindFree ());             /* Find free Ch */
  if (!ptr_ch) return 0;                                      /* If no free   */

  /* Fill in all fields from the USB Standard Endpoint Descriptor             */
  ptr_ch->HCCHAR = (((ep_max_packet_size           ) & 0x07FF)       <<  0) | 
                   (((ep_addr                      ) & 0x000F)       << 11) | 
                   (((ep_addr >> 7                 ) & 0x0001)       << 15) | 
                   (((uint8_t)ep_type              )                 << 18) ; 
  switch (ep_type) {
    case ARM_USB_ENDPOINT_CONTROL:
    case ARM_USB_ENDPOINT_BULK:
      break;
    case ARM_USB_ENDPOINT_ISOCHRONOUS:
    case ARM_USB_ENDPOINT_INTERRUPT:
      cntIntervalMax[USBH_HW_CH_GetIndexFromCh (ptr_ch)] = ep_interval;
      ptr_ch->HCCHAR |= OTG_HS_HCCHARx_MC(1);
      break;
  }

  return ((ARM_USBH_EP_HANDLE)ptr_ch);
}

/**
  \fn          ARM_USBH_STATUS USBH_HW_EndpointConfigure (ARM_USBH_EP_HANDLE ep_hndl,
                                                          uint8_t dev_addr,
                                                          uint8_t ep_speed,
                                                          uint8_t ep_addr,
                                                          ARM_USB_ENDPOINT_TYPE ep_type,
                                                          uint16_t ep_max_packet_size,
                                                          uint8_t  ep_interval)
  \brief       Configure/reconfigure some properties of Endpoint.
  \param[in]   ep_hndl  Endpoint Handle
  \param[in]   dev_addr Device Address
  \param[in]   ep_speed Endpoint Speed
  \param[in]   ep_addr  Endpoint Address
                - ep_addr.0..3: Address
                - ep_addr.7:    Direction
  \param[in]   ep_type  Endpoint Type
  \param[in]   ep_max_packet_size Endpoint Maximum Packet Size
  \param[in]   ep_interval        Endpoint Polling Interval
  \return      \ref ARM_USBH_STATUS
*/
static ARM_USBH_STATUS USBH_HW_EndpointConfigure (ARM_USBH_EP_HANDLE ep_hndl, uint8_t dev_addr, uint8_t ep_speed, uint8_t ep_addr, ARM_USB_ENDPOINT_TYPE ep_type, uint16_t ep_max_packet_size, uint8_t  ep_interval) { 
  OTG_HS_HC *ptr_ch;

  if (!ep_hndl)                     return ARM_USBH_ERROR;

  ptr_ch = (OTG_HS_HC *)(ep_hndl);
  if (!USBH_HW_CH_Disable (ptr_ch)) return ARM_USBH_ERROR;

  /* Fill in all fields of Endpoint Descriptor                                */
  ptr_ch->HCCHAR = (((ep_max_packet_size           ) & 0x07FF)       <<  0) | 
                   (((ep_addr                      ) & 0x000F)       << 11) | 
                   (((ep_addr >> 7                 ) & 0x0001)       << 15) | 
                   ((ep_speed == ARM_USB_SPEED_LOW )                 << 17) |
                   (((uint8_t)ep_type              )                 << 18) | 
                   ((dev_addr                        & 0x007F)       << 22) ;

  switch (ep_type) {
    case ARM_USB_ENDPOINT_CONTROL:
    case ARM_USB_ENDPOINT_BULK:
      break;
    case ARM_USB_ENDPOINT_ISOCHRONOUS:
    case ARM_USB_ENDPOINT_INTERRUPT:
      ptr_ch->HCCHAR |= OTG_HS_HCCHARx_MC(1);
      break;
  }

  return ARM_USBH_OK;
}

/**
  \fn          ARM_USBH_STATUS USBH_HW_EndpointDelete (ARM_USBH_EP_HANDLE ep_hndl)
  \brief       Delete Endpoint from System.
  \param[in]   ep_hndl  Endpoint Handle
  \return      \ref ARM_USBH_STATUS
*/
static ARM_USBH_STATUS USBH_HW_EndpointDelete (ARM_USBH_EP_HANDLE ep_hndl) {
  OTG_HS_HC    *ptr_ch;
  ARM_USBH_URB *ptr_urb;
  uint32_t      ch_idx;

  if (!ep_hndl) 
    return ARM_USBH_ERROR;

  ptr_ch  = (OTG_HS_HC *)(ep_hndl);
  ch_idx  = USBH_HW_CH_GetIndexFromCh (ptr_ch);
  ptr_urb = CHURB[ch_idx];              /* Pointer to channels URB            */
  if (ptr_urb) {                        /* If URB exists cancel it            */
    if (USBH_HW_URB_Abort (ep_hndl, ptr_urb) != ARM_USBH_OK) return ARM_USBH_ERROR;
  }

  ptr_ch->HCCHAR      = 0;
  ptr_ch->HCINT       = 0;
  ptr_ch->HCINTMSK    = 0;
  ptr_ch->HCTSIZ      = 0;

  cntInterval[ch_idx] = 0;

  return ARM_USBH_OK;
}

/**
  \fn          ARM_USBH_STATUS USBH_HW_EndpointClearHalt (ARM_USBH_EP_HANDLE ep_hndl)
  \brief       Clear Halt condition on USB Endpoint.
  \param[in]   ep_hndl  Endpoint Handle
  \return      \ref ARM_USBH_STATUS
*/
static ARM_USBH_STATUS USBH_HW_EndpointClearHalt (ARM_USBH_EP_HANDLE ep_hndl) { return ARM_USBH_OK; }

/**
  \fn          ARM_USBH_STATUS USBH_HW_URB_Submit (ARM_USBH_EP_HANDLE ep_hndl, 
                                                   ARM_USBH_URB *ptr_urb)
  \brief       Submit USB Request Block (URB) for Processing.
  \param[in]   ep_hndl  Endpoint Handle
  \param[in]   ptr_urb  Pointer to USB Request Block (URB)
  \return      \ref ARM_USBH_STATUS
*/
static ARM_USBH_STATUS USBH_HW_URB_Submit (ARM_USBH_EP_HANDLE ep_hndl, ARM_USBH_URB *ptr_urb) {
  OTG_HS_HC *ptr_ch;
  uint32_t   ch_idx;
  uint8_t    resp_type;
  uint16_t   max_cnt;

  if (!ep_hndl)                         return (ARM_USBH_ERROR);
  if (!ptr_urb)                         return (ARM_USBH_ERROR);
  if (!(OTG->HPRT & OTG_HS_HPRT_PCSTS)) return (ARM_USBH_ERROR);
  if ((ptr_urb->submitted == 1) || (ptr_urb->in_progress == 1)) return (ARM_USBH_ERROR);

  resp_type          = ptr_urb->response_packet_type;

  ptr_urb->buf_cur   = ptr_urb->buf;
  ptr_urb->len_cur   = 0;
  ptr_urb->status    = 0;

  ptr_ch             = (OTG_HS_HC *)(ep_hndl);
  ch_idx             = USBH_HW_CH_GetIndexFromCh (ptr_ch);
  CHURB[ch_idx]      = ptr_urb;

  if (ptr_ch->EPTYP == ARM_USB_ENDPOINT_INTERRUPT) {
                                        /* If interrupt endpoint transfer     */
    if (resp_type == ARM_USB_PID_NAK) {
      if (port_state.speed == ARM_USB_SPEED_HIGH) {
        max_cnt = (cntIntervalMax[ch_idx] & 0x0F);
        if (!max_cnt)
          max_cnt = 1;
        max_cnt = 1 << max_cnt;
      } else if ((port_state.speed == ARM_USB_SPEED_FULL) || (port_state.speed == ARM_USB_SPEED_LOW)) {
        max_cnt = (cntIntervalMax[ch_idx] & 0xFF);
        if (!max_cnt)
          max_cnt = 1;
      }
      cntInterval[ch_idx] = max_cnt;
    } else {
      cntInterval[ch_idx] = 1;          /* Enable transmission on next SOF    */
    }
    ptr_urb->submitted    = 1;
  } else if ((ptr_ch->EPTYP == ARM_USB_ENDPOINT_CONTROL) || (ptr_ch->EPTYP == ARM_USB_ENDPOINT_BULK)) {
    HW_Accessing        = true;
    ptr_urb->submitted  = 1;
    ptr_urb->in_progress= 1;
    USBH_HW_CH_TransferEnqueue (ptr_ch, (uint8_t)ptr_urb->parameters, (uint8_t *)ptr_urb->buf, (uint32_t)ptr_urb->len);
    HW_Accessing        = false;
  }

  return ARM_USBH_OK;
}

/**
  \fn          ARM_USBH_STATUS USBH_HW_URB_Abort (ARM_USBH_EP_HANDLE ep_hndl, 
                                                  ARM_USBH_URB *ptr_urb)
  \brief       Abort Processing of USB Request Block (URB).
  \param[in]   ep_hndl  Endpoint Handle
  \param[in]   ptr_urb  Pointer to USB Request Block (URB)
  \return      \ref ARM_USBH_STATUS
*/
static ARM_USBH_STATUS USBH_HW_URB_Abort (ARM_USBH_EP_HANDLE ep_hndl, ARM_USBH_URB *ptr_urb) {
  OTG_HS_HC *ptr_ch;
  uint32_t   ch_idx;

  if (!ep_hndl)            return ARM_USBH_ERROR;
  if (!ptr_urb)            return ARM_USBH_ERROR;
  if (!ptr_urb->submitted) return ARM_USBH_OK;

  ptr_urb->submitted = 0;

  ptr_ch = (OTG_HS_HC *)(ep_hndl);
  if (ptr_urb->in_progress == 1) {
    if (!USBH_HW_CH_Disable (ptr_ch))   /* Stop Endpoint processing           */
      return ARM_USBH_ERROR;
    ptr_urb->in_progress = 0;
  }

  ch_idx = USBH_HW_CH_GetIndexFromCh (ptr_ch);

  if (!cntInterval[ch_idx]) {           /* If interrupt endpoint transfer     */
    cntInterval[ch_idx] = 0;            /* Disable transmission on next SOF   */
  }
  CHURB[ch_idx] = 0;

  ptr_urb->cancelled = 1;

  return ARM_USBH_OK;
}

/**
  \fn          void USBH_HS_IRQ (uint32_t gintsts)
  \brief       USB Host Interrupt Routine (IRQ).
*/
void USBH_HS_IRQ (uint32_t gintsts) {
  OTG_HS_HC    *ptr_ch;
  ARM_USBH_URB *ptr_urb;
  uint8_t      *ptr_data_8;
  uint32_t     *ptr_data_32;
  uint32_t     *dfifo;
  uint32_t      hprt, haint, hcint, pktcnt, mpsiz;
  uint32_t      grxsts, bcnt, ch, dat, len, len_rest;
  uint32_t      act;

  hprt  = OTG->HPRT;
  haint = OTG->HAINT;

  if (gintsts & OTG_HS_GINTSTS_HPRTINT) {         /* If host port interrupt   */
    if (hprt  & OTG_HS_HPRT_PCDET) {              /* Port connect detected    */
      if (!port_reset) {                          /* If port not under reset  */
        port_state.connected = hprt & OTG_HS_HPRT_PCSTS;
        signal_port_event(0, ARM_USBH_EVENT_CONNECT);
      }
    }
    if (hprt & OTG_HS_HPRT_PENCHNG) {             /* If port enable changed   */
      if (hprt & OTG_HS_HPRT_PENA) {              /* If device connected      */
        if (port_reset) port_reset = false;
        port_state.active = 1;
        switch ((hprt >> 17) & 3) {
          case 0:
            port_state.speed |= ARM_USB_SPEED_HIGH;
            break;
          case 1:
            port_state.speed |= ARM_USB_SPEED_FULL;
            break;
          case 2:
            port_state.speed |= ARM_USB_SPEED_LOW;
            break;
          case 3:
            break;
        }
      } 
    }
    hprt &= ~OTG_HS_HPRT_PENA;                    /* Leave PENA bit           */
    OTG->HPRT = hprt;                             /* Clear host port interrupt*/
  }
  if (!port_reset &&                              /* If port not under reset  */
     (gintsts & OTG_HS_GINTSTS_DISCINT)) {        /* If device disconnected   */
    port_state.connected = hprt & OTG_HS_HPRT_PCSTS;
    signal_port_event(0, ARM_USBH_EVENT_DISCONNECT);
  }
                                                  /* Handle reception int     */
  if (gintsts & OTG_HS_GINTSTS_RXFLVL) {          /* If RXFIFO non-empty int  */
    OTG->GINTMSK &= ~OTG_HS_GINTMSK_RXFLVLM;
    grxsts = OTG->GRXSTSR;
    if (((grxsts >> 17) & 0x0F) == 0x02){         /* If PKTSTS = 0x02         */
      grxsts     = (OTG->GRXSTSP);
      ch         = (grxsts >> 0) & 0x00F;
      bcnt       = (grxsts >> 4) & 0x7FF;
      ptr_ch     = USBH_HW_CH_GetChFromIndex (ch);
      dfifo      = OTG_DFIFO[ch];
      ptr_urb    = CHURB[ch];                     /* Pointer to channels URB  */
      ptr_data_32= (uint32_t *)(ptr_urb->buf + ptr_urb->len_cur);
      len        = bcnt / 4;                      /* Received number of bytes */
      len_rest   = bcnt & 3;                      /* Number of bytes left     */
      while (len--) {
        *ptr_data_32++    = *dfifo;
        ptr_urb->len_cur += 4;
      }
      if (len_rest) {
        dat        = *dfifo;
        ptr_data_8 = (uint8_t *)ptr_data_32;
        while (len_rest--) {
          *ptr_data_8++ = dat;
          dat         >>= 8;
          ptr_urb->len_cur ++;
        }
      }
    } else {                                      /* If PKTSTS != 0x02        */
      grxsts     = OTG->GRXSTSP;
    }
    OTG->GINTMSK |= OTG_HS_GINTMSK_RXFLVLM;
  }
                                                  /* Handle host ctrl int     */
  if (gintsts & OTG_HS_GINTSTS_HCINT) {           /* If host channel interrupt*/
    ptr_ch = (OTG_HS_HC *)(&OTG->HCCHAR0);
    for (ch = 0; ch < OTG_MAX_CH; ch++) {
      if (haint & (1 << ch)) {                    /* If channels interrupt act*/
        hcint   = ptr_ch->HCINT & ptr_ch->HCINTMSK;
        ptr_urb = CHURB[ch];                      /* Pointer to channels URB  */
        if (hcint & OTG_HS_HCINTx_XFRC) {         /* If data transfer finished*/
          ptr_ch->HCINTMSK         = 0;
          ptr_urb->error           = 0;
          ptr_urb->len_cur         = ptr_urb->len;
          ptr_urb->status          = 0;
          ptr_urb->response_packet_type = ARM_USB_PID_ACK;
          goto halt_ch;
        } else if (hcint & OTG_HS_HCINTx_STALL) { /* If STALL event           */
          ptr_urb->response_packet_type = ARM_USB_PID_STALL;
          goto halt_ch;
        } else if ((hcint & OTG_HS_HCINTx_NAK)   ||         /* If NAK received*/
                   (hcint & OTG_HS_HCINTx_TXERR) ||         /* If TXERR rece  */
                   (hcint & OTG_HS_HCINTx_BBERR) ||         /* If BBERR rece  */
                   (hcint & OTG_HS_HCINTx_DTERR)) {         /* If DTERR rece  */
          if (hcint & OTG_HS_HCINTx_NAK) {
            ptr_urb->response_packet_type = ARM_USB_PID_NAK;
          } else {
            if (hcint & OTG_HS_HCINTx_TXERR) {
            }
            if (hcint & OTG_HS_HCINTx_BBERR) {
              ptr_urb->error       = 1;
            }
            if (hcint & OTG_HS_HCINTx_DTERR) {
              ptr_urb->error       = 1;
            }
          }
          if (ptr_urb->len) {
                                                  /* Update transfer info     */
            pktcnt                 = (ptr_ch->HCTSIZ >> 19) & 0x3FF;
            mpsiz                  = (ptr_ch->HCCHAR >>  0) & 0x7FF;
            ptr_urb->len_cur       = (((ptr_urb->len + mpsiz - 1) / mpsiz) - pktcnt) * mpsiz;
            switch (ptr_ch->HCTSIZ & OTG_HS_HCTSIZx_DPID_MSK) {
              case OTG_HS_HCTSIZx_DPID_DATA0:
                ptr_urb->toggle_bit= 0;
                break;
              case OTG_HS_HCTSIZx_DPID_DATA1:
                ptr_urb->toggle_bit= 1;
                break;
              case OTG_HS_HCTSIZx_DPID_DATA2:
                break;
              case OTG_HS_HCTSIZx_DPID_MDATA:
                break;
            }
          }
          goto halt_ch;
        } else if (hcint & OTG_HS_HCINTx_CHH) {   /* If channel halted        */
          ptr_ch->HCINTMSK         = 0;
          ptr_urb->submitted       = 0;
          ptr_urb->in_progress     = 0;
          ptr_urb->completed       = 1;
          CHURB[ch]                = 0;
          signal_endpoint_event((ARM_USBH_EP_HANDLE)ptr_ch, ptr_urb);
        } else if (hcint & OTG_HS_HCINTx_ACK) {   /* If ACK received          */
          ptr_urb->response_packet_type = ARM_USB_PID_ACK;
          ptr_urb->error           = 0;
          goto halt_ch;
        } else {
halt_ch:                                          /* Halt the channel         */
          ptr_ch->HCINTMSK         = OTG_HS_HCINTx_CHH;
          ptr_ch->HCCHAR          |= OTG_HS_HCCHARx_CHENA | OTG_HS_HCCHARx_CHDIS;
        }
        ptr_ch->HCINT = 0x7FF;
      }
      ptr_ch++;
    }
    OTG->HAINT = haint;
  }  
  OTG->GINTSTS = gintsts;                         /* Clear core ints          */
  if (gintsts & OTG_HS_GINTSTS_SOF) {             /* If start of frame int    */
    if (HW_Accessing) {
      act = 1;
    } else {
      act = 0;
      for (ch = 0; ch < OTG_MAX_CH; ch++) {
        if (CHURB[ch]) {
          if (CHURB[ch]->in_progress == 1) {      /* If any URB in progress   */
            act = 1;                              /* Set act to 1             */
            break;
          }
        }
      }
    }

    /* At this point act == 1 if there is USB bus activity                    */
    ptr_ch = (OTG_HS_HC *)(&(OTG->HCCHAR0));
    for (ch = 0; ch < OTG_MAX_CH; ch++) {
      ptr_urb = CHURB[ch];                        /* Pointer to channels URB  */
      if (ptr_urb) {
        if ((ptr_urb->submitted  == 1) &&         /* If URB is submitted      */
            (ptr_urb->in_progress == 0)) {        /* If URB not in progress   */
          if (ptr_ch->EPTYP == ARM_USB_ENDPOINT_INTERRUPT) {
            if (cntInterval[ch]) {
              if ((act && (cntInterval[ch] > 1)) || !act)
                cntInterval[ch]--;
              }
              if (!act) {
                if (!cntInterval[ch]) {           /* If period expired        */
                  if (USBH_HW_CH_TransferEnqueue (ptr_ch, (uint8_t)ptr_urb->parameters, (uint8_t *)ptr_urb->buf, (uint32_t)ptr_urb->len)) {
                    ptr_urb->in_progress = 1;
                    act = 1;
                  }
                }
              }
          } else if (!act) {
            if (USBH_HW_CH_TransferEnqueue (ptr_ch, (uint8_t)ptr_urb->parameters, (uint8_t *)ptr_urb->buf, (uint32_t)ptr_urb->len)) {
              ptr_urb->in_progress = 1;
              act = 1;
            }
          }
        }
      }
      ptr_ch++;
    }
  }
}

ARM_DRIVER_USBH Driver_USBH1 = {
  USBH_HW_GetVersion,
  USBH_HW_GetCapabilities,
  USBH_HW_Initialize,
  USBH_HW_Uninitialize,
  USBH_HW_PowerControl,
  USBH_HW_PortPowerOnOff,
  USBH_HW_PortReset,
  USBH_HW_PortSuspend,
  USBH_HW_PortResume,
  USBH_HW_PortGetState,
  USBH_HW_EndpointCreate,
  USBH_HW_EndpointConfigure,
  USBH_HW_EndpointDelete,
  USBH_HW_EndpointClearHalt,
  USBH_HW_URB_Submit,
  USBH_HW_URB_Abort
};
