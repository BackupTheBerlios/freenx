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


#ifndef WIN32

#include "XInterface.h"

//#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBfile.h>
#include <X11/extensions/XKBconfig.h>
#include <X11/extensions/XKBrules.h>
//#include <unistd.h>

#include "Logger.h"




namespace NX
{

XInterface::XInterface( Display* dpy )
{
  if( dpy == NULL )
  {
    mp_display = XOpenDisplay( NULL );
    m_closeDisplayOnExit = true;
  }
  else
  {
    mp_display = dpy;
    m_closeDisplayOnExit = false;
  }
}

XInterface::~XInterface()
{
  if( m_closeDisplayOnExit )
    XCloseDisplay( mp_display );
}

string XInterface::GetKeyboardLayout()
{
  XkbRF_VarDefsRec vd;
  char *retLayout = NULL;
  string sLayout = "";
  bool closeDisplay = false;

  Display *dpy = mp_display;
  if( !dpy )
  {
    dpy = XOpenDisplay( NULL );
    if( !dpy )
      return NULL;

    closeDisplay = true;
  }

  if( XkbRF_GetNamesProp( dpy, &retLayout, &vd ) && retLayout )
  {
    if( vd.layout )
      sLayout = string( vd.layout );

    XFree( retLayout );
  }

  if( closeDisplay )
    XCloseDisplay(dpy);

  return sLayout;
}

bool XInterface::BackingstoreEnabled()
{
  int Back = NotUseful;
  Back = XDoesBackingStore( DefaultScreenOfDisplay( mp_display ) );
  return ( Back == Always || Back == WhenMapped );
}

void XInterface::ClearAgentSignature( Window winID )
{
  Atom agentAtom;
  agentAtom =  XInternAtom( mp_display, "NX_CUT_BUFFER_SERVER", false );
  XSetSelectionOwner( mp_display, agentAtom, winID, CurrentTime);
}

bool XInterface::CheckForAgentSignature( Window winID )
{
  Atom agentAtom;
  Window ownID;
  agentAtom = XInternAtom( mp_display, "NX_CUT_BUFFER_SERVER", false );
  ownID = XGetSelectionOwner( mp_display, agentAtom);
  return (ownID != winID);
}

static unsigned char fromHexNibble(char c)
{
  int uc = (unsigned char)c;

  if(uc >= '0' && uc <= '9') return uc - (unsigned char)'0';
  if(uc >= 'a' && uc <= 'f') return uc + 10 - (unsigned char)'a';
  if(uc >= 'A' && uc <= 'F') return uc + 10 - (unsigned char)'A';

  return 16;      // error
}

int XInterface::GetArtsNetworkPort()
{
  Display *X11display;
  Window rootWin;
  Atom atom;
  Atom type;
  unsigned long bytes_after;
  long offset;
  unsigned long nitems;
  char *buf;
  char *line;
  char *decoded=(char *)malloc(2049);
  char port[6];
  int iport;
  int format;
  int i=0,ok=0,in=0;

  X11display = XOpenDisplay(NULL);
  if(!X11display)
    return 0;
  rootWin = DefaultRootWindow(X11display);
  atom = XInternAtom( X11display, "MCOPGLOBALS" , False);

  offset = 0; bytes_after = 1;
  XGetWindowProperty( X11display, rootWin, atom, offset, 2048,
                    False, XA_STRING, &type, &format, &nitems, &bytes_after,
                    (unsigned char **)&buf);

  XCloseDisplay(X11display);

  if(type == None)
    return 0;


  if((line=(char *)strstr(buf,"Arts_SoundServer"))==0)
    return 0;

  if((line=(char *)strstr(line,":"))==0)
    return 0;

  *line++;

  while(*line!='\n')
  {
    unsigned char h;
    unsigned char l;

    h = fromHexNibble(*line++);
    if(*line=='\0') break;
    if(*line=='\n') break;
    l = fromHexNibble(*line++);

    if(h >= 16 || l >= 16) continue;

    decoded[i]=((h << 4) + l);

    if(decoded[i]=='\0'&&ok==2)
      ok = 0;

    if(ok==2&&in<5)
    {
      port[in]=decoded[i];
      in++;
    }

    if(decoded[i]==':')
    {
      if(ok==1)
        ok=2;

      if(decoded[i-3]=='t'&&decoded[i-2]=='c'&&decoded[i-1]=='p')
        ok=1;
    }

    i++;
  }

  free( decoded );

  if(sscanf(port,"%d",&iport)==1)
    return iport;
  else
    return 0;

}

void XInterface::GrabLocal( Window winID )
{
  XGrabKeyboard( mp_display, winID, True, GrabModeAsync, GrabModeAsync, CurrentTime );
  XGrabPointer( mp_display, winID, True, 0, GrabModeAsync, GrabModeAsync, None, None, CurrentTime );
}

void XInterface::UnmapAgentWindow( Window winID )
{
  NX_LOG_LOGDEBUG( "XInterface: UnmapAgentWindow called." );

  Atom agentAtom;
  Atom returnedType;
  Window defaultID;
  Window rootID;
  Window tmpID;
  int iFormatReturn;
  unsigned long iItems;
  unsigned long iBytesLeft;
  unsigned char* agentType;
  unsigned int nIDs = 0;
  unsigned int i = 0;
  Window childrenID;
  Window* childrensID;
  XWindowAttributes winAttr;

#if defined( NX_KB_TEST )
  qWarning( "Before get root window" );
#endif

  defaultID = XDefaultRootWindow( mp_display );
  agentAtom = XInternAtom( mp_display, "NX_AGENT_SIGNATURE", True );
  if( agentAtom == None )
  {
    agentAtom = XInternAtom( mp_display, "NX_CUT_BUFFER_SERVER", False);
    if( agentAtom == None )
      return;

    tmpID = XGetSelectionOwner( mp_display, agentAtom );
    rootID = 0;
    while( rootID != tmpID )
    {
      defaultID = tmpID;
      XQueryTree( mp_display, tmpID, &rootID, &tmpID, &childrensID, &nIDs );
      XFree( childrensID );
    }

    XUnmapWindow( mp_display, defaultID );
    XRaiseWindow( mp_display, winID );
    GrabLocal( winID );

    return;
  }
#if defined( NX_KB_TEST )
  qWarning( "Before XQueryTree" );
#endif

  if( XQueryTree( mp_display, defaultID, &rootID, &tmpID, &childrensID, &nIDs ) == 0 )
  {
#if defined( NX_KB_TEST )
    qWarning( "Error in XQueryTree" );
#endif
    return;
  }

#if defined( NX_KB_TEST )
  qWarning( "Before FOR" );
#endif
  for( i = 0; i < nIDs; i++ )
  {
#if defined( NX_KB_TEST )
    qWarning( "%d", i );
#endif
    childrenID = childrensID[i];
#if defined( NX_KB_TEST )
    qWarning( "Children id: %x   (%d su %d)", childrenID, i, nIDs );
#endif
    if( XGetWindowProperty( mp_display, childrenID, agentAtom, 0, 32,
      False, XA_STRING, &returnedType, &iFormatReturn, &iItems, &iBytesLeft, &agentType ) == Success )
    {
      if( iItems == 0 )
        continue;

#if defined( NX_KB_TEST )
      qWarning( "Agent type: %.*s ", iItems, agentType );
#endif

      if( XGetWindowAttributes( mp_display, childrenID, &winAttr ) )
      {
#if defined( NX_KB_TEST )
        qWarning( "Get Attributes" );
#endif
        if( winAttr.override_redirect && winAttr.map_state != IsUnmapped )
        {
#if defined( NX_KB_TEST )
          qWarning( "Unmap" );
#endif
          XUnmapWindow( mp_display, childrenID );
          XRaiseWindow( mp_display, winID );
          GrabLocal( winID );
#if defined( NX_KB_TEST )
          qWarning( "after Unmap" );
#endif
        }
      }
      XFree( agentType );
    }
  }
#if defined( NX_KB_TEST )
  qWarning( "before xfree" );
#endif
  XFree( childrensID );
#if defined( NX_KB_TEST )
  qWarning( "end" );
#endif
}

bool XInterface::IsAtomOwned( char *dialogID )
{
  Atom nxDialog = XInternAtom( mp_display, dialogID, false );
  return ( XGetSelectionOwner( mp_display, nxDialog ) != None );
}

void XInterface::SetAtomOnWindow( Window winID, char *dialogID )
{
  Atom nxDialog = XInternAtom( mp_display, dialogID, false );
  XSetSelectionOwner( mp_display, nxDialog, winID, CurrentTime );
}


} /* NX */

#endif /* WIN32 */
