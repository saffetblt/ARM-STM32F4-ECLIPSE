/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        12. September 2013
 * $Revision:    V1.02
 *  
 * Driver:       Driver_ETH_MAC0
 * Configured:   via RTE_Device.h configuration file 
 * Project:      Ethernet Media Access (MAC) Driver for STM32F4xx
 * ---------------------------------------------------------------------- 
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 * 
 *   Configuration Setting                 Value
 *   ---------------------                 -----
 *   Connect to hardware via Driver_ETH# = 0
 * -------------------------------------------------------------------- */

/* History:
 *  Version 1.02
 *    Based on API V1.10 (namespace prefix ARM_ added)
 *  Version 1.01
 *    Added checksum offload
 *    Added multicast MAC address filtering 
 *  Version 1.00
 *    Initial release
 */ 

#include "cmsis_os.h"
#include "stm32f4xx.h"

#include "EMAC_STM32F4xx.h"
#include "GPIO_STM32F4xx.h"

#include "Driver_ETH_MAC.h"

#include "RTE_Device.h"
#include "RTE_Components.h"

#define ARM_ETH_MAC_DRV_VERSION ARM_DRV_VERSION_MAJOR_MINOR(1,02) /* driver version */


#if (defined(RTE_Drivers_EMAC0) && !RTE_ETH)
#error "Ethernet not configured in RTE_Device.h!"
#endif

#if (RTE_ETH_MII && RTE_ETH_RMII)
#error "Ethernet interface configuration in RTE_Device.h is invalid!"
#endif

/* Receive/transmit Checksum offload enable */
#ifndef EMAC_CHECKSUM_OFFLOAD
#define EMAC_CHECKSUM_OFFLOAD  1
#endif

/* MDC management clock prescaler */
#if   (RTE_HCLK >= 150000000)
#define MACMIIAR_CR_Val ETH_MACMIIAR_CR_Div102
#elif (RTE_HCLK >= 100000000)
#define MACMIIAR_CR_Val ETH_MACMIIAR_CR_Div62
#elif (RTE_HCLK >= 60000000)
#define MACMIIAR_CR_Val ETH_MACMIIAR_CR_Div42
#elif (RTE_HCLK >= 35000000)
#define MACMIIAR_CR_Val ETH_MACMIIAR_CR_Div26
#elif (RTE_HCLK >= 25000000)
#define MACMIIAR_CR_Val ETH_MACMIIAR_CR_Div16
#else
#error "HCLK too slow for Ethernet! Check settings in RTE_Device.h!"
#endif

/* Reference 25MHz/50MHz Clock generation */
#if (RTE_ETH_REF_CLOCK)
  #if (RTE_ETH_MII && (RTE_HSE != 25000000))
    #error "25MHz HSE required for MII Reference Clock! Check settings in RTE_Device.h!"
  #endif
  #if (RTE_ETH_RMII)
    #error "Reference Clock for RMII not supported! Check settings in RTE_Device.h!"
  #endif
#endif


/* Timeouts */
#define PHY_TIMEOUT         500         /* PHY Register access timeout in us  */
#define TX_TIMEOUT          5000        /* Packet Transmit timeout in us      */

/* ETH Memory Buffer configuration */
#define NUM_RX_BUF          4           /* 0x1800 for Rx (4*1536=6K)          */
#define NUM_TX_BUF          2           /* 0x0C00 for Tx (2*1536=3K)          */
#define ETH_BUF_SIZE        1536        /* ETH Receive/Transmit buffer size   */


