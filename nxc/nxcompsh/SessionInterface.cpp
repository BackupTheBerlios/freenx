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


#include "SessionInterface.h"
#include "Session.h"
#include "TransportSSH.h"
#include "Proxy.h"
#include "Cookie.h"
#include "XServer.h"

#include "NXCompSh.h"


using namespace NX;


char* GetNXCompShVersion()
{
  char* pVersion = new char[ strlen( NX_COMPSH_VERSION ) + 1 ];
  strcpy( pVersion, NX_COMPSH_VERSION );
  return pVersion;
}

NXSessionPtr CreateSession()
{
  Session* pSession = new Session();
  return pSession;
}

void DeleteSession( NXSessionPtr SessionPtr )
{
  Session* pSession = (Session*)SessionPtr;
  delete pSession;
  SessionPtr = NULL;
}

void SetStringParameter( NXSessionPtr SessionPtr, int ParameterId, const char* ParameterValue )
{
  Session* pSession = (Session*)SessionPtr;
  (*pSession).GetParameters().SetString( ParameterId, ParameterValue );
}

char* GetStringParameter( NXSessionPtr SessionPtr, int ParameterId, const char* DefaultValue )
{
  Session* pSession = (Session*)SessionPtr;
  const char* pParameterTmp = (*pSession).GetParameters().GetString( ParameterId, DefaultValue ).c_str();

  char* pParameter = new char[ strlen( pParameterTmp ) + 1 ];

  strcpy( pParameter, pParameterTmp );

  return pParameter;
}

void SetBoolParameter( NXSessionPtr SessionPtr, int ParameterId, int ParameterValue )
{
  Session* pSession = (Session*)SessionPtr;
  bool bSet = false;
  if( ParameterValue > 0 )
    bSet = true;
  (*pSession).GetParameters().SetBool( ParameterId, bSet );
}

int GetBoolParamater( NXSessionPtr SessionPtr, int ParameterId, int DefaultValue )
{
  Session* pSession = (Session*)SessionPtr;

  if( (*pSession).GetParameters().GetNumber( ParameterId, DefaultValue ) == 1 )
    return 1;
  else
    return 0;
}

void SetNumberParameter( NXSessionPtr SessionPtr, int ParameterId, long int ParameterValue )
{
  Session* pSession = (Session*)SessionPtr;
  (*pSession).GetParameters().SetNumber( ParameterId, ParameterValue );
}

long int GetNumberParamater( NXSessionPtr SessionPtr, int ParameterId, long int DefaultValue )
{
  Session* pSession = (Session*)SessionPtr;
  return (*pSession).GetParameters().GetNumber( ParameterId, DefaultValue );
}

void ClearParameters( NXSessionPtr SessionPtr )
{
  Session* pSession = (Session*)SessionPtr;
  (*pSession).GetParameters().Clear();
}

char* GetLastError( NXSessionPtr SessionPtr )
{
  Session* pSession = (Session*)SessionPtr;
  const char* pErrorTmp = (*pSession).GetException().GetString().c_str();

  char* pError = new char[ strlen( pErrorTmp ) + 1 ];

  strcpy( pError, pErrorTmp );

  return pError;
}

int GetLastErrorType( NXSessionPtr SessionPtr )
{
  Session* pSession = (Session*)SessionPtr;
  return (*pSession).GetException().GetType();
}

int GetCurrentState( NXSessionPtr SessionPtr )
{
  Session* pSession = (Session*)SessionPtr;
  return (*pSession).GetState();
}

int GetExistingSessionCookie( NXSessionPtr SessionPtr )
{
  Session* pSession = (Session*)SessionPtr;
  Cookie cookie( pSession );
  if( cookie.ReadExistingOne() )
  {
    pSession->GetParameters().SetString( NX_SessionCookie, cookie.Get() );
    return NX_Ok;
  }
  else
    return NX_Exception;
}

int GenerateSessionCookie( NXSessionPtr SessionPtr )
{
  Session* pSession = (Session*)SessionPtr;
  Cookie cookie( pSession );
  if( cookie.Generate() )
  {
    pSession->GetParameters().SetString( NX_SessionCookie, cookie.Get() );
    return NX_Ok;
  }
  else
    return NX_Exception;
}

NXConnectionPtr CreateServerConnection( NXSessionPtr SessionPtr, const char* ConnectionType )
{
  Session* pSession = (Session*)SessionPtr;
  Transport *pTransport = NULL;

  string sTmp = ConnectionType;
  if( sTmp != "nxssh" )
  {
    pSession->SetException( NX_RuntimeError, sTmp + ": connection type unavailable." );
  }
  else
  {
    pTransport = new TransportSSH( pSession );
  }

  return pTransport;
}

