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


#ifndef NX_PARAMETERS_LIST_H
#define NX_PARAMETERS_LIST_H


#include <list>
#include <stdexcept>
using namespace std;

#include "ParameterItem.h"

namespace NX
{


class XParameterNotFound : public runtime_error
{
public:
  XParameterNotFound() : runtime_error( "Parameter not found" ) {}

};


class ParametersList
{
  list<ParameterItem> m_parameters;

  string GetValor( int key ) const throw( XParameterNotFound );
  void SetValor( int key, string valor ) throw( XParameterNotFound );
  void AddParameter( int key, string valor );

public:
  ParametersList();
  virtual ~ParametersList();

  void Clear() { m_parameters.clear(); }

  void SetString( int key, string valor );
  void SetBool( int key, bool valor );
  void SetNumber( int key, long valor );

  bool GetBool( int key, bool defaultValor ) const;
  long GetNumber( int key, long defaultValor ) const;
  string GetString( int key, string defaultValor ) const;

};


} /* NX */

#endif /* NX_PARAMETERS_LIST_H */


