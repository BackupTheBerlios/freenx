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


#ifndef NX_SETTINGSTOPARAMETERS_H
#define NX_SETTINGSTOPARAMETERS_H



#include "NXSettings.h"



class SettingsToParameters
{
  NXSettings* sets;
  void *pSession;

public:
  SettingsToParameters( NXSettings*, void* );
  ~SettingsToParameters();
  void Convert();

protected:
  void SetAuth();
  void SetResolution();
  void SetSessionType();
  void SetSessionTypeUnix( string&, string&, string&, string&, string&, bool& );
  void SetSessionTypeWindows( string&, string&, string&, string&, string&, bool& );
  void SetSessionTypeVnc( string&, string&, string&, string&, string&, bool& );

};


#endif /* NX_SETTINGSTOPARAMETERS_H */
