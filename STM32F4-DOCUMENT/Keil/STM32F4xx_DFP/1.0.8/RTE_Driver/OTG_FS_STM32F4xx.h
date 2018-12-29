/* ----------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        13. May 2013
 * $Revision:    V1.01
 * $Comment:     Based on Reference Manual RM0090 Rev 4 (February 2013)
 *  
 * Project:      OTG Full/Low-Speed Driver Header for ST STM32F4xx
 * -------------------------------------------------------------------- */

#include <stdint.h>

// OTG_FS control and status register (OTG_FS_GOTGCTL)
#define  OTG_FS_GOTGCTL_SRQSCS              ((uint32_t)    1 <<  0)
#define  OTG_FS_GOTGCTL_SRQ                 ((uint32_t)    1 <<  1)
#define  OTG_FS_GOTGCTL_HNGSCS              ((uint32_t)    1 <<  8)
#define  OTG_FS_GOTGCTL_HNPRQ               ((uint32_t)    1 <<  9)
#define  OTG_FS_GOTGCTL_HSHNPEN             ((uint32_t)    1 << 10)
#define  OTG_FS_GOTGCTL_DHNPEN              ((uint32_t)    1 << 11)
#define  OTG_FS_GOTGCTL_CIDSTS              ((uint32_t)    1 << 16)
#define  OTG_FS_GOTGCTL_DBCT                ((uint32_t)    1 << 17)
#define  OTG_FS_GOTGCTL_ASVLD               ((uint32_t)    1 << 18)
#define  OTG_FS_GOTGCTL_BSVLD               ((uint32_t)    1 << 19)

// OTG_FS interrupt register (OTG_FS_GOTGINT)
#define  OTG_FS_GOTGINT_SEDET               ((uint32_t)    1 <<  2)
#define  OTG_FS_GOTGINT_SRSSCHG             ((uint32_t)    1 <<  8)
#define  OTG_FS_GOTGINT_HNSSCHG             ((uint32_t)    1 <<  9)
#define  OTG_FS_GOTGINT_HNGDET              ((uint32_t)    1 << 17)
#define  OTG_FS_GOTGINT_ADTOCHG             ((uint32_t)    1 << 18)
#define  OTG_FS_GOTGINT_DBCDNE              ((uint32_t)    1 << 19)

// OTG_FS AHB configuration register (OTG_FS_GAHBCFG)
#define  OTG_FS_GAHBCFG_GINTMSK             ((uint32_t)    1 <<  0)
#define  OTG_FS_GAHBCFG_TXFELVL             ((uint32_t)    1 <<  7)
#define  OTG_FS_GAHBCFG_PTXFELVL            ((uint32_t)    1 <<  8)

// OTG_FS USB configuration register (OTG_FS_GUSBCFG)
#define  OTG_FS_GUSBCFG_TOCAL_POS           ((uint32_t)          0)
#define  OTG_FS_GUSBCFG_TOCAL_MSK           ((uint32_t)    7 <<  OTG_FS_GUSBCFG_TOCAL_POS)
#define  OTG_FS_GUSBCFG_TOCAL(x)            ((uint32_t)(   x <<  OTG_FS_GUSBCFG_TOCAL_POS) & OTG_FS_GUSBCFG_TOCAL_MSK)
#define  OTG_FS_GUSBCFG_PHYSEL              ((uint32_t)    1 <<  6)
#define  OTG_FS_GUSBCFG_SRPCAP              ((uint32_t)    1 <<  8)
#define  OTG_FS_GUSBCFG_HNPCAP              ((uint32_t)    1 <<  9)
#define  OTG_FS_GUSBCFG_TRDT_POS            ((uint32_t)         10)
#define  OTG_FS_GUSBCFG_TRDT_MSK            ((uint32_t) 0x0F << OTG_FS_GUSBCFG_TRDT_POS)
#define  OTG_FS_GUSBCFG_TRDT(x)             ((uint32_t)(   x << OTG_FS_GUSBCFG_TRDT_POS) & OTG_FS_GUSBCFG_TRDT_MSK)
#define  OTG_FS_GUSBCFG_FHMOD               ((uint32_t)    1 << 29)
#define  OTG_FS_GUSBCFG_FDMOD               ((uint32_t)    1 << 30)
#define  OTG_FS_GUSBCFG_CTXPKT              ((uint32_t)    1 << 31)

// OTG_FS reset register (OTG_FS_GRSTCTL)
#define  OTG_FS_GRSTCTL_CSRST               ((uint32_t)    1 <<  0)
#define  OTG_FS_GRSTCTL_HSRST               ((uint32_t)    1 <<  1)
#define  OTG_FS_GRSTCTL_FCRST               ((uint32_t)    1 <<  2)
#define  OTG_FS_GRSTCTL_RXFFLSH             ((uint32_t)    1 <<  4)
#define  OTG_FS_GRSTCTL_TXFFLSH             ((uint32_t)    1 <<  5)
#define  OTG_FS_GRSTCTL_TXFNUM_POS          ((uint32_t)          6)
#define  OTG_FS_GRSTCTL_TXFNUM_MSK          ((uint32_t) 0x1F <<  OTG_FS_GRSTCTL_TXFNUM_POS)
#define  OTG_FS_GRSTCTL_TXFNUM(x)           ((uint32_t)(   x <<  OTG_FS_GRSTCTL_TXFNUM_POS) & OTG_FS_GRSTCTL_TXFNUM_MSK)
#define  OTG_FS_GRSTCTL_AHBIDL              ((uint32_t)    1 << 31)

// OTG_FS core interrupt register (OTG_FS_GINTSTS)
#define  OTG_FS_GINTSTS_CMOD                ((uint32_t)    1 <<  0)
#define  OTG_FS_GINTSTS_MMIS                ((uint32_t)    1 <<  1)
#define  OTG_FS_GINTSTS_OTGINT              ((uint32_t)    1 <<  2)
#define  OTG_FS_GINTSTS_SOF                 ((uint32_t)    1 <<  3)
#define  OTG_FS_GINTSTS_RXFLVL              ((uint32_t)    1 <<  4)
#define  OTG_FS_GINTSTS_NPTXFE              ((uint32_t)    1 <<  5)
#define  OTG_FS_GINTSTS_GINAKEFF            ((uint32_t)    1 <<  6)
#define  OTG_FS_GINTSTS_GONAKEFF            ((uint32_t)    1 <<  7)
#define  OTG_FS_GINTSTS_ESUSP               ((uint32_t)    1 << 10)
#define  OTG_FS_GINTSTS_USBSUSP             ((uint32_t)    1 << 11)
#define  OTG_FS_GINTSTS_USBRST              ((uint32_t)    1 << 12)
#define  OTG_FS_GINTSTS_ENUMDNE             ((uint32_t)    1 << 13)
#define  OTG_FS_GINTSTS_ISOODRP             ((uint32_t)    1 << 14)
#define  OTG_FS_GINTSTS_EOPF                ((uint32_t)    1 << 15)
#define  OTG_FS_GINTSTS_IEPINT              ((uint32_t)    1 << 18)
#define  OTG_FS_GINTSTS_OEPINT              ((uint32_t)    1 << 19)
#define  OTG_FS_GINTSTS_IISOIXFR            ((uint32_t)    1 << 20)
#define  OTG_FS_GINTSTS_IPXFR               ((uint32_t)    1 << 21)
#define  OTG_FS_GINTSTS_INCOMPISOOUT        ((uint32_t)    1 << 21)
#define  OTG_FS_GINTSTS_HPRTINT             ((uint32_t)    1 << 24)
#define  OTG_FS_GINTSTS_HCINT               ((uint32_t)    1 << 25)
#define  OTG_FS_GINTSTS_PTXFE               ((uint32_t)    1 << 26)
#define  OTG_FS_GINTSTS_CIDSCHG             ((uint32_t)    1 << 28)
#define  OTG_FS_GINTSTS_DISCINT             ((uint32_t)    1 << 29)
#define  OTG_FS_GINTSTS_SRQINT              ((uint32_t)    1 << 30)
#define  OTG_FS_GINTSTS_WKUPINT             ((uint32_t)    1 << 31)