/* Ethernet Pin definitions */
static const GPIO_PIN_ID eth_pins[] = {
  { RTE_ETH_MDI_MDC_PORT,      RTE_ETH_MDI_MDC_PIN      },
  { RTE_ETH_MDI_MDIO_PORT,     RTE_ETH_MDI_MDIO_PIN     },
#if (RTE_ETH_RMII)
  { RTE_ETH_RMII_TXD0_PORT,    RTE_ETH_RMII_TXD0_PIN    },
  { RTE_ETH_RMII_TXD1_PORT,    RTE_ETH_RMII_TXD1_PIN    },
  { RTE_ETH_RMII_TX_EN_PORT,   RTE_ETH_RMII_TX_EN_PIN   },
  { RTE_ETH_RMII_RXD0_PORT,    RTE_ETH_RMII_RXD0_PIN    },
  { RTE_ETH_RMII_RXD1_PORT,    RTE_ETH_RMII_RXD1_PIN    },
  { RTE_ETH_RMII_REF_CLK_PORT, RTE_ETH_RMII_REF_CLK_PIN },
  { RTE_ETH_RMII_CRS_DV_PORT,  RTE_ETH_RMII_CRS_DV_PIN  }
#endif
#if (RTE_ETH_MII)
  { RTE_ETH_MII_TX_CLK_PORT,   RTE_ETH_MII_TX_CLK_PIN   },
  { RTE_ETH_MII_TXD0_PORT,     RTE_ETH_MII_TXD0_PIN     },
  { RTE_ETH_MII_TXD1_PORT,     RTE_ETH_MII_TXD1_PIN     },
  { RTE_ETH_MII_TXD2_PORT,     RTE_ETH_MII_TXD2_PIN     },
  { RTE_ETH_MII_TXD3_PORT,     RTE_ETH_MII_TXD3_PIN     },
  { RTE_ETH_MII_TX_EN_PORT,    RTE_ETH_MII_TX_EN_PIN    },
  { RTE_ETH_MII_RX_CLK_PORT,   RTE_ETH_MII_RX_CLK_PIN   },
  { RTE_ETH_MII_RXD0_PORT,     RTE_ETH_MII_RXD0_PIN     },
  { RTE_ETH_MII_RXD1_PORT,     RTE_ETH_MII_RXD1_PIN     },
  { RTE_ETH_MII_RXD2_PORT,     RTE_ETH_MII_RXD2_PIN     },
  { RTE_ETH_MII_RXD3_PORT,     RTE_ETH_MII_RXD3_PIN     },
  { RTE_ETH_MII_RX_DV_PORT,    RTE_ETH_MII_RX_DV_PIN    },
  { RTE_ETH_MII_RX_ER_PORT,    RTE_ETH_MII_RX_ER_PIN    },
  { RTE_ETH_MII_CRS_PORT,      RTE_ETH_MII_CRS_PIN      },
  { RTE_ETH_MII_COL_PORT,      RTE_ETH_MII_COL_PIN      }
#endif
};


