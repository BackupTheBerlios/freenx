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


#include "Samba.h"
#include "Session.h"
#include "Protocol.h"
#include "Logger.h"

namespace NX
{
const int SambaPortOffset = 7000;


Samba::Samba( Session* pSession )
{
  mp_session = pSession;
  m_command = "";
}

Samba::~Samba()
{

}

bool Samba::CreateCommand()
{
  NX_LOG_LOGDEBUG( "Samba: creating command..." );

  return true;
}

bool Samba::Mount()
{
  return true;
}


} /* NX */

