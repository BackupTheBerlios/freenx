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


#ifndef STRING_UTILITIES_H
#define STRING_UTILITIES_H

#include <string>
#include <list>
#include <stdexcept>
using namespace std;


class XInvalidNumber : public runtime_error
{
public:
  XInvalidNumber() : runtime_error( "StringToNumber: cannot convert string to number." ) {}
};


class StringUtilities
{
  StringUtilities();
  ~StringUtilities();

public:
  static string GetUpperString( string string_to_upper );
  static string GetLowerString( string string_to_lower );
  static string NumberToString( long number );
  static long StringToNumber( string string_number ) throw( XInvalidNumber );
  static int ReplaceAll( string&, string old_str, string new_str );
  static int SplitString( string input, char delimiter, list<string>& );

};


#endif /* STRING_UTILITIES_H */