/* Driver Version */
static const ARM_DRV_VERSION DriverVersion = {
  ARM_ETH_MAC_API_VERSION,
  ARM_ETH_MAC_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_ETH_MAC_CAPABILITIES DriverCapabilities = {
  (EMAC_CHECKSUM_OFFLOAD) ? 1 : 0,  /* rx_checksum_offload_ip   */
  (EMAC_CHECKSUM_OFFLOAD) ? 1 : 0,  /* rx_checksum_offload_udp  */
  (EMAC_CHECKSUM_OFFLOAD) ? 1 : 0,  /* rx_checksum_offload_tcp  */
  (EMAC_CHECKSUM_OFFLOAD) ? 1 : 0,  /* rx_checksum_offload_icmp */
  (EMAC_CHECKSUM_OFFLOAD) ? 1 : 0,  /* tx_checksum_offload_ip   */
  (EMAC_CHECKSUM_OFFLOAD) ? 1 : 0,  /* tx_checksum_offload_udp  */
  (EMAC_CHECKSUM_OFFLOAD) ? 1 : 0,  /* tx_checksum_offload_tcp  */
  (EMAC_CHECKSUM_OFFLOAD) ? 1 : 0,  /* tx_checksum_offload_icmp */
  (RTE_ETH_RMII) ?
  ARM_ETH_INTERFACE_RMII :
  ARM_ETH_INTERFACE_MII,            /* media_interface          */
  0,                                /* mac_address              */
  1,                                /* event_rx_frame           */
  0                                 /* reserved                 */
};


/* Local variables */
static ARM_ETH_MAC_SignalEvent_t cb_Event;
static uint8_t                   tx_index;
static uint8_t                   rx_index;
static RX_Desc                   rx_desc[NUM_RX_BUF];
static TX_Desc                   tx_desc[NUM_TX_BUF];
static uint32_t                  rx_buf [NUM_RX_BUF][ETH_BUF_SIZE>>2];
static uint32_t                  tx_buf [NUM_TX_BUF][ETH_BUF_SIZE>>2];


/**
  \fn          ARM_DRV_VERSION ETH_MAC_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRV_VERSION GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          ARM_ETH_MAC_CAPABILITIES ETH_MAC_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_ETH_MAC_CAPABILITIES
*/
static ARM_ETH_MAC_CAPABILITIES GetCapabilities (void) {
  return DriverCapabilities;
}


/**
  \fn          ARM_ETH_STATUS ETH_MAC_Initialize (ARM_ETH_MAC_SignalEvent_t cb_event,
                                                  ARM_ETH_MAC_ADDR         *mac_address)
  \brief       Initialize Ethernet MAC Device.
  \param[in]   cb_event       Pointer to \ref ARM_ETH_MAC_SignalEvent
  \param[in,out] mac_address  Pointer to MAC address
  \return      \ref EARM_TH_STATUS
*/
static ARM_ETH_STATUS Initialize (ARM_ETH_MAC_SignalEvent_t cb_event,
                                  ARM_ETH_MAC_ADDR         *mac_address) {
  const GPIO_PIN_ID *pin;
        uint32_t     i, next;

  if (mac_address == NULL) {
    return ARM_ETH_ERROR;
  }

  /* Register MAC callback functions */
  cb_Event = cb_event;

  /* Enable SYSCFG clock */
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  /* Enable Ethernet clocks */
  RCC->AHB1ENR |= RCC_AHB1ENR_ETHMACEN    |
                  RCC_AHB1ENR_ETHMACTXEN  |
                  RCC_AHB1ENR_ETHMACRXEN  |
                  RCC_AHB1ENR_ETHMACPTPEN;

  /* Reset Ethernet MAC */
  RCC->AHB1RSTR |=  RCC_AHB1RSTR_ETHMACRST;
#if (RTE_ETH_RMII)
  SYSCFG->PMC |=  SYSCFG_PMC_MII_RMII_SEL;
#else
  SYSCFG->PMC &= ~SYSCFG_PMC_MII_RMII_SEL;
#endif
  RCC->AHB1RSTR &= ~RCC_AHB1RSTR_ETHMACRST;

  /* Configure Ethernet pins */
  for (pin = eth_pins; pin != &eth_pins[sizeof(eth_pins)/sizeof(GPIO_PIN_ID)]; pin++) {
    GPIO_PortClock   (pin->port, true);
    GPIO_PinAF       (pin->port, pin->num, GPIO_AF_ETH);
    GPIO_PinConfigure(pin->port, 
                      pin->num,
                      GPIO_MODE_AF,
                      GPIO_OUTPUT_PUSH_PULL,
                      GPIO_OUTPUT_SPEED_100MHz,
                      GPIO_NO_PULL_UP_DOWN);
  }

#if (RTE_ETH_REF_CLOCK && RTE_ETH_MII && (RTE_HSE == 25000000))
  /* 25MHz MCO clock for MII interface */
  GPIO_PortClock   (RTE_ETH_REF_CLOCK_PORT, true);
  GPIO_PinAF       (RTE_ETH_REF_CLOCK_PORT, RTE_ETH_REF_CLOCK_PIN, GPIO_AF_SYSTEM);
  GPIO_PinConfigure(RTE_ETH_REF_CLOCK_PORT,
                    RTE_ETH_REF_CLOCK_PIN,
                    GPIO_MODE_AF,
                    GPIO_OUTPUT_PUSH_PULL,
                    GPIO_OUTPUT_SPEED_100MHz,
                    GPIO_NO_PULL_UP_DOWN);
#if (RTE_ETH_REF_CLOCK_PIN == 8)
  /* MCO1 pin generate 25 MHz */
  RCC->CFGR &= ~(RCC_CFGR_MCO1 | RCC_CFGR_MCO1PRE);
  /* HSE clock select */
  RCC->CFGR |=   RCC_CFGR_MCO1_1;
#else
  /* MCO2 pin generate 25 MHz */
  RCC->CFGR &= ~(RCC_CFGR_MCO2 | RCC_CFGR_MCO2PRE);
  /* HSE clock select */
  RCC->CFGR |=   RCC_CFGR_MCO2_1;
#endif
#endif

  /* Reset DMA */
  ETH->DMABMR |= ETH_DMABMR_SR;
  while (ETH->DMABMR & ETH_DMABMR_SR);

  /* MDC clock range selection */
  ETH->MACMIIAR = MACMIIAR_CR_Val;

  /* Initialize MAC configuration register */
#if (EMAC_CHECKSUM_OFFLOAD)
  ETH->MACCR   = ETH_MACCR_ROD | ETH_MACCR_IPCO;
#else
  ETH->MACCR   = ETH_MACCR_ROD;
#endif
  /* Accept unicast and broadcast packets */
  ETH->MACFFR  = 0x00000000;
  ETH->MACFCR  = ETH_MACFCR_ZQPD;

  /* Set the Ethernet MAC Address registers */
  ETH->MACA0HR = (mac_address->b[5] <<  8) |  mac_address->b[4];
  ETH->MACA0LR = (mac_address->b[3] << 24) | (mac_address->b[2] << 16) |
                 (mac_address->b[1] <<  8) |  mac_address->b[0];
  ETH->MACA1HR = 0x00000000;
  ETH->MACA1LR = 0x00000000;
  ETH->MACA2HR = 0x00000000;
  ETH->MACA2LR = 0x00000000;
  ETH->MACA3HR = 0x00000000;
  ETH->MACA3LR = 0x00000000;
  
  /* Initialize Tx and Rx DMA Descriptors */
  for (i = 0, next = 0; i < NUM_RX_BUF; i++) {
    if (++next == NUM_RX_BUF) next = 0;
    rx_desc[i].Stat = DMA_RX_OWN;
    rx_desc[i].Ctrl = DMA_RX_RCH | ETH_BUF_SIZE;
    rx_desc[i].Addr = (uint32_t)&rx_buf[i];
    rx_desc[i].Next = (uint32_t)&rx_desc[next];
  }
  for (i = 0, next = 0; i < NUM_TX_BUF; i++) {
    if (++next == NUM_TX_BUF) next = 0;
#if (EMAC_CHECKSUM_OFFLOAD)
    tx_desc[i].CtrlStat = DMA_TX_TCH | DMA_TX_LS | DMA_TX_FS | DMA_TX_CIC;
#else
    tx_desc[i].CtrlStat = DMA_TX_TCH | DMA_TX_LS | DMA_TX_FS;
#endif
    tx_desc[i].Addr     = (uint32_t)&tx_buf[i];
    tx_desc[i].Next     = (uint32_t)&tx_desc[next];
  }
  ETH->DMARDLAR = (uint32_t)&rx_desc[0];
  ETH->DMATDLAR = (uint32_t)&tx_desc[0];
  rx_index      = 0;
  tx_index      = 0;

  /* Flush Tx FIFO, reset interrupts */
#if (EMAC_CHECKSUM_OFFLOAD)
  ETH->DMAOMR = ETH_DMAOMR_FTF | ETH_DMAOMR_TSF | ETH_DMAOMR_RSF;
#else
  ETH->DMAOMR = ETH_DMAOMR_FTF;
#endif
  ETH->DMASR  = 0xFFFFFFFF;

  /* Enable Rx interrupts */
  ETH->DMAIER = ETH_DMAIER_NISE | ETH_DMAIER_RIE;

  /* Enable ethernet interrupts */
  NVIC_ClearPendingIRQ(ETH_IRQn);
  NVIC_EnableIRQ(ETH_IRQn);

  return ARM_ETH_OK;
}

/**
  \fn          ARM_ETH_STATUS ETH_MAC_Uninitialize (void)
  \brief       De-initialize ETH_MAC Device.
  \return      \ref ARM_ETH_STATUS
*/
static ARM_ETH_STATUS Uninitialize (void) {
  const GPIO_PIN_ID *pin;

  /* Disable ethernet interrupts */
  ETH->DMAIER = 0x00000000;
  NVIC_DisableIRQ(ETH_IRQn);

  /* Unconfigure ethernet pins */
  for (pin = eth_pins; pin != &eth_pins[sizeof(eth_pins)/sizeof(GPIO_PIN_ID)]; pin++) {
    GPIO_PinConfigure(pin->port, 
                      pin->num,
                      GPIO_MODE_INPUT,
                      GPIO_OUTPUT_PUSH_PULL,
                      GPIO_OUTPUT_SPEED_2MHz,
                      GPIO_NO_PULL_UP_DOWN);
    GPIO_PinAF       (pin->port, pin->num, GPIO_AF_SYSTEM);
  }
#if (RTE_ETH_REF_CLOCK)
  GPIO_PinConfigure(RTE_ETH_REF_CLOCK_PORT,
                    RTE_ETH_REF_CLOCK_PIN,
                    GPIO_MODE_INPUT,
                    GPIO_OUTPUT_PUSH_PULL,
                    GPIO_OUTPUT_SPEED_2MHz,
                    GPIO_NO_PULL_UP_DOWN);
#endif

  return ARM_ETH_OK;
}

/**
  \fn          ARM_ETH_STATUS ETH_MAC_PowerControl (ARM_POWER_STATE state)
  \brief       Control Ethernet MAC Device Power.
  \param[in]   state  Power state
  \return      \ref ARM_ETH_STATUS
*/
static ARM_ETH_STATUS PowerControl (ARM_POWER_STATE state) {

  switch (state) {
    case ARM_POWER_OFF:
      break;

    case ARM_POWER_LOW:
      return ARM_ETH_ERROR_UNSUPPORTED;
      
    case ARM_POWER_FULL:
      break;

    default:
      return ARM_ETH_ERROR_UNSUPPORTED;
  }
  
  return ARM_ETH_OK;
}

/**
  \fn          uint32_t CalculateCRC (uint8_t *data, uint32_t len)
  \brief       Calculate 32-bit Ethernet CRC value.
  \param[in]   data  Pointer to the buffer containing data
  \param[in]   len   Data buffer length in bytes
  \return      Calculated CRC value
*/
static uint32_t CalculateCRC (uint8_t *buf, uint32_t len) {
  uint32_t a,crc,byte;

  for (crc = 0xFFFFFFFF; len > 0; len--) {
    byte = *buf++;
    for (byte |= 0x100; byte > 0x001; byte >>= 1) {
      a = crc, crc <<= 1;
      if ((a ^ (byte << 31)) & 0x80000000) {
        crc ^= 0x04c11db7;
      }
    }
  }
  return (crc ^ 0xFFFFFFFF);
}

/**
  \fn          ARM_ETH_STATUS ETH_MAC_SetMulticastAddr (uint32_t          num_addr,
                                                        ARM_ETH_MAC_ADDR *ptr_addr)
  \brief       Configure Ethernet multicast address filtering.
  \param[in]   num_addr   Number of multicast addresses to configure
  \param[in]   ptr_addr   Pointer to multicast addresses
  \return      \ref ARM_ETH_STATUS
*/
static ARM_ETH_STATUS SetMulticastAddr (uint32_t num_addr, ARM_ETH_MAC_ADDR *ptr_addr) {
  uint32_t crc;

  /* Use unicast address filtering for first 3 MAC addresses */
  ETH->MACFFR  = 0x00000000;
  ETH->MACHTHR = ETH->MACHTLR = 0x00000000;

  if (num_addr == 0) {
    ETH->MACA1HR = ETH->MACA1LR = 0x00000000;
    ETH->MACA2HR = ETH->MACA2LR = 0x00000000;
    ETH->MACA3HR = ETH->MACA3LR = 0x00000000;
    return ARM_ETH_OK;
  }

  ETH->MACA1HR = ETH_MACA1HR_AE | (ptr_addr->b[5] <<  8) | ptr_addr->b[4];
  ETH->MACA1LR = (ptr_addr->b[3] << 24) | (ptr_addr->b[2] << 16) |
                 (ptr_addr->b[1] <<  8) |  ptr_addr->b[0];
  if (--num_addr == 0) {
    ETH->MACA2HR = ETH->MACA2LR = 0x00000000;
    ETH->MACA3HR = ETH->MACA3LR = 0x00000000;
    return ARM_ETH_OK;
  }

  ptr_addr++;
  ETH->MACA2HR = ETH_MACA2HR_AE | (ptr_addr->b[5] <<  8) | ptr_addr->b[4];
  ETH->MACA2LR = (ptr_addr->b[3] << 24) | (ptr_addr->b[2] << 16) |
                 (ptr_addr->b[1] <<  8) |  ptr_addr->b[0];
  if (--num_addr == 0) {
    ETH->MACA3HR = ETH->MACA3LR = 0x00000000;
    return ARM_ETH_OK;
  }

  ptr_addr++;
  ETH->MACA3HR = ETH_MACA3HR_AE | (ptr_addr->b[5] <<  8) | ptr_addr->b[4];
  ETH->MACA3LR = (ptr_addr->b[3] << 24) | (ptr_addr->b[2] << 16) |
                 (ptr_addr->b[1] <<  8) |  ptr_addr->b[0];
  if (--num_addr == 0) {
    return ARM_ETH_OK;
  }

  /* Calculate 64-bit Hash table for remaining MAC addresses */
  ptr_addr++;
  for ( ; num_addr; ptr_addr++, num_addr--) {
    crc = CalculateCRC (&ptr_addr->b[0], 6) >> 26;
    if (crc & 0x20) {
      ETH->MACHTHR |= (1 << (crc & 0x1F));
    }
    else {
      ETH->MACHTLR |= (1 << crc);
    }
  }
  /* Enable both, unicast and hash address filtering */
  ETH->MACFFR = ETH_MACFFR_HPF | ETH_MACFFR_HM;

  return ARM_ETH_OK;
}

/**
  \fn          ARM_ETH_STATUS ETH_MAC_SetMode (ARM_ETH_DUPLEX duplex, ARM_ETH_SPEED speed)
  \brief       Set the Ethernet MAC speed and duplex mode.
  \param[in]   duplex  \ref ARM_ETH_DUPLEX
  \param[in]   speed   \ref ARM_ETH_SPEED
  \return      \ref ARM_ETH_STATUS
*/
static ARM_ETH_STATUS SetMode (ARM_ETH_DUPLEX duplex, ARM_ETH_SPEED speed) {

  /* Configure Full/Half Duplex mode */
  switch (duplex) {
    case ARM_ETH_DUPLEX_HALF:
      ETH->MACCR &= ~ETH_MACCR_DM;
      break;
    case ARM_ETH_DUPLEX_FULL:
      ETH->MACCR |=  ETH_MACCR_DM;
      break;
    default:
      return ARM_ETH_ERROR;
  }
  /* Configure 100MBit/10MBit mode */
  switch (speed) {
    case ARM_ETH_SPEED_10M:
      ETH->MACCR &= ~ETH_MACCR_FES;
#if (RTE_ETH_RMII)
      /* RMII Half Duplex Colision detection does not work */
      ETH->MACCR |=  ETH_MACCR_DM;
#endif
      break;
    case ARM_ETH_SPEED_100M:
      ETH->MACCR |=  ETH_MACCR_FES;
      break;
    default:
      return ARM_ETH_ERROR_UNSUPPORTED;
  }

  return ARM_ETH_OK;
}

/**
  \fn          ARM_ETH_STATUS ETH_MAC_EnableTx (bool enable)
  \brief       Enable the Ethernet MAC transmit engine.
  \param[in]   enable  Action
               - \b true Enable
               - \b false Disable
  \return      \ref ARM_ETH_STATUS
*/
static ARM_ETH_STATUS EnableTx (bool enable) {

  if (enable == true) {
    ETH->DMAOMR |=  ETH_DMAOMR_ST;
    ETH->MACCR  |=  ETH_MACCR_TE;
  }
  else {
    ETH->MACCR  &= ~ETH_MACCR_TE;
    ETH->DMAOMR &= ~ETH_DMAOMR_ST;
  }

  return ARM_ETH_OK;
}

/**
  \fn          ARM_ETH_STATUS ETH_MAC_EnableRx (bool enable)
  \brief       Enable the Ethernet MAC receive engine.
  \param[in]   enable  Action
               - \b true Enable
               - \b false Disable
  \return      \ref ARM_ETH_STATUS
*/
static ARM_ETH_STATUS EnableRx (bool enable) {

  if (enable == true) {  
    ETH->DMAOMR |=  ETH_DMAOMR_SR;
    ETH->MACCR  |=  ETH_MACCR_RE;
  }
  else {
    ETH->DMAOMR &= ~ETH_DMAOMR_SR;
    ETH->MACCR  &= ~ETH_MACCR_RE;
  }

  return ARM_ETH_OK;
}

/**
  \fn          ARM_ETH_STATUS ETH_MAC_SendFrame (uint8_t *buf, uint32_t len)
  \brief       Send Ethernet frame.
  \param[in]   buf  Pointer to frame buffer with data to send
  \param[in]   len  Frame buffer length in bytes
  \return      \ref ARM_ETH_STATUS
*/
static ARM_ETH_STATUS SendFrame (uint8_t *buf, uint32_t len) {
  uint32_t *sp, *dp;
  uint32_t  i, tick;

  if (tx_desc[tx_index].CtrlStat & DMA_TX_OWN) {
    /* Wait until previous packet transmitted */
    tick = osKernelSysTick();
    do {
      if (!(tx_desc[tx_index].CtrlStat & DMA_TX_OWN)) break;
    } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(TX_TIMEOUT));
    if (tx_desc[tx_index].CtrlStat & DMA_TX_OWN) {
      return ARM_ETH_ERROR;
    }
  }

  sp = (uint32_t *)buf;
  dp = (uint32_t *)(tx_desc[tx_index].Addr & ~3);

  /* Copy frame data to ETH IO buffer */
  for (i = (len + 3) >> 2; i; i--) {
    *dp++ = *sp++;
  }
  tx_desc[tx_index].Size      = len;
  tx_desc[tx_index].CtrlStat |= DMA_TX_OWN;
  if (++tx_index == NUM_TX_BUF) tx_index = 0;

  /* Start frame transmission */
  ETH->DMASR   = ETH_DMASR_TPSS;
  ETH->DMATPDR = 0;

  return ARM_ETH_OK;
}

