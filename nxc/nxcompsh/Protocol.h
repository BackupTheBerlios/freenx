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


#ifndef NX_PROTOCOL_H
#define NX_PROTOCOL_H

#include <string>
using namespace std;

namespace NX
{


class ParametersList;
class Session;
class Transport;
class Proxy;


class Protocol
{
  static string m_version;
public:

  static string GetVersion() { return m_version; };

  static string EncodeUrl( const string& );
  static string EncodeUrl( const char* );
  static string DecodeUrl( const string& );

  static string ParameterToString( int parameter_id );

  static int CheckConnectionStatus( const string& , Session*, Transport* );
  static bool CheckConnectionError( Session*, Transport* );
  static bool CloseConnection( Session*, Transport* );

  static string GetProxyOptions( ParametersList& , bool force_creation = false );
  static string GetServerOptions( ParametersList& , bool force_creation = false );
  static string GetOptions( ParametersList& pl, string separator, int first_parameter, int last_parameter , bool encode_value, bool quote_value = false );
  static string GetOption( string sParameterKey, string sParameterValor, bool encode_value, bool quote_value );


  static bool SetStandardSessionLogPath( ParametersList& );
  static bool SetStandardProxyOptionsPath( ParametersList& );
  static bool SetStandardSessionDirectory( ParametersList& );

  static int CheckProxyStatus( const string&, Session*, Proxy* );

protected:
  /* Server */
  static bool ParseResponseFromServer( Session*, const string& NXResponse, int& NXCode, string& NXString );
  static bool ParseLineFromServer( const string& NXResponse, int& NXCode, string& NXString );
  static bool ParseHello( const string& HelloLine, int& NXCode, string& NXString );
  static bool AdvanceConnection( Session*, Transport* );
  static bool SetParametersFromServer( Session*, Transport* );
  static bool SetParameterFromServer( Session*, int NXCode, string NXString );
  static string GetParameterFromResponse( string NXString );
  static void SetConnectionStatus( Session*, int NXCode, string NXString );
  static bool ConnectionErrorInLine( Session* pSession, int NXCode, string NXString );

  static bool SendHello( Session*, Transport* );
  static bool SendConfirmation( Session*, Transport* );
  static bool SendSetModes( Session*, Transport* );
  static bool SendBye( Session*, Transport* );
  static bool SendSwitch( Session*, Transport* );
  static bool StartAuth( Session*, Transport* );
  static bool SendUsernameAndPassword( Session*, Transport* );
  static bool StartSession( Session*, Transport* );
  static bool SendParameters( Session*, Transport* );

  /* Proxy */
  static bool ParseResponseFromProxy( Session*, const string& NXResponse );
  static bool ParseLineFromProxy( Session* pSession, const string& NXResponse );


private:
  Protocol();
  virtual ~Protocol();

};


} /* NX */


#endif /* NX_PROTOCOL_H */

