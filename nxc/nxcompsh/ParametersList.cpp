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


#include "ParametersList.h"
#include "StringUtilities.h"
#include "Logger.h"

const string ParameterTrueValor = "1";
const string ParameterFalseValor = "0";

/*
#undef NX_PARAMETER_DEBUG


#ifdef NX_PARAMETER_DEBUG
#include <iostream>
#endif /* NX_PARAMETER_DEBUG */

namespace NX
{


ParametersList::ParametersList()
{}

ParametersList::~ParametersList()
{
  m_parameters.clear();
}

string ParametersList::GetValor( int key ) const throw( XParameterNotFound )
{
  if( !m_parameters.empty() )
  {
    list<ParameterItem>::const_iterator it = m_parameters.begin();
    while( it != m_parameters.end() )
    {
      if( (*it).GetKey() == key )
      {
        return (*it).GetValor();
      }
      ++it;
    }
  }


  throw XParameterNotFound();
}

void ParametersList::SetValor( int key, string valor ) throw( XParameterNotFound )
{
  if( !m_parameters.empty() )
  {
    list<ParameterItem>::iterator it = m_parameters.begin();
    while( it != m_parameters.end() )
    {
      if( (*it).GetKey() == key )
      {

        (*it).SetValor( valor );
        return;
      }
      ++it;
    }
  }


  throw XParameterNotFound();
}

void ParametersList::AddParameter( int key, string valor )
{
  ParameterItem item( key, valor );
  m_parameters.push_back( item );

}

void ParametersList::SetString( int key, string valor )
{
  try
  {
    SetValor( key, valor );
  }
  catch( XParameterNotFound )
  {
	  AddParameter( key, valor );
  }
}

void ParametersList::SetBool( int key, bool valor )
{
  if( valor )
    SetString( key, ParameterTrueValor );
  else
    SetString( key, ParameterFalseValor );
}

void ParametersList::SetNumber( int key, long valor )
{
  string sValor = StringUtilities::NumberToString( valor );
  SetString( key, sValor );
}

bool ParametersList::GetBool( int key, bool defaultValor ) const
{
  string sValor = "";
  try
  {
    sValor = GetValor( key );
  }
  catch( XParameterNotFound& )
  {
    return defaultValor;
  }

  if( sValor == ParameterTrueValor )
    return true;
  else if( sValor == ParameterFalseValor )
    return false;
  else
    return defaultValor;
}

long ParametersList::GetNumber( int key, long defaultValor ) const
{
  string sValor = "";
  try
  {
    sValor = GetValor( key );
  }
  catch( XParameterNotFound& )
  {
    return defaultValor;
  }

  try
  {
    return StringUtilities::StringToNumber( sValor );
  }
  catch( XInvalidNumber & )
  {
    return defaultValor;
  }
}

string ParametersList::GetString( int key, string defaultValor ) const
{
  try
  {
    return GetValor( key );
  }
  catch( XParameterNotFound& )
  {
    return defaultValor;
  }
}


} /* NX */

