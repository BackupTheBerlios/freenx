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


#include "Utilities.h"
#include "MD5.h"
#include "StringUtilities.h"
#include "XInterface.h"

#include <stdio.h>
#include <sys/stat.h>

#if defined( WIN32 )
#include <direct.h>  // For _mkdir
#include <windows.h>  
#endif

namespace NX
{


string Utilities::GenerateMD5( string my_string )
{
#define MD5_LENGTH   16
  md5_state_t   *md5_state  = new md5_state_t();
  unsigned char *md5_digest = new unsigned char[MD5_LENGTH];
  md5_init(md5_state);
  md5_append(md5_state, (const unsigned char *)my_string.c_str(), my_string.size() );
  md5_finish(md5_state, md5_digest);

  char md5String[(MD5_LENGTH * 2) + 1];
  int di = 0;
  for (di = 0; di < MD5_LENGTH; ++di)
  {
    sprintf(md5String + (di * 2), "%02x", md5_digest[di]);
  }

  *(md5String + (di * 2)) = '\0';
  delete [] md5_state;
  delete [] md5_digest;
  return string( md5String );
}



void Utilities::ConvertSeparatorsInPath( string& path )
{
#if defined( WIN32 )
  string os_separator = "\\";
  string other_separator = "/";
#else
  string os_separator = "/";
  string other_separator = "\\";
#endif
  StringUtilities::ReplaceAll( path, other_separator, os_separator );
}

bool Utilities::IsDirectory( const string& path )
{
  struct stat my_stat;
  if( stat( path.c_str(), &my_stat) != 0 )
    return false;
  return ( ( my_stat.st_mode & S_IFDIR) != 0 );
}

bool Utilities::FileExist( const string& path )
{
  struct stat my_stat;
  return( stat( path.c_str(), &my_stat) == 0 );
}

/*
bool Utilities::CreateDirectory(const string& path)
{
#if defined( WIN32 )
  return ( !( _mkdir( path.c_str() ) == 0 ) );
#else
  return ( !( mkdir( path.c_str(), 0777 ) == 0 ) );
#endif
}
*/

string Utilities::GetCurrentKeyboardLayout()
{
#if defined( WIN32 )
  WORD latoutKey = LOWORD(GetKeyboardLayout(0)); //get current layout.
  switch(latoutKey)
  {
   case 1040: return string("it");
   case 1031: return string("de");
   case 1078: return string("af");
   case 1052: return string("sq");
   case 1025: return string("ar");
   case 1069: return string("eu");
   case 1059: return string("be");
   case 1026: return string("bg");
   case 1027: return string("ca");
   case 1028: return string("ch");
   case 1029: return string("cs");
   case 1030: return string("da");
   case 1043: return string("nl");
   case 1033: return string("us");
   case 1061: return string("et");
   case 1080: return string("fo");
   case 1065: return string("fa");
   case 1035: return string("fi");
   case 1036: return string("fr");
   case 1032: return string("el");
   case 1037: return string("he");
   case 1038: return string("hu");
   case 1039: return string("is");
   case 1057: return string("in");
   case 1041: return string("jp");
   case 1042: return string("ko");
   case 1062: return string("lv");
   case 1063: return string("lt");
   case 1044: return string("no");
   case 1045: return string("pl");
   case 1046: return string("pt");
   case 1048: return string("ro");
   case 1049: return string("ru");
   case 1050: return string("hr");
   case 1051: return string("sk");
   case 1060: return string("sl");
   case 1034: return string("es");
   case 1053: return string("sv");
   case 1054: return string("th");
   case 1055: return string("tr");
   case 1066: return string("vi");
   case 2057: return string("uk");
   default:
     return "";
  };

#else

  XInterface xInterface( NULL );
  string sTmp = xInterface.GetKeyboardLayout().c_str();
  return sTmp;

#endif
}

string Utilities::ConvertKeyboardLayoutToHex( const string& s )
{
  string sRet = "";
  int iRet = 0;

  if     ( s == "it" )  iRet = 1040;
  else if( s == "de" )  iRet = 1031;
  else if( s == "af" )  iRet = 1078;
  else if( s == "sq" )  iRet = 1052;
  else if( s == "ar" )  iRet = 1025;
  else if( s == "eu" )  iRet = 1069;
  else if( s == "be" )  iRet = 1059;
  else if( s == "bg" )  iRet = 1026;
  else if( s == "ca" )  iRet = 1027;
  else if( s == "ch" )  iRet = 1028;
  else if( s == "cs" )  iRet = 1029;
  else if( s == "da" )  iRet = 1030;
  else if( s == "nl" )  iRet = 1043;
  else if( s == "us" )  iRet = 1033;
  else if( s == "et" )  iRet = 1061;
  else if( s == "fo" )  iRet = 1080;
  else if( s == "fa" )  iRet = 1065;
  else if( s == "fi" )  iRet = 1035;
  else if( s == "fr" )  iRet = 1036;
  else if( s == "el" )  iRet = 1032;
  else if( s == "he" )  iRet = 1037;
  else if( s == "hu" )  iRet = 1038;
  else if( s == "is" )  iRet = 1039;
  else if( s == "in" )  iRet = 1057;
  else if( s == "jp" )  iRet = 1041;
  else if( s == "ko" )  iRet = 1042;
  else if( s == "lv" )  iRet = 1062;
  else if( s == "lt" )  iRet = 1063;
  else if( s == "no" )  iRet = 1044;
  else if( s == "pl" )  iRet = 1045;
  else if( s == "pt" )  iRet = 1046;
  else if( s == "ro" )  iRet = 1048;
  else if( s == "ru" )  iRet = 1049;
  else if( s == "hr" )  iRet = 1050;
  else if( s == "sk" )  iRet = 1051;
  else if( s == "sl" )  iRet = 1060;
  else if( s == "es" )  iRet = 1034;
  else if( s == "sv" )  iRet = 1053;
  else if( s == "th" )  iRet = 1054;
  else if( s == "tr" )  iRet = 1055;
  else if( s == "vi" )  iRet = 1066;
  else if( s == "uk" )  iRet = 2057;
  else
  {
    iRet = 1033;
  }

  char cTmp[30] = {0};
  sprintf( cTmp, "0x%x", iRet );
  sRet = cTmp;
  return sRet;
}

} /* NX */

