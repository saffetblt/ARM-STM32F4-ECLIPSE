/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System:Drive
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FS_Config_NOR_0.h
 * Purpose: File System Configuration for NOR Flash Drive
 * Rev.:    V5.04
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>NOR Flash Drive 0
// <i>Configuration for NOR Flash device assigned to drive letter "F0:"
#define NOR0_ENABLE             1

//   <e>NOR Flash Access via Memory Bus
//   <i>NOR Flash Device is memory mapped and accessed via Memory Bus
#define NOR0_MEM_BUS            1

//     <o>Base address <0x0-0xFFFFF000:0x1000>
//     <i>Define NOR Flash Device base address on the Memory Bus
#define NOR0_BASE_ADDR          0x60000000

//   </e>

//   <e>NOR Flash Access via SPI
//   <i>NOR Flash Device is connected via SPI
#define NOR0_SPI                0

//     <o>Connect to hardware via Driver_SPI# <0-255>
//     <i>Select driver control block for hardware interface
#define NOR0_SPI_DRIVER         0

//   </e>

#if (NOR0_MEM_BUS == 0) && (NOR0_SPI == 0)
#error "NOR Flash Access undefined: select Memory Bus or SPI"
#endif

#if (NOR0_MEM_BUS == 1) && (NOR0_SPI == 1)
#error "NOR Flash Access: select either Memory Bus or SPI"
#endif

//   <o>NOR Flash Device
//     <0=>AM29x800BB
//     <1=>M29W640FB
//     <2=>AT45DB642D
//   <i>Specify NOR Flash Device Name (selects Flash Device description) 
#define NOR0_NAME_ID            1
#if    (NOR0_NAME_ID == 0)
#define NOR0_NAME               AM29x800BB
#elif  (NOR0_NAME_ID == 1)
#define NOR0_NAME               M29W640FB
#elif  (NOR0_NAME_ID == 2)
#define NOR0_NAME               AT45DB642D
#else
#error "NOR Flash Device not selected!"
#endif

//   <q>Default Drive "F0:"
//   <i>Use this drive when no drive letter is specified.
#define NOR0_DEFAULT_DRIVE      0

// </h>
