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

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#ifndef WIN32
/* Xlib/Xt stuff */
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#endif

#include "pluginbase.h"
#include "CallbackInterface.h"
#include "NXDriver.h"
#include <iostream>


class nsPluginInstance : public nsPluginInstanceBase, public CallbackInterface
{
public:
  nsPluginInstance(NPP aInstance);
  ~nsPluginInstance();

  NPBool init(NPWindow* aWindow);
  void shut();
  NPBool isInitialized();
 NPError NewStream(NPMIMEType type, NPStream* stream,
                            NPBool seekable, uint16* stype) {*stype = NP_ASFILEONLY; return NPERR_NO_ERROR;}
void    StreamAsFile(NPStream* stream, const char* fname);

  // locals
  const char * getVersion();
  NPError SetWindow(NPWindow* aWindow);
#ifndef WIN32
  void draw();
#endif
  void Callback(unsigned int msg, void *ptr);
  void setMessage(const char *message) {strcpy(mMessage, message); /*draw();*/}
  const char *GetMessage() { return mMessage;}
  void setPostbackTarget(const char *target) {strcpy(mPostbackTarget, target);}
  void setSessionPostbackUrl(char *sessionurl) {strcpy(mSessionPostbackUrl, sessionurl);}
  void setRestoreSessionId(char *sessionid) {strcpy(mRestoreSessionId, sessionid);}
  void setSessionName(char *sessionname) {strcpy(mSessionName, sessionname);}
  const char *getConfigFilePath() { return mConfigFilePath;}
#if defined (WIN32)
#if defined (WINLESS)
  uint16 HandleEvent(void* aEvent);
#endif
#endif
private:

  char mMessage[400];
  char mPostbackTarget[80];
  char mSessionPostbackUrl[256];
  char mRestoreSessionId[40];
  const char *mConfigFilePath;
  char mSessionName[256];
  NXDriver *m_driver;
	
  NPP mInstance;
  NPBool mInitialized;
#if defined (WIN32)
#ifndef WINLESS
  HWND mhWnd;
#endif
  NPWindow * mWindow;
#else
  Window mWindow;
  Display *mDisplay;
  Widget mXtwidget;
  int mX, mY;
  int mWidth, mHeight;
  Visual* mVisual;
  Colormap mColormap;
  unsigned int mDepth;
  XFontStruct *mFontInfo;
  GC mGC;
#endif
};

#endif // __PLUGIN_H__
