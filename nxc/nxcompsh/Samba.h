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


#ifndef NX_SAMBA_H
#define NX_SAMBA_H


#include <string>
using namespace std;


namespace NX
{

class Session;


class Samba
{
  Session* mp_session;
  string m_command;

public:
  Samba( Session* );
  ~Samba();

  string GetCommand() const { return m_command; }
  bool CreateCommand();
  bool Mount();

};

} /* NX */

#endif /* NX_SAMBA_H */
