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


#include "NXSettings.h"

#include "StringUtilities.h"
#include "Logger.h"

#include <stdlib.h>

using namespace std;




NXSettings::NXSettings()
{
  m_filePath = "";
  m_configName = "";
  mp_parserXML = NULL;
  m_fileState = false;
  m_isRead = false;
  m_currentGroup = "General";
}

NXSettings::~NXSettings()
{}

void NXSettings::SetFilePath( const string& file_path )
{
  if( m_filePath != file_path )
  {
    m_filePath = file_path;
    m_isRead = false;
  }
}

const string& NXSettings::GetConfigName() const
{
   if (m_configName.length() == 0)
   {
NX_LOG_LOGDEBUG("Defaulting session name to filePath: " + m_filePath);
      return m_filePath; 
   } 
NX_LOG_LOGDEBUG("Returning session name: " + m_configName);
   return  m_configName;
}

void NXSettings::Clear()
{
  m_groups.clear();
}

void NXSettings::SetString( const string& key, const string& value )
{
  m_groups[ m_currentGroup ][ key ] = value;
}

string NXSettings::GetString( const string& key, const string& def ) const
{
  string sValue = def;
  map< string, PrefMap>::const_iterator itg = m_groups.find( m_currentGroup );
  if( itg != m_groups.end() )
  {
    PrefMap::const_iterator ite = ((*itg).second).find( key );
    if( ite != ((*itg).second).end() )
      sValue = (*ite).second;
  }
  return sValue;
}

long NXSettings::GetNumber( const string& key, long def ) const
{
  long iRet = def;
  string sValue = GetString( key, "NULL" );
  if( sValue != "NULL" )
  {
    try
    {
      iRet = StringUtilities::StringToNumber( sValue );
    }
    catch( XInvalidNumber& )
    {
      iRet = def;
    }
  }

  return iRet;
}

bool NXSettings::GetBool( const string& key, bool def ) const
{
  bool bRet = def;
  string sValue = GetString( key, "NULL" );
  if( sValue != "NULL" )
  {
    if( sValue == "true" || sValue == "1" )
      bRet = true;
    if( sValue == "false" || sValue == "0" )
      bRet = false;
  }

  return bRet;
}

string NXSettings::Dump() const
{
  string sDump = "";
  return sDump;
}

bool NXSettings::InitExpat()
{
  if( mp_parserXML == NULL )
  {
    mp_parserXML = XML_ParserCreate( NULL );
    if( !mp_parserXML )
    {
      NX_LOG_LOGERROR("Error: couldn't allocate memory for XML parser.");
      mp_parserXML == NULL;
      return false;
    }

    XML_SetUserData( mp_parserXML, this );
    XML_SetElementHandler( mp_parserXML, &NXSettings::OnStartElement, &NXSettings::OnEndElement );
  }

  return true;
}

void NXSettings::CleanupExpat()
{
  if( mp_parserXML != NULL )
  {
    XML_ParserFree( mp_parserXML );
    mp_parserXML = NULL;
  }
}

void NXSettings::OnStartElement( void *data, const char *el, const char **attr )
{
  NXSettings* pSets = (NXSettings*)data;

  string elem( el );

  if( elem == "group" && attr[0] && attr[1] )
  {
    string sTmp = attr[0];
    if( sTmp == "name" )
    {
      sTmp = attr[1];
      pSets->SetGroup( sTmp );
    }
    else
    {
      pSets->SetGroup( "General" );
    }
  }

  if( elem == "option" && attr[0] && attr[1] && attr[2] && attr[3] )
  {
    string sKey = attr[1];
    string sValue = attr[3];
    pSets->SetString( sKey, sValue );
  }
}

void NXSettings::OnEndElement( void *data, const char *el )
{
}

bool NXSettings::Read()
{
  Clear();

  m_isRead = false;
  m_fileState = false;

  if( m_filePath.empty() )
  {
    NX_LOG_LOGERROR("Error: file path is not set.");
    return m_fileState;
  }

  ifstream ifs( m_filePath.c_str() );
  if( !ifs )
  {
    cerr << "Error: unable to open file '" << m_filePath << "'." << endl << flush;
    return m_fileState;
  }

  InitExpat();

  char *buffer;

  ifs.seekg( 0, ios::end );
  int length = ifs.tellg();
  ifs.seekg( 0, ios::beg );

  // allocate memory:
  buffer = new char[length];

  // read data as a block:
  ifs.read( buffer, length );

  ifs.close();

  if( XML_Parse( mp_parserXML, buffer, length, 1 ) == 0 ) // Use 0 instead of XML_STATUS_ERROR 
                                                          // for compatibility
  {
    cerr << "Error: parse error at line " << XML_GetCurrentLineNumber( mp_parserXML )
         << ":" << endl << XML_ErrorString( XML_GetErrorCode( mp_parserXML ) ) << endl << flush;
    char buffer[500]; //TODO: Potential Overflow
    sprintf(buffer, "Error: parse error at line %d:\n%s", XML_GetCurrentLineNumber( mp_parserXML ) , XML_ErrorString(XML_GetErrorCode(mp_parserXML) ));
    NX_LOG_LOGERROR(buffer);
    m_fileState = false;
  }
  else
    m_fileState = true;

  delete buffer;

  CleanupExpat();

  m_isRead = true;
  return m_fileState;
}