void DeleteServerConnection( NXConnectionPtr ConnectionPtr )
{
  Transport* pTransport = (Transport*)ConnectionPtr;
  delete pTransport;
  ConnectionPtr = NULL;
}

int InitServerConnection( NXConnectionPtr ConnectionPtr )
{
  Transport* pTransport = (Transport*)ConnectionPtr;

  if( !pTransport->InitConnection() )
    return NX_Exception;
  else
    return pTransport->GetSession()->GetState();
}

int EndServerConnection( NXConnectionPtr ConnectionPtr )
{
  Transport* pTransport = (Transport*)ConnectionPtr;
  if( pTransport->EndConnection() )
    return NX_Ok;
  else
    return pTransport->GetSession()->GetState();
}

void TerminateServerConnection( NXConnectionPtr ConnectionPtr )
{
  Transport* pTransport = (Transport*)ConnectionPtr;
  pTransport->BreakConnection();
}

int AdvanceServerConnection( NXConnectionPtr ConnectionPtr, unsigned int timeout )
{
  Transport* pTransport = (Transport*)ConnectionPtr;
/*
//  if( pTransport->GetSession()->GetState() == NX_SessionAccepted )
  if( pTransport->GetSession()->GetState() == NX_ConnectionEnd )
    pTransport->CloseConnection();
  else
*/
    pTransport->AdvanceConnection( timeout );

  return pTransport->GetSession()->GetState();
}

int SessionNeedToStartNewProxy( NXSessionPtr SessionPtr )
{
  Session* pSession = (Session*)SessionPtr;
  if( pSession->NeedProxy() )
    return NX_Ok;
  else
    return NX_Exception;
}

NXProxyPtr CreateProxyConnection( NXSessionPtr SessionPtr )
{
  Session* pSession = (Session*)SessionPtr;
  Proxy* pProxy = new Proxy( pSession );
  return pProxy;
}

void DeleteProxyConnection( NXProxyPtr ProxyPtr )
{
  Proxy* pProxy = (Proxy*)ProxyPtr;
  delete pProxy;
  ProxyPtr = NULL;
}

int InitProxyConnection( NXProxyPtr ProxyPtr )
{
  Proxy* pProxy = (Proxy*)ProxyPtr;
  if( !pProxy->InitConnection() )
    return NX_Exception;
  else
    return pProxy->GetSession()->GetState();
}

int AdvanceProxyConnection( NXProxyPtr ProxyPtr , unsigned int timeout )
{
  Proxy* pProxy = (Proxy*)ProxyPtr;
  pProxy->AdvanceConnection( timeout );
  return pProxy->GetSession()->GetState();
}

int EndProxyConnection( NXProxyPtr ProxyPtr )
{
  Proxy* pProxy = (Proxy*)ProxyPtr;
  if( pProxy->GetSession()->GetState() == NX_ProxyConnected )
    return NX_Ok;
  else
    return pProxy->GetSession()->GetState();
}

void TerminateProxyConnection( NXProxyPtr ProxyPtr )
{
  Proxy* pProxy = (Proxy*)ProxyPtr;
  pProxy->BreakConnection();
}

NXXServerPtr CreateXServerConnection( NXSessionPtr SessionPtr )
{
  Session* pSession = (Session*)SessionPtr;
  XServer* pXServer = new XServer( pSession );
  return pXServer;
}

void DeleteXServerConnection( NXXServerPtr XServerPtr )
{
  XServer* pXServer = (XServer*)XServerPtr;
  delete pXServer;
  XServerPtr = NULL;
}

int InitXServerConnection( NXXServerPtr XServerPtr )
{
  XServer* pXServer = (XServer*)XServerPtr;
  if( pXServer->InitConnection() )
    return NX_Ok;
  else
    return NX_Exception;
}

int AdvanceXServerConnection( NXXServerPtr XServerPtr )
{
  XServer* pXServer = (XServer*)XServerPtr;
  if( pXServer->AdvanceConnection() )
    return NX_Ok;
  else
    return NX_Exception;
}

int EndXServerConnection( NXXServerPtr XServerPtr )
{
  XServer* pXServer = (XServer*)XServerPtr;
  if( pXServer->IsRunning() )
    return NX_Ok;
  else
    return NX_Exception;
}

void TerminateXServerConnection( NXXServerPtr XServerPtr )
{
  XServer* pXServer = (XServer*)XServerPtr;
  pXServer->BreakConnection();
}

