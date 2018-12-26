/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System:Drive
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    FS_Config_NAND_0.h
 * Purpose: File System Configuration for NAND Flash Drive
 * Rev.:    V5.01
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>NAND Flash Drive 0
// <i>Configuration for NAND device assigned to drive letter "N0:"
#define NAND0_ENABLE            1

//   <o>NAND Type  <0=> NAND
//                 <1=> EZ-NAND
//                 <2=> OneNAND 
//   <i>Define NAND Type.
#define NAND0_TYPE              0

//   <o>Connect to hardware via Driver_NAND# <0-255>
//   <i>Select driver control block for hardware interface
#define NAND0_DRIVER            0

//   <o>Device Number <0-255>
//   <i>Selects NAND device connected to selected driver
#define NAND0_DEV_NUM           0

//   <o>Page Size  <528=> 512 + 16 bytes
//                 <2112=>2048 + 64 bytes
//                 <4224=>4096 + 128 bytes
//                 <8448=>8192 + 256 bytes
//   <i>Define program Page size in bytes (User + Spare area).
#define NAND0_PAGE_SIZE         2112

//   <o>Block Size <8=>8 pages <16=>16 pages <32=>32 pages
//                 <64=>64 pages <128=>128 pages <256=>256 pages
//   <i>Define number of pages in a block.
#define NAND0_PAGE_COUNT        64

//   <o>Device Size [blocks] <512-32768>
//   <i>Define number of blocks in NAND Flash device.
#define NAND0_BLOCK_COUNT       4096

//   <o>Page Caching <0=>OFF <1=>1 page <2=>2 pages <4=>4 pages
//                   <8=>8 pages <16=>16 pages <32=>32 pages
//   <i>Device pages can be cached to speed-up sector read/write
//   <i>operations on this drive.
//   <i>Define number of cached Pages (default: 2 pages).
#define NAND0_PAGE_CACHE        2

//   <o>Block Indexing <0=>OFF <1=>1 block <2=>2 blocks <4=>4 blocks
//                     <8=>8 blocks <16=>16 blocks <32=>32 blocks
//                     <64=>64 blocks <128=>128 blocks <256=>256 blocks
//   <i>Device blocks can be indexed for faster page access time.
//   <i>Increase number of indexed blocks for better performance (default: 16 blocks).
#define NAND0_BLOCK_CACHE       16

//   <o>Software ECC <0=>None <1=>Hamming (SLC)
//   <i>Enable software ECC calculation only if not supported by hardware.
#define NAND0_SW_ECC            1

//   <o>Drive Cache Size <0=>OFF <1=>1 KB <2=>2 KB <4=>4 KB
//                       <8=>8 KB <16=>16 KB <32=>32 KB
//   <i>Drive Cache stores data sectors and may be increased to speed-up
//   <i>file read/write operations on this drive (default: 4 KB)
#define NAND0_CACHE_SIZE        4

//   <e>Locate Drive Cache and Drive Buffer
//   <i>Some microcontrollers support DMA only in specific memory areas and
//   <i>require to locate the drive buffers at a fixed address.
#define NAND0_CACHE_RELOC       0

//     <o>Base address <0x0000-0xFFFFFE00:0x200>
//     <i>Set buffer base address to RAM areas that support DMA with the drive.
#define NAND0_CACHE_ADDR        0x80000000

//   </e>

//   <q>Use FAT Journal
//   <i>Protect File Allocation Table and Directory Entries for
//   <i>fail-safe operation.
#define NAND0_FAT_JOURNAL       0

//   <q>Default Drive "N0:"
//   <i>Use this drive when no drive letter is specified.
#define NAND0_DEFAULT_DRIVE     0

// </h>
