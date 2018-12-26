/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2013 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Config_Telnet_Server.h
 * Purpose: Network Configuration Telnet Server
 * Rev.:    V5.00
 *----------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

// <h>Telnet Server
#define TELNET_SERVER_ENABLE        1

//   <o>Number of Telnet Connections <1-10>
//   <i> Number of simultaneously active Telnet Connections.
//   <i> Default: 1
#define TELNET_SERVER_NUM_SESSISONS 3

//   <o>Port Number <1-65535>
//   <i> Listening port number.
//   <i> Default: 23
#define TELNET_SERVER_PORT_NUM      23

//   <o>Idle Connection Timeout in seconds <0-3600>
//   <i> When timeout expires, the connection is closed.
//   <i> A value of 0 disables disconnection on timeout.
//   <i> Default: 120
#define TELNET_SERVER_TOUT          120

//   <q>Disable Echo
//   <i> When disabled, the server will not echo
//   <i> characters it receives.
//   <i> Default: Not disabled
#define TELNET_SERVER_NO_ECHO       0

//   <e>Enable User Authentication
//   <i> When enabled, the user will have to authenticate
//   <i> himself by username and password before access
//   <i> to the system is allowed.
#define TELNET_SERVER_AUTH_ENABLE   1

//     <s.15>Authentication Username
//     <i> Default: "admin"
#define TELNET_SERVER_AUTH_USER     "admin"

//     <s.15>Authentication Password
//     <i> Default: ""
#define TELNET_SERVER_AUTH_PASS      ""
//   </e>

// </h>
