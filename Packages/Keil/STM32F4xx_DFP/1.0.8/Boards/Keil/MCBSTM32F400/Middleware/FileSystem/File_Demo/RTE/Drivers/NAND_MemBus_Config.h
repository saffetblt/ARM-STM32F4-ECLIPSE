/*------------------------------------------------------------------------------
 *      RL-ARM - FileSystem
 *------------------------------------------------------------------------------
 *      Name:    NAND_MemBus_Config.h 
 *      Purpose: NAND Flash Device connected to Memory Bus Configuration
 *      Rev.:    V5.01
 *------------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2013 KEIL - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>NAND Driver Configuration

//   <o>Connect to hardware via Driver_NAND# <0-255>
//   <i>Select driver control block for hardware interface
#define NAND_DRIVER             0

//   <e>NAND Device 0
//     <h>Memory Map
//       <o1>Base Address
//       <o2>ALE Address
//       <o3>CLE Address
//     </h>
//     <o4>Data Bus Width <8=>8-bit <16=>16-bit
//     <q5>Ready/Busy Pin Interrupt
//   </e>
#define NAND_DEV0               1
#define NAND_DEV0_ADDR_BASE     0x80000000
#define NAND_DEV0_ADDR_ALE      0x80020000
#define NAND_DEV0_ADDR_CLE      0x80010000
#define NAND_DEV0_DATA_WIDTH    8
#define NAND_DEV0_RB_PIN_IRQ    0

//   <e>NAND Device 1
//     <h>Memory Map
//       <o1>Base Address
//       <o2>ALE Address
//       <o3>CLE Address
//     </h>
//     <o4>Data Bus Width <8=>8-bit <16=>16-bit
//     <q5>Ready/Busy Pin Interrupt
//   </e>
#define NAND_DEV1               0
#define NAND_DEV1_ADDR_BASE     0x81000000
#define NAND_DEV1_ADDR_ALE      0x81010000
#define NAND_DEV1_ADDR_CLE      0x81020000
#define NAND_DEV1_DATA_WIDTH    8
#define NAND_DEV1_RB_PIN_IRQ    0

//   <e>NAND Device 2
//     <h>Memory Map
//       <o1>Base Address
//       <o2>ALE Address
//       <o3>CLE Address
//     </h>
//     <o4>Data Bus Width <8=>8-bit <16=>16-bit
//     <q5>Ready/Busy Pin Interrupt
//   </e>
#define NAND_DEV2               0
#define NAND_DEV2_ADDR_BASE     0x82000000
#define NAND_DEV2_ADDR_ALE      0x82010000
#define NAND_DEV2_ADDR_CLE      0x82020000
#define NAND_DEV2_DATA_WIDTH    8
#define NAND_DEV2_RB_PIN_IRQ    0

//   <e>NAND Device 3
//     <h>Memory Map
//       <o1>Base Address
//       <o2>ALE Address
//       <o3>CLE Address
//     </h>
//     <o4>Data Bus Width <8=>8-bit <16=>16-bit
//     <q5>Ready/Busy Pin Interrupt
//   </e>
#define NAND_DEV3               0
#define NAND_DEV3_ADDR_BASE     0x83000000
#define NAND_DEV3_ADDR_ALE      0x83010000
#define NAND_DEV3_ADDR_CLE      0x83020000
#define NAND_DEV3_DATA_WIDTH    8
#define NAND_DEV3_RB_PIN_IRQ    0

// </h>
