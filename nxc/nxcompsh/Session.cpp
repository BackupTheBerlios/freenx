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


#include "Session.h"
#include "NXStates.h"
#include "NXParameters.h"


namespace NX
{


const string InvalidName = "Undefined";


Session::Session()
{
  m_sessionState = NX_Init;
  m_needProxy = false;
  m_parameters.SetString( NX_SessionName, InvalidName );
//  m_parameters.SetString( NX_SessionName, "Undefined" );
}

Session::~Session()
{}

string Session::GetName() const
{
  return (m_parameters.GetString( NX_SessionName, InvalidName ));
}

void Session::SetName( string new_name )
{
  m_parameters.SetString( NX_SessionName, new_name );
}

bool Session::HasInvalidName() const
{
  string name = m_parameters.GetString( NX_SessionName, InvalidName );
  if( name.empty() || name == InvalidName )
    return true;
  else
    return false;
}

bool Session::UseEncryption() const
{
  return m_parameters.GetBool(NX_EnableEncryption, false);
}

void Session::SetException( int ErrorType, string ErrorString )
{
  m_exception.SetType( ErrorType );
  m_exception.SetString( ErrorString );
  m_sessionState = NX_Exception;
}

void Session::SetException( const Exception& NXError )
{
  m_exception = NXError;
  m_sessionState = NX_Exception;
}

} /* NX */