// OTG_FS interrupt mask register (OTG_FS_GINTMSK)
#define  OTG_FS_GINTMSK_MMISM               ((uint32_t)    1 <<  1)
#define  OTG_FS_GINTMSK_OTGINT              ((uint32_t)    1 <<  2)
#define  OTG_FS_GINTMSK_SOFM                ((uint32_t)    1 <<  3)
#define  OTG_FS_GINTMSK_RXFLVLM             ((uint32_t)    1 <<  4)
#define  OTG_FS_GINTMSK_NPTXFEM             ((uint32_t)    1 <<  5)
#define  OTG_FS_GINTMSK_GINAKEFFM           ((uint32_t)    1 <<  6)
#define  OTG_FS_GINTMSK_GONAKEFFM           ((uint32_t)    1 <<  7)
#define  OTG_FS_GINTMSK_ESUSPM              ((uint32_t)    1 << 10)
#define  OTG_FS_GINTMSK_USBSUSPM            ((uint32_t)    1 << 11)
#define  OTG_FS_GINTMSK_USBRST              ((uint32_t)    1 << 12)
#define  OTG_FS_GINTMSK_ENUMDNEM            ((uint32_t)    1 << 13)
#define  OTG_FS_GINTMSK_ISOODRPM            ((uint32_t)    1 << 14)
#define  OTG_FS_GINTMSK_EOPFM               ((uint32_t)    1 << 15)
#define  OTG_FS_GINTMSK_EPMISM              ((uint32_t)    1 << 17)
#define  OTG_FS_GINTMSK_IEPINT              ((uint32_t)    1 << 18)
#define  OTG_FS_GINTMSK_OEPINT              ((uint32_t)    1 << 19)
#define  OTG_FS_GINTMSK_IISOIXFRM           ((uint32_t)    1 << 20)
#define  OTG_FS_GINTMSK_IPXFRM              ((uint32_t)    1 << 21)
#define  OTG_FS_GINTMSK_IISOOXFRM           ((uint32_t)    1 << 21)
#define  OTG_FS_GINTMSK_PRTIM               ((uint32_t)    1 << 24)
#define  OTG_FS_GINTMSK_HCIM                ((uint32_t)    1 << 25)
#define  OTG_FS_GINTMSK_PTXFEM              ((uint32_t)    1 << 26)
#define  OTG_FS_GINTMSK_CIDSCHGM            ((uint32_t)    1 << 28)
#define  OTG_FS_GINTMSK_DISCINT             ((uint32_t)    1 << 29)
#define  OTG_FS_GINTMSK_SRQIM               ((uint32_t)    1 << 30)
#define  OTG_FS_GINTMSK_WUIM                ((uint32_t)    1 << 31)

// OTG_FS status debug read register (OTG_FS_GRXSTSR)
#define  OTG_FS_GRXSTSR_CHNUM_POS           ((uint32_t)          0)
#define  OTG_FS_GRXSTSR_CHNUM_MSK           ((uint32_t) 0x0F << OTG_FS_GRXSTSR_CHNUM_POS)
#define  OTG_FS_GRXSTSR_CHNUM(x)            ((uint32_t)(   x << OTG_FS_GRXSTSR_CHNUM_POS) & OTG_FS_GRXSTSR_CHNUM_MSK)
#define  OTG_FS_GRXSTSR_EPNUM_POS           ((uint32_t)          0)
#define  OTG_FS_GRXSTSR_EPNUM_MSK           ((uint32_t) 0x0F << OTG_FS_GRXSTSR_EPNUM_POS)
#define  OTG_FS_GRXSTSR_EPNUM(x)            ((uint32_t)(   x << OTG_FS_GRXSTSR_EPNUM_POS) & OTG_FS_GRXSTSR_EPNUM_MSK)
#define  OTG_FS_GRXSTSR_BCNT_POS            ((uint32_t)          4)
#define  OTG_FS_GRXSTSR_BCNT_MSK            ((uint32_t)0x7FF << OTG_FS_GRXSTSR_BCNT_POS)
#define  OTG_FS_GRXSTSR_BCNT(x)             ((uint32_t)(   x << OTG_FS_GRXSTSR_BCNT_POS) & OTG_FS_GRXSTSR_BCNT_MSK)
#define  OTG_FS_GRXSTSR_DPID_POS            ((uint32_t)         15)
#define  OTG_FS_GRXSTSR_DPID_MSK            ((uint32_t)    3 << OTG_FS_GRXSTSR_DPID_POS)
#define  OTG_FS_GRXSTSR_DPID(x)             ((uint32_t)(   x << OTG_FS_GRXSTSR_DPID_POS) & OTG_FS_GRXSTSR_DPID_MSK)
#define  OTG_FS_GRXSTSR_PKTSTS_POS          ((uint32_t)         17)
#define  OTG_FS_GRXSTSR_PKTSTS_MSK          ((uint32_t) 0x0F << OTG_FS_GRXSTSR_PKTSTS_POS)
#define  OTG_FS_GRXSTSR_PKTSTS(x)           ((uint32_t)(   x << OTG_FS_GRXSTSR_PKTSTS_POS) & OTG_FS_GRXSTSR_PKTSTS_MSK)
#define  OTG_FS_GRXSTSR_FRMNUM_POS          ((uint32_t)         21)
#define  OTG_FS_GRXSTSR_FRMNUM_MSK          ((uint32_t) 0x0F << OTG_FS_GRXSTSR_FRMNUM_POS)
#define  OTG_FS_GRXSTSR_FRMNUM(x)           ((uint32_t)(   x << OTG_FS_GRXSTSR_FRMNUM_POS) & OTG_FS_GRXSTSR_FRMNUM_MSK)

// OTG_FS status read and pop register (OTG_FS_GRXSTSP)
#define  OTG_FS_GRXSTSP_CHNUM_POS           ((uint32_t)          0)
#define  OTG_FS_GRXSTSP_CHNUM_MSK           ((uint32_t) 0x0F << OTG_FS_GRXSTSP_CHNUM_POS)
#define  OTG_FS_GRXSTSP_CHNUM(x)            ((uint32_t)(   x << OTG_FS_GRXSTSP_CHNUM_POS) & OTG_FS_GRXSTSP_CHNUM_MSK)
#define  OTG_FS_GRXSTSP_EPNUM_POS           ((uint32_t)          0)
#define  OTG_FS_GRXSTSP_EPNUM_MSK           ((uint32_t) 0x0F << OTG_FS_GRXSTSP_EPNUM_POS)
#define  OTG_FS_GRXSTSP_EPNUM(x)            ((uint32_t)(   x << OTG_FS_GRXSTSP_EPNUM_POS) & OTG_FS_GRXSTSP_EPNUM_MSK)
#define  OTG_FS_GRXSTSP_BCNT_POS            ((uint32_t)          4)
#define  OTG_FS_GRXSTSP_BCNT_MSK            ((uint32_t)0x7FF << OTG_FS_GRXSTSP_BCNT_POS)
#define  OTG_FS_GRXSTSP_BCNT(x)             ((uint32_t)(   x << OTG_FS_GRXSTSP_BCNT_POS) & OTG_FS_GRXSTSP_BCNT_MSK)
#define  OTG_FS_GRXSTSP_DPID_POS            ((uint32_t)         15)
#define  OTG_FS_GRXSTSP_DPID_MSK            ((uint32_t)    3 << OTG_FS_GRXSTSP_DPID_POS)
#define  OTG_FS_GRXSTSP_DPID(x)             ((uint32_t)(   x << OTG_FS_GRXSTSP_DPID_POS) & OTG_FS_GRXSTSP_DPID_MSK)
#define  OTG_FS_GRXSTSP_PKTSTS_POS          ((uint32_t)         17)
#define  OTG_FS_GRXSTSP_PKTSTS_MSK          ((uint32_t) 0x0F << OTG_FS_GRXSTSP_PKTSTS_POS)
#define  OTG_FS_GRXSTSP_PKTSTS(x)           ((uint32_t)(   x << OTG_FS_GRXSTSP_PKTSTS_POS) & OTG_FS_GRXSTSP_PKTSTS_MSK)
#define  OTG_FS_GRXSTSP_FRMNUM_POS          ((uint32_t)         21)
#define  OTG_FS_GRXSTSP_FRMNUM_MSK          ((uint32_t) 0x0F << OTG_FS_GRXSTSP_FRMNUM_POS)
#define  OTG_FS_GRXSTSP_FRMNUM(x)           ((uint32_t)(   x << OTG_FS_GRXSTSP_FRMNUM_POS) & OTG_FS_GRXSTSP_FRMNUM_MSK)

