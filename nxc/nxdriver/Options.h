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


#ifndef NX_OPTIONS_H
#define NX_OPTIONS_H

#include <string>
using namespace std;


class Options
{
public:
  Options();
  ~Options();

  void CleanOldFiles();

  void RestoreDefault();

  void SetValue( const string& key, const string& value );
  string GetValue( const string& key, const string& def ) const;

  string GetVersion() const;

  string GetNXSystemDirectory() const;
  string GetNXPersonalDirectory() const;
  string GetNXBinDirectory() const;
  string GetNXShareDirectory() const;
  string GetNXImagesDirectory() const;
  string GetNXIconsDirectory() const;
  string GetNXLibDirectory() const;
  string GetNXConfigDirectory() const;
  string GetNXConfigFileSuffix() const;
  string GetNXTempDirectory() const;
  string GetNXPidTempDirectory() const;
  string GetNXSshPath() const;
  string GetNXProxyPath() const;
  string GetNXSshKeyPath() const;
  string GetNXSshLogPath() const;
  string GetNXArtsLogPath() const;
  string GetNXConfigFilePath( const string& ) const;
  string GetNXCookieFilePath() const;

//  bool CreateDirectory( const string& );

};

extern Options *options;

#endif /* NX_OPTIONS_H */
