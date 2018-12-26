/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        13. May 2013
 * $Revision:    V1.01
 * $Comment:     Based on Reference Manual RM0090 Rev 4 (February 2013)
 *  
 * Project:      OTG High-Speed Driver Header for ST STM32F4xx
 * -------------------------------------------------------------------- */

#include <stdint.h>

// OTG_HS control and status register (OTG_HS_GOTGCTL)
#define  OTG_HS_GOTGCTL_SRQSCS              ((uint32_t)    1 <<  0)
#define  OTG_HS_GOTGCTL_SRQ                 ((uint32_t)    1 <<  1)
#define  OTG_HS_GOTGCTL_HNGSCS              ((uint32_t)    1 <<  8)
#define  OTG_HS_GOTGCTL_HNPRQ               ((uint32_t)    1 <<  9)
#define  OTG_HS_GOTGCTL_HSHNPEN             ((uint32_t)    1 << 10)
#define  OTG_HS_GOTGCTL_DHNPEN              ((uint32_t)    1 << 11)
#define  OTG_HS_GOTGCTL_CIDSTS              ((uint32_t)    1 << 16)
#define  OTG_HS_GOTGCTL_DBCT                ((uint32_t)    1 << 17)
#define  OTG_HS_GOTGCTL_ASVLD               ((uint32_t)    1 << 18)
#define  OTG_HS_GOTGCTL_BSVLD               ((uint32_t)    1 << 19)

// OTG_HS interrupt register (OTG_HS_GOTGINT)
#define  OTG_HS_GOTGINT_SEDET               ((uint32_t)    1 <<  2)
#define  OTG_HS_GOTGINT_SRSSCHG             ((uint32_t)    1 <<  8)
#define  OTG_HS_GOTGINT_HNSSCHG             ((uint32_t)    1 <<  9)
#define  OTG_HS_GOTGINT_HNGDET              ((uint32_t)    1 << 17)
#define  OTG_HS_GOTGINT_ADTOCHG             ((uint32_t)    1 << 18)
#define  OTG_HS_GOTGINT_DBCDNE              ((uint32_t)    1 << 19)

// OTG_HS AHB configuration register (OTG_HS_GAHBCFG)
#define  OTG_HS_GAHBCFG_GINT                ((uint32_t)    1 <<  0)
#define  OTG_HS_GAHBCFG_HBSTLEN_POS         ((uint32_t)          1)
#define  OTG_HS_GAHBCFG_HBSTLEN_MSK         ((uint32_t) 0x0F <<  OTG_HS_GAHBCFG_HBSTLEN_POS)
#define  OTG_HS_GAHBCFG_HBSTLEN(x)          ((uint32_t)(   x <<  OTG_HS_GAHBCFG_HBSTLEN_POS) & OTG_HS_GAHBCFG_HBSTLEN_MSK)
#define  OTG_HS_GAHBCFG_DMAEN               ((uint32_t)    1 <<  5)
#define  OTG_HS_GAHBCFG_TXFELVL             ((uint32_t)    1 <<  7)
#define  OTG_HS_GAHBCFG_PTXFELVL            ((uint32_t)    1 <<  8)

// OTG_HS USB configuration register (OTG_HS_GUSBCFG)
#define  OTG_HS_GUSBCFG_TOCAL_POS           ((uint32_t)          0)
#define  OTG_HS_GUSBCFG_TOCAL_MSK           ((uint32_t)    7 <<  OTG_HS_GUSBCFG_TOCAL_POS)
#define  OTG_HS_GUSBCFG_TOCAL(x)            ((uint32_t)(   x <<  OTG_HS_GUSBCFG_TOCAL_POS) & OTG_HS_GUSBCFG_TOCAL_MSK)
#define  OTG_HS_GUSBCFG_PHSEL               ((uint32_t)    1 <<  6)
#define  OTG_HS_GUSBCFG_SRPCAP              ((uint32_t)    1 <<  8)
#define  OTG_HS_GUSBCFG_HNPCAP              ((uint32_t)    1 <<  9)
#define  OTG_HS_GUSBCFG_TRDT_POS            ((uint32_t)         10)
#define  OTG_HS_GUSBCFG_TRDT_MSK            ((uint32_t) 0x0F << OTG_HS_GUSBCFG_TRDT_POS)
#define  OTG_HS_GUSBCFG_TRDT(x)             ((uint32_t)(   x << OTG_HS_GUSBCFG_TRDT_POS) & OTG_HS_GUSBCFG_TRDT_MSK)
#define  OTG_HS_GUSBCFG_PHYLPCS             ((uint32_t)    1 << 15)
#define  OTG_HS_GUSBCFG_ULPIFSLS            ((uint32_t)    1 << 17)
#define  OTG_HS_GUSBCFG_ULPIAR              ((uint32_t)    1 << 18)
#define  OTG_HS_GUSBCFG_ULPICSM             ((uint32_t)    1 << 19)
#define  OTG_HS_GUSBCFG_ULPIEVBUSD          ((uint32_t)    1 << 20)
#define  OTG_HS_GUSBCFG_ULPIEVBUSI          ((uint32_t)    1 << 21)
#define  OTG_HS_GUSBCFG_TSDPS               ((uint32_t)    1 << 22)
#define  OTG_HS_GUSBCFG_PCCI                ((uint32_t)    1 << 23)
#define  OTG_HS_GUSBCFG_PTCI                ((uint32_t)    1 << 24)
#define  OTG_HS_GUSBCFG_ULPIPD              ((uint32_t)    1 << 25)
#define  OTG_HS_GUSBCFG_FHMOD               ((uint32_t)    1 << 29)
#define  OTG_HS_GUSBCFG_FDMOD               ((uint32_t)    1 << 30)
#define  OTG_HS_GUSBCFG_CTXPKT              ((uint32_t)    1 << 31)

// OTG_HS reset register (OTG_HS_GRSTCTL)
#define  OTG_HS_GRSTCTL_CSRST               ((uint32_t)    1 <<  0)
#define  OTG_HS_GRSTCTL_HSRST               ((uint32_t)    1 <<  1)
#define  OTG_HS_GRSTCTL_FCRST               ((uint32_t)    1 <<  2)
#define  OTG_HS_GRSTCTL_RXFFLSH             ((uint32_t)    1 <<  4)
#define  OTG_HS_GRSTCTL_TXFFLSH             ((uint32_t)    1 <<  5)
#define  OTG_HS_GRSTCTL_TXFNUM_POS          ((uint32_t)          6)
#define  OTG_HS_GRSTCTL_TXFNUM_MSK          ((uint32_t) 0x1F <<  OTG_HS_GRSTCTL_TXFNUM_POS)
#define  OTG_HS_GRSTCTL_TXFNUM(x)           ((uint32_t)(   x <<  OTG_HS_GRSTCTL_TXFNUM_POS) & OTG_HS_GRSTCTL_TXFNUM_MSK)
#define  OTG_HS_GRSTCTL_DMAREQ              ((uint32_t)    1 << 30)
#define  OTG_HS_GRSTCTL_AHBIDL              ((uint32_t)    1 << 31)

// OTG_HS core interrupt register (OTG_HS_GINTSTS)
#define  OTG_HS_GINTSTS_CMOD                ((uint32_t)    1 <<  0)
#define  OTG_HS_GINTSTS_MMIS                ((uint32_t)    1 <<  1)
#define  OTG_HS_GINTSTS_OTGINT              ((uint32_t)    1 <<  2)
#define  OTG_HS_GINTSTS_SOF                 ((uint32_t)    1 <<  3)
#define  OTG_HS_GINTSTS_RXFLVL              ((uint32_t)    1 <<  4)
#define  OTG_HS_GINTSTS_NPTXFE              ((uint32_t)    1 <<  5)
#define  OTG_HS_GINTSTS_GINAKEFF            ((uint32_t)    1 <<  6)
#define  OTG_HS_GINTSTS_GONAKEFF            ((uint32_t)    1 <<  7)
#define  OTG_HS_GINTSTS_ESUSP               ((uint32_t)    1 << 10)
#define  OTG_HS_GINTSTS_USBSUSP             ((uint32_t)    1 << 11)
#define  OTG_HS_GINTSTS_USBRST              ((uint32_t)    1 << 12)
#define  OTG_HS_GINTSTS_ENUMDNE             ((uint32_t)    1 << 13)
#define  OTG_HS_GINTSTS_ISOODRP             ((uint32_t)    1 << 14)
#define  OTG_HS_GINTSTS_EOPF                ((uint32_t)    1 << 15)
#define  OTG_HS_GINTSTS_IEPINT              ((uint32_t)    1 << 18)
#define  OTG_HS_GINTSTS_OEPINT              ((uint32_t)    1 << 19)
#define  OTG_HS_GINTSTS_IISOIXFR            ((uint32_t)    1 << 20)
#define  OTG_HS_GINTSTS_IPXFR               ((uint32_t)    1 << 21)
#define  OTG_HS_GINTSTS_INCOMPISOOUT        ((uint32_t)    1 << 21)
#define  OTG_HS_GINTSTS_DATAFSUSP           ((uint32_t)    1 << 22)
#define  OTG_HS_GINTSTS_HPRTINT             ((uint32_t)    1 << 24)
#define  OTG_HS_GINTSTS_HCINT               ((uint32_t)    1 << 25)
#define  OTG_HS_GINTSTS_PTXFE               ((uint32_t)    1 << 26)
#define  OTG_HS_GINTSTS_CIDSCHG             ((uint32_t)    1 << 28)
#define  OTG_HS_GINTSTS_DISCINT             ((uint32_t)    1 << 29)
#define  OTG_HS_GINTSTS_SRQINT              ((uint32_t)    1 << 30)
#define  OTG_HS_GINTSTS_WKUPINT             ((uint32_t)    1 << 31)

