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


#ifndef NX_UTILITIES_H
#define NX_UTILITIES_H

#include <string>
#include <stdio.h>

#if defined( WIN32 )
#include <direct.h>  // For _mkdir
#else
#include <sys/stat.h>
#endif

using namespace std;


namespace NX
{


class Utilities
{
  Utilities() {};
  ~Utilities() {};

public:
  static string GenerateMD5( string );
  static void ConvertSeparatorsInPath( string& );

  static bool CreateDirectory( const string& path ){
#if defined( WIN32 )
  return ( !( _mkdir( path.c_str() ) == 0 ) );
#else
  return ( !( mkdir( path.c_str(), 0777 ) == 0 ) );
#endif
}

   static bool IsDirectory( const string& );
  static bool FileExist( const string& );

  static string GetCurrentKeyboardLayout();
  static string ConvertKeyboardLayoutToHex( const string& );

};


} /* NX */


#endif /* NX_UTILITIES_H */
