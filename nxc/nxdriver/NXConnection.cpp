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


#include "NXConnection.h"
#include "NXSettings.h"
#include "Options.h"
#include "SettingsToParameters.h"
#include "Logger.h"






NXConnection::NXConnection()
{
  NX_LOG_LOGDEBUG( "NXConnection created.");
  pSession = CreateSession();
  pConnection = 0;
  pProxy = 0;
  pXServer = 0;
}

NXConnection::~NXConnection()
{
  NX_LOG_LOGDEBUG( "NXConnection's destructor called.");
  HandleCleanup();
}

string NXConnection::GetSessionId() const
{
   return GetStringParameter(pSession, NX_SessionId, "undefined");
}

void NXConnection::SetSessionId(string sessionid)
{
  SetStringParameter(pSession, NX_SessionId, sessionid.c_str());
  SetStringParameter(pSession, NX_ProxySessionId, sessionid.c_str());
}

void NXConnection::HandleCleanup()
{
  NX_LOG_LOGDEBUG( "cleaning up NXConnection.");
  if( pSession )
    DeleteSession( pSession );
  if( pConnection )
  {
    NX_LOG_LOGDEBUG( "deleting Server Connection");
    DeleteServerConnection( pConnection );
  }
  if( pProxy )
    DeleteProxyConnection( pProxy );
  if( pXServer )
    DeleteXServerConnection( pXServer );
  NX_LOG_LOGDEBUG( "NXConnection clean.");
}

int NXConnection::GetStateId() const
{
  return GetCurrentState( pSession );
}

string NXConnection::GetState() const
{
  if( ErrorOccurred() )
    return "Connection error";

  string sRet = "";

  switch( GetCurrentState( pSession ) )
  {
  case NX_ReadyForConnection:
    sRet = "Initializing connection";
    break;

  case NX_Connecting:
    sRet = "Connecting to '";
    sRet += string( GetStringParameter( pSession, NX_HostName, "unknown" ) );
    sRet += "'";
    break;

  case NX_Connected:
    sRet = "Connected to '";
    sRet += string( GetStringParameter( pSession, NX_HostName, "unknown" ) );
    sRet += "'";
    break;

  case NX_SendingVersion:
  case NX_AcceptingVersion:
    sRet = "Negotiating protocol version";
    break;

  case NX_VersionAccepted:
    sRet = "NX protocol version accepted";
    break;

  case NX_WaitingForAuth:
  case NX_SendingAuth:
  case NX_Authenticating:
    sRet = "Waiting for authentication";
    break;

  case NX_Authenticated:
  case NX_StartingSession:
    sRet = "Authentication completed";
    break;

  case NX_SendingParameters:
  case NX_Negotiating:
    sRet = "Negotiating session parameters";
    break;

  case NX_SessionAccepted:
    sRet = "Starting session";
    break;

  case NX_ConnectionEnd:
    sRet = "";
    break;

  case NX_ProxyReady:
    sRet = "Starting session";
    break;

  case NX_ProxyConnecting:
    sRet = "Initializing X protocol compression";
    break;

  case NX_ProxyNegotiating:
    sRet = "Negotiating link parameters";
    break;

  case NX_ProxyConnected:
    sRet = "Established X server connection";
    break;

  case NX_ProxyConnectionEnd:
    sRet = "";
    break;

  default:
    sRet = "";
  }

  return sRet;
}

bool NXConnection::ErrorOccurred() const
{
  return ( GetCurrentState( pSession ) == NX_Exception );
}

string NXConnection::GetError() const
{
  char *pError = GetLastError( pSession );
  string sError( pError );
  delete pError;
  return sError;
}

void NXConnection::Start()
{
#if defined(WIN32)
	// Start the X Server
  pXServer = CreateXServerConnection( pSession );
  if( InitXServerConnection( pXServer ) == NX_Exception )
  {
    NX_LOG_LOGERROR("Cannot init XServer.\n" );
       return;
  }
else
   {
    if( AdvanceXServerConnection( pXServer ) == NX_Exception )
    {
      NX_LOG_LOGERROR("XServer cannot start.\n" );
	  return;
    }
}
	
#endif
  pConnection = CreateServerConnection( pSession, "nxssh" );
  if( pConnection )
    InitServerConnection( pConnection );
}

bool NXConnection::Advance( unsigned int timeout )
{
  if( ErrorOccurred() )
    return false;

  if( !pProxy && SessionNeedToStartNewProxy( pSession) == NX_Ok )
  {
    NX_LOG_LOGDEBUG( "create proxy connection.");
    pProxy = CreateProxyConnection( pSession );
    if( InitProxyConnection( pProxy ) == NX_Exception )
      return false;
  }


  int iState = GetStateId();

  if( iState >= NX_ReadyForConnection && iState < NX_InitProxy )
  {
	if( EndServerConnection( pConnection ) != NX_Ok )
    {
      return ( AdvanceServerConnection( pConnection, timeout ) != NX_Exception );
    }
  }

  if( pProxy && EndProxyConnection( pProxy ) != NX_Ok )
  {
    if ( AdvanceProxyConnection( pProxy, timeout ) != NX_Exception )
      return ( AdvanceServerConnection( pConnection, timeout ) != NX_Exception );
  }

  return true;
}