// OTG_HS interrupt mask register (OTG_HS_GINTMSK)
#define  OTG_HS_GINTMSK_MMISM               ((uint32_t)    1 <<  1)
#define  OTG_HS_GINTMSK_OTGINT              ((uint32_t)    1 <<  2)
#define  OTG_HS_GINTMSK_SOFM                ((uint32_t)    1 <<  3)
#define  OTG_HS_GINTMSK_RXFLVLM             ((uint32_t)    1 <<  4)
#define  OTG_HS_GINTMSK_NPTXFEM             ((uint32_t)    1 <<  5)
#define  OTG_HS_GINTMSK_GINAKEFFM           ((uint32_t)    1 <<  6)
#define  OTG_HS_GINTMSK_GONAKEFFM           ((uint32_t)    1 <<  7)
#define  OTG_HS_GINTMSK_ESUSPM              ((uint32_t)    1 << 10)
#define  OTG_HS_GINTMSK_USBSUSPM            ((uint32_t)    1 << 11)
#define  OTG_HS_GINTMSK_USBRST              ((uint32_t)    1 << 12)
#define  OTG_HS_GINTMSK_ENUMDNEM            ((uint32_t)    1 << 13)
#define  OTG_HS_GINTMSK_ISOODRPM            ((uint32_t)    1 << 14)
#define  OTG_HS_GINTMSK_EOPFM               ((uint32_t)    1 << 15)
#define  OTG_HS_GINTMSK_EPMISM              ((uint32_t)    1 << 17)
#define  OTG_HS_GINTMSK_IEPINT              ((uint32_t)    1 << 18)
#define  OTG_HS_GINTMSK_OEPINT              ((uint32_t)    1 << 19)
#define  OTG_HS_GINTMSK_IISOIXFRM           ((uint32_t)    1 << 20)
#define  OTG_HS_GINTMSK_IPXFRM              ((uint32_t)    1 << 21)
#define  OTG_HS_GINTMSK_IISOOXFRM           ((uint32_t)    1 << 21)
#define  OTG_HS_GINTMSK_FSUSPM              ((uint32_t)    1 << 22)
#define  OTG_HS_GINTMSK_PRTIM               ((uint32_t)    1 << 24)
#define  OTG_HS_GINTMSK_HCIM                ((uint32_t)    1 << 25)
#define  OTG_HS_GINTMSK_PTXFEM              ((uint32_t)    1 << 26)
#define  OTG_HS_GINTMSK_CIDSCHGM            ((uint32_t)    1 << 28)
#define  OTG_HS_GINTMSK_DISCINT             ((uint32_t)    1 << 29)
#define  OTG_HS_GINTMSK_SRQIM               ((uint32_t)    1 << 30)
#define  OTG_HS_GINTMSK_WUIM                ((uint32_t)    1 << 31)

// OTG_HS status debug read register (OTG_HS_GRXSTSR)
#define  OTG_HS_GRXSTSR_CHNUM_POS           ((uint32_t)          0)
#define  OTG_HS_GRXSTSR_CHNUM_MSK           ((uint32_t) 0x0F << OTG_HS_GRXSTSR_CHNUM_POS)
#define  OTG_HS_GRXSTSR_CHNUM(x)            ((uint32_t)(   x << OTG_HS_GRXSTSR_CHNUM_POS) & OTG_HS_GRXSTSR_CHNUM_MSK)
#define  OTG_HS_GRXSTSR_EPNUM_POS           ((uint32_t)          0)
#define  OTG_HS_GRXSTSR_EPNUM_MSK           ((uint32_t) 0x0F << OTG_HS_GRXSTSR_EPNUM_POS)
#define  OTG_HS_GRXSTSR_EPNUM(x)            ((uint32_t)(   x << OTG_HS_GRXSTSR_EPNUM_POS) & OTG_HS_GRXSTSR_EPNUM_MSK)
#define  OTG_HS_GRXSTSR_BCNT_POS            ((uint32_t)          4)
#define  OTG_HS_GRXSTSR_BCNT_MSK            ((uint32_t)0x7FF << OTG_HS_GRXSTSR_BCNT_POS)
#define  OTG_HS_GRXSTSR_BCNT(x)             ((uint32_t)(   x << OTG_HS_GRXSTSR_BCNT_POS) & OTG_HS_GRXSTSR_BCNT_MSK)
#define  OTG_HS_GRXSTSR_DPID_POS            ((uint32_t)         15)
#define  OTG_HS_GRXSTSR_DPID_MSK            ((uint32_t)    3 << OTG_HS_GRXSTSR_DPID_POS)
#define  OTG_HS_GRXSTSR_DPID(x)             ((uint32_t)(   x << OTG_HS_GRXSTSR_DPID_POS) & OTG_HS_GRXSTSR_DPID_MSK)
#define  OTG_HS_GRXSTSR_PKTSTS_POS          ((uint32_t)         17)
#define  OTG_HS_GRXSTSR_PKTSTS_MSK          ((uint32_t) 0x0F << OTG_HS_GRXSTSR_PKTSTS_POS)
#define  OTG_HS_GRXSTSR_PKTSTS(x)           ((uint32_t)(   x << OTG_HS_GRXSTSR_PKTSTS_POS) & OTG_HS_GRXSTSR_PKTSTS_MSK)
#define  OTG_HS_GRXSTSR_FRMNUM_POS          ((uint32_t)         21)
#define  OTG_HS_GRXSTSR_FRMNUM_MSK          ((uint32_t) 0x0F << OTG_HS_GRXSTSR_FRMNUM_POS)
#define  OTG_HS_GRXSTSR_FRMNUM(x)           ((uint32_t)(   x << OTG_HS_GRXSTSR_FRMNUM_POS) & OTG_HS_GRXSTSR_FRMNUM_MSK)

// OTG_HS status read and pop register (OTG_HS_GRXSTSP)
#define  OTG_HS_GRXSTSP_CHNUM_POS           ((uint32_t)          0)
#define  OTG_HS_GRXSTSP_CHNUM_MSK           ((uint32_t) 0x0F << OTG_HS_GRXSTSP_CHNUM_POS)
#define  OTG_HS_GRXSTSP_CHNUM(x)            ((uint32_t)(   x << OTG_HS_GRXSTSP_CHNUM_POS) & OTG_HS_GRXSTSP_CHNUM_MSK)
#define  OTG_HS_GRXSTSP_EPNUM_POS           ((uint32_t)          0)
#define  OTG_HS_GRXSTSP_EPNUM_MSK           ((uint32_t) 0x0F << OTG_HS_GRXSTSP_EPNUM_POS)
#define  OTG_HS_GRXSTSP_EPNUM(x)            ((uint32_t)(   x << OTG_HS_GRXSTSP_EPNUM_POS) & OTG_HS_GRXSTSP_EPNUM_MSK)
#define  OTG_HS_GRXSTSP_BCNT_POS            ((uint32_t)          4)
#define  OTG_HS_GRXSTSP_BCNT_MSK            ((uint32_t)0x7FF << OTG_HS_GRXSTSP_BCNT_POS)
#define  OTG_HS_GRXSTSP_BCNT(x)             ((uint32_t)(   x << OTG_HS_GRXSTSP_BCNT_POS) & OTG_HS_GRXSTSP_BCNT_MSK)
#define  OTG_HS_GRXSTSP_DPID_POS            ((uint32_t)         15)
#define  OTG_HS_GRXSTSP_DPID_MSK            ((uint32_t)    3 << OTG_HS_GRXSTSP_DPID_POS)
#define  OTG_HS_GRXSTSP_DPID(x)             ((uint32_t)(   x << OTG_HS_GRXSTSP_DPID_POS) & OTG_HS_GRXSTSP_DPID_MSK)
#define  OTG_HS_GRXSTSP_PKTSTS_POS          ((uint32_t)         17)
#define  OTG_HS_GRXSTSP_PKTSTS_MSK          ((uint32_t) 0x0F << OTG_HS_GRXSTSP_PKTSTS_POS)
#define  OTG_HS_GRXSTSP_PKTSTS(x)           ((uint32_t)(   x << OTG_HS_GRXSTSP_PKTSTS_POS) & OTG_HS_GRXSTSP_PKTSTS_MSK)
#define  OTG_HS_GRXSTSP_FRMNUM_POS          ((uint32_t)         21)
#define  OTG_HS_GRXSTSP_FRMNUM_MSK          ((uint32_t) 0x0F << OTG_HS_GRXSTSP_FRMNUM_POS)
#define  OTG_HS_GRXSTSP_FRMNUM(x)           ((uint32_t)(   x << OTG_HS_GRXSTSP_FRMNUM_POS) & OTG_HS_GRXSTSP_FRMNUM_MSK)

// OTG_HS Receive FIFO size register (OTG_HS_GRXFSIZ)
#define  OTG_HS_GRXFSIZ_RXFD_POS            ((uint32_t)          0)
#define  OTG_HS_GRXFSIZ_RXFD_MSK            ((uint32_t)0xFFFF<< OTG_HS_GRXFSIZ_RXFD_POS)
#define  OTG_HS_GRXFSIZ_RXFD(x)             ((uint32_t)(   x << OTG_HS_GRXFSIZ_RXFD_POS) & OTG_HS_GRXFSIZ_RXFD_MSK)

// OTG_HS Host non-periodic transmit FIFO size register (OTG_HS_HNPTXFSIZ) 
#define  OTG_HS_HNPTXFSIZ_NPTXFSA_POS       ((uint32_t)          0)
#define  OTG_HS_HNPTXFSIZ_NPTXFSA_MSK       ((uint32_t)0xFFFF<< OTG_HS_HNPTXFSIZ_NPTXFSA_POS)
#define  OTG_HS_HNPTXFSIZ_NPTXFSA(x)        ((uint32_t)(   x << OTG_HS_HNPTXFSIZ_NPTXFSA_POS) & OTG_HS_HNPTXFSIZ_NPTXFSA_MSK)
#define  OTG_HS_HNPTXFSIZ_NPTXFD_POS        ((uint32_t)         16)
#define  OTG_HS_HNPTXFSIZ_NPTXFD_MSK        ((uint32_t)0xFFFF<< OTG_HS_HNPTXFSIZ_NPTXFD_POS)
#define  OTG_HS_HNPTXFSIZ_NPTXFD(x)         ((uint32_t)(   x << OTG_HS_HNPTXFSIZ_NPTXFD_POS) & OTG_HS_HNPTXFSIZ_NPTXFD_MSK)

