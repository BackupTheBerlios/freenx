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

#ifndef NXCONNECTION_H
#define NXCONNECTION_H

#include <SessionInterface.h>

#include <string>
using namespace std;


class NXSettings;



class NXConnection
{
public:
  NXConnection();
  ~NXConnection();

  void Start();
  bool Advance( unsigned int timeout );
  void Stop();

  bool Accepted() const;
  bool SessionAccepted() const;

  int GetStateId() const;
  string GetState() const;
  bool ErrorOccurred() const;
  string GetError() const;

  bool SetParameters( NXSettings* );

  string GetLogPath() const;
  string GetSessionId() const;
  void SetSessionId(string sessionid);

protected:
  void HandleCleanup();

private:
  NXSessionPtr pSession;
  NXConnectionPtr pConnection;
  NXProxyPtr pProxy;
  NXXServerPtr pXServer;

};
#endif // NX_CONNECTION_H



