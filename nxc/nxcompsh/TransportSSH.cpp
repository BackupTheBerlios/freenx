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


#include "TransportSSH.h"
#include "Session.h"
#include "Protocol.h"
#include "Process.h"

#include "NXErrors.h"
#include "NXStates.h"
#include "NXParameters.h"

#ifdef WIN32
#include "ProcessWin32.h"
#else
#include "ProcessUnix.h"
#endif

#include "Logger.h"

#if defined( NX_DEBUG ) || defined( NX_SSH_DEBUG )
#include <iostream>
#endif

#include <stdexcept>
using namespace std;



namespace NX
{

TransportSSH::TransportSSH( Session *pSession ) : Transport( pSession )
{
  m_type = "nxssh";
  mp_process = NULL;
}

TransportSSH::~TransportSSH()
{
  DeleteProcess();
}

void TransportSSH::DeleteProcess()
{
  if( mp_process != NULL )
  {
    delete mp_process;
    mp_process = NULL;
  }
}

bool TransportSSH::InitProcess()
{
  if( mp_process != NULL )
  {
    NX_LOG_LOGERROR( "NXTransport (nxssh): a process already exists." );
    mp_session->SetException( NX_RuntimeError, "Cannot create another nxssh process." );
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
    NX_LOG_LOGERROR( "NXTransport (nxssh): unable to find memory to create process." );
    mp_session->SetException( NX_RuntimeError, "Cannot create new nxssh process." );
    return false;
  }

  return true;
}

bool TransportSSH::Write( const string &in )
{
  if( mp_process != NULL )
  {
    if( !mp_process->Write( in ) )
    {
      mp_session->SetException( mp_process->GetException() );
      return false;
    }
    else
      return true;
  }
  else
  {
    mp_session->SetException( NX_RuntimeError, "Cannot write on stdin of nxssh process." );
    return false;
  }
}

int TransportSSH::Read( string & out, unsigned int timeout )
{
  int iRet = -1;
  if( mp_process != NULL )
    iRet = mp_process->Read( out, timeout );

  if( iRet == -1 )
  {
    out = "";
    mp_session->SetException( NX_RuntimeError, "Cannot read from stout or stderr of nxssh process." );
  }

  return iRet;
}

bool TransportSSH::EndConnection()
{
  return ( mp_session->GetState() == NX_ConnectionEnd );
//  return ( mp_session->GetState() == NX_ConnectionEnd ||
//           mp_session->GetState() == NX_SessionAccepted );
}

bool TransportSSH::BreakConnection()
{
  NX_LOG_LOGDEBUG( "NXTransport (nxssh): break connection." );

  if( mp_process == NULL )
  {
    NX_LOG_LOGERROR( "NXTransport (nxssh): process is NULL." );
    return false;
  }

  if( mp_process->IsRunning() )
    if( mp_process->StopProcess() )
      mp_process->WaitProcessTerminate();

  DeleteProcess();

  if( mp_session->GetState() != NX_Exception )
    mp_session->SetState( NX_ConnectionEnd );

  return true;
}

bool TransportSSH::InitConnection()
{
  NX_LOG_LOGDEBUG("NXTransport (nxssh): creating new process." );

  if( !InitProcess() )
    return false;

  NX_LOG_LOGDEBUG( "NXTransport (nxssh): new process created." );

  ParametersList& parameters = mp_session->GetParameters();

  string sTmp;

  mp_process->SetRedirectFlags( Process::NX_STDIN | Process::NX_STDERR | Process::NX_STDOUT );

  NX_LOG_LOGDEBUG( "NXTransport (nxssh): set flags for redirection." );

  sTmp = parameters.GetString( NX_SshPath, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXTransport (nxssh): parameter 'NX_SshPath' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_SshPath' is empty." );
    return false;
  }
  else
    mp_process->SetProcessPath( sTmp );
  

  sTmp = parameters.GetString( NX_SshName, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXTransport (nxssh): set default parameter 'NX_SshName'." );
    mp_process->SetProcessName( "nxssh" );
  }
  else
    mp_process->SetProcessName( sTmp );

  mp_process->AddArgument( "-nx" );

  sTmp = parameters.GetString( NX_SshKeyPath, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXTransport (nxssh): parameter 'NX_SshKeyPath' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_SshKeyPath' is empty." );
    return false;
  }
  else
  {

    mp_process->AddArgument( "-i" );
#if defined (WIN32)
	sTmp = "\"" + sTmp + "\""; 
#endif 

    mp_process->AddArgument( sTmp );
  }
/*
  if( parameters.GetBool( NX_EnableEncryption, false ) )
  {
    sTmp = parameters.GetString( NX_EncryptionPort, "" );
    if( sTmp.empty() )
    {
#if defined( NX_DEBUG )
      cout << "NXTransport (nxssh): parameter 'NX_EncryptionPort' is empty." << endl << flush;
#endif
      mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_EncryptionPort' is empty." );
      return false;
    }
    else
    {
      mp_process->AddArgument( "-D" );
      mp_process->AddArgument( sTmp );
    }
  }
*/
  for( unsigned int i = NX_Ssh_FirstCustomOption; i <= NX_Ssh_LastCustomOption; i++ )
  {
    sTmp = parameters.GetString( i, "" );
    if( !sTmp.empty() )
    {
      NX_LOG_LOGERROR( "NXTransport (nxssh): adding custom parameter '" + sTmp + "'." );
      mp_process->AddArgument( sTmp );
    }
  }

  sTmp = parameters.GetString( NX_SshUsername, "nx" );
  sTmp += string("@");

  string sHost = parameters.GetString( NX_HostName, "" );

  if( sHost.empty() )
  {
    NX_LOG_LOGERROR( "NXTransport (nxssh): parameter 'NX_Hostname' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_HostName' is empty." );
    return false;
  }
  else
  {
    sTmp += sHost;
    mp_process->AddArgument( sTmp );
  }

  sTmp = parameters.GetString(NX_HostPort, "");
  if (!sTmp.empty())
  {
     mp_process->AddArgument("-p");
     mp_process->AddArgument(sTmp);
  }

  sTmp = parameters.GetString( NX_SshLogPath, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXTransport (nxssh): parameter 'NX_SshLogPath' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_SshLogPath' is empty." );
    return false;
  }
  else
    mp_process->SetDeviceName( sTmp );


  if( parameters.GetBool( NX_EnableEncryption, false ) )
  {
     string sshmode;
#if defined (WIN32)
     sshmode = "-v";
     mp_process->AddArgument(sshmode);
#endif
     sshmode = "-B";
     mp_process->AddArgument(sshmode);
  }


  sTmp = parameters.GetString( NX_Username, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXTransport (nxssh): parameter 'NX_Username' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_Username' is empty." );
    return false;
  }

  sTmp = parameters.GetString( NX_Password, "" );
  if( sTmp.empty() )
  {
    sTmp = parameters.GetString( NX_PasswordMD5, "" );
    if( sTmp.empty() )
    {
      NX_LOG_LOGERROR( "NXTransport (nxssh): parameter 'NX_Password' is empty." );
      mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_Password' is empty." );
      return false;
    }
  }

  sTmp = parameters.GetString( NX_SessionCookie, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "NXTransport (nxssh): parameter 'NX_SessionCookie' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_SessionCookie' is empty." );
    return false;
  }

