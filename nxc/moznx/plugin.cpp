/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: NPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is 
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or 
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the NPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the NPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
#include <pthread.h>
#include <sstream>

#if defined WIN32
  #include <windows.h>
  #include <windowsx.h>
#endif

#include "plugin.h"

#include "CallbackMessageTypes.h"

#include "Options.h"
#include "Process.h"
#include "Utilities.h"
#include "Logger.h"

#define MIME_TYPES_HANDLED  "application/x-nx"

#define PLUGIN_NAME         "NX Client Plug-in for Mozilla"
#define MIME_TYPES_DESCRIPTION  MIME_TYPES_HANDLED":conf:"PLUGIN_NAME
#define PLUGIN_DESCRIPTION  PLUGIN_NAME " (allows users to connect to an NXServer through a browser.  go to http://www.nomachine.com/ for more information about NX technology.)"

#ifndef WIN32
#define _stricmp(a,b) strcasecmp(a,b)
#endif

void *thread_doit(void *ptr);

//////////////////////////////////////
//

// general initialization and shutdown
//

#ifndef WIN32
void ConfirmBinDirectory()
{
  string sshpath = options->GetNXSshPath();
  if (!NX::Utilities::FileExist(sshpath)) {
     NX_LOG_LOGDEBUG("changing bin oath to " + options->GetNXPersonalDirectory());
     NX::Process::OverwriteEnvironment("NXDIR", options->GetNXPersonalDirectory());
  }
}
#endif

NPError NS_PluginInitialize()
{
#ifndef WIN32
  ConfirmBinDirectory();
#endif
  return NPERR_NO_ERROR;
}

void NS_PluginShutdown()
{
}

#ifndef WIN32
char* NPP_GetMIMEDescription(void)
{
    return(MIME_TYPES_DESCRIPTION);
}

// get values per plugin
NPError NS_PluginGetValue(NPPVariable aVariable, void *aValue)
{
  NPError err = NPERR_NO_ERROR;
  switch (aVariable) {
    case NPPVpluginNameString:
      *((char **)aValue) = PLUGIN_NAME;
      break;
    case NPPVpluginDescriptionString:
      *((char **)aValue) = PLUGIN_DESCRIPTION;
      break;
    default:
      err = NPERR_INVALID_PARAM;
      break;
  }
  return err;
}

#endif

/////////////////////////////////////////////////////////////
//
// construction and destruction of our plugin instance object
//
nsPluginInstanceBase * NS_NewPluginInstance(nsPluginCreateData * aCreateDataStruct)
{
  if(!aCreateDataStruct)
    return NULL;


  nsPluginInstance * plugin = new nsPluginInstance(aCreateDataStruct->instance);
   for (int i=0; i < aCreateDataStruct->argc; i++) 
   {
      if (_stricmp(aCreateDataStruct->argn[i], "postback_target") == 0)
      {
         plugin->setPostbackTarget(aCreateDataStruct->argv[i]);
      }
      else if (_stricmp(aCreateDataStruct->argn[i], "postback_session") == 0)
      {
         plugin->setSessionPostbackUrl(aCreateDataStruct->argv[i]);
      }
      else if (_stricmp(aCreateDataStruct->argn[i], "restore_session_id") == 0)
      {
         plugin->setRestoreSessionId(aCreateDataStruct->argv[i]);
      }
      else if (_stricmp(aCreateDataStruct->argn[i], "session_name") == 0)
      {
         plugin->setSessionName(aCreateDataStruct->argv[i]);
      }
      else if (_stricmp(aCreateDataStruct->argn[i], "log_flags") == 0)
      {
         string s(aCreateDataStruct->argv[i]);
         NX_LOG_SETFLAGS(s);
      }
   }

   // winless
#if defined (WINLESS)
   NPN_SetValue(aCreateDataStruct->instance, NPPVpluginWindowBool, NULL);
   NPN_SetValue(aCreateDataStruct->instance, NPPVpluginTransparentBool, NULL);
#endif
  return plugin;
}

void NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin)
{
  if(aPlugin)
    delete (nsPluginInstance *)aPlugin;
}

////////////////////////////////////////
//
// nsPluginInstance class implementation
//
nsPluginInstance::nsPluginInstance(NPP aInstance) : nsPluginInstanceBase(),
  mInstance(aInstance),
  mInitialized(FALSE)
#ifndef WIN32
  ,mWindow(0),
  mXtwidget(0),
  mFontInfo(0)
#endif
{
#if defined WIN32
#ifndef WINLESS
	mhWnd = NULL;
#endif
#endif
   mRestoreSessionId[0] = 0; 
   mMessage[0] = 0;
   mSessionName[0] = 0; 
   mPostbackTarget[0] = 0;
   mSessionPostbackUrl[0] = 0;


}

nsPluginInstance::~nsPluginInstance()
{
   if (m_driver) delete m_driver;
}
#if defined WIN32
#ifndef WINLESS
static LRESULT CALLBACK PluginWinProc(HWND, UINT, WPARAM, LPARAM);
static WNDPROC lpOldProc = NULL;
#endif
#endif

