/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Device
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBD_Config_CDC_1.h
 * Purpose: USB Device Communication Device Class (CDC) Configuration
 * Rev.:    V5.10
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>USB Device: Communication Device Class (CDC) 1
//   <o>Assign Device Class to USB Device # <0-3>
//   <i>Select USB Device that is used for this Device Class instance
#define USBD_CDC1_DEV                   1

//   <o>Communication Class Subclass
//   <i>Specifies the model used by the CDC class.
//     <2=>Abstract Control Model
#define USBD_CDC1_SUBCLASS              2

//   <h>Interrupt Endpoint Settings
//   <i>By default, the settings match the first USB Class instance in a USB Device.
//   <i>Endpoint conflicts are flagged by compile-time error messages.

//     <o.0..3>Interrupt IN Endpoint Number
//               <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//       <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
#define USBD_CDC1_EP_INT_IN             1


//     <h>Endpoint Settings
//       <i>Parameters are used to create USB Descriptors and for memory 
//       <i>allocation in the USB component.

//       <h>Full/Low-speed (High-speed disabled)
//       <i>Parameters apply when High-speed is disabled in USBD_Config_1.c
//         <o.0..6>Maximum Endpoint Packet Size (in bytes) <0-64>
//         <i>Specifies the physical packet size used for information exchange.
//         <i>Maximum value is 64.
#define USBD_CDC1_WMAXPACKETSIZE        16

//         <o.0..7>Endpoint polling Interval (in ms) <1-255>
//         <i>Specifies the frequency of requests initiated by USB Host for 
//         <i>getting the notification.
#define USBD_CDC1_BINTERVAL             2

//       </h>

//       <h>High-speed
//       <i>Parameters apply when High-speed is enabled in USBD_Config_1.c
//
//         <o.0..10>Maximum Endpoint Packet Size (in bytes) <0-1024>
//         <i>Specifies the physical packet size used for information exchange.
//         <i>Maximum value is 1024.
//         <o.11..12>Additional transactions per microframe 
//         <i>Additional transactions improve communication performance.
//           <0=>None <1=>1 additional <2=>2 additional
#define USBD_CDC1_HS_WMAXPACKETSIZE     16

//         <o.0..4>Endpoint polling Interval (in 125 us intervals) 
//         <i>Specifies the frequency of requests initiated by USB Host for 
//         <i>getting the notification.
//           <1=>    1 <2=>    2 <3=>     4 <4=>     8
//           <5=>   16 <6=>   32 <7=>    64 <8=>   128
//           <9=>  256 <10=> 512 <11=> 1024 <12=> 2048
//           <13=>4096 <14=>8192 <15=>16384 <16=>32768
#define USBD_CDC1_HS_BINTERVAL          2

//       </h>
//     </h>
//   </h>


//   <h>Bulk Endpoint Settings
//   <i>By default, the settings match the first USB Class instance in a USB Device.
//   <i>Endpoint conflicts are flagged by compile-time error messages.

//     <o.0..3>Bulk IN Endpoint Number
//               <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//       <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
#define USBD_CDC1_EP_BULK_IN            2

//     <o.0..3>Bulk OUT Endpoint Number
//               <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
//       <8=>8   <9=>9   <10=>10 <11=>11 <12=>12 <13=>13 <14=>14 <15=>15
#define USBD_CDC1_EP_BULK_OUT           2


//     <h>Endpoint Settings
//       <i>Parameters are used to create USB Descriptors and for memory 
//       <i>allocation in the USB component.
//
//       <h>Full/Low-speed (High-speed disabled)
//       <i>Parameters apply when High-speed is disabled in USBD_Config_0.c
//         <o.0..6>Maximum Endpoint Packet Size (in bytes) <8=>8 <16=>16 <32=>32 <64=>64
//         <i>Specifies the physical packet size used for information exchange.
//         <i>Maximum value is 64.
#define USBD_CDC1_WMAXPACKETSIZE1       64

//       </h>

//       <h>High-speed
//       <i>Parameters apply when High-speed is enabled in USBD_Config_0.c
//
//         <o.0..9>Maximum Endpoint Packet Size (in bytes) <512=>512
//         <i>Specifies the physical packet size used for information exchange.
//         <i>Only available value is 512.
#define USBD_CDC1_HS_WMAXPACKETSIZE1    512

//         <o.0..7>Maximum NAK Rate <0-255>
//         <i>Specifies the interval in which Bulk Endpoint can NAK. 
//         <i>Value of 0 indicates that Bulk Endpoint never NAKs.
#define USBD_CDC1_HS_BINTERVAL1         0

//       </h>
//     </h>
//   </h>

//   <h>Communication Device Class Settings
//   <i>Parameters are used to create USB Descriptors and for memory allocation 
//   <i>in the USB component.
//
//     <s.126>Communication Class Interface String
#define USBD_CDC1_CIF_STR_DESC          L"USB_CDC1_0"

//     <s.126>Data Class Interface String
#define USBD_CDC1_DIF_STR_DESC          L"USB_CDC1_1"

//     <o>Maximum Communication Device Send Buffer Size
//     <i>Specifies size of buffer used for sending of data to USB Host.
//       <8=>    8 Bytes <16=>  16 Bytes <32=>  32 Bytes <64=>    64 Bytes 
//       <128=>128 Bytes <256=>256 Bytes <512=>512 Bytes <1024=>1024 Bytes
#define USBD_CDC1_SEND_BUF_SIZE         1024

//     <o>Maximum Communication Device Receive Buffer Size
//     <i>Specifies size of buffer used for receiving of data from USB Host.
//     <i>Minimum size must be as big as Maximum Packet Size for Bulk OUT Endpoint.
//       <8=>    8 Bytes <16=>  16 Bytes <32=>  32 Bytes <64=>    64 Bytes 
//       <128=>128 Bytes <256=>256 Bytes <512=>512 Bytes <1024=>1024 Bytes
#define USBD_CDC1_RECEIVE_BUF_SIZE      1024

//   </h>

//   <h>OS Resources Settings
//   <i>These settings are used to optimize usage of OS resources.
//     <o>Communication Device Class Thread Stack Size <64-65536>
#define USBD_CDC1_THREAD_STACK_SIZE     512

//        Communication Device Class Thread Priority
#define USBD_CDC1_THREAD_PRIORITY       osPriorityAboveNormal

//   </h>
// </h>
