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


#ifndef NX_TRANSPORT_SSH_H
#define NX_TRANSPORT_SSH_H


#include "Transport.h"

namespace NX
{

class Process;


class TransportSSH : public Transport
{
  Process *mp_process;

public:
  TransportSSH( Session* );
  ~TransportSSH();

  virtual bool Write( const string & in );
  virtual int  Read( string & out, unsigned int timeout );
  
  virtual bool InitConnection();
  virtual bool StartConnection();
  virtual bool AdvanceConnection( unsigned int timeout );
  virtual bool EndConnection();
  virtual bool BreakConnection();
  virtual bool CloseConnection();

protected:
  virtual bool InitProcess();
  virtual void DeleteProcess();

private:
  bool CheckCurrentState( unsigned int );

};


} /* NX */


#endif /* NX_TRANSPORT_SSH_H */