#ifndef WIN32
static void
xt_event_handler(Widget xtwidget, nsPluginInstance *plugin, XEvent *xevent, Boolean *b)
{
  switch (xevent->type) {
    case Expose:
      // get rid of all other exposure events
      if (plugin) {
        //while(XCheckTypedWindowEvent(plugin->Display(), plugin->Window(), Expose, xevent));
        plugin->draw();
      }
      default:
        break;
  }
}

void nsPluginInstance::draw()
{
  unsigned int h = mHeight/2;
  unsigned int w = 3 * mWidth/4;
  int x = (mWidth - w)/2; // center
  int y = h/2;
  if (x >= 0 && y >= 0) {
    GC gc = XCreateGC(mDisplay, mWindow, 0, NULL);
    if (!gc)
      return;
    int xforeground;
    int xscreen;
    xscreen = DefaultScreen(mDisplay);
    xforeground = WhitePixel(mDisplay, xscreen);
    XSetForeground(mDisplay, gc, xforeground);
    XFillRectangle(mDisplay, mWindow, gc, 0, 0, mWidth, mHeight);
//  const char *string = getVersion();
  if (mMessage && *mMessage) {
//    int l = strlen(string);
    int fmba = mFontInfo->max_bounds.ascent;
    int fmbd = mFontInfo->max_bounds.descent;
    int fh = fmba + fmbd;
    y += fh;
    x += 32;
    xforeground = BlackPixel(mDisplay, xscreen);
    XSetForeground(mDisplay, gc, xforeground);
      XDrawString(mDisplay, mWindow, gc, x, y, mMessage, strlen(mMessage));
//      XDrawString(mDisplay, mWindow, gc, x, y, getVersion(), strlen(getVersion()));
    }
    XFreeGC(mDisplay, gc);
  }
}
#endif

NPBool nsPluginInstance::init(NPWindow* aWindow)
{
#ifndef WINLESS
  if(aWindow == NULL)
    return FALSE;
#if defined WIN32
  mhWnd = (HWND)aWindow->window;
  if(mhWnd == NULL)
    return FALSE;

  // subclass window so we can intercept window messages and
  // do our drawing to it
  lpOldProc = SubclassWindow(mhWnd, (WNDPROC)PluginWinProc);

  // associate window with our nsPluginInstance object so we can access 
  // it in the window procedure
  SetWindowLong(mhWnd, GWL_USERDATA, (LONG)this);
#else
  if (SetWindow(aWindow))
  {
     mInitialized = TRUE;
  }
#endif
#endif
  mInitialized = TRUE;
  return TRUE;
}

NPError nsPluginInstance::SetWindow(NPWindow* aWindow)
{
#if defined (WIN32)
  // keep window parameters
  mWindow = aWindow;
  return NPERR_NO_ERROR;
#else
  if(aWindow == NULL)
    return FALSE;

  mX = aWindow->x;
  mY = aWindow->y;
  mWidth = aWindow->width;
  mHeight = aWindow->height;
  if (mWindow == (Window) aWindow->window) {
    // The page with the plugin is being resized.
    // Save any UI information because the next time
    // around expect a SetWindow with a new window id.
  } else {
    mWindow = (Window) aWindow->window;
    NPSetWindowCallbackStruct *ws_info = (NPSetWindowCallbackStruct *)aWindow->ws_info;
    mDisplay = ws_info->display;
    mVisual = ws_info->visual;
    mDepth = ws_info->depth;
    mColormap = ws_info->colormap;

    if (!mFontInfo) {
      if (!(mFontInfo = XLoadQueryFont(mDisplay, "9x15")))
        printf("Cannot open 9X15 font\n");
    }
    // add xt event handler
    Widget xtwidget = XtWindowToWidget(mDisplay, mWindow);
    if (xtwidget && mXtwidget != xtwidget) {
      mXtwidget = xtwidget;
      long event_mask = ExposureMask;
      XSelectInput(mDisplay, mWindow, event_mask);
      XtAddEventHandler(xtwidget, event_mask, False, (XtEventHandler)xt_event_handler, this);
    }
  }
  return TRUE;
#endif   
}

static unsigned char toHex( const unsigned char &x )
{
  return x > 9 ? x + 55: x + 48;
}
string encodeUrl( const string& sIn )
{
  string sOut = "";
  char c;
  for( string::const_iterator it = sIn.begin(); it != sIn.end(); it++ )
  {
    c = *it;
    if(!isalnum(c) )
    {
      if( isspace(c) )
        sOut += '+';
      else
      {
        sOut += '%';
        sOut += toHex(c>>4);
        sOut += toHex(c%16);
      }
    }
    else
      sOut += c;
  }

  return sOut;
}

string encodeUrl( const char* pString )
{
  string sIn( pString );
  return encodeUrl( sIn );
}

