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


#include "Proxy.h"
#include "Session.h"
#include "Process.h"
#include "Protocol.h"
#include "Utilities.h"
#include "StringUtilities.h"

#include "NXParameters.h"
#include "NXStates.h"

#ifdef WIN32
#include "ProcessWin32.h"
#else
#include "ProcessUnix.h"
#endif

#include "Logger.h"

#include <sys/types.h>
#include <sys/stat.h>


using namespace std;

namespace NX
{


Proxy::Proxy( Session* pSession )
{
  mp_session = pSession;
  mp_process = NULL;
  m_buffer = "";
}

Proxy::~Proxy()
{
  DeleteProcess();
}

void Proxy::ClearBuffer()
{
  m_buffer = "";
  NX_LOG_LOGDEBUG( "NXProxy: buffer cleared." );
}

void Proxy::DeleteProcess()
{
  if( mp_process != NULL )
  {
    delete mp_process;
    mp_process = NULL;
  }
}

bool Proxy::InitProcess()
{
  if( mp_process != NULL )
  {
    NX_LOG_LOGERROR( "NXProxy: a process already exists." );
    mp_session->SetException( NX_RuntimeError, "Cannot create another nxproxy process." );
    return false;
  }

  try
  {
#ifdef WIN32
    mp_process = new ProcessWin32();
#else
    mp_process = new ProcessUnix();
#endif
  }
  catch( bad_alloc& )
  {
    NX_LOG_LOGERROR( "NXProxy: unable to find memory to create process." );
    mp_session->SetException( NX_RuntimeError, "Cannot create new nxproxy process." );
    return false;
  }

  return true;
}

bool Proxy::InitConnection()
{
  NX_LOG_LOGDEBUG( "NXProxy: initializing..." );

  NX_LOG_LOGDEBUG( "NXProxy: creating new process." );

  if( !InitProcess() )
    return false;

  NX_LOG_LOGDEBUG( "NXProxy: new process created." );

  ParametersList& parameters = mp_session->GetParameters();

  string sTmp;

  mp_process->SetRedirectFlags( Process::NX_STDERR | Process::NX_STDOUT );

  NX_LOG_LOGDEBUG( "NXProxy: set flags for redirection." );

  sTmp = parameters.GetString( NX_ProxyPath, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXProxy: parameter 'NX_ProxyPath' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_ProxyPath' is empty." );
    return false;
  }
  else
  {
    mp_process->SetProcessName( "nxproxy" );
    mp_process->SetProcessPath( sTmp );
  }

  sTmp = parameters.GetString( NX_ProxyName, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGDEBUG( "NXProxy: set default parameter 'NX_ProxyName'." );
    mp_process->SetProcessName( "nxproxy" );
  }
  else
    mp_process->SetProcessName( sTmp );
  
  sTmp = parameters.GetString( NX_ProxyMode, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXProxy: parameter 'NX_ProxyMode' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_ProxyMode' is empty." );
    return false;
  }
  else
  {
    string mode( "-" );
    mode += sTmp;
    mp_process->AddArgument( mode );
  }

  sTmp = parameters.GetString( NX_ProxyLogPath, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGDEBUG( "NXProxy: parameter 'NX_ProxyLogPath' is empty." );
    if( Protocol::SetStandardSessionLogPath( parameters ) )
    {
      sTmp = parameters.GetString( NX_ProxyLogPath, "" );
      NX_LOG_LOGDEBUG( "NXProxy: set default parameter 'NX_ProxyLogPath'." );
    }
    else
    {
      NX_LOG_LOGERROR( "NXProxy: cannot create standard parameter 'NX_ProxyLogPath'." );
      mp_session->SetException( NX_InvalidParameter, "Cannot create standard parameter 'NX_ProxyLogPath'." );
      return false;
    }
  }

  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXProxy: parameter 'NX_ProxyLogPath' is empty." );
    return false;
  }
  else
    mp_process->SetDeviceName( sTmp );
/*
  sTmp = parameters.GetString( NX_EncryptionPort, "" );
  if( !sTmp.empty() && parameters.GetBool( NX_EnableEncryption, false ) )
  {
#if defined( NX_PROXY_DEBUG )
    cout << "NXProxy: add parameter 'NX_ProxyEncryptionPort'." << endl << flush;
   os.close();
#endif
    mp_process->AddArgument( "-P" );
    mp_process->AddArgument( sTmp );
  }
*/

