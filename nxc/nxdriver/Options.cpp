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


#include "Options.h"
#include "Process.h"
#include "Utilities.h"
#include "StringUtilities.h"

#include <sys/stat.h>
#include <sys/types.h>

const string NXOptionsVersionValue = "OptionsVersion";
const string NXSystemDirectoryValue = "NXSystemDirectory";
const string NXPersonalDirectoryValue = "NXPersonalDirectory";
const string NXConnectionCheckValue = "ConnectionCheck";
const string NXConnectionTimeoutValue = "ConnectionTimeout";
const string NXRemoveOldSessions = "RemoveOldSessions";
const string NXSshLogFileName = "sshlog";
const string NXCookieFileName = "authlog";
string NXSshKeyFileName = "client.id_dsa.key";
const string NXConfigFileSuffix = "conf";
const string NXLastSession = "LastSession";
const string NXArtsLogFileName = "artslog";
const string NXDefaultValue = "Default";



Options *options = 0;


Options::Options()
{
  options = this;
}

Options::~Options()
{
  options = 0;
}

void Options::SetValue( const string& key, const string& value )
{
  string sKey = key;
  string sValue = value;
  if (key == "NXSshKeyFileName")
     NXSshKeyFileName = value;
}

string Options::GetValue( const string& key, const string& def ) const
{
  string sRet = def;
  return sRet;
}

string Options::GetVersion() const
{
  return GetValue( NXOptionsVersionValue, "1.2.2" );
}

string Options::GetNXSystemDirectory() const
{
  string sys_dir = GetValue( NXSystemDirectoryValue, NXDefaultValue );
  if( sys_dir == NXDefaultValue )
  {
    sys_dir = NX::Process::GetEnvironment( "NXDIR" ).c_str();
    if( sys_dir.empty() )
    {
#ifdef WIN32
       sys_dir = "c:\\Program Files\\NX Client for Windows";
#else
      sys_dir = "/usr/NX";
#endif
    }
  }

  NX::Utilities::ConvertSeparatorsInPath( sys_dir );
  return sys_dir;
}

string Options::GetNXPersonalDirectory() const
{
  string my_dir = GetValue( NXPersonalDirectoryValue, NXDefaultValue );
  if( my_dir == NXDefaultValue )
  {
    my_dir = NX::Process::GetEnvironment( "HOME" ).c_str();
#ifdef WIN32
   my_dir = "c:\\.nx";
#else
    my_dir += "/.nx";
#endif
  }

  NX::Utilities::ConvertSeparatorsInPath( my_dir );
  NX::Utilities::CreateDirectory( my_dir );


  
  return my_dir;
}

string Options::GetNXConfigDirectory() const
{
  string my_config = GetNXPersonalDirectory();
  my_config += "/config";

  NX::Utilities::ConvertSeparatorsInPath( my_config );
  NX::Utilities::CreateDirectory( my_config );
  return my_config;
}

string Options::GetNXConfigFileSuffix() const
{
  return NXConfigFileSuffix;
}

string Options::GetNXTempDirectory() const
{
  string my_temp = GetNXPersonalDirectory();
  my_temp += "/temp";

  NX::Utilities::ConvertSeparatorsInPath( my_temp );
  NX::Utilities::CreateDirectory( my_temp );
	
  return my_temp;
}

string Options::GetNXBinDirectory() const
{
  string dir_path = GetNXSystemDirectory();
  dir_path += "/bin";
  return dir_path;
}

string Options::GetNXSshPath() const
{
  string bin_path = GetNXBinDirectory();
#if defined( WIN32 )
  bin_path += "\\nxssh.exe";
#else
  bin_path += "/nxssh";
#endif
  return bin_path;
}

string Options::GetNXProxyPath() const
{
  string bin_path = GetNXBinDirectory();
#if defined( WIN32 )
  bin_path += "\\nxproxy.exe";
#else
  bin_path += "/nxproxy";
#endif
  return bin_path;
}

string Options::GetNXSshKeyPath() const
{
  string file_path = GetNXShareDirectory();
#if defined( WIN32 )
  file_path += "\\";
#else
  file_path += "/";
#endif
  file_path += NXSshKeyFileName;

  return file_path;
}

string Options::GetNXSshLogPath() const
{
  string file_path = GetNXPidTempDirectory();
#if defined( WIN32 )
  file_path += "\\";
#else
  file_path += "/";
#endif
  file_path += NXSshLogFileName;

  return file_path;
}

string Options::GetNXArtsLogPath() const
{
  string file_path = GetNXPidTempDirectory();
#if defined( WIN32 )
  file_path += "\\";
#else
  file_path += "/";
#endif
  file_path += NXArtsLogFileName;

  return file_path;
}

string Options::GetNXCookieFilePath() const
{
  string file_path = GetNXPidTempDirectory();
#if defined( WIN32 )
  file_path += "\\";
#else
  file_path += "/";
#endif
  file_path += NXCookieFileName;

  return file_path;
}

string Options::GetNXLibDirectory() const
{
  string dir_path = GetNXSystemDirectory();
  dir_path += "/lib";
  return dir_path;
}

string Options::GetNXShareDirectory() const
{
  string dir_path = GetNXSystemDirectory();
#if defined ( WIN32 )
  dir_path += "\\share";
#else
  dir_path += "/share";
#endif
  return dir_path;
}

string Options::GetNXImagesDirectory() const
{
  string dir_path = GetNXShareDirectory();
  dir_path += "/images";
  return dir_path;
}

string Options::GetNXIconsDirectory() const
{
  string dir_path = GetNXShareDirectory();
  dir_path += "/icons";
  return dir_path;
}

string Options::GetNXConfigFilePath( const string& session_name ) const
{
  string file_path = GetNXConfigDirectory();
  if( session_name.empty() )
    file_path += "/default";
  else
    file_path += "/" + session_name;
  file_path += ".";
  file_path += NXConfigFileSuffix;
  return file_path;
}

string Options::GetNXPidTempDirectory() const
{
#ifdef WIN32
  unsigned long pid = (unsigned long)::GetCurrentProcessId();
#else
  unsigned long pid = getpid();
#endif
  string my_pidtemp = GetNXTempDirectory();
  my_pidtemp += "/";
  my_pidtemp += StringUtilities::NumberToString( pid );

  if( !NX::Utilities::IsDirectory( my_pidtemp ) )
    NX::Utilities::CreateDirectory( my_pidtemp );
  return my_pidtemp;
}


void Options::CleanOldFiles()
{
}

void Options::RestoreDefault()
{
}



