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


#ifndef NX_SETTINGS_H
#define NX_SETTINGS_H

#include <string>
#include <map>
using namespace std;

#include <expat.h>


class NXSettings
{
public:
  NXSettings();
  virtual ~NXSettings();

  const string& GetFilePath() const { return m_filePath; }
  virtual void SetFilePath( const string& );
  const string& GetConfigName() const;
  virtual void SetConfigName(const string& configName) { m_configName = configName;}

  virtual bool Read();
  bool GetFileState() const { return m_fileState; }
  bool IsRead() const { return m_isRead; }

  void Clear();

  string Dump() const;

  void SetGroup( const string& group ) { m_currentGroup = group; }
  string GetGroup() const { return m_currentGroup; }
  void SetString( const string& key, const string& value );
  string GetString( const string& key, const string& def = "" ) const;
  long GetNumber( const string& key, long def = 0 ) const;
  bool GetBool( const string& key, bool def = false ) const;

protected:
  static void OnStartElement( void *data, const char *el, const char **attr );
  static void OnEndElement( void *data, const char *el );

  virtual bool InitExpat();
  virtual void CleanupExpat();

private:
  string m_filePath;
  string m_configName;
  XML_Parser mp_parserXML;
  bool m_fileState;
  bool m_isRead;
  string m_currentGroup;

  typedef map< string, string > PrefMap;
  map< string, PrefMap > m_groups;

};

#endif /* NX_SETTINGS_H */