  if( !CreateOptions() )
  {
     NX_LOG_LOGERROR("NXProxy: CreateOptions Failed");
	  return false;
  }

  if( !CreateSessionDirectory() )
  {
     NX_LOG_LOGERROR("NXProxy: CreateSessionDirectory Failed");
  
	  return false;
  }

  if( parameters.GetBool( NX_ProxyUseOptionsFile, false ) )
  {
     if( !CreateOptionsFile() )
	 {
     NX_LOG_LOGERROR("NXProxy: CreateOptionsFile Failed");
return false;
	 }
     sTmp = "options=";
     sTmp += parameters.GetString( NX_ProxyOptionsPath, "" );
     sTmp += ":";
     sTmp += parameters.GetString( NX_ProxyPort, "" );

     mp_process->AddArgument( sTmp );
  }
  else
  {
     sTmp = parameters.GetString( NX_ProxyOptions, "" );
     mp_process->AddArgument( sTmp );
  }

  sTmp = parameters.GetString( NX_ProxyDisplay, "" );
  if( !sTmp.empty() )
  {
    NX_LOG_LOGDEBUG( "NXProxy: add parameter 'NX_ProxyDisplay'." );
    mp_process->AddArgument( "-D" );
    mp_process->AddArgument( sTmp );
  }

  ClearBuffer();
  mp_session->SetState( NX_ProxyReady );
  return true;
}

