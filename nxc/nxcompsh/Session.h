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



#ifndef NX_SESSION_H
#define NX_SESSION_H


#include "ParametersList.h"
#include "Exception.h"


namespace NX
{


class Session
{
  ParametersList m_parameters;
  Exception m_exception;
  int m_sessionState;
  bool m_needProxy;

public:
  Session();
  ~Session();

  string GetName() const;
  void SetName( string new_name );
  bool HasInvalidName() const;

  bool NeedProxy() const { return m_needProxy; }
  void NeedProxy( bool value ) { m_needProxy = value; }

  const ParametersList& GetParameters() const { return m_parameters; }
  ParametersList& GetParameters() { return m_parameters; }

  int GetState() const { return m_sessionState; }
  void SetState( int new_state ) { m_sessionState = new_state; }

  const Exception& GetException() const { return m_exception; }
  void SetException( int ErrorType, string ErrorString );
  void SetException( const Exception& );
  bool UseEncryption() const;
};


} /* NX */


#endif /* NX_SESSION_H */