// OTG_FS Receive FIFO size register (OTG_FS_GRXFSIZ)
#define  OTG_FS_GRXFSIZ_RXFD_POS            ((uint32_t)          0)
#define  OTG_FS_GRXFSIZ_RXFD_MSK            ((uint32_t)0xFFFF<< OTG_FS_GRXFSIZ_RXFD_POS)
#define  OTG_FS_GRXFSIZ_RXFD(x)             ((uint32_t)(   x << OTG_FS_GRXFSIZ_RXFD_POS) & OTG_FS_GRXFSIZ_RXFD_MSK)

// OTG_FS Host non-periodic transmit FIFO size register (OTG_FS_HNPTXFSIZ) 
#define  OTG_FS_HNPTXFSIZ_NPTXFSA_POS       ((uint32_t)          0)
#define  OTG_FS_HNPTXFSIZ_NPTXFSA_MSK       ((uint32_t)0xFFFF<< OTG_FS_HNPTXFSIZ_NPTXFSA_POS)
#define  OTG_FS_HNPTXFSIZ_NPTXFSA(x)        ((uint32_t)(   x << OTG_FS_HNPTXFSIZ_NPTXFSA_POS) & OTG_FS_HNPTXFSIZ_NPTXFSA_MSK)
#define  OTG_FS_HNPTXFSIZ_NPTXFD_POS        ((uint32_t)         16)
#define  OTG_FS_HNPTXFSIZ_NPTXFD_MSK        ((uint32_t)0xFFFF<< OTG_FS_HNPTXFSIZ_NPTXFD_POS)
#define  OTG_FS_HNPTXFSIZ_NPTXFD(x)         ((uint32_t)(   x << OTG_FS_HNPTXFSIZ_NPTXFD_POS) & OTG_FS_HNPTXFSIZ_NPTXFD_MSK)

// OTG_FS Endpoint 0 transmit FIFO size register (OTG_FS_DIEPTXF0)
#define  OTG_FS_DIEPTXF0_TX0FSA_POS         ((uint32_t)          0)
#define  OTG_FS_DIEPTXF0_TX0FSA_MSK         ((uint32_t)0xFFFF<< OTG_FS_DIEPTXF0_TX0FSA_POS)
#define  OTG_FS_DIEPTXF0_TX0FSA(x)          ((uint32_t)(   x << OTG_FS_DIEPTXF0_TX0FSA_POS) & OTG_FS_DIEPTXF0_TX0FSA_MSK)
#define  OTG_FS_DIEPTXF0_TX0FD_POS          ((uint32_t)         16)
#define  OTG_FS_DIEPTXF0_TX0FD_MSK          ((uint32_t)0xFFFF<< OTG_FS_DIEPTXF0_TX0FD_POS)
#define  OTG_FS_DIEPTXF0_TX0FD(x)           ((uint32_t)(   x << OTG_FS_DIEPTXF0_TX0FD_POS) & OTG_FS_DIEPTXF0_TX0FD_MSK)

// OTG_FS Host non-periodic transmit FIFO/queue status register (OTG_FS_HNPTXSTS) 
#define  OTG_FS_HNPTXSTS_NPTXFSAV_POS       ((uint32_t)          0)
#define  OTG_FS_HNPTXSTS_NPTXFSAV_MSK       ((uint32_t)0xFFFF<< OTG_FS_HNPTXSTS_NPTXFSAV_POS)
#define  OTG_FS_HNPTXSTS_NPTXFSAV(x)        ((uint32_t)(   x << OTG_FS_HNPTXSTS_NPTXFSAV_POS) & OTG_FS_HNPTXSTS_NPTXFSAV_MSK)
#define  OTG_FS_HNPTXSTS_NPTQXSAV_POS       ((uint32_t)         16)
#define  OTG_FS_HNPTXSTS_NPTQXSAV_MSK       ((uint32_t) 0xFF << OTG_FS_HNPTXSTS_NPTQXSAV_POS)
#define  OTG_FS_HNPTXSTS_NPTQXSAV(x)        ((uint32_t)(   x << OTG_FS_HNPTXSTS_NPTQXSAV_POS) & OTG_FS_HNPTXSTS_NPTQXSAV_MSK)
#define  OTG_FS_HNPTXSTS_NPTXQTOP_POS       ((uint32_t)         24)
#define  OTG_FS_HNPTXSTS_NPTXQTOP_MSK       ((uint32_t) 0x7F << OTG_FS_HNPTXSTS_NPTXQTOP_POS)
#define  OTG_FS_HNPTXSTS_NPTXQTOP(x)        ((uint32_t)(   x << OTG_FS_HNPTXSTS_NPTXQTOP_POS) & OTG_FS_HNPTXSTS_NPTXQTOP_MSK)

// OTG_FS general core configuration register (OTG_FS_GCCFG)
#define  OTG_FS_GCCFG_PWRDWN                ((uint32_t)    1 << 16)
#define  OTG_FS_GCCFG_VBUSASEN              ((uint32_t)    1 << 18)
#define  OTG_FS_GCCFG_VBUSBSEN              ((uint32_t)    1 << 19)
#define  OTG_FS_GCCFG_SOFOUTEN              ((uint32_t)    1 << 20)
#define  OTG_FS_GCCFG_NOVBUSSENS            ((uint32_t)    1 << 21)

// OTG_FS Host periodic transmit FIFO size register (OTG_FS_HPTXFSIZ) 
#define  OTG_FS_HPTXFSIZ_PTXSA_POS          ((uint32_t)          0)
#define  OTG_FS_HPTXFSIZ_PTXSA_MSK          ((uint32_t)0xFFFF<< OTG_FS_HPTXFSIZ_PTXSA_POS)
#define  OTG_FS_HPTXFSIZ_PTXSA(x)           ((uint32_t)(   x << OTG_FS_HPTXFSIZ_PTXSA_POS) & OTG_FS_HPTXFSIZ_PTXSA_MSK)
#define  OTG_FS_HPTXFSIZ_PTXFD_POS          ((uint32_t)         16)
#define  OTG_FS_HPTXFSIZ_PTXFD_MSK          ((uint32_t)0xFFFF<< OTG_FS_HPTXFSIZ_PTXFD_POS)
#define  OTG_FS_HPTXFSIZ_PTXFD(x)           ((uint32_t)(   x << OTG_FS_HPTXFSIZ_PTXFD_POS) & OTG_FS_HPTXFSIZ_PTXFD_MSK)

// OTG_FS device IN endpoint transmit FIFO size register (OTG_FS_DIEPTXFx)
#define  OTG_FS_DIEPTXFx_INEPTXSA_POS       ((uint32_t)          0)
#define  OTG_FS_DIEPTXFx_INEPTXSA_MSK       ((uint32_t)0xFFFF<< OTG_FS_DIEPTXFx_INEPTXSA_POS)
#define  OTG_FS_DIEPTXFx_INEPTXSA(x)        ((uint32_t)(   x << OTG_FS_DIEPTXFx_INEPTXSA_POS) & OTG_FS_DIEPTXFx_INEPTXSA_MSK)
#define  OTG_FS_DIEPTXFx_INEPTXFD_POS       ((uint32_t)         16)
#define  OTG_FS_DIEPTXFx_INEPTXFD_MSK       ((uint32_t)0xFFFF<< OTG_FS_DIEPTXFx_INEPTXFD_POS)
#define  OTG_FS_DIEPTXFx_INEPTXFD(x)        ((uint32_t)(   x << OTG_FS_DIEPTXFx_INEPTXFD_POS) & OTG_FS_DIEPTXFx_INEPTXFD_MSK)

// OTG_FS Host configuration register (OTG_FS_HCFG)
#define  OTG_FS_HCFG_FSLSPCS_POS            ((uint32_t)          0)
#define  OTG_FS_HCFG_FSLSPCS_MSK            ((uint32_t)    3 << OTG_FS_HCFG_FSLSPCS_POS)
#define  OTG_FS_HCFG_FSLSPCS(x)             ((uint32_t)(   x << OTG_FS_HCFG_FSLSPCS_POS) & OTG_FS_HCFG_FSLSPCS_MSK)
#define  OTG_FS_HCFG_FSLSS                  ((uint32_t)    1 <<  2)

