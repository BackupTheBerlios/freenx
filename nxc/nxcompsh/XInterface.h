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


#ifndef NX_XINTERFACE_H
#define NX_XINTERFACE_H

#ifndef WIN32

#include <X11/Xlib.h>

#include <string>
using namespace std;


namespace NX
{

class XInterface
{
  Display *mp_display;
  bool m_closeDisplayOnExit;

public:
  XInterface( Display* dpy );
  ~XInterface();

  string GetKeyboardLayout();
  bool BackingstoreEnabled();
  int GetArtsNetworkPort();
  void ClearAgentSignature( Window );
  bool CheckForAgentSignature( Window );

  void UnmapAgentWindow( Window );

  bool IsAtomOwned( char * );
  void SetAtomOnWindow( Window, char * );

protected:
  void GrabLocal( Window );


}; /* XInterface */

} /* NX */

#endif /* WIN32 */

#endif /* NX_XINTERFACE_H */