bool Proxy::CreateOptions()
{
  NX_LOG_LOGDEBUG( "NXProxy: initializing options." );

  ParametersList& parameters = mp_session->GetParameters();

  string sTmp;

  sTmp = parameters.GetString( NX_SessionId, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXProxy: parameter 'NX_SessionId' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_SessionId' is empty." );
    return false;
  }
  else
    parameters.SetString( NX_ProxySessionId, sTmp );

  sTmp = parameters.GetString( NX_SessionName, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXProxy: parameter 'NX_SessionName' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_SessionName' is empty." );
    return false;
  }
  else
    parameters.SetString( NX_ProxySessionName, sTmp );

  if(!parameters.GetBool( NX_EnableEncryption, false ) )
  {
  sTmp = parameters.GetString( NX_ProxyConnect, "" );
  if( sTmp.empty() )
  {
//    string host = parameters.GetString( NX_yServer, "" );
    string host = parameters.GetString( NX_HostName, "" );
    if( host.empty() )
    {
      NX_LOG_LOGDEBUG( "NXProxy: parameter 'NX_ProxyServer' is empty." );
      NX_LOG_LOGDEBUG( "NXProxy: set default parameter 'NX_ProxyServer' = 'NX_HostName'." );
      host = parameters.GetString( NX_HostName, "" );

      if( host.empty() )
      {
        NX_LOG_LOGERROR( "NXProxy: parameter 'NX_HostName' is empty." );
        mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_ProxyServer' is empty." );
        return false;
      }
    }

    string port = parameters.GetString( NX_ProxyPort, "" );
    if( port.empty() )
    {
      NX_LOG_LOGERROR( "NXProxy: parameter 'NX_ProxyPort' is empty.");
      mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_ProxyPort' is empty." );
      return false;
    }

    sTmp += host;
    sTmp += string( ":" );
    sTmp += port;

    parameters.SetString( NX_ProxyConnect, sTmp );

  }
  }  else {

  sTmp = parameters.GetString( NX_ProxyListen, "" );
  if( sTmp.empty() )
  {
    string encrport = parameters.GetString( NX_EncryptionPort, "" );
    if( encrport.empty() )
    {
      NX_LOG_LOGERROR( "NXProxy: parameter 'NX_EncryptionPort' is empty." );
        mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_EncryptionPort' is empty." );
        return false;
    }

    string port = parameters.GetString( NX_ProxyPort, "" );
    if( port.empty() )
    {
      NX_LOG_LOGERROR( "NXProxy: parameter 'NX_ProxyPort' is empty." );
      mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_ProxyPort' is empty." );
      return false;
    }

    sTmp += encrport;
    sTmp += string( ":" );
    sTmp += port;

    parameters.SetString( NX_ProxyListen, sTmp );
  }
}

  sTmp = parameters.GetString( NX_ProxyCookie, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXProxy: parameter 'NX_ProxyCookie' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_ProxyCookie' is empty." );
    return false;
  }

  sTmp = parameters.GetString( NX_ProxyRoot, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGDEBUG( "NXProxy: parameter 'NX_ProxyRoot' is empty." );

    sTmp = parameters.GetString( NX_PersonalDirectory, "" );
    if( sTmp.empty() )
    {
      NX_LOG_LOGERROR( "NXProxy: parameter 'NX_PersonalDirectory' is empty." );
      mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_PersonalDirectory' is empty." );
      return false;
    }
    else
    {
      NX_LOG_LOGDEBUG( "NXProxy: set default parameter 'NX_ProxyRoot'." );
	  // Because this path is passed on the command line, even the double backslash needs to be double backslashed
#if defined ( WIN32 )
      parameters.SetString( NX_ProxyRoot, "c:\\\\.nx" );
#else
      parameters.SetString( NX_ProxyRoot, sTmp );
#endif
    }

  }

  /* Not useful since proxy 1.2.2
  sTmp = parameters.GetString( NX_ProxyCacheType, "" );
  if( sTmp.empty() )
  {
#if defined( NX_DEBUG )
    cout << "NXProxy: parameter 'NX_ProxyCacheType' is empty." << endl << flush;
#endif
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_ProxyCacheType' is empty." );
    return false;
  }
  */
  
  sTmp = parameters.GetString( NX_ProxyNoDelay, "" );
  if( sTmp.empty() )
  {
    sTmp = parameters.GetString( NX_EnableTcpNoDelay, "" );
    if( !sTmp.empty() )
    {
      if( parameters.GetBool( NX_EnableTcpNoDelay, false ) )
        parameters.SetNumber( NX_ProxyNoDelay, 1 );
      else
        parameters.SetNumber( NX_ProxyNoDelay, 0 );
    }
  }

  if( parameters.GetBool( NX_EnableMedia, false ) )
  {
    sTmp = parameters.GetString( NX_ProxyMediaPort, "" );
    if( sTmp.empty() )
    {
      NX_LOG_LOGERROR( "NXProxy: parameter 'NX_ProxyMediaPort' is empty." );
      mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_ProxyMediaPort' is empty." );
      return false;
    }
  }

  if( parameters.GetBool( NX_EnableSamba, false ) )
  {
    sTmp = parameters.GetString( NX_ProxySamba, "" );
    if( sTmp.empty() )
    {
      NX_LOG_LOGERROR( "NXProxy: parameter 'NX_ProxySamba' is empty." );
      mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_ProxySamba' is empty." );
      return false;
    }
  }

  sTmp = Protocol::GetProxyOptions( parameters );

  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXProxy: unable to create options." );
    mp_session->SetException( NX_RuntimeError, "Unable to create options for proxy." );
    return false;
  }
  else
    parameters.SetString( NX_ProxyOptions, sTmp );

  return true;
}

bool Proxy::CreateOptionsFile()
{
  ParametersList& parameters = mp_session->GetParameters();
  string sTmp = parameters.GetString( NX_ProxyOptionsPath, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGDEBUG( "NXProxy: parameter 'NX_ProxyOptionsPath' is empty." );
    if( Protocol::SetStandardProxyOptionsPath( parameters ) )
    {
      sTmp = parameters.GetString( NX_ProxyOptionsPath, "" );
      NX_LOG_LOGDEBUG( "NXProxy: set default parameter 'NX_ProxyOptionsPath'." );
    }
    else
    {
      NX_LOG_LOGERROR( "NXProxy: cannot create standard parameter 'NX_ProxyOptionsPath'." );
      mp_session->SetException( NX_InvalidParameter, "Cannot create standard parameter 'NX_ProxyOptionsPath'." );
      return false;
    }
  }

  ofstream ofs( sTmp.c_str(), ios::out );
  if( !ofs )
  {
      NX_LOG_LOGERROR( "NXProxy: cannot create options file (" + sTmp + ")." );
      mp_session->SetException( NX_InvalidParameter, "Cannot create options file for proxy." );
      return false;
  }

  sTmp = parameters.GetString( NX_ProxyOptions, "" );
  ofs << sTmp << flush;
  ofs.close();

  return true;
}

