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

#include "Process.h"

#ifndef WIN32
#include <sys/types.h>
#include <sys/wait.h>
#endif

#include "Logger.h"

using namespace std;

#define CHECKSTD(val1,val2) (val1 & val2)




namespace NX
{

Process::Process()
{
#ifdef WIN32
  m_process = INVALID_HANDLE_VALUE;
#else
  m_process = -1;
#endif
  m_device = "";
  m_redirect = (unsigned int)Process::NX_NOREDIRECT;
  m_endofstream = false;
}

Process::~Process()  //virtual destructor.
{}

void Process::AddArgument( string arg )
{
  m_arguments.push_back( arg );
  NX_LOG_LOGDEBUG( "NXProcess: argument '" +  arg + "' added." );
}

bool Process::StartProcess() //call SpawnProcess in derivated class.
{
  return SpawnProcess();
}

bool Process::StopProcess()
{
#ifdef WIN32
  if(m_process == INVALID_HANDLE_VALUE ) return true;

  DWORD exitCode = 7;
  return (::TerminateProcess(m_process , exitCode) == 1);
#else
  if(m_process == -1 ) return true;

  return (kill(m_process , SIGTERM) != -1); //send SIGTERM to stop the process.
#endif
}

bool Process::IsRunning() const
{
  bool retValue = false;

  if(GetExitCode() == NX_PROCESSACTIVE)
     retValue = true;
  else
     retValue = false;

  return retValue;
}

void Process::WaitProcessTerminate()  const
{
#ifdef WIN32
  WaitForSingleObject(m_process , INFINITE);
#else
  int current_status = 0;
  waitpid( m_process, &current_status, 0 /*WNOHANG*/ );
#endif
}

int Process::GetExitCode() const
{
#ifdef WIN32
  if(m_process == INVALID_HANDLE_VALUE ) return false;

  DWORD exitStatus;
  ::GetExitCodeProcess(m_process , &exitStatus);

  if(exitStatus == STILL_ACTIVE )
    return (int)NX_PROCESSACTIVE;
  return NX_PROCESSEXITED;
#else
  if(m_process == -1 ) return false;
  int exitCode , current_status;

  exitCode = waitpid(m_process, &current_status, WNOHANG);

  if( exitCode == 0 )
    return (int)NX_PROCESSACTIVE;
  return current_status;
#endif
}

bool Process::IsStdinRedirected() const
{
  if(CHECKSTD(m_redirect,NX_STDIN))
    return true;
  else
    return false;
}

bool Process::IsStdoutRedirected() const
{
  if(CHECKSTD(m_redirect,NX_STDOUT))
    return true;
  else
    return false;
}

bool Process::IsStderrRedirected() const
{
  if(CHECKSTD(m_redirect,NX_STDERR))
    return true;
  else
    return false;
}

string Process::GetEnvironment( string key )
{
#ifdef WIN32
  char stroutVar[256];
  memset( stroutVar, 0, 256 );
  ::GetEnvironmentVariable( key.c_str() ,stroutVar ,255 );
  return string( stroutVar );
#else
  char *pChar = getenv( key.c_str() );
  if( pChar == NULL )
    return string( "" );
  else
    return string( pChar );
#endif
}

void Process::SetEnvironment( string key, string value )
{
#ifdef WIN32
  ::SetEnvironmentVariable( key.c_str(), value.c_str() );
#else
  setenv( key.c_str(), value.c_str(), 0 );
#endif
}


void Process::OverwriteEnvironment( string key, string value )
{
#ifdef WIN32
  ::SetEnvironmentVariable( key.c_str(), value.c_str() );
#else
  setenv( key.c_str(), value.c_str(), 1 );
#endif
}

} /* NX */
