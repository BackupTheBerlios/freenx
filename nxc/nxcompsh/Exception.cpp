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


#include "Exception.h"

namespace NX
{

Exception::Exception() : m_type( 0 ), m_error( "No error" )
{}

Exception::Exception( int type, string error ) : m_type( type ), m_error( error )
{}

Exception::~Exception()
{}

Exception::Exception( const Exception& x )
{
  m_type = x.m_type;
  m_error = x.m_error;
}

Exception& Exception::operator=( const Exception& x )
{
  m_type = x.m_type;
  m_error = x.m_error;
  return *this;
}

string Exception::GetTypeName( int type )
{
  switch( type )
  {
  case NX_NoError           : return "No Error";
  case NX_RuntimeError      : return "Runtime error";
  case NX_InvalidParameter  : return "Invalid paramater";
  case NX_SSHError          : return "SSH error";
  case NX_ProxyError        : return "Proxy error";

  default:
    return "Undefined";
  };
}

} /* NX */