// OTG_FS Host frame interval register (OTG_FS_HFIR)
#define  OTG_FS_HFIR_FRIVL_POS              ((uint32_t)          0)
#define  OTG_FS_HFIR_FRIVL_MSK              ((uint32_t)0xFFFF<< OTG_FS_HFIR_FRIVL_POS)
#define  OTG_FS_HFIR_FRIVL(x)               ((uint32_t)(   x << OTG_FS_HFIR_FRIVL_POS) & OTG_FS_HFIR_FRIVL_MSK)

// OTG_FS Host frame number/frame time remaining register (OTG_FS_HFNUM) 
#define  OTG_FS_HFNUM_FRNUM_POS             ((uint32_t)          0)
#define  OTG_FS_HFNUM_FRNUM_MSK             ((uint32_t)0xFFFF<< OTG_FS_HFNUM_FRNUM_POS)
#define  OTG_FS_HFNUM_FRNUM(x)              ((uint32_t)(   x << OTG_FS_HFNUM_FRNUM_POS) & OTG_FS_HFNUM_FRNUM_MSK)
#define  OTG_FS_HFNUM_FTREM_POS             ((uint32_t)         16)
#define  OTG_FS_HFNUM_FTREM_MSK             ((uint32_t)0xFFFF<< OTG_FS_HFNUM_FRNUM_POS)
#define  OTG_FS_HFNUM_FTREM(x)              ((uint32_t)(   x << OTG_FS_HFNUM_FRNUM_POS) & OTG_FS_HFNUM_FRNUM_MSK)

// OTG_FS Host periodic transmit FIFO/queue status register (OTG_FS_HPTXSTS) 
#define  OTG_FS_HPTXSTS_PTXFSAVL_POS        ((uint32_t)          0)
#define  OTG_FS_HPTXSTS_PTXFSAVL_MSK        ((uint32_t)0xFFFF<< OTG_FS_HPTXSTS_PTXFSAVL_POS)
#define  OTG_FS_HPTXSTS_PTXFSAVL(x)         ((uint32_t)(   x << OTG_FS_HPTXSTS_PTXFSAVL_POS) & OTG_FS_HPTXSTS_PTXFSAVL_MSK)
#define  OTG_FS_HPTXSTS_PTXQSAV_POS         ((uint32_t)         16)
#define  OTG_FS_HPTXSTS_PTXQSAV_MSK         ((uint32_t) 0xFF << OTG_FS_HPTXSTS_PTXQSAV_POS)
#define  OTG_FS_HPTXSTS_PTXQSAV(x)          ((uint32_t)(   x << OTG_FS_HPTXSTS_PTXQSAV_POS) & OTG_FS_HPTXSTS_PTXQSAV_MSK)
#define  OTG_FS_HPTXSTS_PTXQTOP_POS         ((uint32_t)         24)
#define  OTG_FS_HPTXSTS_PTXQTOP_MSK         ((uint32_t) 0xFF << OTG_FS_HPTXSTS_PTXQTOP_POS)
#define  OTG_FS_HPTXSTS_PTXQTOP(x)          ((uint32_t)(   x << OTG_FS_HPTXSTS_PTXQTOP_POS) & OTG_FS_HPTXSTS_PTXQTOP_MSK)

// OTG_FS Host all channels interrupt register (OTG_FS_HAINT)
#define  OTG_FS_HAINT_HAINT_POS             ((uint32_t)          0)
#define  OTG_FS_HAINT_HAINT_MSK             ((uint32_t)0xFFFF<< OTG_FS_HAINT_HAINT_POS)
#define  OTG_FS_HAINT_HAINT(x)              ((uint32_t)(   x << OTG_FS_HAINT_HAINT_POS) & OTG_FS_HAINT_HAINT_MSK)

// OTG_FS Host all channels interrupt mask register (OTG_FS_HAINTMSK)
#define  OTG_FS_HAINTMSK_HAINTM_POS         ((uint32_t)          0)
#define  OTG_FS_HAINTMSK_HAINTM_MSK         ((uint32_t)0xFFFF<< OTG_FS_HAINTMSK_HAINTM_POS)
#define  OTG_FS_HAINTMSK_HAINTM(x)          ((uint32_t)(   x << OTG_FS_HAINTMSK_HAINTM_POS) & OTG_FS_HAINTMSK_HAINTM_MSK)

// OTG_FS Host port control and status register (OTG_FS_HPRT)
#define  OTG_FS_HPRT_PCSTS                  ((uint32_t)    1 <<  0)
#define  OTG_FS_HPRT_PCDET                  ((uint32_t)    1 <<  1)
#define  OTG_FS_HPRT_PENA                   ((uint32_t)    1 <<  2)
#define  OTG_FS_HPRT_PENCHNG                ((uint32_t)    1 <<  3)
#define  OTG_FS_HPRT_POCA                   ((uint32_t)    1 <<  4)
#define  OTG_FS_HPRT_POCCHNG                ((uint32_t)    1 <<  5)
#define  OTG_FS_HPRT_PRES                   ((uint32_t)    1 <<  6)
#define  OTG_FS_HPRT_PSUSP                  ((uint32_t)    1 <<  7)
#define  OTG_FS_HPRT_PRST                   ((uint32_t)    1 <<  8)
#define  OTG_FS_HPRT_PLSTS_POS              ((uint32_t)         10)
#define  OTG_FS_HPRT_PLSTS_MSK              ((uint32_t)    3 << OTG_FS_HPRT_PLSTS_POS)
#define  OTG_FS_HPRT_PLSTS(x)               ((uint32_t)(   x << OTG_FS_HPRT_PLSTS_POS) & OTG_FS_HPRT_PLSTS_MSK)
#define  OTG_FS_HPRT_PPWR                   ((uint32_t)    1 << 12)
#define  OTG_FS_HPRT_PTCTL_POS              ((uint32_t)         13)
#define  OTG_FS_HPRT_PTCTL_MSK              ((uint32_t) 0x0F << OTG_FS_HPRT_PTCTL_POS)
#define  OTG_FS_HPRT_PTCTL(x)               ((uint32_t)(   x << OTG_FS_HPRT_PTCTL_POS) & OTG_FS_HPRT_PTCTL_MSK)
#define  OTG_FS_HPRT_PSPD_POS               ((uint32_t)         17)
#define  OTG_FS_HPRT_PSPD_MSK               ((uint32_t)    3 << OTG_FS_HPRT_PSPD_POS)
#define  OTG_FS_HPRT_PSPD(x)                ((uint32_t)(   x << OTG_FS_HPRT_PSPD_POS) & OTG_FS_HPRT_PSPD_MSK)

// OTG_FS Host channel-x characteristics register (OTG_FS_HCCHARx)
#define  OTG_FS_HCCHARx_MPSIZ_POS           ((uint32_t)          0)
#define  OTG_FS_HCCHARx_MPSIZ_MSK           ((uint32_t)0x7FF << OTG_FS_HCCHARx_MPSIZ_POS)
#define  OTG_FS_HCCHARx_MPSIZ(x)            ((uint32_t)(   x << OTG_FS_HCCHARx_MPSIZ_POS) & OTG_FS_HCCHARx_MPSIZ_MSK)
#define  OTG_FS_HCCHARx_EPNUM_POS           ((uint32_t)         11)
#define  OTG_FS_HCCHARx_EPNUM_MSK           ((uint32_t) 0x0F << OTG_FS_HCCHARx_EPNUM_POS)
#define  OTG_FS_HCCHARx_EPNUM(x)            ((uint32_t)(   x << OTG_FS_HCCHARx_EPNUM_POS) & OTG_FS_HCCHARx_EPNUM_MSK)
#define  OTG_FS_HCCHARx_EPDIR               ((uint32_t)    1 << 15)
#define  OTG_FS_HCCHARx_LSDEV               ((uint32_t)    1 << 17)
#define  OTG_FS_HCCHARx_EPTYP_POS           ((uint32_t)         18)
#define  OTG_FS_HCCHARx_EPTYP_MSK           ((uint32_t)    3 << OTG_FS_HCCHARx_EPTYP_POS)
#define  OTG_FS_HCCHARx_EPTYP(x)            ((uint32_t)(   x << OTG_FS_HCCHARx_EPTYP_POS) & OTG_FS_HCCHARx_EPTYP_MSK)
#define  OTG_FS_HCCHARx_MCNT_POS            ((uint32_t)         20)
#define  OTG_FS_HCCHARx_MCNT_MSK            ((uint32_t)    3 << OTG_FS_HCCHARx_MCNT_POS)
#define  OTG_FS_HCCHARx_MCNT(x)             ((uint32_t)(   x << OTG_FS_HCCHARx_MCNT_POS) & OTG_FS_HCCHARx_MCNT_MSK)
#define  OTG_FS_HCCHARx_DAD_POS             ((uint32_t)         22)
#define  OTG_FS_HCCHARx_DAD_MSK             ((uint32_t) 0x7F << OTG_FS_HCCHARx_DAD_POS)
#define  OTG_FS_HCCHARx_DAD(x)              ((uint32_t)(   x << OTG_FS_HCCHARx_DAD_POS) & OTG_FS_HCCHARx_DAD_MSK)
#define  OTG_FS_HCCHARx_ODDFRM              ((uint32_t)    1 << 29)
#define  OTG_FS_HCCHARx_CHDIS               ((uint32_t)    1 << 30)
#define  OTG_FS_HCCHARx_CHENA               ((uint32_t)    1 << 31)