/**
  \fn          int32_t ETH_MAC_ReadFrame (uint8_t *buf, uint32_t len)
  \brief       Read data of received Ethernet frame.
  \param[in]   buf  Pointer to frame buffer for data to read into
  \param[in]   len  Frame buffer length in bytes
  \return      number of bytes read, error code if negative
*/
static int32_t ReadFrame (uint8_t *buf, uint32_t len) {
  uint32_t *sp, *dp;
  int32_t i;

  sp = (uint32_t *)(rx_desc[rx_index].Addr & ~3);
  dp = (uint32_t *)buf;
  for (i = (len + 3) >> 2; i; i--) {
    *dp++ = *sp++;
  }
  /* Return this block back to ETH-DMA */
  rx_desc[rx_index].Stat = DMA_RX_OWN;

  if (++rx_index == NUM_RX_BUF) rx_index = 0;

  if (ETH->DMASR & ETH_DMASR_RBUS) {
    /* Receive buffer unavailable, resume DMA */
    ETH->DMASR   = ETH_DMASR_RBUS;
    ETH->DMARPDR = 0;
  }
  return (len);
}

/**
  \fn          int32_t ETH_MAC_GetRxFrameSize (void)
  \brief       Get size of received Ethernet frame.
  \return      number of bytes in received frame, error code if negative
*/
static int32_t GetRxFrameSize (void) {
  uint32_t stat = rx_desc[rx_index].Stat;

  if (stat & DMA_RX_OWN) {
    /* Owned by DMA */
    return (0);
  }
  if ((stat & DMA_RX_ES) || !(stat & DMA_RX_FS) || !(stat & DMA_RX_LS)) {
    /* Error, this block is invalid */
    return (-1);
  }
  return (((stat & DMA_RX_FL) >> 16) - 4);
}