// OTG_HS Endpoint 0 transmit FIFO size register (OTG_HS_DIEPTXF0)
#define  OTG_HS_DIEPTXF0_TX0FSA_POS         ((uint32_t)          0)
#define  OTG_HS_DIEPTXF0_TX0FSA_MSK         ((uint32_t)0xFFFF<< OTG_HS_DIEPTXF0_TX0FSA_POS)
#define  OTG_HS_DIEPTXF0_TX0FSA(x)          ((uint32_t)(   x << OTG_HS_DIEPTXF0_TX0FSA_POS) & OTG_HS_DIEPTXF0_TX0FSA_MSK)
#define  OTG_HS_DIEPTXF0_TX0FD_POS          ((uint32_t)         16)
#define  OTG_HS_DIEPTXF0_TX0FD_MSK          ((uint32_t)0xFFFF<< OTG_HS_DIEPTXF0_TX0FD_POS)
#define  OTG_HS_DIEPTXF0_TX0FD(x)           ((uint32_t)(   x << OTG_HS_DIEPTXF0_TX0FD_POS) & OTG_HS_DIEPTXF0_TX0FD_MSK)

// OTG_HS Host non-periodic transmit FIFO/queue status register (OTG_HS_HNPTXSTS) 
#define  OTG_HS_HNPTXSTS_NPTXFSAV_POS       ((uint32_t)          0)
#define  OTG_HS_HNPTXSTS_NPTXFSAV_MSK       ((uint32_t)0xFFFF<< OTG_HS_HNPTXSTS_NPTXFSAV_POS)
#define  OTG_HS_HNPTXSTS_NPTXFSAV(x)        ((uint32_t)(   x << OTG_HS_HNPTXSTS_NPTXFSAV_POS) & OTG_HS_HNPTXSTS_NPTXFSAV_MSK)
#define  OTG_HS_HNPTXSTS_NPTQXSAV_POS       ((uint32_t)         16)
#define  OTG_HS_HNPTXSTS_NPTQXSAV_MSK       ((uint32_t) 0xFF << OTG_HS_HNPTXSTS_NPTQXSAV_POS)
#define  OTG_HS_HNPTXSTS_NPTQXSAV(x)        ((uint32_t)(   x << OTG_HS_HNPTXSTS_NPTQXSAV_POS) & OTG_HS_HNPTXSTS_NPTQXSAV_MSK)
#define  OTG_HS_HNPTXSTS_NPTXQTOP_POS       ((uint32_t)         24)
#define  OTG_HS_HNPTXSTS_NPTXQTOP_MSK       ((uint32_t) 0x7F << OTG_HS_HNPTXSTS_NPTXQTOP_POS)
#define  OTG_HS_HNPTXSTS_NPTXQTOP(x)        ((uint32_t)(   x << OTG_HS_HNPTXSTS_NPTXQTOP_POS) & OTG_HS_HNPTXSTS_NPTXQTOP_MSK)

// OTG_HS I2C access register (OTG_HS_GI2CCTL) 
#define  OTG_HS_GI2CCTL_RWDATA_POS          ((uint32_t)          0)
#define  OTG_HS_GI2CCTL_RWDATA_MSK          ((uint32_t) 0xFF << OTG_HS_GI2CCTL_RWDATA_POS)
#define  OTG_HS_GI2CCTL_RWDATA(x)           ((uint32_t)(   x << OTG_HS_GI2CCTL_RWDATA_POS) & OTG_HS_GI2CCTL_RWDATA_MSK)
#define  OTG_HS_GI2CCTL_REGADDR_POS         ((uint32_t)          8)
#define  OTG_HS_GI2CCTL_REGADDR_MSK         ((uint32_t) 0xFF << OTG_HS_GI2CCTL_REGADDR_POS)
#define  OTG_HS_GI2CCTL_REGADDR(x)          ((uint32_t)(   x << OTG_HS_GI2CCTL_REGADDR_POS) & OTG_HS_GI2CCTL_REGADDR_MSK)
#define  OTG_HS_GI2CCTL_ADDR_POS            ((uint32_t)         16)
#define  OTG_HS_GI2CCTL_ADDR_MSK            ((uint32_t) 0xFF << OTG_HS_GI2CCTL_ADDR_POS)
#define  OTG_HS_GI2CCTL_ADDR(x)             ((uint32_t)(   x << OTG_HS_GI2CCTL_ADDR_POS) & OTG_HS_GI2CCTL_ADDR_MSK)
#define  OTG_HS_GI2CCTL_I2CEN               ((uint32_t)    1 << 23)
#define  OTG_HS_GI2CCTL_ACK                 ((uint32_t)    1 << 24)
#define  OTG_HS_GI2CCTL_I2CDEVADR_POS       ((uint32_t)         26)
#define  OTG_HS_GI2CCTL_I2CDEVADR_MSK       ((uint32_t)    3 << OTG_HS_GI2CCTL_I2CDEVADR_POS)
#define  OTG_HS_GI2CCTL_I2CDEVADR(x)        ((uint32_t)(   x << OTG_HS_GI2CCTL_I2CDEVADR_POS) & OTG_HS_GI2CCTL_I2CDEVADR_MSK)
#define  OTG_HS_GI2CCTL_I2CDATSE0           ((uint32_t)    1 << 28)
#define  OTG_HS_GI2CCTL_RW                  ((uint32_t)    1 << 30)
#define  OTG_HS_GI2CCTL_BSYDNE              ((uint32_t)    1 << 31)

// OTG_HS general core configuration register (OTG_HS_GCCFG)
#define  OTG_HS_GCCFG_PWRDWN                ((uint32_t)    1 << 16)
#define  OTG_HS_GCCFG_I2CPADEN              ((uint32_t)    1 << 17)
#define  OTG_HS_GCCFG_VBUSASEN              ((uint32_t)    1 << 18)
#define  OTG_HS_GCCFG_VBUSBSEN              ((uint32_t)    1 << 19)
#define  OTG_HS_GCCFG_SOFOUTEN              ((uint32_t)    1 << 20)
#define  OTG_HS_GCCFG_NOVBUSSENS            ((uint32_t)    1 << 21)

// OTG_HS Host periodic transmit FIFO size register (OTG_HS_HPTXFSIZ) 
#define  OTG_HS_HPTXFSIZ_PTXSA_POS          ((uint32_t)          0)
#define  OTG_HS_HPTXFSIZ_PTXSA_MSK          ((uint32_t)0xFFFF<< OTG_HS_HPTXFSIZ_PTXSA_POS)
#define  OTG_HS_HPTXFSIZ_PTXSA(x)           ((uint32_t)(   x << OTG_HS_HPTXFSIZ_PTXSA_POS) & OTG_HS_HPTXFSIZ_PTXSA_MSK)
#define  OTG_HS_HPTXFSIZ_PTXFD_POS          ((uint32_t)         16)
#define  OTG_HS_HPTXFSIZ_PTXFD_MSK          ((uint32_t)0xFFFF<< OTG_HS_HPTXFSIZ_PTXFD_POS)
#define  OTG_HS_HPTXFSIZ_PTXFD(x)           ((uint32_t)(   x << OTG_HS_HPTXFSIZ_PTXFD_POS) & OTG_HS_HPTXFSIZ_PTXFD_MSK)

// OTG_HS device IN endpoint transmit FIFO size register (OTG_HS_DIEPTXFx)
#define  OTG_HS_DIEPTXFx_INEPTXSA_POS       ((uint32_t)          0)
#define  OTG_HS_DIEPTXFx_INEPTXSA_MSK       ((uint32_t)0xFFFF<< OTG_HS_DIEPTXFx_INEPTXSA_POS)
#define  OTG_HS_DIEPTXFx_INEPTXSA(x)        ((uint32_t)(   x << OTG_HS_DIEPTXFx_INEPTXSA_POS) & OTG_HS_DIEPTXFx_INEPTXSA_MSK)
#define  OTG_HS_DIEPTXFx_INEPTXFD_POS       ((uint32_t)         16)
#define  OTG_HS_DIEPTXFx_INEPTXFD_MSK       ((uint32_t)0xFFFF<< OTG_HS_DIEPTXFx_INEPTXFD_POS)
#define  OTG_HS_DIEPTXFx_INEPTXFD(x)        ((uint32_t)(   x << OTG_HS_DIEPTXFx_INEPTXFD_POS) & OTG_HS_DIEPTXFx_INEPTXFD_MSK)

// OTG_HS Host configuration register (OTG_HS_HCFG)
#define  OTG_HS_HCFG_FSLSPCS_POS            ((uint32_t)          0)
#define  OTG_HS_HCFG_FSLSPCS_MSK            ((uint32_t)    3 << OTG_HS_HCFG_FSLSPCS_POS)
#define  OTG_HS_HCFG_FSLSPCS(x)             ((uint32_t)(   x << OTG_HS_HCFG_FSLSPCS_POS) & OTG_HS_HCFG_FSLSPCS_MSK)
#define  OTG_HS_HCFG_FSLSS                  ((uint32_t)    1 <<  2)

// OTG_HS Host frame interval register (OTG_HS_HFIR)
#define  OTG_HS_HFIR_FRIVL_POS              ((uint32_t)          0)
#define  OTG_HS_HFIR_FRIVL_MSK              ((uint32_t)0xFFFF<< OTG_HS_HFIR_FRIVL_POS)
#define  OTG_HS_HFIR_FRIVL(x)               ((uint32_t)(   x << OTG_HS_HFIR_FRIVL_POS) & OTG_HS_HFIR_FRIVL_MSK)

