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


#include "ProcessWin32.h"

//#include <iostream>
#include <fstream>
#include "Logger.h"

using namespace std;



namespace NX
{

ProcessWin32::ProcessWin32() : Process()
{
  hClientOut_rd = hClientOut_wr = hClientIn_rd = hClientIn_wr = INVALID_HANDLE_VALUE;
  m_predirect = NULL;
}

ProcessWin32::~ProcessWin32()
{}

bool ProcessWin32::SpawnProcess()
{
  bool retValue  = false;

  bool stdinput  = false;
  bool stdouterr = false;

  if( m_device.empty() ) //no redirect in device.
  {
     if( IsStderrRedirected() || IsStdoutRedirected() )
     {
       if(!CreatePipe(&hClientOut_rd,&hClientOut_wr,NULL,0))
       {
         m_exception.SetRuntimeError( "Cannot create pipe for stderr or stdout." );
         NX_LOG_LOGERROR("NXProcess: cannot create pipe for stderr or stdout.");
         return retValue;
       }

       stdouterr = true;
     }

     if( IsStdinRedirected() )
     {
       if(!CreatePipe(&hClientIn_rd,&hClientIn_wr,NULL,0))
	   {
         m_exception.SetRuntimeError( "Cannot create pipe for stderr or stdout." );
         NX_LOG_LOGERROR( "NXProcess: cannot create pipe for stdin.");
         return retValue;
	   }

       stdinput = true;
     }
  }
  else //redirect in device.
  {
    if( IsStdinRedirected() )
    {
       if(!CreatePipe(&hClientIn_rd,&hClientIn_wr,NULL,0))
       {
         m_exception.SetRuntimeError( "Cannot create pipe for stdin." );
         NX_LOG_LOGERROR( "NXProcess: cannot create pipe for stdin.");
         return retValue;
       }

       stdinput  = true;

    }
    hClientOut_wr = CreateFile(m_device.c_str(), GENERIC_WRITE | GENERIC_READ,
                             FILE_SHARE_WRITE | FILE_SHARE_READ,
                             NULL,
                             CREATE_ALWAYS ,
                             0,NULL);

    m_predirect = fopen(m_device.c_str() , "r");

    if(hClientOut_wr  == INVALID_HANDLE_VALUE  || m_predirect == NULL )
    {
      m_exception.SetRuntimeError( m_device + ": cannot open this file." );
      NX_LOG_LOGERROR( "NXProcess: cannot open device to redirect stdout and stderr." );
      return retValue;
    }

    stdouterr = true;
  }

  if( stdinput )
  {
    DuplicateHandle(GetCurrentProcess(),hClientIn_rd,GetCurrentProcess(),
                   &hClientIn_rd,0,TRUE,DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE);
  }

  if( stdouterr )
  {
    DuplicateHandle(GetCurrentProcess(),hClientOut_wr,GetCurrentProcess(),
                   &hClientOut_wr,0,TRUE,DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE);
  }

  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  memset(&si,0,sizeof(STARTUPINFO));
  memset(&pi,0,sizeof(PROCESS_INFORMATION));

  si.cb         = sizeof(STARTUPINFO);
  si.dwFlags    = STARTF_USESTDHANDLES;

  if( stdinput )
    si.hStdInput  = hClientIn_rd;

  if(stdouterr)
  {
    si.hStdOutput = hClientOut_wr;
    si.hStdError  = hClientOut_wr;
  }

  string tmp_command( m_processPath );

  for(list<string>::const_iterator it = m_arguments.begin(); it != m_arguments.end(); ++it)
  {
    tmp_command += " ";
    tmp_command += *it;
  }
   NX_LOG_LOGDEBUG( "Info: Process is " + tmp_command);

  if(!CreateProcess(NULL,(char*)tmp_command.c_str(),NULL,NULL,TRUE,NORMAL_PRIORITY_CLASS | DETACHED_PROCESS,NULL,NULL,&si,&pi))
  {
    m_exception.SetRuntimeError( "Cannot create new process." );
    NX_LOG_LOGERROR( "NXProcess: cannot create new process." );

    retValue = false;
  }
  else
  {
    m_process = pi.hProcess;
    retValue = true;
  }

  CloseHandle(pi.hThread);

  if(stdinput)
    CloseHandle(hClientIn_rd);

  if(stdouterr)
    CloseHandle(hClientOut_wr);

  return retValue;
}

bool ProcessWin32::Write( const string & in )
{
  NX_LOG_LOGDEBUG("Write: " + in);
  bool retValue = false;
  if( IsStdinRedirected() )
  {
    DWORD ignore;
    ::WriteFile(hClientIn_wr,in.c_str(),in.length(),&ignore,NULL);
    return true;
  }
  return retValue;
}

bool ProcessWin32::EndOfStream()
{
  if(m_device.empty())
  {
    return m_endofstream;
  }
  else
  {
    if(IsRunning())
      return m_endofstream;
    return (m_endofstream = true);
  }
}

int ProcessWin32::Read( string & out, unsigned int timeout )
{
  DWORD num;
  char buff[MAX_BUFFER_SIZE] = {0};
  if(m_device.empty())
  {
   int bTest;

   bTest = ::ReadFile(hClientOut_rd,buff,(MAX_BUFFER_SIZE -1),&num,NULL);

   if(bTest == 0)
   {
     m_endofstream = true;
     return -1;
   }
   out = buff;
   return (int)num;
  }
  else
  {

    if( m_predirect == NULL )
    {
      m_exception.SetRuntimeError( "Unable to read. Stdout and stderr are not redirected." );
      NX_LOG_LOGERROR( "NXProcess: unable to read, stdout and stderr are not redirected.");
      out = "";
      return -1;
    }

    unsigned int nBytes = fread( buff, sizeof( char ),(MAX_BUFFER_SIZE -1),m_predirect);

    out = buff;
   if (nBytes > 0) {
   NX_LOG_LOGDEBUG( "Read: " + out);
   }
    return nBytes;
  }
}

bool ProcessWin32::CloseStdin()
{
  bool retValue = false;

  if( IsStdinRedirected() && (hClientIn_wr != INVALID_HANDLE_VALUE))
  {
    ::CloseHandle(hClientIn_wr);
    retValue = true;
  }

  return retValue;
}

bool ProcessWin32::CloseStdout()
{
  bool retValue = false;

  if( IsStdoutRedirected() || IsStderrRedirected() && (hClientOut_rd != INVALID_HANDLE_VALUE) )
  {
    ::CloseHandle(hClientOut_rd);
    retValue = true;
  }
  return retValue;
}

bool ProcessWin32::CloseStderr()
{
  bool retValue = false;
  if( IsStdoutRedirected() || IsStderrRedirected() && (hClientOut_rd != INVALID_HANDLE_VALUE) )
  {
    ::CloseHandle(hClientOut_rd);
    retValue = true;
  }
  return retValue;
}

bool ProcessWin32::CloseDevice()
{
  bool retValue = false;
  if(!m_device.empty() && m_predirect != NULL)
  {
    fclose(m_predirect);
    retValue = true;
  }
  return retValue;
}

bool ProcessWin32::WaitInitProcess() const
{
  ::WaitForInputIdle( m_process, INFINITE );
  return IsRunning();
}

} /* NX */