// OTG_FS Host channel-x interrupt register (OTG_FS_HCINTx)
#define  OTG_FS_HCINTx_XFRC                 ((uint32_t)    1 <<  0)
#define  OTG_FS_HCINTx_CHH                  ((uint32_t)    1 <<  1)
#define  OTG_FS_HCINTx_STALL                ((uint32_t)    1 <<  3)
#define  OTG_FS_HCINTx_NAK                  ((uint32_t)    1 <<  4)
#define  OTG_FS_HCINTx_ACK                  ((uint32_t)    1 <<  5)
#define  OTG_FS_HCINTx_TXERR                ((uint32_t)    1 <<  7)
#define  OTG_FS_HCINTx_BBERR                ((uint32_t)    1 <<  8)
#define  OTG_FS_HCINTx_FRMOR                ((uint32_t)    1 <<  9)
#define  OTG_FS_HCINTx_DTERR                ((uint32_t)    1 << 10)
#define  OTG_FS_HCINTx_ERR                  ((uint32_t) 0x0F <<  7)

// OTG_FS Host channel-x interrupt mask register (OTG_FS_HCINTMSKx)
#define  OTG_FS_HCINTMSKx_XFRCM             ((uint32_t)    1 <<  0)
#define  OTG_FS_HCINTMSKx_CHHM              ((uint32_t)    1 <<  1)
#define  OTG_FS_HCINTMSKx_STALLM            ((uint32_t)    1 <<  3)
#define  OTG_FS_HCINTMSKx_NAKM              ((uint32_t)    1 <<  4)
#define  OTG_FS_HCINTMSKx_ACKM              ((uint32_t)    1 <<  5)
#define  OTG_FS_HCINTMSKx_NYET              ((uint32_t)    1 <<  6)
#define  OTG_FS_HCINTMSKx_TXERRM            ((uint32_t)    1 <<  7)
#define  OTG_FS_HCINTMSKx_BBERRM            ((uint32_t)    1 <<  8)
#define  OTG_FS_HCINTMSKx_FRMORM            ((uint32_t)    1 <<  9)
#define  OTG_FS_HCINTMSKx_DTERRM            ((uint32_t)    1 << 10)

// OTG_FS Host channel-x transfer size register (OTG_FS_HCTSIZx)
#define  OTG_FS_HCTSIZx_XFRSIZ_POS          ((uint32_t)          0)
#define  OTG_FS_HCTSIZx_XFRSIZ_MSK          ((uint32_t)0x7FFFF<<OTG_FS_HCTSIZx_XFRSIZ_POS)
#define  OTG_FS_HCTSIZx_XFRSIZ(x)           ((uint32_t)(   x << OTG_FS_HCTSIZx_XFRSIZ_POS) & OTG_FS_HCTSIZx_XFRSIZ_MSK)
#define  OTG_FS_HCTSIZx_PKTCNT_POS          ((uint32_t)         19)
#define  OTG_FS_HCTSIZx_PKTCNT_MSK          ((uint32_t)0x3FF << OTG_FS_HCTSIZx_PKTCNT_POS)
#define  OTG_FS_HCTSIZx_PKTCNT(x)           ((uint32_t)(   x << OTG_FS_HCTSIZx_PKTCNT_POS) & OTG_FS_HCTSIZx_PKTCNT_MSK)
#define  OTG_FS_HCTSIZx_DPID_POS            ((uint32_t)         29)
#define  OTG_FS_HCTSIZx_DPID_MSK            ((uint32_t)    3 << OTG_FS_HCTSIZx_DPID_POS)
#define  OTG_FS_HCTSIZx_DPID(x)             ((uint32_t)(   x << OTG_FS_HCTSIZx_DPID_POS) & OTG_FS_HCTSIZx_DPID_MSK)
#define  OTG_FS_HCTSIZx_DPID_DATA0          (OTG_FS_HCTSIZx_DPID(0))
#define  OTG_FS_HCTSIZx_DPID_DATA2          (OTG_FS_HCTSIZx_DPID(1))
#define  OTG_FS_HCTSIZx_DPID_DATA1          (OTG_FS_HCTSIZx_DPID(2))
#define  OTG_FS_HCTSIZx_DPID_MDATA          (OTG_FS_HCTSIZx_DPID(3))
#define  OTG_FS_HCTSIZx_DPID_SETUP          (OTG_FS_HCTSIZx_DPID(3))

// OTG_FS device configuration register (OTG_FS_DCFG)
#define  OTG_FS_DCFG_DSPD_POS               ((uint32_t)          0)
#define  OTG_FS_DCFG_DSPD_MSK               ((uint32_t)    3 << OTG_FS_DCFG_DSPD_POS)
#define  OTG_FS_DCFG_DSPD(x)                ((uint32_t)(   x << OTG_FS_DCFG_DSPD_POS) & OTG_FS_DCFG_DSPD_MSK)
#define  OTG_FS_DCFG_NZLSOHSK               ((uint32_t)    1 <<  2)
#define  OTG_FS_DCFG_DAD_POS                ((uint32_t)          4)
#define  OTG_FS_DCFG_DAD_MSK                ((uint32_t) 0x7F << OTG_FS_DCFG_DAD_POS)
#define  OTG_FS_DCFG_DAD(x)                 ((uint32_t)(   x << OTG_FS_DCFG_DAD_POS) & OTG_FS_DCFG_DAD_MSK)
#define  OTG_FS_DCFG_PFIVL_POS              ((uint32_t)         11)
#define  OTG_FS_DCFG_PFIVL_MSK              ((uint32_t)    3 << OTG_FS_DCFG_PFIVL_POS)
#define  OTG_FS_DCFG_PFIVL(x)               ((uint32_t)(   x << OTG_FS_DCFG_PFIVL_POS) & OTG_FS_DCFG_PFIVL_MSK)

// OTG_FS device control register (OTG_FS_DCTL)
#define  OTG_FS_DCTL_RWUSIG                 ((uint32_t)    1 <<  0)
#define  OTG_FS_DCTL_SDIS                   ((uint32_t)    1 <<  1)
#define  OTG_FS_DCTL_GINSTS                 ((uint32_t)    1 <<  2)
#define  OTG_FS_DCTL_GONSTS                 ((uint32_t)    1 <<  3)
#define  OTG_FS_DCTL_TCTL_POS               ((uint32_t)          4)
#define  OTG_FS_DCTL_TCTL_MSK               ((uint32_t)    7 << OTG_FS_DCTL_TCTL_POS)
#define  OTG_FS_DCTL_TCTL(x)                ((uint32_t)(   x << OTG_FS_DCTL_TCTL_POS) & OTG_FS_DCTL_TCTL_MSK)
#define  OTG_FS_DCTL_SGINAK                 ((uint32_t)    1 <<  7)
#define  OTG_FS_DCTL_CGINAK                 ((uint32_t)    1 <<  8)
#define  OTG_FS_DCTL_SGONAK                 ((uint32_t)    1 <<  9)
#define  OTG_FS_DCTL_CGONAK                 ((uint32_t)    1 << 10)
#define  OTG_FS_DCTL_POPRGDNE               ((uint32_t)    1 << 11)