// OTG_HS Host frame number/frame time remaining register (OTG_HS_HFNUM) 
#define  OTG_HS_HFNUM_FRNUM_POS             ((uint32_t)          0)
#define  OTG_HS_HFNUM_FRNUM_MSK             ((uint32_t)0xFFFF<< OTG_HS_HFNUM_FRNUM_POS)
#define  OTG_HS_HFNUM_FRNUM(x)              ((uint32_t)(   x << OTG_HS_HFNUM_FRNUM_POS) & OTG_HS_HFNUM_FRNUM_MSK)
#define  OTG_HS_HFNUM_FTREM_POS             ((uint32_t)         16)
#define  OTG_HS_HFNUM_FTREM_MSK             ((uint32_t)0xFFFF<< OTG_HS_HFNUM_FRNUM_POS)
#define  OTG_HS_HFNUM_FTREM(x)              ((uint32_t)(   x << OTG_HS_HFNUM_FRNUM_POS) & OTG_HS_HFNUM_FRNUM_MSK)

// OTG_HS Host periodic transmit FIFO/queue status register (OTG_HS_HPTXSTS) 
#define  OTG_HS_HPTXSTS_PTXFSAVL_POS        ((uint32_t)          0)
#define  OTG_HS_HPTXSTS_PTXFSAVL_MSK        ((uint32_t)0xFFFF<< OTG_HS_HPTXSTS_PTXFSAVL_POS)
#define  OTG_HS_HPTXSTS_PTXFSAVL(x)         ((uint32_t)(   x << OTG_HS_HPTXSTS_PTXFSAVL_POS) & OTG_HS_HPTXSTS_PTXFSAVL_MSK)
#define  OTG_HS_HPTXSTS_PTXQSAV_POS         ((uint32_t)         16)
#define  OTG_HS_HPTXSTS_PTXQSAV_MSK         ((uint32_t) 0xFF << OTG_HS_HPTXSTS_PTXQSAV_POS)
#define  OTG_HS_HPTXSTS_PTXQSAV(x)          ((uint32_t)(   x << OTG_HS_HPTXSTS_PTXQSAV_POS) & OTG_HS_HPTXSTS_PTXQSAV_MSK)
#define  OTG_HS_HPTXSTS_PTXQTOP_POS         ((uint32_t)         24)
#define  OTG_HS_HPTXSTS_PTXQTOP_MSK         ((uint32_t) 0xFF << OTG_HS_HPTXSTS_PTXQTOP_POS)
#define  OTG_HS_HPTXSTS_PTXQTOP(x)          ((uint32_t)(   x << OTG_HS_HPTXSTS_PTXQTOP_POS) & OTG_HS_HPTXSTS_PTXQTOP_MSK)

// OTG_HS Host all channels interrupt register (OTG_HS_HAINT)
#define  OTG_HS_HAINT_HAINT_POS             ((uint32_t)          0)
#define  OTG_HS_HAINT_HAINT_MSK             ((uint32_t)0xFFFF<< OTG_HS_HAINT_HAINT_POS)
#define  OTG_HS_HAINT_HAINT(x)              ((uint32_t)(   x << OTG_HS_HAINT_HAINT_POS) & OTG_HS_HAINT_HAINT_MSK)

// OTG_HS Host all channels interrupt mask register (OTG_HS_HAINTMSK)
#define  OTG_HS_HAINTMSK_HAINTM_POS         ((uint32_t)          0)
#define  OTG_HS_HAINTMSK_HAINTM_MSK         ((uint32_t)0xFFFF<< OTG_HS_HAINTMSK_HAINTM_POS)
#define  OTG_HS_HAINTMSK_HAINTM(x)          ((uint32_t)(   x << OTG_HS_HAINTMSK_HAINTM_POS) & OTG_HS_HAINTMSK_HAINTM_MSK)

// OTG_HS Host port control and status register (OTG_HS_HPRT)
#define  OTG_HS_HPRT_PCSTS                  ((uint32_t)    1 <<  0)
#define  OTG_HS_HPRT_PCDET                  ((uint32_t)    1 <<  1)
#define  OTG_HS_HPRT_PENA                   ((uint32_t)    1 <<  2)
#define  OTG_HS_HPRT_PENCHNG                ((uint32_t)    1 <<  3)
#define  OTG_HS_HPRT_POCA                   ((uint32_t)    1 <<  4)
#define  OTG_HS_HPRT_POCCHNG                ((uint32_t)    1 <<  5)
#define  OTG_HS_HPRT_PRES                   ((uint32_t)    1 <<  6)
#define  OTG_HS_HPRT_PSUSP                  ((uint32_t)    1 <<  7)
#define  OTG_HS_HPRT_PRST                   ((uint32_t)    1 <<  8)
#define  OTG_HS_HPRT_PLSTS_POS              ((uint32_t)         10)
#define  OTG_HS_HPRT_PLSTS_MSK              ((uint32_t)    3 << OTG_HS_HPRT_PLSTS_POS)
#define  OTG_HS_HPRT_PLSTS(x)               ((uint32_t)(   x << OTG_HS_HPRT_PLSTS_POS) & OTG_HS_HPRT_PLSTS_MSK)
#define  OTG_HS_HPRT_PPWR                   ((uint32_t)    1 << 12)
#define  OTG_HS_HPRT_PTCTL_POS              ((uint32_t)         13)
#define  OTG_HS_HPRT_PTCTL_MSK              ((uint32_t) 0x0F << OTG_HS_HPRT_PTCTL_POS)
#define  OTG_HS_HPRT_PTCTL(x)               ((uint32_t)(   x << OTG_HS_HPRT_PTCTL_POS) & OTG_HS_HPRT_PTCTL_MSK)
#define  OTG_HS_HPRT_PSPD_POS               ((uint32_t)         17)
#define  OTG_HS_HPRT_PSPD_MSK               ((uint32_t)    3 << OTG_HS_HPRT_PSPD_POS)
#define  OTG_HS_HPRT_PSPD(x)                ((uint32_t)(   x << OTG_HS_HPRT_PSPD_POS) & OTG_HS_HPRT_PSPD_MSK)

// OTG_HS Host channel-x characteristics register (OTG_HS_HCCHARx)
#define  OTG_HS_HCCHARx_MPSIZ_POS           ((uint32_t)          0)
#define  OTG_HS_HCCHARx_MPSIZ_MSK           ((uint32_t)0x7FF << OTG_HS_HCCHARx_MPSIZ_POS)
#define  OTG_HS_HCCHARx_MPSIZ(x)            ((uint32_t)(   x << OTG_HS_HCCHARx_MPSIZ_POS) & OTG_HS_HCCHARx_MPSIZ_MSK)
#define  OTG_HS_HCCHARx_EPNUM_POS           ((uint32_t)         11)
#define  OTG_HS_HCCHARx_EPNUM_MSK           ((uint32_t) 0x0F << OTG_HS_HCCHARx_EPNUM_POS)
#define  OTG_HS_HCCHARx_EPNUM(x)            ((uint32_t)(   x << OTG_HS_HCCHARx_EPNUM_POS) & OTG_HS_HCCHARx_EPNUM_MSK)
#define  OTG_HS_HCCHARx_EPDIR               ((uint32_t)    1 << 15)
#define  OTG_HS_HCCHARx_LSDEV               ((uint32_t)    1 << 17)
#define  OTG_HS_HCCHARx_EPTYP_POS           ((uint32_t)         18)
#define  OTG_HS_HCCHARx_EPTYP_MSK           ((uint32_t)    3 << OTG_HS_HCCHARx_EPTYP_POS)
#define  OTG_HS_HCCHARx_EPTYP(x)            ((uint32_t)(   x << OTG_HS_HCCHARx_EPTYP_POS) & OTG_HS_HCCHARx_EPTYP_MSK)
#define  OTG_HS_HCCHARx_MC_POS              ((uint32_t)         20)
#define  OTG_HS_HCCHARx_MC_MSK              ((uint32_t)    3 << OTG_HS_HCCHARx_MC_POS)
#define  OTG_HS_HCCHARx_MC(x)               ((uint32_t)(   x << OTG_HS_HCCHARx_MC_POS) & OTG_HS_HCCHARx_MC_MSK)
#define  OTG_HS_HCCHARx_DAD_POS             ((uint32_t)         22)
#define  OTG_HS_HCCHARx_DAD_MSK             ((uint32_t) 0x7F << OTG_HS_HCCHARx_DAD_POS)
#define  OTG_HS_HCCHARx_DAD(x)              ((uint32_t)(   x << OTG_HS_HCCHARx_DAD_POS) & OTG_HS_HCCHARx_DAD_MSK)
#define  OTG_HS_HCCHARx_ODDFRM              ((uint32_t)    1 << 29)
#define  OTG_HS_HCCHARx_CHDIS               ((uint32_t)    1 << 30)
#define  OTG_HS_HCCHARx_CHENA               ((uint32_t)    1 << 31)

// OTG_HS Host channel-x split control register (OTG_HS_HCSPLTx)
#define  OTG_HS_HCSPLTx_PRTADDR_POS         ((uint32_t)          0)
#define  OTG_HS_HCSPLTx_PRTADDR_MSK         ((uint32_t) 0x7F << OTG_HS_HCSPLTx_PRTADDR_POS)
#define  OTG_HS_HCSPLTx_PRTADDR(x)          ((uint32_t)(   x << OTG_HS_HCSPLTx_PRTADDR_POS) & OTG_HS_HCSPLTx_PRTADDR_MSK)
#define  OTG_HS_HCSPLTx_HUBADDR_POS         ((uint32_t)          7)
#define  OTG_HS_HCSPLTx_HUBADDR_MSK         ((uint32_t) 0x7F << OTG_HS_HCSPLTx_HUBADDR_POS)
#define  OTG_HS_HCSPLTx_HUBADDR(x)          ((uint32_t)(   x << OTG_HS_HCSPLTx_HUBADDR_POS) & OTG_HS_HCSPLTx_HUBADDR_MSK)
#define  OTG_HS_HCSPLTx_XACTPOS_POS         ((uint32_t)         14)
#define  OTG_HS_HCSPLTx_XACTPOS_MSK         ((uint32_t)    3 << OTG_HS_HCSPLTx_XACTPOS_POS)
#define  OTG_HS_HCSPLTx_XACTPOS(x)          ((uint32_t)(   x << OTG_HS_HCSPLTx_XACTPOS_POS) & OTG_HS_HCSPLTx_XACTPOS_MSK)
#define  OTG_HS_HCSPLTx_COMPLSPLT           ((uint32_t)    1 << 16)
#define  OTG_HS_HCSPLTx_SPLITEN             ((uint32_t)    1 << 31)

