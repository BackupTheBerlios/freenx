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


#ifndef NX_SESSION_INTERFACE_H
#define NX_SESSION_INTERFACE_H

#include "NXParameters.h"
#include "NXStates.h"

typedef  void*  NXSessionPtr;
typedef  void*  NXConnectionPtr;
typedef  void*  NXProxyPtr;
typedef  void*  NXXServerPtr;

// char* returned from a function must be deallocated by user

char* GetNXCompShVersion();

NXSessionPtr CreateSession();
void DeleteSession( NXSessionPtr );

void SetStringParameter( NXSessionPtr, int ParameterId, const char* ParameterValue );
char* GetStringParameter( NXSessionPtr, int ParameterId, const char* DefaultValue );

void SetBoolParameter( NXSessionPtr, int ParameterId, int ParameterValue );
int GetBoolParameter( NXSessionPtr, int ParameterId, int DefaultValue );

void SetNumberParameter( NXSessionPtr, int ParameterId, long int ParameterValue );
long int GetNumberParameter( NXSessionPtr, int ParameterId, long int DefaultValue );

void ClearParameters( NXSessionPtr );

char* GetLastError( NXSessionPtr );
int GetLastErrorType( NXSessionPtr );

int GetCurrentState( NXSessionPtr );

int GetExistingSessionCookie( NXSessionPtr );
int GenerateSessionCookie( NXSessionPtr );

NXConnectionPtr CreateServerConnection( NXSessionPtr, const char* ConnectionType );
void DeleteServerConnection( NXConnectionPtr );
int InitServerConnection( NXConnectionPtr );
int AdvanceServerConnection( NXConnectionPtr, unsigned int timeout );
int EndServerConnection( NXConnectionPtr );
void TerminateServerConnection( NXConnectionPtr );

int SessionNeedToStartNewProxy( NXSessionPtr );

NXProxyPtr CreateProxyConnection( NXSessionPtr );
void DeleteProxyConnection( NXProxyPtr );
int InitProxyConnection( NXProxyPtr );
int AdvanceProxyConnection( NXProxyPtr, unsigned int timeout );
int EndProxyConnection( NXProxyPtr );
void TerminateProxyConnection( NXProxyPtr );

NXXServerPtr CreateXServerConnection( NXSessionPtr );
void DeleteXServerConnection( NXXServerPtr );
int InitXServerConnection( NXXServerPtr );
int AdvanceXServerConnection( NXXServerPtr );
int EndXServerConnection( NXXServerPtr );
void TerminateXServerConnection( NXXServerPtr );


#endif /* NX_SESSION_INTERFACE_H */