  m_buffer = "";
  mp_session->SetState( NX_ReadyForConnection );
  return true;
}


bool TransportSSH::StartConnection()
{
  if( mp_process == NULL )
  {
    NX_LOG_LOGERROR( "NXTransport (nxssh): nxssh process not found." );
    mp_session->SetException( NX_RuntimeError, "Process nxssh is not correctly initialized." );
    return false;
  }

  if( !mp_process->StartProcess() )
  {
    NX_LOG_LOGERROR( "NXTransport (nxssh): nxssh cannot start." );
    mp_session->SetException( mp_process->GetException() );
    return false;
  }

  NX_LOG_LOGDEBUG( "NXTransport (nxssh): connecting..." );

  mp_session->SetState( NX_Connecting );
  return true;
}

bool TransportSSH::AdvanceConnection( unsigned int timeout )
{
  if( mp_session->GetState() == NX_ReadyForConnection )
    return StartConnection();

  if( !CheckCurrentState( timeout )  )
  {
    BreakConnection();
    return false;
  }

  return true;
}

bool TransportSSH::CloseConnection()
{
  NX_LOG_LOGDEBUG("Transport: Closing Connection");
  if( mp_process->IsRunning() )
    return Protocol::CloseConnection( mp_session, this );
  return true;
}

bool TransportSSH::CheckCurrentState( unsigned int timeout )
{
  string line = "";
  int iSize = mp_process->Read( line, timeout );
  if( iSize == -1 )
  {
    mp_session->SetException( mp_process->GetException() );
    return false;
  }
if (line.length() > 0) 
  m_buffer += line;

  if( mp_process->IsRunning() )
  {
    if( Protocol::CheckConnectionStatus( line, mp_session, this ) == NX_Exception )
      return false;
    if( mp_session->GetState() == NX_SessionAccepted )
    {
      if( !mp_session->GetParameters().GetBool( NX_EnableEncryption, false ) )
        Protocol::CloseConnection( mp_session, this );
    }
  }
  else
  {
    if( !Protocol::CheckConnectionError( mp_session, this ) &&
        mp_session->GetState() != NX_SessionAccepted )
    {
      mp_session->SetException( NX_SSHError, m_buffer );
      return false;
    }
  }

  return true;
}


} /* NX */

