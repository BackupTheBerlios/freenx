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


#include "StringUtilities.h"

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <vector>
using namespace std;



string StringUtilities::GetUpperString( string string_to_upper )
{
  string sRet = "";
  string::const_iterator p = string_to_upper.begin();
  while( p != string_to_upper.end() )
  {
    sRet += (char)toupper( *p );
    ++p;
  }
  return sRet;
}

string StringUtilities::GetLowerString( string string_to_lower )
{
  string sRet = "";
  string::const_iterator p = string_to_lower.begin();
  while( p != string_to_lower.end() )
  {
    sRet += (char)tolower( *p );
    ++p;
  }
  return sRet;
}

string StringUtilities::NumberToString( long number )
{
  char buf[256];
  *buf = '\0';
  string sRet = "";
  sprintf( buf, "%ld", number );
  sRet = buf;
  return sRet;
}

long StringUtilities::StringToNumber( string string_number ) throw( XInvalidNumber )
{
  if( string_number.empty() )
    return 0;

  long number = 0;
  bool sign = false;

  string::const_iterator it = string_number.begin();

  while( isspace( *it ) )
    ++it;

  if( *it == '+' )
    ++it;
  else if( *it == '-' )
  {
    sign = true;
    ++it;
  }

  if( !isdigit( *it ) )
  {
    throw XInvalidNumber();
  }

  while( isdigit( *it ) && it != string_number.end() )
  {
    number = number * 10 + *it - '0';
    ++it;
  }

  if( sign )
    number = 0 - number;

  return number;
}

int StringUtilities::ReplaceAll( string& s, string old_str, string new_str )
{
  int iRes = 0, x = 0;

  x = s.find( old_str );
  while( x != string::npos )
  {
    s.replace( x, old_str.size(), new_str );
    iRes++;
    x = s.find( old_str, x + 1 );
  }

  return iRes;
}

int StringUtilities::SplitString( string input, char delimiter, list<string>& results )
{
  string::const_iterator it = input.begin();
  string sArg = "";

  while( it != input.end() )
  {
    if( *it == delimiter )
    {
      results.push_back( sArg );
      sArg = "";
    }
    else
      sArg += *it;

    ++it;
  }

  if( !sArg.empty() )
    results.push_back( sArg );

  return results.size();
}