bool NXConnection::Accepted() const
{
  return ( GetStateId() == NX_ProxyConnected );
}

bool NXConnection::SessionAccepted() const
{
   return (GetStateId() == NX_SessionAccepted );
}

void NXConnection::Stop()
{
  NX_LOG_LOGDEBUG( "Info: stop connection called.");
  int iState = GetStateId();

  if( iState > NX_ReadyForConnection && iState < NX_InitProxy )
  {
    NX_LOG_LOGDEBUG( "Info: terminating server connection.");
    TerminateServerConnection( pConnection );
  }

  if( iState > NX_InitProxy )
  {
    NX_LOG_LOGDEBUG( "terminating proxy connection." );
    TerminateProxyConnection( pProxy );
  }
}

bool NXConnection::SetParameters( NXSettings* sets )
{
  string sTmp = "";

  ClearParameters( pSession );

  sTmp = options->GetNXPersonalDirectory();

  NX_LOG_LOGDEBUG( "setting 'NX_PersonalDirectory' to '" + sTmp + "'." );
  SetStringParameter( pSession, NX_PersonalDirectory, sTmp.c_str() );

  NX_LOG_LOGDEBUG( "checking cookie...");

  sTmp = options->GetNXCookieFilePath();
  NX_LOG_LOGDEBUG( "Info: setting 'NX_CookieTempFilePath' to '" + sTmp + "'." );
  SetStringParameter( pSession, NX_CookieTempFilePath, sTmp.c_str() );

  if( GetExistingSessionCookie( pSession ) != NX_Ok )
  {
    if( GenerateSessionCookie( pSession ) != NX_Ok )
    {
      NX_LOG_LOGERROR( "session cookie NOT found... and cannot create a new one." );
      return false;
    }
  }
  NX_LOG_LOGDEBUG( "cookie found...");

  sTmp = options->GetNXSshPath();
  NX_LOG_LOGDEBUG( "Info: setting 'NX_SshPath' to '" + sTmp + "'.");
  SetStringParameter( pSession, NX_SshPath, options->GetNXSshPath().c_str() );

  sTmp = options->GetNXSshKeyPath();
  NX_LOG_LOGDEBUG( "Info: setting 'NX_SshKeyPath' to '" + sTmp + "'.");
  SetStringParameter( pSession, NX_SshKeyPath, sTmp.c_str() );

  sTmp = options->GetNXSshLogPath();
  NX_LOG_LOGDEBUG( "Info: setting 'NX_SshLogPath' to '" + sTmp + "'." );
  SetStringParameter( pSession, NX_SshLogPath, sTmp.c_str() );

#ifndef WIN32
SetBoolParameter(pSession, NX_ProxyUseOptionsFile, true);
#endif

  /* Convert other Paramaters */
  SettingsToParameters stp( sets, pSession );
  stp.Convert();

  SetStringParameter( pSession, NX_ProxyPath, options->GetNXProxyPath().c_str() );
  SetStringParameter( pSession, NX_ProxyLibraryPath, options->GetNXLibDirectory().c_str() );
  SetStringParameter( pSession, NX_ProxyMode, "S" );

#if defined (WIN32)
    /* XServer */
  string xspath = options->GetNXBinDirectory() + "\\nxwin.exe";
 
  SetStringParameter( pSession, NX_XServerPath, xspath.c_str() );
  SetStringParameter( pSession, NX_XServerName, "nxwin.exe" );
  SetStringParameter( pSession, NX_XServerPort, ":0" );
  SetStringParameter( pSession, NX_ProxyDisplay, ":0" );
 
  SetStringParameter( pSession, NX_XServer_FirstCustomOption, "-emulate3buttons" );
  SetStringParameter( pSession, NX_XServer_FirstCustomOption + 1, "-agent" );
  SetStringParameter( pSession, NX_XServer_FirstCustomOption + 2, "-hide" );
  SetStringParameter( pSession, NX_XServer_FirstCustomOption + 3 , "-noreset" );
  SetStringParameter( pSession, NX_XServer_FirstCustomOption + 4 , "-ac" );
#endif
  return true;
}

string NXConnection::GetLogPath() const
{
  string file_path = "";
  if( GetStateId() < NX_ProxyReady )
    file_path = options->GetNXSshLogPath();
  else
  {
    char *pLogPath = GetStringParameter( pSession, NX_ProxyLogPath, "" );
    file_path = pLogPath;
    delete pLogPath;
  }

  return file_path;
}

