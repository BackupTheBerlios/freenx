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


#ifndef NX_PROCESSWIN32_H
#define NX_PROCESSWIN32_H

#include "Process.h"

namespace NX
{


class ProcessWin32 : public Process
{
  virtual bool SpawnProcess(); //CreateProcess and redirect in file.

  //pipe handle.
  HANDLE hClientOut_rd, hClientOut_wr;
  HANDLE hClientIn_rd, hClientIn_wr;

  //to redirect in device.
  FILE * m_predirect;

public:
  ProcessWin32();
  ~ProcessWin32();

  virtual bool Write( const string & in );  //write into descriptor.
  virtual int  Read( string & out, unsigned int timeout = 0 ); //read from the descriptor.

  virtual bool WaitInitProcess() const;
  
  virtual bool EndOfStream();
  
  virtual bool CloseStdin();
  virtual bool CloseStdout();
  virtual bool CloseStderr();
  virtual bool CloseDevice();

};


} /* NX */


#endif /* NX_PROCESSWIN32_H */

