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

#include "ProcessUnix.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include "Logger.h"

#include <sstream>

using namespace std;


#include <errno.h>



namespace NX
{


ProcessUnix::ProcessUnix() : Process()
{
  mp_stdin = NULL;
  mp_stdouterr = NULL;
}

ProcessUnix::~ProcessUnix()
{}

bool ProcessUnix::SpawnProcess()
{
  NX_LOG_LOGDEBUG( "NXProcess: process being spawned " + m_processPath);
/* Check if bin exists */
  FILE* pFileTmp = fopen( m_processPath.c_str(), "r" );
  if( pFileTmp == NULL )
  {
    m_exception.SetRuntimeError( m_processPath + ": file not found." );
    NX_LOG_LOGERROR( "NXProcess: " + m_processPath + " not found." );
    return false;
  }
  else
    fclose( pFileTmp );

/* Check for redirection */
  int p[2];
  int myside, hisside;

  bool rd_stdin = IsStdinRedirected();
  bool rd_stdout = IsStdoutRedirected();
  bool rd_stderr = IsStderrRedirected();

  if( rd_stdout || rd_stderr )
  {
    if( m_device.empty() )
    {
      m_exception.SetRuntimeError( "Invalid redirection's device." );
      NX_LOG_LOGERROR( "NXProcess: device is empty." );
      return false;
    }

    mp_stdouterr = fopen( m_device.c_str(), "w+" );
    if( mp_stdouterr == NULL )
    {
      m_exception.SetRuntimeError( m_device + ": cannot open this file." );
      NX_LOG_LOGERROR( "NXProcess: cannot open device to redirect stdout and stderr." );
      return false;
    }
  }

  if( rd_stdin )
  {
    if( pipe( p ) < 0 )
    {
      m_exception.SetRuntimeError( "Cannot open redirection's pipe." );
      stringstream ss;
      ss <<  "NXProcess: cannot open pipe, errno = " << errno << " (" << strerror(errno);
      NX_LOG_LOGERROR(ss.str());
      return false;
    }

  /*
    p[1] : write
    p[0] : read
  */

    myside = p[1];
    hisside = p[0];
  }

  m_process = fork();

  if( m_process == 0 )  // child
  {
    if( rd_stdin )
    {
      close( myside );
      dup2( hisside, fileno( stdin ) );
      close( hisside );
    }

    if( rd_stdout )
      dup2( fileno( mp_stdouterr ), fileno( stdout ) );

    if( rd_stderr )
      dup2( fileno( mp_stdouterr ), fileno( stderr ) );

    if( m_arguments.size() == 0 )
      execl( m_processPath.c_str(), m_processName.c_str(), NULL );
    else
    {
      int iSize = m_arguments.size() + 2;
      char *cmdArgv[ iSize ];
      cmdArgv[ 0 ] = (char*)m_processName.c_str();
NX_LOG_LOGDEBUG( cmdArgv[0]);
      int i = 1;

      for( list<string>::const_iterator it = m_arguments.begin(); it != m_arguments.end(); ++it )
      {
        cmdArgv[i] = (char*) ((*it).c_str());
NX_LOG_LOGDEBUG(cmdArgv[i]);
        i++;
      }
NX_LOG_LOGDEBUG("- end process args");
      cmdArgv[ (iSize-1) ] = NULL;
      execv( m_processPath.c_str(), cmdArgv );
    }

    if( rd_stdout || rd_stderr )
      close( fileno( mp_stdouterr ) );
    exit( 0 );
  }
  else if( m_process == -1 )  // error occurred
  {
    if( rd_stdin )
    {
      close( myside );
      close( hisside );
    }

    m_exception.SetRuntimeError( "Fork failed: cannot launch new process." );

    stringstream ss;
    ss << "NXProcess: cannot fork, errno = " << errno
    << " (" << strerror( errno ) << ")." << endl << flush;
    NX_LOG_LOGERROR(ss.str());
    return false;
  }

  if( rd_stdin )
  {
    close( hisside );

    mp_stdin = fdopen( myside, "w" );

    if( mp_stdin == NULL )
    {
      m_exception.SetRuntimeError( "Unable to open stdin." );
      NX_LOG_LOGDEBUG( "NXProcess: cannot redirect stdin." );
      return false;
    }
  }

  if( rd_stdout || rd_stderr )
  {
    fclose( mp_stdouterr );
    mp_stdouterr = fopen( m_device.c_str(), "r" );
    if( mp_stdouterr == NULL )
    {
      m_exception.SetRuntimeError( "Unable to open stdout or stderr." );
      NX_LOG_LOGERROR( "NXProcess: cannot redirect stdout and stderr (2).");
      return false;
    }
  }

  return true;
}

bool ProcessUnix::Write( const string & in )
{
  if( mp_stdin == NULL )
  {
    m_exception.SetRuntimeError( "Unable to write. Stdin is not redirected." );
    NX_LOG_LOGERROR( "NXProcess: unable to write, stdin is not redirected." );
    return false;
  }

  NX_LOG_LOGDEBUG("WRITE: " + in );

  int iRet = fprintf( mp_stdin, "%s", in.c_str() );

  if( iRet > 0 )
    fflush( mp_stdin );

  return (iRet >= 0);
}

bool ProcessUnix::EndOfStream()
{
  return ( feof( mp_stdouterr ) != 0 );
}

int ProcessUnix::Read( string & out, unsigned int timeout )
{
  if( mp_stdouterr == NULL )
  {
    m_exception.SetRuntimeError( "Unable to read. Stdout and stderr are not redirected." );
    NX_LOG_LOGERROR( "NXProcess: unable to read, stdout and stderr are not redirected." );
    out = "";
    return -1;
  }

  fd_set rfds;
  struct timeval tv;
  int retval;
  int myfd = fileno( mp_stdouterr );

  /* Watch mp_stdouterr to see when it has input. */
  FD_ZERO(&rfds);
  FD_SET( myfd, &rfds);

  /* Wait up to timeout. */
  tv.tv_sec = 0;
  tv.tv_usec = 1000 * timeout;

  retval = select( myfd + 1, &rfds, NULL, NULL, &tv );

  if( retval == 0 )
  {
    out = "";
    return 0;
  }
  else if( retval < 0 )
  {
    NX_LOG_LOGERROR( "NXProcess: error occurred while reading." );
    out = "";
    return -1;
  }
  else
  {
    char buff[MAX_BUFFER_SIZE] = {0};

    unsigned int nBytes = fread( buff, sizeof( char ), (MAX_BUFFER_SIZE -1), mp_stdouterr );
    //fflush( mp_stdouterr );
    if( nBytes <= 0 )
      out = "";
    else {
      out = buff;
#if defined (NX_ENABLE_LOGGING)    
    stringstream ss;
    ss << "\n---\nREAD: " << buff << "\n---";
    NX_LOG_LOGDEBUG(ss.str());
#endif
}
    return nBytes;
  }
}

bool ProcessUnix::CloseStdin()
{
  if( mp_stdin == NULL )
    return false;
  else
    return ( fclose( mp_stdin ) == 0 );
}

bool ProcessUnix::CloseStdout()
{
  return ( fclose( mp_stdouterr ) == 0 );
}

bool ProcessUnix::CloseStderr()
{
  return ( fclose( mp_stdouterr ) == 0 );
}

bool ProcessUnix::CloseDevice()
{
  return ( fclose( mp_stdouterr ) == 0 );
}



}  /* NX */