// OTG_FS device status register (OTG_FS_DSTS)
#define  OTG_FS_DSTS_SUSPSTS                ((uint32_t)    1 <<  0)
#define  OTG_FS_DSTS_ENUMSPD_POS            ((uint32_t)          1)
#define  OTG_FS_DSTS_ENUMSPD_MSK            ((uint32_t)    3 << OTG_FS_DSTS_ENUMSPD_POS)
#define  OTG_FS_DSTS_ENUMSPD(x)             ((uint32_t)(   x << OTG_FS_DSTS_ENUMSPD_POS) & OTG_FS_DSTS_ENUMSPD_MSK)
#define  OTG_FS_DSTS_EERR                   ((uint32_t)    1 <<  3)
#define  OTG_FS_DSTS_FNSOF_POS              ((uint32_t)          8)
#define  OTG_FS_DSTS_FNSOF_MSK              ((uint32_t)0x3FFF<< OTG_FS_DSTS_FNSOF_POS)
#define  OTG_FS_DSTS_FNSOF(x)               ((uint32_t)(   x << OTG_FS_DSTS_FNSOF_POS) & OTG_FS_DSTS_FNSOF_MSK)

// OTG_FS device IN endpoint common interrupt mask register (OTG_FS_DIEPMSK)
#define  OTG_FS_DIEPMSK_XFRCM               ((uint32_t)    1 <<  0)
#define  OTG_FS_DIEPMSK_EPDM                ((uint32_t)    1 <<  1)
#define  OTG_FS_DIEPMSK_TOM                 ((uint32_t)    1 <<  3)
#define  OTG_FS_DIEPMSK_ITTXFEMSK           ((uint32_t)    1 <<  4)
#define  OTG_FS_DIEPMSK_INEPNMM             ((uint32_t)    1 <<  5)
#define  OTG_FS_DIEPMSK_INEPNEM             ((uint32_t)    1 <<  6)

// OTG_FS device OUT endpoint common interrupt mask register (OTG_FS_DOEPMSK)
#define  OTG_FS_DOEPMSK_XFRCM               ((uint32_t)    1 <<  0)
#define  OTG_FS_DOEPMSK_EPDM                ((uint32_t)    1 <<  1)
#define  OTG_FS_DOEPMSK_STUPM               ((uint32_t)    1 <<  3)
#define  OTG_FS_DOEPMSK_OTEPDM              ((uint32_t)    1 <<  4)

// OTG_FS device all endpoints interrupt register (OTG_FS_DAINT)
#define  OTG_FS_DAINT_IEPINT(in_ep)         ((uint32_t)((1 <<        in_ep)  & 0x0000FFFF))
#define  OTG_FS_DAINT_OEPINT(out_ep)        ((uint32_t)((1 << (16 + out_ep)) & 0xFFFF0000))

// OTG_FS device all endpoints interrupt mask register (OTG_FS_DAINTMSK)
#define  OTG_FS_DAINTMSK_IEPM(in_ep)        ((uint32_t)((1 <<        in_ep)  & 0x0000FFFF))
#define  OTG_FS_DAINTMSK_OEPM(out_ep)       ((uint32_t)((1 << (16 + out_ep)) & 0xFFFF0000))

// OTG_FS device Vbus discharge time register (OTG_FS_DVBUSDIS)
#define  OTG_FS_DVBUSDIS_VBUSDT_POS         ((uint32_t)          0)
#define  OTG_FS_DVBUSDIS_VBUSDT_MSK         ((uint32_t)0xFFFF<< OTG_FS_DVBUSDIS_VBUSDT_POS)
#define  OTG_FS_DVBUSDIS_VBUSDT(x)          ((uint32_t)(   x << OTG_FS_DVBUSDIS_VBUSDT_POS) & OTG_FS_DVBUSDIS_VBUSDT_MSK)

// OTG_FS device Vbus pulsing time register (OTG_FS_DVBUSPULSE)
#define  OTG_FS_DVBUSPULSE_DVBUSP_POS       ((uint32_t)          0)
#define  OTG_FS_DVBUSPULSE_DVBUSP_MSK       ((uint32_t)0xFFF << OTG_FS_DVBUSPULSE_DVBUSP_POS)
#define  OTG_FS_DVBUSPULSE_DVBUSP(x)        ((uint32_t)(   x << OTG_FS_DVBUSPULSE_DVBUSP_POS) & OTG_FS_DVBUSPULSE_DVBUSP)

// OTG_FS device IN endpoint FIFO emtpy interrupt mask register (OTG_FS_DIEPEMPMSK)
#define  OTG_FS_DIEPEMPMSK_INEPTXFEM_POS    ((uint32_t)          0)
#define  OTG_FS_DIEPEMPMSK_INEPTXFEM_MSK    ((uint32_t)0xFFFF<< OTG_FS_DIEPEMPMSK_INEPTXFEM_POS)
#define  OTG_FS_DIEPEMPMSK_INEPTXFEM(x)     ((uint32_t)(   x << OTG_FS_DIEPEMPMSK_INEPTXFEM_POS) & OTG_FS_DIEPEMPMSK_INEPTXFEM_MSK)

// OTG_FS device control IN endpoint 0 control register (OTG_FS_DIEPCTL0)
#define  OTG_FS_DIEPCTL0_MPSIZ_POS          ((uint32_t)          0)
#define  OTG_FS_DIEPCTL0_MPSIZ_MSK          ((uint32_t)    3 << OTG_FS_DIEPCTL0_MPSIZ_POS)
#define  OTG_FS_DIEPCTL0_MPSIZ(x)           ((uint32_t)(   x << OTG_FS_DIEPCTL0_MPSIZ_POS) & OTG_FS_DIEPCTL0_MPSIZ_MSK)
#define  OTG_FS_DIEPCTL0_USBAEP             ((uint32_t)    1 << 15)
#define  OTG_FS_DIEPCTL0_NAKSTS             ((uint32_t)    1 << 17)
#define  OTG_FS_DIEPCTL0_EPTYP_POS          ((uint32_t)         18)
#define  OTG_FS_DIEPCTL0_EPTYP_MSK          ((uint32_t)    3 << OTG_FS_DIEPCTL0_EPTYP_POS)
#define  OTG_FS_DIEPCTL0_EPTYP(x)           ((uint32_t)(   x << OTG_FS_DIEPCTL0_EPTYP_POS) & OTG_FS_DIEPCTL0_EPTYP_MSK)
#define  OTG_FS_DIEPCTL0_STALL              ((uint32_t)    1 << 21)
#define  OTG_FS_DIEPCTL0_TXFNUM_POS         ((uint32_t)         22)
#define  OTG_FS_DIEPCTL0_TXFNUM_MSK         ((uint32_t) 0x0F << OTG_FS_DIEPCTL0_TXFNUM_POS)
#define  OTG_FS_DIEPCTL0_TXFNUM(x)          ((uint32_t)(   x << OTG_FS_DIEPCTL0_TXFNUM_POS) & OTG_FS_DIEPCTL0_TXFNUM_MSK)
#define  OTG_FS_DIEPCTL0_CNAK               ((uint32_t)    1 << 26)
#define  OTG_FS_DIEPCTL0_SNAK               ((uint32_t)    1 << 27)
#define  OTG_FS_DIEPCTL0_EPDIS              ((uint32_t)    1 << 30)
#define  OTG_FS_DIEPCTL0_EPENA              ((uint32_t)    1 << 31)

