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


#ifndef NX_XSERVER_H
#define NX_XSERVER_H


namespace NX
{

class Session;
class Process;

class XServer
{
  Process *mp_process;
  Session *mp_session;
  int m_state;

public:
  XServer( Session* );
  virtual ~XServer();

  enum State { NoInit = 0, Init, Running, NotRunning };

  const Session* GetSession() const { return mp_session; }
  const Process* GetProcess() const { return mp_process; }

  int GetState() const { return m_state; }

  static bool SetStandardWindowName( Session* );

  bool InitConnection();
  bool AdvanceConnection();
  bool IsRunning();
  bool BreakConnection();

protected:
  void DeleteProcess();
  bool InitProcess();
  bool StartConnection();
  bool CheckCurrentState();

};

} /* NX */


#endif /* NX_XSERVER_H */
