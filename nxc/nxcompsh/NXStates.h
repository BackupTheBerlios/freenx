/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2001,2002 NoMachine, http://www.nomachine.com.           */
/*                                                                        */
/* NXPROXY, NX protocol compression and NX extensions to this software    */
/* are copyright of NoMachine. Redistribution and use of the present      */
/* software is allowed according to terms specified in the file LICENSE   */
/* which comes in the source distribution.                                */
/*                                                                        */
/* Check http://www.nomachine.com/licensing.html for applicability.       */
/*                                                                        */
/* NX and NoMachine are trademarks of Medialogic S.p.A.                   */
/*                                                                        */
/* All rigths reserved.                                                   */
/*                                                                        */
/**************************************************************************/


#ifndef NX_STATES_H
#define NX_STATES_H

#define NX_Exception            0
#define NX_Ok                   1

#define NX_Init                 10

/* Server */
#define NX_ReadyForConnection   11
#define NX_Connecting           12
#define NX_Connected            13
#define NX_SendingVersion       14
#define NX_AcceptingVersion     15
#define NX_VersionAccepted      16
#define NX_WaitingForAuth       17
#define NX_SendingAuth          18
#define NX_Authenticating       19
#define NX_Authenticated        20
#define NX_StartingSession      21
#define NX_SendingParameters    22
#define NX_Negotiating          23
#define NX_SessionAccepted      24
#define NX_ConnectionEnd        25
#define NX_ConfirmAddHost       26
#define NX_ServerParameters     27
#define NX_ReadyForBye          28
/* Server end */


/* Proxy */
#define NX_InitProxy            100

#define NX_ProxyReady           101
#define NX_ProxyConnecting      102
#define NX_ProxyNegotiating     103
#define NX_ProxyConnected       104
#define NX_ProxyConnectionEnd   105

/* Proxy end */


#endif /* NX_STATES_H */