// OTG_HS Host channel-x interrupt register (OTG_HS_HCINTx)
#define  OTG_HS_HCINTx_XFRC                 ((uint32_t)    1 <<  0)
#define  OTG_HS_HCINTx_CHH                  ((uint32_t)    1 <<  1)
#define  OTG_HS_HCINTx_AHBERR               ((uint32_t)    1 <<  2)
#define  OTG_HS_HCINTx_STALL                ((uint32_t)    1 <<  3)
#define  OTG_HS_HCINTx_NAK                  ((uint32_t)    1 <<  4)
#define  OTG_HS_HCINTx_ACK                  ((uint32_t)    1 <<  5)
#define  OTG_HS_HCINTx_NYET                 ((uint32_t)    1 <<  6)
#define  OTG_HS_HCINTx_TXERR                ((uint32_t)    1 <<  7)
#define  OTG_HS_HCINTx_BBERR                ((uint32_t)    1 <<  8)
#define  OTG_HS_HCINTx_FRMOR                ((uint32_t)    1 <<  9)
#define  OTG_HS_HCINTx_DTERR                ((uint32_t)    1 << 10)
#define  OTG_HS_HCINTx_ERR                  ((uint32_t) 0x0F <<  7)

// OTG_HS Host channel-x interrupt mask register (OTG_HS_HCINTMSKx)
#define  OTG_HS_HCINTMSKx_XFRCM             ((uint32_t)    1 <<  0)
#define  OTG_HS_HCINTMSKx_CHHM              ((uint32_t)    1 <<  1)
#define  OTG_HS_HCINTMSKx_AHBERR            ((uint32_t)    1 <<  2)
#define  OTG_HS_HCINTMSKx_STALLM            ((uint32_t)    1 <<  3)
#define  OTG_HS_HCINTMSKx_NAKM              ((uint32_t)    1 <<  4)
#define  OTG_HS_HCINTMSKx_ACKM              ((uint32_t)    1 <<  5)
#define  OTG_HS_HCINTMSKx_NYET              ((uint32_t)    1 <<  6)
#define  OTG_HS_HCINTMSKx_TXERRM            ((uint32_t)    1 <<  7)
#define  OTG_HS_HCINTMSKx_BBERRM            ((uint32_t)    1 <<  8)
#define  OTG_HS_HCINTMSKx_FRMORM            ((uint32_t)    1 <<  9)
#define  OTG_HS_HCINTMSKx_DTERRM            ((uint32_t)    1 << 10)

// OTG_HS Host channel-x transfer size register (OTG_HS_HCTSIZx)
#define  OTG_HS_HCTSIZx_XFRSIZ_POS          ((uint32_t)          0)
#define  OTG_HS_HCTSIZx_XFRSIZ_MSK          ((uint32_t)0x7FFFF<<OTG_HS_HCTSIZx_XFRSIZ_POS)
#define  OTG_HS_HCTSIZx_XFRSIZ(x)           ((uint32_t)(   x << OTG_HS_HCTSIZx_XFRSIZ_POS) & OTG_HS_HCTSIZx_XFRSIZ_MSK)
#define  OTG_HS_HCTSIZx_PKTCNT_POS          ((uint32_t)         19)
#define  OTG_HS_HCTSIZx_PKTCNT_MSK          ((uint32_t)0x3FF << OTG_HS_HCTSIZx_PKTCNT_POS)
#define  OTG_HS_HCTSIZx_PKTCNT(x)           ((uint32_t)(   x << OTG_HS_HCTSIZx_PKTCNT_POS) & OTG_HS_HCTSIZx_PKTCNT_MSK)
#define  OTG_HS_HCTSIZx_DPID_POS            ((uint32_t)         29)
#define  OTG_HS_HCTSIZx_DPID_MSK            ((uint32_t)    3 << OTG_HS_HCTSIZx_DPID_POS)
#define  OTG_HS_HCTSIZx_DPID(x)             ((uint32_t)(   x << OTG_HS_HCTSIZx_DPID_POS) & OTG_HS_HCTSIZx_DPID_MSK)
#define  OTG_HS_HCTSIZx_DPID_DATA0          (OTG_HS_HCTSIZx_DPID(0))
#define  OTG_HS_HCTSIZx_DPID_DATA2          (OTG_HS_HCTSIZx_DPID(1))
#define  OTG_HS_HCTSIZx_DPID_DATA1          (OTG_HS_HCTSIZx_DPID(2))
#define  OTG_HS_HCTSIZx_DPID_MDATA          (OTG_HS_HCTSIZx_DPID(3))
#define  OTG_HS_HCTSIZx_DPID_SETUP          (OTG_HS_HCTSIZx_DPID(3))
#define  OTG_HS_HCISIZx_DOPING              ((uint32_t)    1 << 31)

// OTG_HS device configuration register (OTG_HS_DCFG)
#define  OTG_HS_DCFG_DSPD_POS               ((uint32_t)          0)
#define  OTG_HS_DCFG_DSPD_MSK               ((uint32_t)    3 << OTG_HS_DCFG_DSPD_POS)
#define  OTG_HS_DCFG_DSPD(x)                ((uint32_t)(   x << OTG_HS_DCFG_DSPD_POS) & OTG_HS_DCFG_DSPD_MSK)
#define  OTG_HS_DCFG_NZLSOHSK               ((uint32_t)    1 <<  2)
#define  OTG_HS_DCFG_DAD_POS                ((uint32_t)          4)
#define  OTG_HS_DCFG_DAD_MSK                ((uint32_t) 0x7F << OTG_HS_DCFG_DAD_POS)
#define  OTG_HS_DCFG_DAD(x)                 ((uint32_t)(   x << OTG_HS_DCFG_DAD_POS) & OTG_HS_DCFG_DAD_MSK)
#define  OTG_HS_DCFG_PFIVL_POS              ((uint32_t)         11)
#define  OTG_HS_DCFG_PFIVL_MSK              ((uint32_t)    3 << OTG_HS_DCFG_PFIVL_POS)
#define  OTG_HS_DCFG_PFIVL(x)               ((uint32_t)(   x << OTG_HS_DCFG_PFIVL_POS) & OTG_HS_DCFG_PFIVL_MSK)
#define  OTG_HS_DCFG_PERSCHIVL_POS          ((uint32_t)         24)
#define  OTG_HS_DCFG_PERSCHIVL_MSK          ((uint32_t)    3 << OTG_HS_DCFG_PERSCHIVL_POS)
#define  OTG_HS_DCFG_PERSCHIVL(x)           ((uint32_t)(   x << OTG_HS_DCFG_PERSCHIVL_POS) & OTG_HS_DCFG_PERSCHIVL_MSK)

// OTG_HS device control register (OTG_HS_DCTL)
#define  OTG_HS_DCTL_RWUSIG                 ((uint32_t)    1 <<  0)
#define  OTG_HS_DCTL_SDIS                   ((uint32_t)    1 <<  1)
#define  OTG_HS_DCTL_GINSTS                 ((uint32_t)    1 <<  2)
#define  OTG_HS_DCTL_GONSTS                 ((uint32_t)    1 <<  3)
#define  OTG_HS_DCTL_TCTL_POS               ((uint32_t)          4)
#define  OTG_HS_DCTL_TCTL_MSK               ((uint32_t)    7 << OTG_HS_DCTL_TCTL_POS)
#define  OTG_HS_DCTL_TCTL(x)                ((uint32_t)(   x << OTG_HS_DCTL_TCTL_POS) & OTG_HS_DCTL_TCTL_MSK)
#define  OTG_HS_DCTL_SGINAK                 ((uint32_t)    1 <<  7)
#define  OTG_HS_DCTL_CGINAK                 ((uint32_t)    1 <<  8)
#define  OTG_HS_DCTL_SGONAK                 ((uint32_t)    1 <<  9)
#define  OTG_HS_DCTL_CGONAK                 ((uint32_t)    1 << 10)
#define  OTG_HS_DCTL_POPRGDNE               ((uint32_t)    1 << 11)

// OTG_HS device status register (OTG_HS_DSTS)
#define  OTG_HS_DSTS_SUSPSTS                ((uint32_t)    1 <<  0)
#define  OTG_HS_DSTS_ENUMSPD_POS            ((uint32_t)          1)
#define  OTG_HS_DSTS_ENUMSPD_MSK            ((uint32_t)    3 << OTG_HS_DSTS_ENUMSPD_POS)
#define  OTG_HS_DSTS_ENUMSPD(x)             ((uint32_t)(   x << OTG_HS_DSTS_ENUMSPD_POS) & OTG_HS_DSTS_ENUMSPD_MSK)
#define  OTG_HS_DSTS_EERR                   ((uint32_t)    1 <<  3)
#define  OTG_HS_DSTS_FNSOF_POS              ((uint32_t)          8)
#define  OTG_HS_DSTS_FNSOF_MSK              ((uint32_t)0x3FFF<< OTG_HS_DSTS_FNSOF_POS)
#define  OTG_HS_DSTS_FNSOF(x)               ((uint32_t)(   x << OTG_HS_DSTS_FNSOF_POS) & OTG_HS_DSTS_FNSOF_MSK)

