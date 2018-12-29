/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_User_CDC_1.c
 * Purpose: USB Device Communication Device Class (CDC) User module
 * Rev.:    V5.04
 *----------------------------------------------------------------------------*/
/**
 * \addtogroup usbd_cdcFunctions
 *
 * USBD_User_CDC_1.c implements the application specific functionality of the
 * CDC class and is used to receive and send data to the USB Host.
 *
 * The implementation must match the configuration file USBD_Config_CDC_1.h.  
 * The following values in USBD_Config_CDC_1.h affect the user code:
 *
 *  - 'Maximum Communication Device Send Buffer Size' specifies the maximum 
 *    value for \em len in \ref USBD_CDC_ACM_WriteData
 *
 *  - 'Maximum Communication Device Receive Buffer Size' specifies the maximum 
 *    value for \em len in \ref USBD_CDC_ACM_ReadData
 *
 */


//! [code_USBD_User_CDC] 

#include "usb_cdc.h"

#include "Driver_UART.h"

extern ARM_DRIVER_UART Driver_UART3;

// Global Variables
static CDC_LINE_CODING LineCoding;

// Called during USBD_Initialize to initialize the USB Device class.
void USBD_CDC1_ACM_Initialize (void) {
  Driver_UART3.Initialize   (NULL, 0);
  Driver_UART3.PowerControl (ARM_POWER_FULL);
}


// Called during USBD_Uninitialize to de-initialize the USB Device class.
void USBD_CDC1_ACM_Uninitialize (void) {
  Driver_UART3.Uninitialize ();
}


// Called upon USB Reset Event
void USBD_CDC1_ACM_Reset (void) {
  Driver_UART3.FlushTxBuffer();
  Driver_UART3.FlushRxBuffer();
}


// Called upon USB request to Set Line Coding.
// \param[in]   line_coding   pointer to \ref CDC_LINE_CODING structure.
// \return      true          set line coding request processed. 
// \return      false         set line coding request not supported or not processed.
bool USBD_CDC1_ACM_SetLineCoding (CDC_LINE_CODING *line_coding) {
    if (Driver_UART3.Configure (                     line_coding->dwDTERate,
                                                   line_coding->bDataBits,
                              (ARM_UART_PARITY)    line_coding->bParityType,
                              (ARM_UART_STOP_BITS) line_coding->bCharFormat,
                                                   ARM_UART_FLOW_CONTROL_NONE)
                               == ARM_UART_OK) {

    LineCoding.dwDTERate   = line_coding->dwDTERate;
    LineCoding.bDataBits   = line_coding->bDataBits;
    LineCoding.bParityType = line_coding->bParityType;
    LineCoding.bCharFormat = line_coding->bCharFormat;
  }

  return true;
}


// Called upon USB request to Get Line Coding.
// \param[out]  line_coding   pointer to \ref CDC_LINE_CODING structure.
// \return      true          get line coding request processed. 
// \return      false         get line coding request not supported or not processed.
bool USBD_CDC1_ACM_GetLineCoding (CDC_LINE_CODING *line_coding) {

  line_coding->dwDTERate   = LineCoding.dwDTERate;
  line_coding->bDataBits   = LineCoding.bDataBits;
  line_coding->bParityType = LineCoding.bParityType;
  line_coding->bCharFormat = LineCoding.bCharFormat;

  return true;
}


// Called upon Set Control Line State request.
// \param [in]  state         control line settings bitmap.
//                - bit 0: DTR state 
//                - bit 1: RTS state
// \return      true          set control line state request processed. 
// \return      false         set control line state request not supported or not processed.
bool USBD_CDC1_ACM_SetControlLineState (uint16_t state) {
  // ToDo: add code for set control line state

  return true;
}

//! [code_USBD_User_CDC] 
