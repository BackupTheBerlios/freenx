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


#ifndef NX_TRANSPORT_H
#define NX_TRANSPORT_H


#include <string>
using namespace std;


namespace NX
{

class Session;
class Process;


class Transport
{
public:
  Transport( Session* );
  virtual ~Transport();

  string GetType() const { return m_type; }
  string GetBuffer() const { return m_buffer; }

  const Session* GetSession() const { return mp_session; }

  virtual bool Write( const string & in ) = 0;
  virtual int  Read( string & out, unsigned int timeout ) = 0;

  virtual bool InitConnection() = 0;
  virtual bool StartConnection() = 0;
  virtual bool AdvanceConnection( unsigned int ) = 0;
  virtual bool EndConnection() = 0;
  virtual bool BreakConnection() = 0;
  virtual bool CloseConnection() = 0;

protected:
  string m_type;
  string m_buffer;
  Session *mp_session;

};


} /* NX */


#endif /* NX_TRANSPORT_H */