bool Proxy::CreateSessionDirectory()
{
  ParametersList& parameters = mp_session->GetParameters();
  string sTmp = parameters.GetString( NX_SessionDirectory, "" );;
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXProxy: parameter 'NX_SessionDirectory' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_SessionDirectory' is empty." );
    return false;
  }

  Utilities::ConvertSeparatorsInPath( sTmp );   

  if ((Utilities::FileExist(sTmp)) && (Utilities::IsDirectory(sTmp))) return true;
  if( Utilities::CreateDirectory( sTmp ) )
  {
    NX_LOG_LOGERROR( "NXProxy: cannot create directory: '" + sTmp + "'." );
    mp_session->SetException( NX_InvalidParameter, "NXProxy: cannot create Session Directory." );
    return false;
  }

  return true;
}

bool Proxy::StartConnection()
{
  if( mp_process == NULL )
  {
    NX_LOG_LOGERROR( "NXProxy: nxproxy process not found." );
    mp_session->SetException( NX_RuntimeError, "Process nxproxy is not correctly initialized." );
    return false;
  }

  NX_LOG_LOGDEBUG( "NXProxy: starting..." );

  string sTmp = mp_session->GetParameters().GetString( NX_ProxyLibraryPath, "" );
  if( !sTmp.empty() )
  {
    NX_LOG_LOGDEBUG( "NXProxy: using library path '" +  sTmp + "'." );
    // First see if the path is already set
    string sCurrent = Process::GetEnvironment("LD_LIBRARY_PATH");
    if (sCurrent.find(sTmp) == string::npos)
    {
       sTmp += ":" + sCurrent;
    }
    NX_LOG_LOGDEBUG( "NXProxy: using library path '" + sTmp + "'." );
    Process::OverwriteEnvironment( "LD_LIBRARY_PATH", sTmp );
  }
  NX_LOG_LOGDEBUG( "new library path: " + Process::GetEnvironment("LD_LIBRARY_PATH"));

  if( !mp_process->StartProcess() )
  {
    NX_LOG_LOGERROR( "NXProxy: nxproxy cannot start." );
    mp_session->SetException( mp_process->GetException() );
    return false;
  }

  NX_LOG_LOGDEBUG( "NXProxy: connecting..." );

  mp_session->SetState( NX_ProxyConnecting );
  return true;
}

bool Proxy::CheckCurrentState( unsigned int timeout )
{
  string line = "";
  int iSize = mp_process->Read( line, timeout );
  if( iSize == -1 )
  {
    mp_session->SetException( mp_process->GetException() );
    return false;
  }

  m_buffer += line;

  if( Protocol::CheckProxyStatus( line, mp_session, this ) == NX_Exception )
    return false;
  else
    return true;
}

bool Proxy::AdvanceConnection( unsigned int timeout )
{
  if( mp_session->GetState() == NX_ProxyReady )
    return StartConnection();

  if( !CheckCurrentState( timeout )  )
    return false;

  if( mp_session->GetState() == NX_ProxyConnected ) {
    NX_LOG_LOGINFO( "NXProxy: session started." );
  }

  return true;
}

bool Proxy::BreakConnection()
{
  NX_LOG_LOGDEBUG( "NXProxy: break connection." );

  if( mp_process == NULL )
  {
    NX_LOG_LOGERROR( "NXProxy: process is NULL." );
    return false;
  }

  if( mp_process->IsRunning() )
    if( mp_process->StopProcess() )
      mp_process->WaitProcessTerminate();

  DeleteProcess();

  if( mp_session->GetState() != NX_Exception )
    mp_session->SetState( NX_ProxyConnectionEnd );

  return true;
}


} /* NX */


