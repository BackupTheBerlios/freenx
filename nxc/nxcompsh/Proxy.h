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


#ifndef NX_PROXY_H
#define NX_PROXY_H

#include <string>
using namespace std;


namespace NX
{

class Session;
class Process;


class Proxy
{
  Process* mp_process;
  Session* mp_session;
  string m_buffer;

public:
  Proxy( Session* );
  ~Proxy();

  const Session* GetSession() const { return mp_session; }
  const Process* GetProcess() const { return mp_process; }
  const string& GetBuffer() const { return m_buffer; }

  void ClearBuffer();

  bool InitConnection();
  bool AdvanceConnection( unsigned int timeout = 0 );
  bool BreakConnection();

protected:
  void DeleteProcess();
  bool InitProcess();
  bool CreateSessionDirectory();
  bool CreateOptions();
  bool CreateOptionsFile();
  bool StartConnection();
  bool CheckCurrentState( unsigned int );

};


} /* NX */


#endif /* NX_PROXY_H */