// OTG_HS device IN endpoint common interrupt mask register (OTG_HS_DIEPMSK)
#define  OTG_HS_DIEPMSK_XFRCM               ((uint32_t)    1 <<  0)
#define  OTG_HS_DIEPMSK_EPDM                ((uint32_t)    1 <<  1)
#define  OTG_HS_DIEPMSK_TOM                 ((uint32_t)    1 <<  3)
#define  OTG_HS_DIEPMSK_ITTXFEMSK           ((uint32_t)    1 <<  4)
#define  OTG_HS_DIEPMSK_INEPNMM             ((uint32_t)    1 <<  5)
#define  OTG_HS_DIEPMSK_INEPNEM             ((uint32_t)    1 <<  6)
#define  OTG_HS_DIEPMSK_TXFURM              ((uint32_t)    1 <<  8)
#define  OTG_HS_DIEPMSK_BIM                 ((uint32_t)    1 <<  9)

// OTG_HS device OUT endpoint common interrupt mask register (OTG_HS_DOEPMSK)
#define  OTG_HS_DOEPMSK_XFRCM               ((uint32_t)    1 <<  0)
#define  OTG_HS_DOEPMSK_EPDM                ((uint32_t)    1 <<  1)
#define  OTG_HS_DOEPMSK_STUPM               ((uint32_t)    1 <<  3)
#define  OTG_HS_DOEPMSK_OTEPDM              ((uint32_t)    1 <<  4)
#define  OTG_HS_DOEPMSK_B2BSTUP             ((uint32_t)    1 <<  6)
#define  OTG_HS_DOEPMSK_OPEM                ((uint32_t)    1 <<  8)
#define  OTG_HS_DOEPMSK_BOIM                ((uint32_t)    1 <<  9)

// OTG_HS device all endpoints interrupt register (OTG_HS_DAINT)
#define  OTG_HS_DAINT_IEPINT(in_ep)         ((uint32_t)((1 <<        in_ep)  & 0x0000FFFF))
#define  OTG_HS_DAINT_OEPINT(out_ep)        ((uint32_t)((1 << (16 + out_ep)) & 0xFFFF0000))

// OTG_HS device all endpoints interrupt mask register (OTG_HS_DAINTMSK)
#define  OTG_HS_DAINTMSK_IEPM(in_ep)        ((uint32_t)((1 <<        in_ep)  & 0x0000FFFF))
#define  OTG_HS_DAINTMSK_OEPM(out_ep)       ((uint32_t)((1 << (16 + out_ep)) & 0xFFFF0000))

// OTG_HS device Vbus discharge time register (OTG_HS_DVBUSDIS)
#define  OTG_HS_DVBUSDIS_VBUSDT_POS         ((uint32_t)          0)
#define  OTG_HS_DVBUSDIS_VBUSDT_MSK         ((uint32_t)0xFFFF<< OTG_HS_DVBUSDIS_VBUSDT_POS)
#define  OTG_HS_DVBUSDIS_VBUSDT(x)          ((uint32_t)(   x << OTG_HS_DVBUSDIS_VBUSDT_POS) & OTG_HS_DVBUSDIS_VBUSDT_MSK)

// OTG_HS device Vbus pulsing time register (OTG_HS_DVBUSPULSE)
#define  OTG_HS_DVBUSPULSE_DVBUSP_POS       ((uint32_t)          0)
#define  OTG_HS_DVBUSPULSE_DVBUSP_MSK       ((uint32_t)0xFFF << OTG_HS_DVBUSPULSE_DVBUSP_POS)
#define  OTG_HS_DVBUSPULSE_DVBUSP(x)        ((uint32_t)(   x << OTG_HS_DVBUSPULSE_DVBUSP_POS) & OTG_HS_DVBUSPULSE_DVBUSP)

// OTG_HS Device threshold control register (OTG_HS_DTHRCTL)
#define  OTG_HS_DTHRCTL_NONISOTHREN         ((uint32_t)    1 <<  0)
#define  OTG_HS_DTHRCTL_ISOTHREN            ((uint32_t)    1 <<  1)
#define  OTG_HS_DTHRCTL_TXTHRLEN_POS        ((uint32_t)          2)
#define  OTG_HS_DTHRCTL_TXTHRLEN_MSK        ((uint32_t)0x1FF << OTG_HS_DTHRCTL_TXTHRLEN_POS)
#define  OTG_HS_DTHRCTL_TXTHRLEN(x)         ((uint32_t)(   x << OTG_HS_DTHRCTL_TXTHRLEN_POS) & OTG_HS_DTHRCTL_TXTHRLEN_MSK)
#define  OTG_HS_DTHRCTL_RXTHREN             ((uint32_t)    1 << 16)
#define  OTG_HS_DTHRCTL_RXTHRLEN_POS        ((uint32_t)         17)
#define  OTG_HS_DTHRCTL_RXTHRLEN_MSK        ((uint32_t)0x1FF << OTG_HS_DTHRCTL_RXTHRLEN_POS)
#define  OTG_HS_DTHRCTL_RXTHRLEN(x)         ((uint32_t)(   x << OTG_HS_DTHRCTL_RXTHRLEN_POS) & OTG_HS_DTHRCTL_RXTHRLEN_MSK)
#define  OTG_HS_DTHRCTL_ARPEN               ((uint32_t)    1 << 27)

// OTG_HS device IN endpoint FIFO emtpy interrupt mask register (OTG_HS_DIEPEMPMSK)
#define  OTG_HS_DIEPEMPMSK_INEPTXFEM_POS    ((uint32_t)          0)
#define  OTG_HS_DIEPEMPMSK_INEPTXFEM_MSK    ((uint32_t)0xFFFF<< OTG_HS_DIEPEMPMSK_INEPTXFEM_POS)
#define  OTG_HS_DIEPEMPMSK_INEPTXFEM(x)     ((uint32_t)(   x << OTG_HS_DIEPEMPMSK_INEPTXFEM_POS) & OTG_HS_DIEPEMPMSK_INEPTXFEM_MSK)

// OTG_HS device each endpoint interrupt register (OTG_HS_DEACHINT)
#define  OTG_HS_DEACHINT_IEP1INT            ((uint32_t)    1 <<  1)
#define  OTG_HS_DEACHINT_OEP1INT            ((uint32_t)    1 << 17)

// OTG_HS device each endpoint interrupt register mask (OTG_HS_DEACHINTMSK)
#define  OTG_HS_DEACHINTMSK_IEP1INTM        ((uint32_t)    1 <<  1)
#define  OTG_HS_DEACHINTMSK_OEP1INTM        ((uint32_t)    1 << 17)

// OTG_HS device each in endpoint-1 interrupt register (OTG_HS_DIEPEACHMSK1)
#define  OTG_HS_DIEPEACHMSK1_XFRCM          ((uint32_t)    1 <<  0)
#define  OTG_HS_DIEPEACHMSK1_EPDM           ((uint32_t)    1 <<  1)
#define  OTG_HS_DIEPEACHMSK1_TOM            ((uint32_t)    1 <<  3)
#define  OTG_HS_DIEPEACHMSK1_ITTXFEMSK      ((uint32_t)    1 <<  4)
#define  OTG_HS_DIEPEACHMSK1_INEPNMM        ((uint32_t)    1 <<  5)
#define  OTG_HS_DIEPEACHMSK1_INEPNEM        ((uint32_t)    1 <<  6)
#define  OTG_HS_DIEPEACHMSK1_TXFURM         ((uint32_t)    1 <<  8)
#define  OTG_HS_DIEPEACHMSK1_BIM            ((uint32_t)    1 <<  9)
#define  OTG_HS_DIEPEACHMSK1_NAKM           ((uint32_t)    1 << 13)

// OTG_HS device each OUT endpoint-1 interrupt register (OTG_HS_DOEPEACHMSK1)
#define  OTG_HS_DOEPEACHMSK1_XFRCM          ((uint32_t)    1 <<  0)
#define  OTG_HS_DOEPEACHMSK1_EPDM           ((uint32_t)    1 <<  1)
#define  OTG_HS_DOEPEACHMSK1_AHBERRM        ((uint32_t)    1 <<  2)
#define  OTG_HS_DOEPEACHMSK1_OPEM           ((uint32_t)    1 <<  8)
#define  OTG_HS_DOEPEACHMSK1_BIM            ((uint32_t)    1 <<  9)
#define  OTG_HS_DOEPEACHMSK1_BERRM          ((uint32_t)    1 << 12)
#define  OTG_HS_DOEPEACHMSK1_NAKM           ((uint32_t)    1 << 13)
#define  OTG_HS_DOEPEACHMSK1_NYETM          ((uint32_t)    1 << 14)

