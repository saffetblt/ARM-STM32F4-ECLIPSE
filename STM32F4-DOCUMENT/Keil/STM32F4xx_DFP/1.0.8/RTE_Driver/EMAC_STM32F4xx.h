/* ---------------------------------------------------------------------------
 * Copyright (C) 2013 ARM Limited. All rights reserved.
 *  
 * $Date:        31. January 2013
 * $Revision:    V1.00
 *  
 * Project:      Ethernet Media Access (MAC) Definitions for STM32F4xx
 * --------------------------------------------------------------------------*/

#include <stdint.h>

/* DMA Descriptor TX Packet Control/Status */
#define DMA_TX_OWN          0x80000000  /* Own bit 1=DMA,0=CPU               */
#define DMA_TX_IC           0x40000000  /* Interrupt on completition         */
#define DMA_TX_LS           0x20000000  /* Last segment                      */
#define DMA_TX_FS           0x10000000  /* First segment                     */
#define DMA_TX_DC           0x08000000  /* Disable CRC                       */
#define DMA_TX_DP           0x04000000  /* Disable pad                       */
#define DMA_TX_TTSE         0x02000000  /* Transmit time stamp enable        */
#define DMA_TX_CIC          0x00C00000  /* Checksum insertion control        */
#define DMA_TX_TER          0x00200000  /* Transmit end of ring              */
#define DMA_TX_TCH          0x00100000  /* Second address chained            */
#define DMA_TX_TTSS         0x00020000  /* Transmit time stamp status        */
#define DMA_TX_IHE          0x00010000  /* IP header error status            */
#define DMA_TX_ES           0x00008000  /* Error summary                     */
#define DMA_TX_JT           0x00004000  /* Jabber timeout                    */
#define DMA_TX_FF           0x00002000  /* Frame flushed                     */
#define DMA_TX_IPE          0x00001000  /* IP payload error                  */
#define DMA_TX_LC           0x00000800  /* Loss of carrier                   */
#define DMA_TX_NC           0x00000400  /* No carrier                        */
#define DMA_TX_LCOL         0x00000200  /* Late collision                    */
#define DMA_TX_EC           0x00000100  /* Excessive collision               */
#define DMA_TX_VF           0x00000080  /* VLAN frame                        */
#define DMA_TX_CC           0x00000078  /* Collision count                   */
#define DMA_TX_ED           0x00000004  /* Excessive deferral                */
#define DMA_TX_UF           0x00000002  /* Underflow error                   */
#define DMA_TX_DB           0x00000001  /* Deferred bit                      */

/* DMA Descriptor RX Packet Status */
#define DMA_RX_OWN          0x80000000  /* Own bit 1=DMA,0=CPU               */
#define DMA_RX_AFM          0x40000000  /* Destination address filter fail   */
#define DMA_RX_FL           0x3FFF0000  /* Frame length mask                 */
#define DMA_RX_ES           0x00008000  /* Error summary                     */
#define DMA_RX_DE           0x00004000  /* Descriptor error                  */
#define DMA_RX_SAF          0x00002000  /* Source address filter fail        */
#define DMA_RX_LE           0x00001000  /* Length error                      */
#define DMA_RX_OE           0x00000800  /* Overflow error                    */
#define DMA_RX_VLAN         0x00000400  /* VLAN tag                          */
#define DMA_RX_FS           0x00000200  /* First descriptor                  */
#define DMA_RX_LS           0x00000100  /* Last descriptor                   */
#define DMA_RX_IPHCE        0x00000080  /* IPv4 header checksum error        */
#define DMA_RX_LC           0x00000040  /* late collision                    */
#define DMA_RX_FT           0x00000020  /* Frame type                        */
#define DMA_RX_RWT          0x00000010  /* Receive watchdog timeout          */
#define DMA_RX_RE           0x00000008  /* Receive error                     */
#define DMA_RX_DRE          0x00000004  /* Dribble bit error                 */
#define DMA_RX_CE           0x00000002  /* CRC error                         */
#define DMA_RX_RMAM         0x00000001  /* Rx MAC adr.match/payload cks.error*/

/* DMA Descriptor RX Packet Control */
#define DMA_RX_DIC          0x80000000  /* Disable interrupt on completition */
#define DMA_RX_RBS2         0x1FFF0000  /* Receive buffer 2 size             */
#define DMA_RX_RER          0x00008000  /* Receove end of ring               */
#define DMA_RX_RCH          0x00004000  /* Second address chained            */
#define DMA_RX_RBS1         0x00003FFF  /* Receive buffer 1 size             */

/* DMA Descriptors. */
typedef struct {
  uint32_t volatile Stat;
  uint32_t Ctrl;
  uint32_t Addr;
  uint32_t Next;
} RX_Desc;

typedef struct {
  uint32_t volatile CtrlStat;
  uint32_t Size;
  uint32_t Addr;
  uint32_t Next;
} TX_Desc;