/* Ethernet IRQ Handler */
void ETH_IRQHandler (void) {
  cb_Event (ARM_ETH_MAC_EVENT_RX_FRAME);
  ETH->DMASR = ETH_DMASR_NIS | ETH_DMASR_RS;
}

/**
  \fn          ARM_ETH_STATUS ETH_MAC_PHY_Read (uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
  \brief       Read Ethernet PHY Register through Management Interface.
  \param[in]   phy_addr  5-bit device address
  \param[in]   reg_addr  5-bit register address
  \param[out]  data      Pointer where the result is written to
  \return      \ref ARM_ETH_STATUS
*/
static ARM_ETH_STATUS PHY_Read (uint8_t phy_addr, uint8_t reg_addr, uint16_t *data) {
  uint32_t tick;

  ETH->MACMIIAR = MACMIIAR_CR_Val | ETH_MACMIIAR_MB |
                 (phy_addr << 11) | (reg_addr << 6);

  /* Wait until operation completed */
  tick = osKernelSysTick();
  do {
    if ((ETH->MACMIIAR & ETH_MACMIIAR_MB) == 0) break;
  } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(PHY_TIMEOUT));

  if ((ETH->MACMIIAR & ETH_MACMIIAR_MB) == 0) {
    *data = ETH->MACMIIDR & ETH_MACMIIDR_MD;
    return ARM_ETH_OK;
  }

  return ARM_ETH_ERROR;
}

