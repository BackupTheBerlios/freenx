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


#include "Cookie.h"
#include "Session.h"
#include "Process.h"
#include "Utilities.h"

#include "NXErrors.h"
#include "NXParameters.h"
#include "Process.h"
#include "Logger.h"

#if defined (WIN32)
#else
#include "ProcessUnix.h"
#endif

#include <time.h>
/*
#include <fstream>
#include <iostream>
*/
using namespace std;




namespace NX
{

const string CookieFileName = ".cookie.tmp";



Cookie::Cookie( Session* pSession )
{
  mp_session = pSession;
  m_cookie = "";
}

Cookie::~Cookie()
{}

bool Cookie::GenerateRandomOne()
{
  /* Generate a random cookie */
  /* username + current date + current time */
  string sTmp = mp_session->GetParameters().GetString( NX_Username, "nobody" );
  char cTmp[256];
  memset( cTmp, 0, sizeof( cTmp ) );
  sprintf( cTmp, " %ld ", time(0) );
  sTmp += string( cTmp );
  m_cookie = Utilities::GenerateMD5( sTmp );
  return true;
}

bool Cookie::ReadFromFile( string file_name )
{
  ifstream ifs( file_name.c_str() );
  if( !ifs )
  {
    NX_LOG_LOGERROR( "NXCookie: cannot open file '" + file_name + "'.");
    mp_session->SetException( NX_RuntimeError, "Cannot read cookie from file." );
    return false;
  }

  ifs >> m_cookie;

  ifs.close();

  if( m_cookie.empty() )
  {
    NX_LOG_LOGDEBUG( "NXCookie: cookie is empty.");
    mp_session->SetException( NX_RuntimeError, "Cookie is empty." );
    return false;
  }
  else
  {
    NX_LOG_LOGDEBUG( "NXCookie: read cookie '" + m_cookie + "'.");
    return true;
  }
}

bool Cookie::GetDisplay( string& display )
{
  display = "";
  string sTmp = Process::GetEnvironment( "DISPLAY" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGDEBUG( "NXCookie: environment 'DISPLAY' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Environment 'DISPLAY' is empty." );
    return false;
  }

  int iPos1 = sTmp.find( ":" );
  if( iPos1 == string::npos )
  {
    NX_LOG_LOGDEBUG( "NXCookie: environment 'DISPLAY' is invalid.");
    mp_session->SetException( NX_InvalidParameter, "Environment 'DISPLAY' is invalid." );
    return false;
  }

  int iPos2 = sTmp.find( "." );
  if( iPos2 == string::npos || iPos2 < iPos1 )
    iPos2 = sTmp.size();

  display = sTmp.substr( iPos1 + 1, iPos2 - iPos1 - 1 );
  NX_LOG_LOGDEBUG( "NXCookie: display is '" + display + "'.");
  return true;
}

bool Cookie::ExecuteCommand( string cmd )
{
  NX_LOG_LOGDEBUG( "NXCookie: executing command '" + cmd + "'." );
  if( system( cmd.c_str() ) == -1 )
  {
  /* Error occurred: cannot fork */
    NX_LOG_LOGDEBUG( "NXCookie: cannot execute command." );
    mp_session->SetException( NX_RuntimeError, "Cannot execute xauth command." );
    return false;
  }
  NX_LOG_LOGDEBUG( "NXCookie: command executed.");
  return true;
}

#if defined( WIN32 )
bool Cookie::Generate()
{
  return GenerateRandomOne();
}

bool Cookie::ReadExistingOne()
{
  return false;
}
#else
bool Cookie::ReadExistingOne()
{  
  /* Generate cookie using xauth */
  ParametersList& parameters = mp_session->GetParameters();
  string sTmp;
  string file_cookie = parameters.GetString( NX_CookieTempFilePath, "" );
  if( file_cookie.empty() )
  {
    NX_LOG_LOGERROR( "Warning: parameter 'NX_CookieTempFilePath' is empty. Setting default one." );

    file_cookie = parameters.GetString( NX_PersonalDirectory, "" );
    if( file_cookie.empty() )
    {
      NX_LOG_LOGDEBUG( "NXCookie: parameter 'NX_PersonalDirectory' is empty.");
      mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_PersonalDirectory' is empty." );
      return false;
    }
    file_cookie += "/";
    file_cookie += CookieFileName;
  }

  sTmp = Process::GetEnvironment( "HOSTNAME" );
  if( sTmp.empty() )
  {
     sTmp = getHostnameTheHardWay();
     if (sTmp.empty())
     {
       NX_LOG_LOGDEBUG( "NXCookie: environment 'HOSTNAME' is empty.");
       mp_session->SetException( NX_InvalidParameter, "Environment 'HOSTNAME' is empty." );
       return false;
     }
  }
  string hostname = sTmp;

  string display;
  if( !GetDisplay( display ) )
    return false;

  string xauth_cmd( "xauth" );

  sTmp = Process::GetEnvironment( "XAUTHORITY" );
  if( !sTmp.empty() )
  {
    xauth_cmd += " -f ";
    xauth_cmd += sTmp;
  }

  xauth_cmd += " list | grep MIT-MAGIC-COOKIE-1 | grep \"";
  xauth_cmd += hostname;
  xauth_cmd += "/unix:";
  xauth_cmd += display;
  xauth_cmd += "\" | cut -f 5 -d ' ' 1>";
  xauth_cmd += file_cookie;

  if( !ExecuteCommand( xauth_cmd ) )
    return false;

  return ReadFromFile( file_cookie );
}

string Cookie::getHostnameTheHardWay()
{
   FILE *fp;
   char host[1048];
   host[0] = '\0';                                                                             
   fp = popen("/bin/hostname", "r");
   if (fp != NULL)
   {
      fgets(host, sizeof host, fp);
      pclose(fp);
   }
   return host;
}


bool Cookie::Generate()
{
  ParametersList& parameters = mp_session->GetParameters();
  string sTmp;
  string file_cookie = parameters.GetString( NX_PersonalDirectory, "" );
  if( file_cookie.empty() )
  {
    NX_LOG_LOGDEBUG( "NXCookie: parameter 'NX_PersonalDirectory' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Parameter 'NX_PersonalDirectory' is empty." );
    return false;
  }
  file_cookie += CookieFileName;

  sTmp = "mcookie 1>";
  sTmp += file_cookie;

   NX_LOG_LOGDEBUG( "NXCookie: creating session cookie..." );

  if( !ExecuteCommand( sTmp ) )
    return false;

  if( !ReadFromFile( file_cookie ) )
    return false;

  sTmp = Process::GetEnvironment( "HOSTNAME" );
  if( sTmp.empty() )
  {
    NX_LOG_LOGDEBUG( "NXCookie: environment 'HOSTNAME' is empty." );
    mp_session->SetException( NX_InvalidParameter, "Environment 'HOSTNAME' is empty." );
    return false;
  }

  string hostname = sTmp;
  string display;
  if( !GetDisplay( display ) )
    return false;

  string xauth_cmd = "xauth add ";
  xauth_cmd += hostname;
  xauth_cmd +="/unix:";
  xauth_cmd += display;
  xauth_cmd +=" MIT-MAGIC-COOKIE-1 ";
  xauth_cmd += m_cookie;

  if( !ExecuteCommand( xauth_cmd ) )
    return false;

  xauth_cmd = "xauth ";
  sTmp = Process::GetEnvironment( "XAUTHORITY" );
  if( !sTmp.empty() )
  {
    xauth_cmd += "-f ";
    xauth_cmd += sTmp;
  }

  xauth_cmd += " add ";
  xauth_cmd += hostname;
  xauth_cmd += ":";
  xauth_cmd += display;
  xauth_cmd += " MIT-MAGIC-COOKIE-1 ";
  xauth_cmd += m_cookie;  // Read from file

  if( !ExecuteCommand( xauth_cmd ) )
    return false;

  NX_LOG_LOGDEBUG("NXCookie: session cookie created.");

  return true;
}

#endif /* WIN32 */

} /* NX */

