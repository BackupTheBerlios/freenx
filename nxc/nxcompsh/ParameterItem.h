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


#ifndef NX_PARAMETER_ITEM_H
#define NX_PARAMETER_ITEM_H

#include <string>
using namespace std;


namespace NX
{


class ParameterItem
{
  int m_key;
  string m_valor;

public:
  ParameterItem() : m_key( -1 ), m_valor( "" ) {};
  ParameterItem( int key, const string& valor ) : m_key( key ), m_valor( valor ) {};
  ParameterItem( const ParameterItem& item )
  {
    m_key = item.m_key;
    m_valor = item.m_valor;
  }

  ParameterItem& operator=( const ParameterItem& item )
  {
    m_key = item.m_key;
    m_valor = item.m_valor;
    return *this;
  }
  
  bool operator==( const ParameterItem& item )
  {
    return ( m_key == item.m_key );
  }

  bool IsValid() const { return ( m_key >= 0 ); }

  int GetKey() const { return m_key; }
  const string& GetValor() const { return m_valor; }

  void SetKey( int key ) { m_key = key; }
  void SetValor( string valor ) { m_valor = valor; }

};

} /* NX */

#endif /* NX_PARAMETER_ITEM_H */

