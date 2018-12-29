/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_MSC_1.c
 * Purpose: USB Device Mass Storage Device class (MSC) User module
 * Rev.:    V5.04
 *----------------------------------------------------------------------------*/
/**
 * \addtogroup usbd_mscFunctions
 *
 */


//! [code_USBD_User_MSC]

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "rl_usbd.h"

// If the USE_FILE_SYSTEM is defined then File System is used and media can 
// be accessed from application code or can be accessed by USB Mass Storage 
// requests, default media in that case is SD Card or NAND Flash.  
// If the USE_FILE_SYSTEM is not defined (commented out) then File System is 
// not used and media can only be accessed by USB Mass Storage requests,  
// default media in that case is RAM memory containing dummy disk image.    
#define USE_FILE_SYSTEM                 // Use File System for Memory Storage

static usbd_msc_data_t  *msc_data_ptr;  // Pointer to the MSC run-time settings 
                                        // in RAM
extern const usbd_msc_t *usbd_msc_ptr[];// Pointer to the MSC configuration in 
                                        // code memory

#ifdef USE_FILE_SYSTEM
#include "rl_fs.h"
static int32_t     DrvId;               // FAT drive id
static fsMediaInfo Media;               // Media Information
#else
static uint8_t memory    [8192];        // Memory in RAM for dummy disk image
static uint8_t block_buf [512];         // Buffer for block read/write to media
extern
const  uint8_t memory_disk_image[4096]; // Dummy Memory Disk Image
#endif

// Called during USBD_Initialize to initialize the USB Device class.
void USBD_MSC1_Initialize (void) {
#ifdef USE_FILE_SYSTEM
  fsIOC_Cache cache_info;

                                        // Get pointer to the MSC run-time 
                                        // settings in RAM, so it can be altered
  msc_data_ptr = usbd_msc_ptr[1]->data_ptr;
  
  DrvId = fs_ioc_get_id (NULL);         // Get ID of File System drive
  if (DrvId < 0) return;                // If ID is invalid initialization 
                                        // failed

  if (fs_ioc_lock (DrvId) == 0) {       // Lock the media for usage by USB
    fs_ioc_read_info(DrvId, &Media);    // Read media info of actual media
    fs_ioc_get_cache(DrvId, &cache_info); // Get cache settings of File System

    msc_data_ptr->block_size  = 512;    // Block size of Mass Storage
                                        // Cache buffer from File System is 
                                        // used for media read/write
    msc_data_ptr->block_buf   = cache_info.buffer;
                                        // Number of blocks that can fit to cache
    msc_data_ptr->block_group = cache_info.size / 512;
                                        // Total number of blocks on media
    msc_data_ptr->block_count = Media.block_cnt * (Media.read_blen / 512);
                                        // Total size of media
    msc_data_ptr->memory_size = msc_data_ptr->block_count * Media.read_blen;
  }

#else
                                        // Get pointer to the MSC run-time 
                                        // settings in RAM, so it can be altered
  msc_data_ptr = usbd_msc_ptr[1]->data_ptr;

  msc_data_ptr->memory_size = 8192;     // Total size of dummy image
  msc_data_ptr->block_size  = 512;      // Block size of Mass Storage
  msc_data_ptr->block_group = 1;        // Number of blocks buffered
                                        // Total number of blocks that dummy 
                                        // image contains
  msc_data_ptr->block_count = msc_data_ptr->memory_size / msc_data_ptr->block_size;
                                        // Cache buffer from File System is 
                                        // used for data buffer
  msc_data_ptr->block_buf   = block_buf;// Buffer used for media read/write

                                        // Copy the dummy image from code to RAM
  memcpy(memory, memory_disk_image, sizeof(memory_disk_image));
#endif

  msc_data_ptr->media_ready = true;     // Media is ready now
}


// Called during USBD_Uninitialize to de-initialize the USB Device class.
void USBD_MSC1_Uninitialize (void) {
  // ToDo: add code for de-initialization
}


// Read data from media.
// \param[in]   lba           logical address of first block to read.
// \param[in]   cnt           number of contiguous blocks to read from media.
// \param[out]  buf           data buffer for data read from media.
// \return      true          read succeeded.
// \return      false         read failed.
bool USBD_MSC1_Read (uint32_t lba, uint16_t cnt, uint8_t *buf) {

  if (msc_data_ptr->media_ready) {
#ifdef USE_FILE_SYSTEM
                                        // Read data directly from media
    if (fs_ioc_read_sector(DrvId, lba, buf, cnt) == fsOK) 
      return true;
#else
                                        // Read data from dummy image in RAM 
    memcpy(buf, &memory[lba * msc_data_ptr->block_size], cnt * msc_data_ptr->block_size);
    return true;
#endif
  }

  return false;
}


// \brief Write data to media.
// \param[in]   lba           logical address of first block to write.
// \param[in]   cnt           number of contiguous blocks to write to media.
// \param[out]  buf           data buffer containing data to write to media.
// \return      true          write succeeded.
// \return      false         write failed.
bool USBD_MSC1_Write (uint32_t lba, uint16_t cnt, const uint8_t *buf) {

  if (msc_data_ptr->media_ready) {      // Write to media only if media is ready
#ifdef USE_FILE_SYSTEM
                                        // Write data directly to media
    if (fs_ioc_write_sector(DrvId, lba, buf, cnt) == fsOK) 
      return true;    
#else
                                        // Write data to image in RAM 
    memcpy(&memory[lba * msc_data_ptr->block_size], buf, cnt * msc_data_ptr->block_size);
    return true;
#endif
  }

  return false;
}
//! [code_USBD_User_MSC]

