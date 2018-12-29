/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::USB:Host
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    USBH_Config_0.c
 * Purpose: USB Host Configuration
 * Rev.:    V5.10
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>USB Host 0
//   <o>Connect to hardware via Driver_USBH# <0-255>
//   <i>Select driver control block for hardware interface
#define USBH0_HC_NUM                    0

//   <o>Controller Interface 
//   <i> Selects the USB Host controller interface
//     <0=> Custom <1=> OHCI <2=> EHCI with OHCI support
//     <i>Custom Controller Interface is used for any non-standard USB Host 
//     <i>Controller
#define USBH0_HC_IF                     0


//     <h>OHCI settings
//     <i>These settings apply for OHCI controller interface

//       <o>Memory-mapped Host Controller registers base address 
#define USBH0_HC_OHCI_BASE_ADDRESS      0x5000C000

//       <h> Root HUB ports used by OHCI Controller
//       <i>Select the ports that OHCI will use.
//         <o1.0>  Port 1
//         <o1.1>  Port 2
//         <o1.2>  Port 3
//         <o1.3>  Port 4
//         <o1.4>  Port 5
//         <o1.5>  Port 6
//         <o1.6>  Port 7
//         <o1.7>  Port 8
//         <o1.8>  Port 9
//         <o1.9>  Port 10
//         <o1.10> Port 11
//         <o1.11> Port 12
//         <o1.12> Port 13
//         <o1.13> Port 14
//         <o1.14> Port 15
//       </h>
#define USBH0_HC_OHCI_PORT_MASK         0x00000001

//       <o>Maximum Endpoints in system
//       <i>Maximum number of Endpoint Descriptors used by OHCI controller.
//       <i>This setting affects memory allocated by OHCI controller.
#define USBH0_HC_OHCI_ENDPOINT_NUM      3

//       <o>Maximum concurrent Transfers in system
//       <i>Maximum number of Transfer Descriptors used by OHCI controller.
//       <i>This setting affects memory allocated by OHCI controller.
#define USBH0_HC_OHCI_TRANSFER_NUM      2

//     </h>


//     <h>EHCI settings
//     <i>These settings apply for EHCI with OHCI support controller interface

//       <o>Memory-mapped Host Controller registers base address 
#define USBH0_HC_EHCI_BASE_ADDRESS      0x40006100

//       <h> Root HUB ports used by EHCI Controller
//       <i>Select the ports that EHCI will use.
//         <o1.0>  Port 1
//         <o1.1>  Port 2
//         <o1.2>  Port 3
//         <o1.3>  Port 4
//         <o1.4>  Port 5
//         <o1.5>  Port 6
//         <o1.6>  Port 7
//         <o1.7>  Port 8
//         <o1.8>  Port 9
//         <o1.9>  Port 10
//         <o1.10> Port 11
//         <o1.11> Port 12
//         <o1.12> Port 13
//         <o1.13> Port 14
//         <o1.14> Port 15
//       </h>
#define USBH0_HC_EHCI_PORT_MASK         0x00000001

//       <o>Maximum Endpoints in system
//       <i>Maximum number of Head Descriptors used by EHCI controller.
//       <i>This setting affects memory allocated by EHCI controller.
#define USBH0_HC_EHCI_ENDPOINT_NUM      3

//       <o>Maximum concurrent Transfers in system
//       <i>Maximum number of Transfer Descriptors used by EHCI controller.
//       <i>This setting affects memory allocated by EHCI controller.
#define USBH0_HC_EHCI_TRANSFER_NUM      2

//     </h>


//   <o>Memory Pool Size <0-1048576:4>
//   <i>Specify size of memory pool (in bytes) that the USB Host Controller 
//   <i>will use for USB communication data.
#define USBH0_HC_MEM_POOL_SIZE          524

//   <e>Relocate Memory Pool
//   <i>Locate the Memory Pool at a specific address.
#define USBH0_HC_MEM_POOL_RELOC         0

//     <o>Memory Pool Address <0-0xFFFFFE00:0x200>
//     <i>Start address of the Memory Pool.
#define USBH0_HC_MEM_POOL_ADDR          0x00000000

//   </e>

//   <h>OS Resources Settings
//   <i>These settings are used to optimize usage of OS resources.
//     <o>Core Thread Stack Size <64-65536>
#define USBH0_CORE_THREAD_STACK_SIZE    512

//        Core Thread Priority
#define USBH0_CORE_THREAD_PRIORITY      osPriorityNormal

//   </h>
// </h>


#include "..\RTE_Components.h"

#ifdef  RTE_USB_Host_MSC
#include "USBH_Config_MSC.h"
#endif

#ifdef  RTE_USB_Host_HID
#include "USBH_Config_HID.h"
#endif

#ifdef  RTE_USB_Host_Class
#include "USBH_Config_Class.h"
#endif

#include "usbh_config.h"