/**
  \fn          ARM_ETH_STATUS ETH_MAC_PHY_Write (uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
  \brief       Write Ethernet PHY Register through Management Interface.
  \param[in]   phy_addr  5-bit device address
  \param[in]   reg_addr  5-bit register address
  \param[in]   data      16-bit data to write
  \return      \ref ARM_ETH_STATUS
*/
static ARM_ETH_STATUS PHY_Write (uint8_t phy_addr, uint8_t reg_addr, uint16_t data) {
  uint32_t tick;

  ETH->MACMIIDR = data;
  ETH->MACMIIAR = MACMIIAR_CR_Val | ETH_MACMIIAR_MB | ETH_MACMIIAR_MW |
                 (phy_addr << 11) | (reg_addr << 6);

  /* Wait until operation completed */
  tick = osKernelSysTick();
  do {
    if ((ETH->MACMIIAR & ETH_MACMIIAR_MB) == 0) break;
  } while ((osKernelSysTick() - tick) < osKernelSysTickMicroSec(PHY_TIMEOUT));

  if ((ETH->MACMIIAR & ETH_MACMIIAR_MB) == 0) {
    return ARM_ETH_OK;
  }

  return ARM_ETH_ERROR;
}


/* MAC Driver Control Block */
ARM_DRIVER_ETH_MAC Driver_ETH_MAC0 = {
  GetVersion,
  GetCapabilities,
  Initialize,
  Uninitialize,
  PowerControl,
  SetMulticastAddr,
  SetMode,
  EnableTx,
  EnableRx,
  SendFrame,
  ReadFrame,
  GetRxFrameSize,
  PHY_Read,
  PHY_Write
};
