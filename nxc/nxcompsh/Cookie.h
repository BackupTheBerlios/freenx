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


#ifndef NX_COOKIE_H
#define NX_COOKIE_H

#include <string>
using namespace std;

namespace NX
{
  
class Session;

class Cookie
{
  Session* mp_session;
  string m_cookie;

public:
  Cookie( Session* );
  ~Cookie();

  bool ReadExistingOne();
  bool GenerateRandomOne();
  bool ReadFromFile( string file_name );
  bool Generate();
  string Get() const { return m_cookie; }

protected:
  bool GetDisplay( string& display );
  bool ExecuteCommand( string );
  string getHostnameTheHardWay();
};

}  /* NX */

#endif /* NX_COOKIE_H */

