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


#ifndef NX_PROCESSUNIX_H
#define NX_PROCESSUNIX_H

#include "Process.h"

namespace NX
{


class ProcessUnix : public Process
{
  virtual bool SpawnProcess(); //fork() and redirect in file.

  FILE* mp_stdin;
  FILE* mp_stdouterr;

public:
  ProcessUnix();
  ~ProcessUnix();

  virtual bool Write( const string& in );  //write into descriptor.
  virtual int  Read( string& out, unsigned int timeout ); //read from the descriptor.

  virtual bool EndOfStream();

  virtual bool CloseStdin();
  virtual bool CloseStdout();
  virtual bool CloseStderr();
  virtual bool CloseDevice();
};


} /* NX */


#endif  /* NX_PROCESSUNIX_H */