// OTG_FS device endpoint-x control register (OTG_FS_DIEPCTLx)
#define  OTG_FS_DIEPCTLx_MPSIZ_POS          ((uint32_t)          0)
#define  OTG_FS_DIEPCTLx_MPSIZ_MSK          ((uint32_t)0x7FF << OTG_FS_DIEPCTLx_MPSIZ_POS)
#define  OTG_FS_DIEPCTLx_MPSIZ(x)           ((uint32_t)(   x << OTG_FS_DIEPCTLx_MPSIZ_POS) & OTG_FS_DIEPCTLx_MPSIZ_MSK)
#define  OTG_FS_DIEPCTLx_USBAEP             ((uint32_t)    1 << 15)
#define  OTG_FS_DIEPCTLx_EONUM_POS          ((uint32_t)         16)
#define  OTG_FS_DIEPCTLx_EONUM              ((uint32_t)    1 << 16)
#define  OTG_FS_DIEPCTLx_DPID               ((uint32_t)    1 << 16)
#define  OTG_FS_DIEPCTLx_NAKSTS             ((uint32_t)    1 << 17)
#define  OTG_FS_DIEPCTLx_EPTYP_POS          ((uint32_t)         18)
#define  OTG_FS_DIEPCTLx_EPTYP_MSK          ((uint32_t)    3 << OTG_FS_DIEPCTLx_EPTYP_POS)
#define  OTG_FS_DIEPCTLx_EPTYP(x)           ((uint32_t)(   x << OTG_FS_DIEPCTLx_EPTYP_POS) & OTG_FS_DIEPCTLx_EPTYP_MSK)
#define  OTG_FS_DIEPCTLx_STALL              ((uint32_t)    1 << 21)
#define  OTG_FS_DIEPCTLx_TXFNUM_POS         ((uint32_t)         22)
#define  OTG_FS_DIEPCTLx_TXFNUM_MSK         ((uint32_t) 0x0F << OTG_FS_DIEPCTLx_TXFNUM_POS)
#define  OTG_FS_DIEPCTLx_TXFNUM(x)          ((uint32_t)(   x << OTG_FS_DIEPCTLx_TXFNUM_POS) & OTG_FS_DIEPCTLx_TXFNUM_MSK)
#define  OTG_FS_DIEPCTLx_CNAK               ((uint32_t)    1 << 26)
#define  OTG_FS_DIEPCTLx_SNAK               ((uint32_t)    1 << 27)
#define  OTG_FS_DIEPCTLx_SD0PID             ((uint32_t)    1 << 28)
#define  OTG_FS_DIEPCTLx_SEVNFRM            ((uint32_t)    1 << 28)
#define  OTG_FS_DIEPCTLx_SODDFRM            ((uint32_t)    1 << 29)
#define  OTG_FS_DIEPCTLx_EPDIS              ((uint32_t)    1 << 30)
#define  OTG_FS_DIEPCTLx_EPENA              ((uint32_t)    1 << 31)

// OTG_FS device control OUT endpoint 0 control register (OTG_FS_DOEPCTL0)
#define  OTG_FS_DOEPCTL0_MPSIZ_POS          ((uint32_t)          0)
#define  OTG_FS_DOEPCTL0_MPSIZ_MSK          ((uint32_t)    3 << OTG_FS_DOEPCTL0_MPSIZ_POS)
#define  OTG_FS_DOEPCTL0_MPSIZ(x)           ((uint32_t)(   x << OTG_FS_DOEPCTL0_MPSIZ_POS) & OTG_FS_DOEPCTL0_MPSIZ_MSK)
#define  OTG_FS_DOEPCTL0_USBAEP             ((uint32_t)    1 << 15)
#define  OTG_FS_DOEPCTL0_NAKSTS             ((uint32_t)    1 << 17)
#define  OTG_FS_DOEPCTL0_EPTYP_POS          ((uint32_t)         18)
#define  OTG_FS_DOEPCTL0_EPTYP_MSK          ((uint32_t)    3 << OTG_FS_DOEPCTL0_EPTYP_POS)
#define  OTG_FS_DOEPCTL0_EPTYP(x)           ((uint32_t)(   x << OTG_FS_DOEPCTL0_EPTYP_POS) & OTG_FS_DOEPCTL0_EPTYP_MSK)
#define  OTG_FS_DOEPCTL0_SNPM               ((uint32_t)    1 << 20)
#define  OTG_FS_DOEPCTL0_STALL              ((uint32_t)    1 << 21)
#define  OTG_FS_DOEPCTL0_CNAK               ((uint32_t)    1 << 26)
#define  OTG_FS_DOEPCTL0_SNAK               ((uint32_t)    1 << 27)
#define  OTG_FS_DOEPCTL0_EPDIS              ((uint32_t)    1 << 30)
#define  OTG_FS_DOEPCTL0_EPENA              ((uint32_t)    1 << 31)

// OTG_FS device endpoint-x control register (OTG_FS_DOEPCTLx)
#define  OTG_FS_DOEPCTLx_MPSIZ_POS          ((uint32_t)          0)
#define  OTG_FS_DOEPCTLx_MPSIZ_MSK          ((uint32_t)0x7FF << OTG_FS_DOEPCTLx_MPSIZ_POS)
#define  OTG_FS_DOEPCTLx_MPSIZ(x)           ((uint32_t)(   x << OTG_FS_DOEPCTLx_MPSIZ_POS) & OTG_FS_DOEPCTLx_MPSIZ_MSK)
#define  OTG_FS_DOEPCTLx_USBAEP             ((uint32_t)    1 << 15)
#define  OTG_FS_DOEPCTLx_EONUM_POS          ((uint32_t)         16)
#define  OTG_FS_DOEPCTLx_EONUM              ((uint32_t)    1 << 16)
#define  OTG_FS_DOEPCTLx_DPID               ((uint32_t)    1 << 16)
#define  OTG_FS_DOEPCTLx_NAKSTS             ((uint32_t)    1 << 17)
#define  OTG_FS_DOEPCTLx_EPTYP_POS          ((uint32_t)         18)
#define  OTG_FS_DOEPCTLx_EPTYP_MSK          ((uint32_t)    3 << OTG_FS_DOEPCTLx_EPTYP_POS)
#define  OTG_FS_DOEPCTLx_EPTYP(x)           ((uint32_t)(   x << OTG_FS_DOEPCTLx_EPTYP_POS) & OTG_FS_DOEPCTLx_EPTYP_MSK)
#define  OTG_FS_DOEPCTLx_SNPM               ((uint32_t)    1 << 20)
#define  OTG_FS_DOEPCTLx_STALL              ((uint32_t)    1 << 21)
#define  OTG_FS_DOEPCTLx_CNAK               ((uint32_t)    1 << 26)
#define  OTG_FS_DOEPCTLx_SNAK               ((uint32_t)    1 << 27)
#define  OTG_FS_DOEPCTLx_SD0PID             ((uint32_t)    1 << 28)
#define  OTG_FS_DOEPCTLx_SEVNFRM            ((uint32_t)    1 << 28)
#define  OTG_FS_DOEPCTLx_SODDFRM            ((uint32_t)    1 << 29)
#define  OTG_FS_DOEPCTLx_SD1PID             ((uint32_t)    1 << 29)
#define  OTG_FS_DOEPCTLx_EPDIS              ((uint32_t)    1 << 30)
#define  OTG_FS_DOEPCTLx_EPENA              ((uint32_t)    1 << 31)

// OTG_FS device endpoint-x interrupt register (OTG_FS_DIEPINTx)
#define  OTG_FS_DIEPINTx_XFCR               ((uint32_t)    1 <<  0)
#define  OTG_FS_DIEPINTx_EPDISD             ((uint32_t)    1 <<  1)
#define  OTG_FS_DIEPINTx_TOC                ((uint32_t)    1 <<  3)
#define  OTG_FS_DIEPINTx_ITTXFE             ((uint32_t)    1 <<  4)
#define  OTG_FS_DIEPINTx_INEPNE             ((uint32_t)    1 <<  6)
#define  OTG_FS_DIEPINTx_TXFE               ((uint32_t)    1 <<  7)

// OTG_FS device endpoint-x interrupt register (OTG_FS_DOEPINTx)
#define  OTG_FS_DOEPINTx_XFCR               ((uint32_t)    1 <<  0)
#define  OTG_FS_DOEPINTx_EPDISD             ((uint32_t)    1 <<  1)
#define  OTG_FS_DOEPINTx_STUP               ((uint32_t)    1 <<  3)
#define  OTG_FS_DOEPINTx_OTEPDIS            ((uint32_t)    1 <<  4)
#define  OTG_FS_DOEPINTx_B2BSTUP            ((uint32_t)    1 <<  6)

