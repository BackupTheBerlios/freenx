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


#ifndef NX_EXCEPTION_H
#define NX_EXCEPTION_H

#include "NXErrors.h"

#include <string>
using namespace std;

namespace NX
{


class Exception
{
  int m_type;
  string m_error;

public:
  Exception();
  Exception( int type, string error );
  Exception( const Exception& );

  Exception& operator=( const Exception& );

  virtual ~Exception();

  const string& GetString() const { return m_error; }
  int GetType() const { return m_type; }

  string GetTypeName() const { return GetTypeName( m_type ); }
  static string GetTypeName( int type );

  void SetRuntimeError( string Error ) { m_type = NX_RuntimeError; m_error = Error; }

  void SetString( string Error ) { m_error = Error; }
  void SetType( int ErrorType ) { m_type = ErrorType; }

};

} /* NX */

#endif /* NX_EXCEPTION_H */