// OTG_HS device endpoint-x control register (OTG_HS_DIEPCTLx)
#define  OTG_HS_DIEPCTLx_MPSIZ_POS          ((uint32_t)          0)
#define  OTG_HS_DIEPCTLx_MPSIZ_MSK          ((uint32_t)0x7FF << OTG_HS_DIEPCTLx_MPSIZ_POS)
#define  OTG_HS_DIEPCTLx_MPSIZ(x)           ((uint32_t)(   x << OTG_HS_DIEPCTLx_MPSIZ_POS) & OTG_HS_DIEPCTLx_MPSIZ_MSK)
#define  OTG_HS_DIEPCTLx_USBAEP             ((uint32_t)    1 << 15)
#define  OTG_HS_DIEPCTLx_EONUM_POS          ((uint32_t)         16)
#define  OTG_HS_DIEPCTLx_EONUM              ((uint32_t)    1 << OTG_HS_DIEPCTLx_EONUM_POS)
#define  OTG_HS_DIEPCTLx_DPID               ((uint32_t)    1 << 16)
#define  OTG_HS_DIEPCTLx_NAKSTS             ((uint32_t)    1 << 17)
#define  OTG_HS_DIEPCTLx_EPTYP_POS          ((uint32_t)         18)
#define  OTG_HS_DIEPCTLx_EPTYP_MSK          ((uint32_t)    3 << OTG_HS_DIEPCTLx_EPTYP_POS)
#define  OTG_HS_DIEPCTLx_EPTYP(x)           ((uint32_t)(   x << OTG_HS_DIEPCTLx_EPTYP_POS) & OTG_HS_DIEPCTLx_EPTYP_MSK)
#define  OTG_HS_DIEPCTLx_STALL              ((uint32_t)    1 << 21)
#define  OTG_HS_DIEPCTLx_TXFNUM_POS         ((uint32_t)         22)
#define  OTG_HS_DIEPCTLx_TXFNUM_MSK         ((uint32_t) 0x0F << OTG_HS_DIEPCTLx_TXFNUM_POS)
#define  OTG_HS_DIEPCTLx_TXFNUM(x)          ((uint32_t)(   x << OTG_HS_DIEPCTLx_TXFNUM_POS) & OTG_HS_DIEPCTLx_TXFNUM_MSK)
#define  OTG_HS_DIEPCTLx_CNAK               ((uint32_t)    1 << 26)
#define  OTG_HS_DIEPCTLx_SNAK               ((uint32_t)    1 << 27)
#define  OTG_HS_DIEPCTLx_SD0PID             ((uint32_t)    1 << 28)
#define  OTG_HS_DIEPCTLx_SEVNFRM            ((uint32_t)    1 << 28)
#define  OTG_HS_DIEPCTLx_SODDFRM            ((uint32_t)    1 << 29)
#define  OTG_HS_DIEPCTLx_EPDIS              ((uint32_t)    1 << 30)
#define  OTG_HS_DIEPCTLx_EPENA              ((uint32_t)    1 << 31)

// OTG_HS device control OUT endpoint 0 control register (OTG_HS_DOEPCTL0)
#define  OTG_HS_DOEPCTL0_MPSIZ_POS          ((uint32_t)          0)
#define  OTG_HS_DOEPCTL0_MPSIZ_MSK          ((uint32_t)    3 << OTG_HS_DOEPCTL0_MPSIZ_POS)
#define  OTG_HS_DOEPCTL0_MPSIZ(x)           ((uint32_t)(   x << OTG_HS_DOEPCTL0_MPSIZ_POS) & OTG_HS_DOEPCTL0_MPSIZ_MSK)
#define  OTG_HS_DOEPCTL0_USBAEP             ((uint32_t)    1 << 15)
#define  OTG_HS_DOEPCTL0_NAKSTS             ((uint32_t)    1 << 17)
#define  OTG_HS_DOEPCTL0_EPTYP_POS          ((uint32_t)         18)
#define  OTG_HS_DOEPCTL0_EPTYP_MSK          ((uint32_t)    3 << OTG_HS_DOEPCTL0_EPTYP_POS)
#define  OTG_HS_DOEPCTL0_EPTYP(x)           ((uint32_t)(   x << OTG_HS_DOEPCTL0_EPTYP_POS) & OTG_HS_DOEPCTL0_EPTYP_MSK)
#define  OTG_HS_DOEPCTL0_SNPM               ((uint32_t)    1 << 20)
#define  OTG_HS_DOEPCTL0_STALL              ((uint32_t)    1 << 21)
#define  OTG_HS_DOEPCTL0_CNAK               ((uint32_t)    1 << 26)
#define  OTG_HS_DOEPCTL0_SNAK               ((uint32_t)    1 << 27)
#define  OTG_HS_DOEPCTL0_EPDIS              ((uint32_t)    1 << 30)
#define  OTG_HS_DOEPCTL0_EPENA              ((uint32_t)    1 << 31)

// OTG_HS device endpoint-x control register (OTG_HS_DOEPCTLx)
#define  OTG_HS_DOEPCTLx_MPSIZ_POS          ((uint32_t)          0)
#define  OTG_HS_DOEPCTLx_MPSIZ_MSK          ((uint32_t)0x7FF << OTG_HS_DOEPCTLx_MPSIZ_POS)
#define  OTG_HS_DOEPCTLx_MPSIZ(x)           ((uint32_t)(   x << OTG_HS_DOEPCTLx_MPSIZ_POS) & OTG_HS_DOEPCTLx_MPSIZ_MSK)
#define  OTG_HS_DOEPCTLx_USBAEP             ((uint32_t)    1 << 15)
#define  OTG_HS_DOEPCTLx_EONUM_POS          ((uint32_t)         16)
#define  OTG_HS_DOEPCTLx_EONUM              ((uint32_t)    1 << OTG_HS_DOEPCTLx_EONUM_POS)
#define  OTG_HS_DOEPCTLx_DPID               ((uint32_t)    1 << 16)
#define  OTG_HS_DOEPCTLx_NAKSTS             ((uint32_t)    1 << 17)
#define  OTG_HS_DOEPCTLx_EPTYP_POS          ((uint32_t)         18)
#define  OTG_HS_DOEPCTLx_EPTYP_MSK          ((uint32_t)    3 << OTG_HS_DOEPCTLx_EPTYP_POS)
#define  OTG_HS_DOEPCTLx_EPTYP(x)           ((uint32_t)(   x << OTG_HS_DOEPCTLx_EPTYP_POS) & OTG_HS_DOEPCTLx_EPTYP_MSK)
#define  OTG_HS_DOEPCTLx_SNPM               ((uint32_t)    1 << 20)
#define  OTG_HS_DOEPCTLx_STALL              ((uint32_t)    1 << 21)
#define  OTG_HS_DOEPCTLx_CNAK               ((uint32_t)    1 << 26)
#define  OTG_HS_DOEPCTLx_SNAK               ((uint32_t)    1 << 27)
#define  OTG_HS_DOEPCTLx_SD0PID             ((uint32_t)    1 << 28)
#define  OTG_HS_DOEPCTLx_SEVNFRM            ((uint32_t)    1 << 28)
#define  OTG_HS_DOEPCTLx_SODDFRM            ((uint32_t)    1 << 29)
#define  OTG_HS_DOEPCTLx_EPDIS              ((uint32_t)    1 << 30)
#define  OTG_HS_DOEPCTLx_EPENA              ((uint32_t)    1 << 31)

// OTG_HS device endpoint-x interrupt register (OTG_HS_DIEPINTx)
#define  OTG_HS_DIEPINTx_XFCR               ((uint32_t)    1 <<  0)
#define  OTG_HS_DIEPINTx_EPDISD             ((uint32_t)    1 <<  1)
#define  OTG_HS_DIEPINTx_TOC                ((uint32_t)    1 <<  3)
#define  OTG_HS_DIEPINTx_ITTXFE             ((uint32_t)    1 <<  4)
#define  OTG_HS_DIEPINTx_INEPNE             ((uint32_t)    1 <<  6)
#define  OTG_HS_DIEPINTx_TXFE               ((uint32_t)    1 <<  7)
#define  OTG_HS_DIEPINTx_TXFIFOUDRN         ((uint32_t)    1 <<  8)
#define  OTG_HS_DIEPINTx_BNA                ((uint32_t)    1 <<  9)
#define  OTG_HS_DIEPINTx_PKTDRPSTS          ((uint32_t)    1 << 11)
#define  OTG_HS_DIEPINTx_BERR               ((uint32_t)    1 << 12)
#define  OTG_HS_DIEPINTx_NAK                ((uint32_t)    1 << 13)

// OTG_HS device endpoint-x interrupt register (OTG_HS_DOEPINTx)
#define  OTG_HS_DOEPINTx_XFCR               ((uint32_t)    1 <<  0)
#define  OTG_HS_DOEPINTx_EPDISD             ((uint32_t)    1 <<  1)
#define  OTG_HS_DOEPINTx_STUP               ((uint32_t)    1 <<  3)
#define  OTG_HS_DOEPINTx_OTEPDIS            ((uint32_t)    1 <<  4)
#define  OTG_HS_DOEPINTx_B2BSTUP            ((uint32_t)    1 <<  6)
#define  OTG_HS_DOEPINTx_NYET               ((uint32_t)    1 << 14)

// OTG_HS device IN endpoint 0 transfer size register (OTG_HS_DIEPTSIZ0)
#define  OTG_HS_DIEPTSIZ0_XFRSIZ_POS        ((uint32_t)          0)
#define  OTG_HS_DIEPTSIZ0_XFRSIZ_MSK        ((uint32_t) 0x7F << OTG_HS_DIEPTSIZ0_XFRSIZ_POS)
#define  OTG_HS_DIEPTSIZ0_XFRSIZ(x)         ((uint32_t)(   x << OTG_HS_DIEPTSIZ0_XFRSIZ_POS) & OTG_HS_DIEPTSIZ0_XFRSIZ_MSK)
#define  OTG_HS_DIEPTSIZ0_PKTCNT_POS        ((uint32_t)         19)
#define  OTG_HS_DIEPTSIZ0_PKTCNT_MSK        ((uint32_t)    3 << OTG_HS_DIEPTSIZ0_PKTCNT_POS)
#define  OTG_HS_DIEPTSIZ0_PKTCNT(x)         ((uint32_t)(   x << OTG_HS_DIEPTSIZ0_PKTCNT_POS) & OTG_HS_DIEPTSIZ0_PKTCNT_MSK)