// OTG_FS device IN endpoint 0 transfer size register (OTG_FS_DIEPTSIZ0)
#define  OTG_FS_DIEPTSIZ0_XFRSIZ_POS        ((uint32_t)          0)
#define  OTG_FS_DIEPTSIZ0_XFRSIZ_MSK        ((uint32_t) 0x7F << OTG_FS_DIEPTSIZ0_XFRSIZ_POS)
#define  OTG_FS_DIEPTSIZ0_XFRSIZ(x)         ((uint32_t)(   x << OTG_FS_DIEPTSIZ0_XFRSIZ_POS) & OTG_FS_DIEPTSIZ0_XFRSIZ_MSK)
#define  OTG_FS_DIEPTSIZ0_PKTCNT_POS        ((uint32_t)         19)
#define  OTG_FS_DIEPTSIZ0_PKTCNT_MSK        ((uint32_t)    3 << OTG_FS_DIEPTSIZ0_PKTCNT_POS)
#define  OTG_FS_DIEPTSIZ0_PKTCNT(x)         ((uint32_t)(   x << OTG_FS_DIEPTSIZ0_PKTCNT_POS) & OTG_FS_DIEPTSIZ0_PKTCNT_MSK)

// OTG_FS device OUT endpoint 0 transfer size register (OTG_FS_DOEPTSIZ0)
#define  OTG_FS_DOEPTSIZ0_XFRSIZ_POS        ((uint32_t)          0)
#define  OTG_FS_DOEPTSIZ0_XFRSIZ_MSK        ((uint32_t) 0x7F << OTG_FS_DOEPTSIZ0_XFRSIZ_POS)
#define  OTG_FS_DOEPTSIZ0_XFRSIZ(x)         ((uint32_t)(   x << OTG_FS_DOEPTSIZ0_XFRSIZ_POS) & OTG_FS_DOEPTSIZ0_XFRSIZ_MSK)
#define  OTG_FS_DOEPTSIZ0_PKTCNT            ((uint32_t)    1 << 19)
#define  OTG_FS_DOEPTSIZ0_STUPCNT_POS       ((uint32_t)         29)
#define  OTG_FS_DOEPTSIZ0_STUPCNT_MSK       ((uint32_t)    3 << OTG_FS_DOEPTSIZ0_STUPCNT_POS)
#define  OTG_FS_DOEPTSIZ0_STUPCNT(x)        ((uint32_t)(   x << OTG_FS_DOEPTSIZ0_STUPCNT_POS) & OTG_FS_DOEPTSIZ0_STUPCNT_MSK)

// OTG_FS device IN endpoints transver size register (OTG_FS_DIEPTSIZx)
#define  OTG_FS_DIEPTSIZx_XFRSIZ_POS        ((uint32_t)          0)
#define  OTG_FS_DIEPTSIZx_XFRSIZ_MSK        ((uint32_t)0x7FFFF<<OTG_FS_DIEPTSIZx_XFRSIZ_POS)
#define  OTG_FS_DIEPTSIZx_XFRSIZ(x)         ((uint32_t)(   x << OTG_FS_DIEPTSIZx_XFRSIZ_POS) & OTG_FS_DIEPTSIZx_XFRSIZ_MSK)
#define  OTG_FS_DIEPTSIZx_PKTCNT_POS        ((uint32_t)         19)
#define  OTG_FS_DIEPTSIZx_PKTCNT_MSK        ((uint32_t)0x3FF << OTG_FS_DIEPTSIZx_PKTCNT_POS)
#define  OTG_FS_DIEPTSIZx_PKTCNT(x)         ((uint32_t)(   x << OTG_FS_DIEPTSIZx_PKTCNT_POS) & OTG_FS_DIEPTSIZx_PKTCNT_MSK)
#define  OTG_FS_DIEPTSIZx_MCNT_POS          ((uint32_t)         29)
#define  OTG_FS_DIEPTSIZx_MCNT_MSK          ((uint32_t)    3 << OTG_FS_DIEPTSIZx_MCNT_POS)
#define  OTG_FS_DIEPTSIZx_MCNT(x)           ((uint32_t)(   x << OTG_FS_DIEPTSIZx_MCNT_POS) & OTG_FS_DIEPTSIZx_MCNT_MSK)

// OTG_FS device IN endpoint transmit FIFO status register (OTG_FS_DTXFSTSx)
#define  OTG_FS_DTXFSTSx_INEPTFSAV_POS      ((uint32_t)          0)
#define  OTG_FS_DTXFSTSx_INEPTFSAV_MSK      ((uint32_t)0xFFFF<< OTG_FS_DTXFSTSx_INEPTFSAV_POS)
#define  OTG_FS_DTXFSTSx_INEPTFSAV(x)       ((uint32_t)(   x << OTG_FS_DTXFSTSx_INEPTFSAV_POS) & OTG_FS_DTXFSTSx_INEPTFSAV_MSK)

// OTG_FS device OUT endpoint-x transfer size register (OTG_FS_DOEPTSIZx)
#define  OTG_FS_DOEPTSIZx_XFRSIZ_POS        ((uint32_t)          0)
#define  OTG_FS_DOEPTSIZx_XFRSIZ_MSK        ((uint32_t)0x7FFFF<<OTG_FS_DOEPTSIZx_XFRSIZ_POS)
#define  OTG_FS_DOEPTSIZx_XFRSIZ(x)         ((uint32_t)(   x << OTG_FS_DOEPTSIZx_XFRSIZ_POS) & OTG_FS_DOEPTSIZx_XFRSIZ_MSK)
#define  OTG_FS_DOEPTSIZx_PKTCNT_POS        ((uint32_t)         19)
#define  OTG_FS_DOEPTSIZx_PKTCNT_MSK        ((uint32_t)0x3FF << OTG_FS_DOEPTSIZx_PKTCNT_POS)
#define  OTG_FS_DOEPTSIZx_PKTCNT(x)         ((uint32_t)(   x << OTG_FS_DOEPTSIZx_PKTCNT_POS) & OTG_FS_DOEPTSIZx_PKTCNT_MSK)
#define  OTG_FS_DOEPTSIZx_RXDPID_POS        ((uint32_t)         29)
#define  OTG_FS_DOEPTSIZx_RXDPID_MSK        ((uint32_t)    3 << OTG_FS_DOEPTSIZx_RXDPID_POS)
#define  OTG_FS_DOEPTSIZx_RXDPID(x)         ((uint32_t)(   x << OTG_FS_DOEPTSIZx_RXDPID_POS) & OTG_FS_DOEPTSIZx_RXDPID_MSK)
#define  OTG_FS_DOEPTSIZx_STUPCNT_POS       ((uint32_t)         29)
#define  OTG_FS_DOEPTSIZx_STUPCNT_MSK       ((uint32_t)    3 << OTG_FS_DOEPTSIZx_STUPCNT_POS)
#define  OTG_FS_DOEPTSIZx_STUPCNT(x)        ((uint32_t)(   x << OTG_FS_DOEPTSIZx_STUPCNT_POS) & OTG_FS_DOEPTSIZx_STUPCNT_MSK)

// OTG_FS power and gate clock gating control register (OTG_FS_PCGCCTL)
#define  OTG_FS_PCGCCTL_STPPCLK             ((uint32_t)    1 <<  0)
#define  OTG_FS_PCGCCTL_GATEHCLK            ((uint32_t)    1 <<  1)
#define  OTG_FS_PCGCCTL_PHYSUSP             ((uint32_t)    1 <<  4)

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
      uint32_t MCNT      :  2;          /* Periodic Endpoint Multicount       */
      uint32_t DAD       :  7;          /* Device Address                     */
      uint32_t ODDFRM    :  1;          /* Periodic Transaction Odd Frame     */
      uint32_t CHDIS     :  1;          /* Channel Disable                    */
      uint32_t CHENA     :  1;          /* Channel Enable                     */
    };
  };
  uint32_t Reserved1;                   /* Reserved                           */
  __packed union {
    uint32_t HCINT;                     /* Channel Interrupt                  */
    __packed struct {
      uint32_t XFCR      :  1;          /* Transfer Completed                 */
      uint32_t CHH       :  1;          /* Channel Halted                     */
      uint32_t Reserved2 :  1;          /* Reserved                           */
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
      uint32_t Reserved4 :  1;          /* Reserved                           */
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
      uint32_t Reserved6 :  1;          /* Reserved                           */
    };
  };
  uint32_t Reserved7;                   /* Reserved                           */
  uint32_t Reserved8;                   /* Reserved                           */
  uint32_t Reserved9;                   /* Reserved                           */
} OTG_FS_HC;

#pragma no_anon_unions


