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


#include "XServer.h"
#include "Session.h"
#include "Process.h"

#include "NXParameters.h"
#include "Logger.h"

#ifdef WIN32
#include "ProcessWin32.h"
#else
#include "ProcessUnix.h"
#endif

#if defined( NX_DEBUG )
#include <iostream>
#endif

using namespace std;



namespace NX
{


XServer::XServer( Session* pSession )
{
  mp_session = pSession;
  mp_process = NULL;
  m_state = NoInit;
}

XServer::~XServer()
{
  DeleteProcess();
}

void XServer::DeleteProcess()
{
  if( mp_process != NULL )
  {
    delete mp_process;
    mp_process = NULL;
    m_state = NoInit;
  }
}

bool XServer::InitProcess()
{
  if( mp_process != NULL )
  {
    NX_LOG_LOGERROR( "XServer: a process already exists." );
    mp_session->SetException( NX_RuntimeError, "Cannot create another XServer process." );
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
    NX_LOG_LOGERROR( "XServer: unable to find memory to create process." );
    mp_session->SetException( NX_RuntimeError, "Cannot create new XServer process." );
    return false;
  }

  return true;
}

bool XServer::SetStandardWindowName( Session* pSession )
{
  string sTmp = "";
  string sName = "";
  ParametersList& parameters = pSession->GetParameters();

  sTmp = parameters.GetString( NX_Username, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "XServer: parameter 'NX_Username' is empty." );
    pSession->SetException( NX_InvalidParameter, "Parameter 'NX_Username' is empty." );
    return false;
  }
  
  sName += sTmp;
  sName += "@";

  sTmp = parameters.GetString( NX_HostName, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "XServer: parameter 'NX_HostName' is empty." );
    pSession->SetException( NX_InvalidParameter, "Parameter 'NX_HostName' is empty." );
    return false;
  }

  sName += sTmp;

  parameters.SetString( NX_XServerWindowName, sName );

  return true;
}

bool XServer::InitConnection()
{
  string sTmp = "";
  const ParametersList& parameters = mp_session->GetParameters();

  if( !InitProcess() )
    return false;

  sTmp = parameters.GetString( NX_XServerPath, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "XServer: parameter 'NX_XServerPath' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_XServerPath' is empty." );
    DeleteProcess();
    return false;
  }

  mp_process->SetProcessPath( sTmp );

  sTmp = parameters.GetString( NX_XServerName, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "XServer: parameter 'NX_XServerName' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_XServerName' is empty." );
    DeleteProcess();
    return false;
  }

  mp_process->SetProcessName( sTmp );

  sTmp = parameters.GetString( NX_XServerWindowName, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGDEBUG( "XServer: add default parameter 'NX_XServerWindowName'." );
    if( !SetStandardWindowName( mp_session ) )
    {
      NX_LOG_LOGERROR( "XServer: cannot create default parameter 'NX_XServerWindowName'." );
      DeleteProcess();
      return false;
    }
  }

  for( unsigned int i = NX_XServer_FirstCustomOption; i <= NX_XServer_LastCustomOption; i++ )
  {
    sTmp = parameters.GetString( i, "" );
    if( !sTmp.empty() )
    {
      NX_LOG_LOGDEBUG( "XServer: adding custom parameter '" + sTmp + "'." );
      mp_process->AddArgument( sTmp );
    }
  }

  mp_process->AddArgument( "-name" );
  mp_process->AddArgument( parameters.GetString( NX_XServerWindowName, "Undefined" ) );


  sTmp = parameters.GetString( NX_XServerPort, "" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGERROR( "XServer: parameter 'NX_XServerPort' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_XServerPort' is empty." );
    DeleteProcess();
    return false;
  }

  mp_process->AddArgument( sTmp );

  m_state = Init;

  return true;
}
 
bool XServer::StartConnection()
{
  if( mp_process->StartProcess() )
  {
    m_state = Running;
    return true;
  }
  
  return false;
}

bool XServer::IsRunning()
{
  return( mp_process->WaitInitProcess() );
}
  
bool XServer::CheckCurrentState()
{
  if( mp_process->IsRunning() )
  {
    m_state = Running;
    return true;
  }
  else
  {
    m_state = NotRunning;
    return false;
  }
  
}

bool XServer::AdvanceConnection()
{
  switch( m_state )
  {
  case NoInit:
     NX_LOG_LOGERROR("XServer: not init yet, cannot advance connection." );
    mp_session->SetException( NX_RuntimeError, "XServer is not initialized." );
    return false;

  case Init:
    return StartConnection();
    break;

  case Running:
    return CheckCurrentState();
    break;

  case NotRunning:
    NX_LOG_LOGDEBUG( "XServer: not running." );
    break;

  default:
    NX_LOG_LOGERROR( "XServer: invalid connection state." );
    mp_session->SetException( NX_RuntimeError, "XServer has invalid connection state." );
    DeleteProcess();
    return false;
  };
  
  return true;
}

bool XServer::BreakConnection()
{
  NX_LOG_LOGDEBUG( "XServer: break connection." );

  if( mp_process == NULL )
  {
    NX_LOG_LOGERROR( "XServer: process is NULL." );
    return false;
  }

  if( mp_process->IsRunning() )
    if( mp_process->StopProcess() )
      mp_process->WaitProcessTerminate();

  DeleteProcess();

  return true;
}


} /* NX */