// OTG_HS device OUT endpoint 0 transfer size register (OTG_HS_DOEPTSIZ0)
#define  OTG_HS_DOEPTSIZ0_XFRSIZ_POS        ((uint32_t)          0)
#define  OTG_HS_DOEPTSIZ0_XFRSIZ_MSK        ((uint32_t) 0x7F << OTG_HS_DOEPTSIZ0_XFRSIZ_POS)
#define  OTG_HS_DOEPTSIZ0_XFRSIZ(x)         ((uint32_t)(   x << OTG_HS_DOEPTSIZ0_XFRSIZ_POS) & OTG_HS_DOEPTSIZ0_XFRSIZ_MSK)
#define  OTG_HS_DOEPTSIZ0_PKTCNT            ((uint32_t)    1 << 19)
#define  OTG_HS_DOEPTSIZ0_STUPCNT_POS       ((uint32_t)         29)
#define  OTG_HS_DOEPTSIZ0_STUPCNT_MSK       ((uint32_t)    3 << OTG_HS_DOEPTSIZ0_STUPCNT_POS)
#define  OTG_HS_DOEPTSIZ0_STUPCNT(x)        ((uint32_t)(   x << OTG_HS_DOEPTSIZ0_STUPCNT_POS) & OTG_HS_DOEPTSIZ0_STUPCNT_MSK)

// OTG_HS device IN endpoints transver size register (OTG_HS_DIEPTSIZx)
#define  OTG_HS_DIEPTSIZx_XFRSIZ_POS        ((uint32_t)          0)
#define  OTG_HS_DIEPTSIZx_XFRSIZ_MSK        ((uint32_t)0x7FFFF<<OTG_HS_DIEPTSIZx_XFRSIZ_POS)
#define  OTG_HS_DIEPTSIZx_XFRSIZ(x)         ((uint32_t)(   x << OTG_HS_DIEPTSIZx_XFRSIZ_POS) & OTG_HS_DIEPTSIZx_XFRSIZ_MSK)
#define  OTG_HS_DIEPTSIZx_PKTCNT_POS        ((uint32_t)         19)
#define  OTG_HS_DIEPTSIZx_PKTCNT_MSK        ((uint32_t)0x3FF << OTG_HS_DIEPTSIZx_PKTCNT_POS)
#define  OTG_HS_DIEPTSIZx_PKTCNT(x)         ((uint32_t)(   x << OTG_HS_DIEPTSIZx_PKTCNT_POS) & OTG_HS_DIEPTSIZx_PKTCNT_MSK)
#define  OTG_HS_DIEPTSIZx_MCNT_POS          ((uint32_t)         29)
#define  OTG_HS_DIEPTSIZx_MCNT_MSK          ((uint32_t)    3 << OTG_HS_DIEPTSIZx_MCNT_POS)
#define  OTG_HS_DIEPTSIZx_MCNT(x)           ((uint32_t)(   x << OTG_HS_DIEPTSIZx_MCNT_POS) & OTG_HS_DIEPTSIZx_MCNT_MSK)

// OTG_HS device IN endpoint transmit FIFO status register (OTG_HS_DTXFSTSx)
#define  OTG_HS_DTXFSTSx_INEPTFSAV_POS      ((uint32_t)          0)
#define  OTG_HS_DTXFSTSx_INEPTFSAV_MSK      ((uint32_t)0xFFFF<< OTG_HS_DTXFSTSx_INEPTFSAV_POS)
#define  OTG_HS_DTXFSTSx_INEPTFSAV(x)       ((uint32_t)(   x << OTG_HS_DTXFSTSx_INEPTFSAV_POS) & OTG_HS_DTXFSTSx_INEPTFSAV_MSK)

// OTG_HS device OUT endpoint-x transfer size register (OTG_HS_DOEPTSIZx)
#define  OTG_HS_DOEPTSIZx_XFRSIZ_POS        ((uint32_t)          0)
#define  OTG_HS_DOEPTSIZx_XFRSIZ_MSK        ((uint32_t)0x7FFFF<<OTG_HS_DOEPTSIZx_XFRSIZ_POS)
#define  OTG_HS_DOEPTSIZx_XFRSIZ(x)         ((uint32_t)(   x << OTG_HS_DOEPTSIZx_XFRSIZ_POS) & OTG_HS_DOEPTSIZx_XFRSIZ_MSK)
#define  OTG_HS_DOEPTSIZx_PKTCNT_POS        ((uint32_t)         19)
#define  OTG_HS_DOEPTSIZx_PKTCNT_MSK        ((uint32_t)0x3FF << OTG_HS_DOEPTSIZx_PKTCNT_POS)
#define  OTG_HS_DOEPTSIZx_PKTCNT(x)         ((uint32_t)(   x << OTG_HS_DOEPTSIZx_PKTCNT_POS) & OTG_HS_DOEPTSIZx_PKTCNT_MSK)
#define  OTG_HS_DOEPTSIZx_RXDPID_POS        ((uint32_t)         29)
#define  OTG_HS_DOEPTSIZx_RXDPID_MSK        ((uint32_t)    3 << OTG_HS_DOEPTSIZx_RXDPID_POS)
#define  OTG_HS_DOEPTSIZx_RXDPID(x)         ((uint32_t)(   x << OTG_HS_DOEPTSIZx_RXDPID_POS) & OTG_HS_DOEPTSIZx_RXDPID_MSK)
#define  OTG_HS_DOEPTSIZx_STUPCNT_POS       ((uint32_t)         29)
#define  OTG_HS_DOEPTSIZx_STUPCNT_MSK       ((uint32_t)    3 << OTG_HS_DOEPTSIZx_STUPCNT_POS)
#define  OTG_HS_DOEPTSIZx_STUPCNT(x)        ((uint32_t)(   x << OTG_HS_DOEPTSIZx_STUPCNT_POS) & OTG_HS_DOEPTSIZx_STUPCNT_MSK)

// OTG_HS power and gate clock gating control register (OTG_HS_PCGCCTL)
#define  OTG_HS_PCGCCTL_STPPCLK             ((uint32_t)    1 <<  0)
#define  OTG_HS_PCGCCTL_GATEHCLK            ((uint32_t)    1 <<  1)
#define  OTG_HS_PCGCCTL_PHYSUSP             ((uint32_t)    1 <<  4)

#pragma anon_unions

// OTG_FS Host Channel
typedef __packed struct {               /* Host Channel typedef (HC)          */
  __packed union {
    uint32_t HCCHAR;                    /* Channel Characteristics            */
    __packed struct {
      uint32_t MPSIZ     : 11;          /* Endpoint Maximum Packet Size       */
      uint32_t EPNUM     :  4;          /* Endpoint Number                    */
      uint32_t EPDIR     :  1;          /* Endpoint Direction                 */
      uint32_t Reserved0 :  1;          /* Reserved                           */
      uint32_t LSDEV     :  1;          /* Endpoint Low-speed                 */
      uint32_t EPTYP     :  2;          /* Endpoint Type                      */
      uint32_t MC        :  2;          /* Multicount (MC) / Error Count (EC) */
      uint32_t DAD       :  7;          /* Device Address                     */
      uint32_t ODDFRM    :  1;          /* Periodic Transaction Odd Frame     */
      uint32_t CHDIS     :  1;          /* Channel Disable                    */
      uint32_t CHENA     :  1;          /* Channel Enable                     */
    };
  };
  __packed union {
    uint32_t HCSPLT;                    /* Split Control                      */
    __packed struct {
      uint32_t PRTADDR   :  7;          /* Port Address                       */
      uint32_t HUBADDR   :  7;          /* HUB Address                        */
      uint32_t XACTPOS   :  2;          /* Transaction Position               */
      uint32_t COMPLSPLT :  1;          /* Do Complete Split                  */
      uint32_t Reserved1 : 14;          /* Reserved                           */
      uint32_t SPLITEN   :  1;          /* Split Enable                       */
    };
  };

  __packed union {
    uint32_t HCINT;                     /* Channel Interrupt                  */
    __packed struct {
      uint32_t XFCR      :  1;          /* Transfer Completed                 */
      uint32_t CHH       :  1;          /* Channel Halted                     */
      uint32_t AHBERR    :  1;          /* AHB Error                          */
      uint32_t STALL     :  1;          /* STALL Response Received Interrupt  */
      uint32_t NAK       :  1;          /* NAK Response Received Interrupt    */
      uint32_t ACK       :  1;          /* ACK Response Rece/Transmit Int     */
      uint32_t NYET      :  1;          /* NYET Response Received Interrupt   */
      uint32_t TXERR     :  1;          /* Transaction Error                  */
      uint32_t BBERR     :  1;          /* Babble Error                       */
      uint32_t FRMOR     :  1;          /* Frame Overrun                      */
      uint32_t DTERR     :  1;          /* Data Toggle Error                  */
    };
  };
  __packed union {
    uint32_t HCINTMSK;                  /* Channel Interrupt Mask             */
    __packed struct {
      uint32_t XFCRM     :  1;          /* Transfer Completed Mask            */
      uint32_t CHHM      :  1;          /* Channel Halted Mask                */
      uint32_t AHBERRM   :  1;          /* AHB Error                          */
      uint32_t STALLM    :  1;          /* STALL Response Rece Interrupt Mask */
      uint32_t NAKM      :  1;          /* NAK Response Rece Interrupt Mask   */
      uint32_t ACKM      :  1;          /* ACK Response Rece/Transmit Int Mask*/
      uint32_t NYETM     :  1;          /* NYET Response Rece/Transmit Int Msk*/
      uint32_t TXERRM    :  1;          /* Transaction Error Mask             */
      uint32_t BBERRM    :  1;          /* Babble Error Mask                  */
      uint32_t FRMORM    :  1;          /* Frame Overrun Mask                 */
      uint32_t DTERRM    :  1;          /* Data Toggle Error Mask             */
    };
  };
  __packed union {
    uint32_t HCTSIZ;                    /* Channel Transfer Size              */
    __packed struct {
      uint32_t XFRSIZ    : 19;          /* Transfer Size                      */
      uint32_t PKTCNT    : 10;          /* Packet Count                       */
      uint32_t DPID      :  2;          /* Data PID                           */
      uint32_t DOPING    :  1;          /* Do Pingd                           */
    };
  };
  uint32_t DMAADDR;                     /* DMA Address                        */
  uint32_t Reserved8;                   /* Reserved                           */
  uint32_t Reserved9;                   /* Reserved                           */
} OTG_HS_HC;

#pragma no_anon_unions

