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

#ifndef NX_PROCESS_H
#define NX_PROCESS_H

#include "Exception.h"

#include <string>
#include <list>
using namespace std;

#ifdef WIN32
#include <windows.h>
#else
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

typedef pid_t HANDLE;

#endif

/*
  Base interface for unix and windows processes
*/

namespace NX
{

const unsigned int MAX_BUFFER_SIZE = 2001;


class Process
{
  virtual bool SpawnProcess() = 0; //fork() or CreateProcess and redirect in file.

public:
  static void SetEnvironment( string key, string value );
  static void OverwriteEnvironment( string key, string value );
  static string GetEnvironment( string key );

  enum NXSTDFLAGS { NX_NOREDIRECT = 0x0000 ,NX_STDIN = 0x0001 , NX_STDOUT = 0x0002 , NX_STDERR = 0x0004 };
  enum NXSTATEPROCESS { NX_PROCESSACTIVE , NX_PROCESSEXITED};

  Process();
  virtual ~Process();

  const Exception& GetException() const { return m_exception; }

  virtual bool Write( const string & in ) = 0;  //write into descriptor.
  virtual int Read( string & out, unsigned int timeout ) = 0; //read from the descriptor.
  virtual bool EndOfStream() = 0;

  virtual bool StartProcess(); //call SpawnProcess in derivated class.
  virtual bool StopProcess();

  virtual bool CloseStdin() =  0;
  virtual bool CloseStdout() =  0;
  virtual bool CloseStderr() =  0;
  virtual bool CloseDevice() =  0;


  virtual bool IsRunning() const;
  virtual void WaitProcessTerminate()  const;
  virtual bool WaitInitProcess() const { return IsRunning(); }
  virtual int  GetExitCode() const;

  void SetDeviceName( const string& device_name ) { m_device = device_name; }
  const string & GetDeviceName() const { return m_device; }

  void SetProcessPath( const string& process_path ) { m_processPath = process_path; }
  void SetProcessName( const string& process_name ) { m_processName = process_name; }
  const string& GetProcessName() const { return m_processName; }
  const string& GetProcessPath() const { return m_processPath; }

  void SetRedirectFlags( const unsigned int redirect ) { m_redirect |= redirect; }

  void AddArgument( string arg );

  bool IsStdinRedirected()  const;
  bool IsStdoutRedirected() const;
  bool IsStderrRedirected() const;

protected:

  HANDLE m_process;   // returned from fork() or CreateProcess().
  string m_device;    // name of redirection.
  list<string> m_arguments;

  string m_processPath;
  string m_processName;

  bool m_endofstream;

  unsigned int m_redirect;
  Exception m_exception;
};


} /* NX */


#endif /* NX_PROCESS_H */