void nsPluginInstance::Callback(unsigned int msg, void *ptr)
{
    string strUrl(mSessionPostbackUrl);
    stringstream ss;
   switch (msg)
   {
      case CB_MSG_STATUS:
         strcpy(mMessage, (const char *)ptr);
#if defined (WIN32)
#if defined (WINLESS)
        NPRect rc;
        rc.bottom = mWindow->y + mWindow->height;
        rc.left   = mWindow->x;
        rc.right  = mWindow->x + mWindow->width;
        rc.top    = mWindow->y;
		 NPN_InvalidateRect(mInstance, &rc);
		 NPN_ForceRedraw(mInstance);
#else
		 InvalidateRect(mhWnd, NULL, true);
#endif
#else
		 draw();
#endif     
      break;
      case CB_MSG_SESSION:
         ss <<  "Got the session id: " << (const char *)ptr ;
         NX_LOG_LOGDEBUG( ss.str() );
         // post the session id back to the server
		  if ((strlen(mSessionPostbackUrl) > 0) && ((strlen(mPostbackTarget) > 0)))
		  {
             ss <<  "post this url: " << mSessionPostbackUrl << "sessionid=" << (const char *)ptr << " to target " << mPostbackTarget ;
             NX_LOG_LOGDEBUG(ss.str());
             strUrl = strUrl + "op=postsessionid&sessionid=" + (const char *)ptr; 
             NPN_GetURL(mInstance, strUrl.c_str(), mPostbackTarget); 
		  }
		 break;
      case CB_MSG_COMPLETE:
		  if ((strlen(mSessionPostbackUrl) > 0) && ((strlen(mPostbackTarget) > 0)))
		  {
            strUrl = strUrl + "op=postconnected"; 
            NPN_GetURL(mInstance, strUrl.c_str(), mPostbackTarget); 
		  }
      break;
      case CB_MSG_ERROR:
		  if ((strlen(mSessionPostbackUrl) > 0) && ((strlen(mPostbackTarget) > 0)))
		  {
            strUrl = strUrl + "op=posterror&errordesc=" + encodeUrl((const char *)ptr); 
            NPN_GetURL(mInstance, strUrl.c_str(), mPostbackTarget); 
		  }
      break;
   } 
}

 void    nsPluginInstance::StreamAsFile(NPStream* stream, const char* fname)
{
   setMessage(fname);
   mConfigFilePath = fname;
   m_driver = new NXDriver();
   m_driver->SetCallback(this);
   m_driver->SetConfigFilePath(fname);
   m_driver->SetRestoreSessionId(mRestoreSessionId);
   m_driver->SetSessionName(mSessionName);
   pthread_t thethread;
   pthread_create(&thethread, NULL, thread_doit, (void *)m_driver);
}


void nsPluginInstance::shut()
{
#if defined (WIN32)
#ifndef WINLESS
  // subclass it back
  SubclassWindow(mhWnd, lpOldProc);
  mhWnd = NULL;
#endif
#endif
  mInitialized = FALSE;
  if (m_driver) m_driver->HandleCloseAll(0);
}

NPBool nsPluginInstance::isInitialized()
{
  return mInitialized;
}

const char * nsPluginInstance::getVersion()
{
  return NPN_UserAgent(mInstance);
}
#if defined (WIN32)
#ifndef WINLESS
static LRESULT CALLBACK PluginWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) {
    case WM_PAINT:
      {
        // draw a frame and display the string
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc;
        GetClientRect(hWnd, &rc);
//        FrameRect(hdc, &rc, GetStockBrush(BLACK_BRUSH));

        // get our plugin instance object and ask it for the version string
        nsPluginInstance *plugin = (nsPluginInstance *)GetWindowLong(hWnd, GWL_USERDATA);
        if (plugin) {
//          const char * string = plugin->getVersion();
//          const char * string = "test";
			const char *message = plugin->GetMessage();
			if (message && *message) {
				DrawText(hdc, message, strlen(message), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
        }
        else {
          char string[] = "Error occured";
          DrawText(hdc, string, strlen(string), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        }

        EndPaint(hWnd, &ps);
      }
      break;
    default:
      break;
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

#else
uint16 nsPluginInstance::HandleEvent(void* aEvent)
{
  NPEvent * event = (NPEvent *)aEvent;
  HDC hdc = (HDC)event->wParam;
  switch (event->event) {
    case WM_PAINT: 
    {
			const char *message = GetMessage();
      if(!mWindow)
        break;

			if (message && *message) {
      // get the dirty rectangle to update or repaint the whole window
      RECT * drc = (RECT *)event->lParam;
        RECT rc;
        rc.bottom = mWindow->y + mWindow->height;
        rc.left   = mWindow->x;
        rc.right  = mWindow->x + mWindow->width;
        rc.top    = mWindow->y;
//        nsPluginInstance *plugin = (nsPluginInstance *)GetWindowLong(hWnd, GWL_USERDATA);
//        if (plugin) {
//          const char * string = plugin->getVersion();
//          const char * string = "test";
				DrawText(hdc, message, strlen(message), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
//        }
  //    }
			break;
    }
    case WM_KEYDOWN:
    {
      Beep(1000,100);
      break;
    }
    default:
      return 0;
  }
  return 1;
}
#endif
#endif

void *thread_doit(void *ptr)
{
   ((NXDriver *)ptr)->Run();
   return NULL;
}
