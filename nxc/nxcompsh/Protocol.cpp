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


#include "Protocol.h"
#include "Session.h"
#include "TransportSSH.h"
#include "ParametersList.h"
#include "StringUtilities.h"
#include "Utilities.h"

#include "NXParameters.h"
#include "NXErrors.h"
#include "NXStates.h"
#include "NXCompSh.h"
#include "Logger.h"

#include <ctype.h>
#include <string>
#include <sstream>
using namespace std;


namespace NX
{


string Protocol::m_version = NX_PROTOCOL_VERSION;

const unsigned int SSHErrorsNumber = 8;
static char * SSHErrors[SSHErrorsNumber] =
{
  "command not found",
  "No such file or directory",
  "Connection refused",
  "Attempt to connect timed out without establishing a connection",
  "no address associated with hostname.",
  "Name or service not known",
  "No route to host",
  "Host key verification failed"
};


Protocol::Protocol()
{}

Protocol::~Protocol()
{}

static unsigned char toHex( const unsigned char &x )
{
  return x > 9 ? x + 55: x + 48;
}

string Protocol::EncodeUrl( const string& sIn )
{
  string sOut = "";
  char c;
  for( string::const_iterator it = sIn.begin(); it != sIn.end(); it++ )
  {
    c = *it;
    if(!isalnum(c) )
    {
      if( isspace(c) )
        sOut += '+';
      else
      {
        sOut += '%';
        sOut += toHex(c>>4);
        sOut += toHex(c%16);
      }
    }
    else
      sOut += c;
  }

  return sOut;
}

string Protocol::EncodeUrl( const char* pString )
{
  string sIn( pString );
  return EncodeUrl( sIn );
}

string Protocol::DecodeUrl( const string& sIn )
{
  // TODO
  string sOut = sIn;
  return sOut;
}

string Protocol::GetServerOptions( ParametersList& pl, bool force_creation )
{
  string sRet = "";

  sRet = pl.GetString( NX_ServerOptions, "" );
  if( !force_creation && !sRet.empty() )
  {
    NX_LOG_LOGDEBUG( "NXProtocol: getting options for server connection." );

    NX_LOG_LOGDEBUG( "NXProtocol: options for server = '" + sRet + "'." );

    return sRet;
  }

  NX_LOG_LOGDEBUG( "NXProtocol: creating options for server connection." );


    sRet = "--" + GetOptions( pl, " --", NX_Server_FirstParameter, NX_Server_LastParameter, false, true );
  // If we are resuming a session, then we need to add the id to the options
  string sValue = pl.GetString(NX_ProxySessionId, "");
  if (!sValue.empty())
  { 
     string sep = "";
     if (!sRet.empty()) sep = " --";  
     string sKey = ParameterToString(NX_ProxySessionId);
     sRet = GetOption(sKey, sValue, true, true) + sep + sRet;   
  }
 
  NX_LOG_LOGDEBUG( "NXProtocol: options for server = '" + sRet + "'." );

  return sRet;
}

string Protocol::GetOptions( ParametersList& pl, string separator, int first_parameter, int last_parameter , bool encode_value, bool quote_value )
{
  string sRet = "";
  string sParameterKey = "";
  string sParameterValor = "";
  bool addSeparator = false;

  for( int i = first_parameter; i <= last_parameter; i++ )  
  {
    sParameterKey = ParameterToString( i );
    sParameterValor = pl.GetString( i, "" );

    if( !sParameterKey.empty() && !sParameterValor.empty() )
    {
      NX_LOG_LOGDEBUG( "NXProtocol: set option '" + sParameterKey +
            "' to '" + sParameterValor + "'." );
      if( addSeparator )
        sRet += separator;
      else
        addSeparator = true;
     
      sRet += GetOption (sParameterKey, sParameterValor, encode_value, quote_value);
    }
  }
  return sRet;
}

string Protocol::GetOption(string sParameterKey, string sParameterValor, bool encode_value, bool quote_value)
{
  string sRet = "";
  sRet = sParameterKey;
  sRet += "=";
  if (quote_value)
	  sRet += "\"";
  StringUtilities::ReplaceAll( sParameterValor, " ", "_" );
  if( encode_value )
    sRet += EncodeUrl( sParameterValor );
  else
    sRet += sParameterValor ;
  if (quote_value)
	  sRet += "\"";
  return sRet;
}

string Protocol::ParameterToString( int parameter_id )
{
  switch( parameter_id )
  {
  /* Server */
  case NX_SessionName:              return "session";
  case NX_SessionCookie:            return "cookie";
  case NX_SessionType:              return "type";
  case NX_ApplicationPath:          return "application";
  case NX_CacheSizeInMemory:        return "cache";
  case NX_LinkSpeed:                return "link";
  case NX_EnableBackingstore:       return "backingstore";
  case NX_DesktopGeometry:          return "geometry";
  case NX_Keyboard:                 return "keyboard";
  case NX_EnableEncryption:         return "encryption";
  case NX_RestoreCache:             return "cache";
  case NX_EnableTcpNoDelay:         return "nodelay";
  case NX_EnableStreamCompression:  return "stream";
  case NX_EnableMedia:              return "media";
  case NX_MediaDriver:              return "mediahelper";
  case NX_EnableSamba:              return "samba";
  case NX_AgentServer:              return "agent_server";
  case NX_AgentUser:                return "agent_user";
  case NX_AgentPassword:            return "agent_password";
  case NX_ImageCompressionType:     return "imagecompressionmethod";
  case NX_ImageCompressionLevel:    return "imagecompressionlevel";
  case NX_EnableLowBandwidth:       return "lowbandwidth";
  case NX_EnableVirtualDesktop:     return "virtualdesktop";
  case NX_EnableTaint:              return "taint";
  case NX_EnableRender:             return "render";
  case NX_UseSessionId:             return "sessionid";
  case NX_Keymap:                   return "keymap";
  case NX_CacheSizeOnDisk:          return "images";
  /* Proxy */
  case NX_ProxySessionName:         return "session";
  case NX_ProxyConnect:             return "connect";
  case NX_ProxyCookie:              return "cookie";
  case NX_ProxyRoot:                return "root";
  case NX_ProxySessionId:           return "id";
  //case NX_ProxyCacheType:           return "type"; not useful since proxy 1.2.2
  case NX_ProxyStream:              return "stream";
  case NX_ProxyNoDelay:             return "nodelay";
  case NX_ProxyRender:              return "render";
  case NX_ProxyTaint:               return "taint";
  case NX_ProxySamba:               return "samba";
  case NX_ProxyMediaPort:           return "media";
  case NX_ProxyListen:              return "listen";

  default:
    return "";
  }
}

bool Protocol::ParseHello( const string& HelloLine, int& NXCode, string& NXString )
{
  string ServerHello = "HELLO NXSERVER - Version ";
  int iHelloPos1 = HelloLine.find( ServerHello );
  if( iHelloPos1 != string::npos )
  {
    int iHelloPos2 = HelloLine.find( " - ", ServerHello.size() );
    if( iHelloPos2 == string::npos )
      iHelloPos2 = HelloLine.size() - 1;
    NXCode = 100;
    NXString = HelloLine.substr( ServerHello.size(), iHelloPos2 - ServerHello.size() );
    iHelloPos2 = NXString.find( "-" );
    if( iHelloPos2 != string::npos )
      NXString = NXString.substr( 0, iHelloPos2 );
    return true;
  }

  return false;
}

bool Protocol::ParseLineFromServer( const string& NXResponse, int& NXCode, string& NXString )
{
  if( ParseHello( NXResponse, NXCode, NXString ) )
    return true;

  int iPtr = 0;
  int iNXPos = NXResponse.find( "NX> " );
  NXCode = -1;
  NXString = NXResponse;

  if( iNXPos == string::npos )
  {
    NX_LOG_LOGDEBUG( "NXProtocol: line does not contain token NX." );
    NX_LOG_LOGDEBUG( "NXProtocol: line is: " + NXResponse );
    return false;
  }

  iPtr = iNXPos + 4;  // skip NX>
  string sNXCode = "";
  for( unsigned int i = iPtr; i < NXResponse.size(); i++ )
  {
    if( isspace( NXResponse[i] ) )
    {
      iPtr = i + 1; // skip space
      break;
    }
    else
      sNXCode += NXResponse[i];
  }

  try
  {
    NXCode = StringUtilities::StringToNumber( sNXCode );
  }
  catch( XInvalidNumber )
  {
    NX_LOG_LOGDEBUG( "NXProtocol: '" + NXResponse + "' is invalid response." );
    return false;
  }

  NXString = NXResponse.substr( iPtr, NXResponse.size() -1 );

  StringUtilities::ReplaceAll( NXString, "\n", " " );
  StringUtilities::ReplaceAll( NXString, "\r", " " );

  NX_LOG_LOGDEBUG( "NXProtocol: parsed line has code '" + sNXCode
       + "' and string '" + NXString + "'." );
  return true;
}

bool Protocol::ParseResponseFromServer( Session* pSession, const string& NXResponse,
                                          int& NXCode, string& NXString )
{
  NXCode = -1;
  NXString = "";

  if( NXResponse.empty() )
  {
    NX_LOG_LOGDEBUG( "NXProtocol: response is empty." );
    return false;
  }

  list<string> lines;
  StringUtilities::SplitString( NXResponse, '\n', lines );

  bool error_found = false;

  for( list<string>::const_iterator it = lines.begin(); it != lines.end(); ++it )
  {
    if( ParseLineFromServer( *it, NXCode, NXString ) )
      SetConnectionStatus( pSession, NXCode, NXString );

    if( ConnectionErrorInLine( pSession, NXCode, NXString ) )
      return false;
  }

  return true;
}

int Protocol::CheckConnectionStatus( const string& buffer, Session* pSession, Transport* pTransport )
{
  int connection_state = pSession->GetState();

  if( connection_state == NX_Exception)
// || connection_state == NX_SessionAccepted )
    return connection_state;  // nothing to do...

  if( buffer.empty() )
    return connection_state;

  int iCode = 0;
  string sString = "";

  if( !ParseResponseFromServer( pSession, buffer, iCode, sString ) )
    return pSession->GetState();

  bool connectionOk = AdvanceConnection( pSession, pTransport );
  if( !connectionOk )
    NX_LOG_LOGERROR( "NXProtocol: error occurred during advance connection." );

  return pSession->GetState();
}

void Protocol::SetConnectionStatus( Session* pSession, int NXCode, string NXString )
{
  int connection_state = pSession->GetState();

  switch( NXCode )
  {
  case 203:
    connection_state = NX_Connecting;
    break;

  case 200:
    connection_state = NX_Connecting;
    NX_LOG_LOGINFO( "NXProtocol: connecting..." );
    break;

  case 205:
     connection_state = NX_ConfirmAddHost;
     NX_LOG_LOGINFO( "NXProtocol: confirm known host." );
     break;

  case 202:
    connection_state = NX_Connected;
    NX_LOG_LOGINFO( "NXProtocol: connected." );
    break;



  case 100: // FIXME: maybe 100 is not a valid code
    connection_state = NX_SendingVersion;
    pSession->GetParameters().SetString( NX_ServerVersion, NXString );
    NX_LOG_LOGINFO( "NXProtocol: set server version to '" + NXString + "'." );
    NX_LOG_LOGINFO( "NXProtocol: sending version." );
    break;

  case 134:
    connection_state = NX_VersionAccepted;
    NX_LOG_LOGINFO( "NXProtocol: version accepted." );
    break;

  case 101:
    connection_state = NX_SendingAuth;
    break;

  case 105:
    break;

  case 102:
  case 109:
    connection_state = NX_Authenticating;
    NX_LOG_LOGINFO( "NXProtocol: authenticating..." );
    break;

  case 103:
    connection_state = NX_Authenticated;
    NX_LOG_LOGINFO( "NXProtocol: authenticated." );
    break;

  case 106:
    connection_state = NX_SendingParameters;
    break;

  case 710:
    connection_state = NX_ServerParameters;
    NX_LOG_LOGINFO( "NXProtocol: session accepted (proxy needed)." );
    pSession->NeedProxy( true );
    break;

  case 715:
    connection_state = NX_SessionAccepted;
    NX_LOG_LOGINFO( "NXProtocol: session accepted (proxy not needed)." );
    pSession->NeedProxy( false );
    break;

  case 999:
    connection_state = NX_ConnectionEnd;
    NX_LOG_LOGINFO( "NXProtocol: connection closed." );
    break;

  default:
    stringstream ss;
    ss << "NXProtocol: skip NX code '" << NXCode << "'" ;
    NX_LOG_LOGDEBUG(ss.str());
    break;
  }
  pSession->SetState( connection_state );
}

bool Protocol::AdvanceConnection( Session *pSession, Transport *pTransport )
{
  switch( pSession->GetState() )
  {
  case NX_SendingVersion:
    if (SendHello( pSession, pTransport ))
		return SendSetModes(pSession, pTransport);
	else return false;
    break;

  case NX_ConfirmAddHost:
    return SendConfirmation(pSession, pTransport);
    break;

  case NX_VersionAccepted:
    return StartAuth( pSession, pTransport );
    break;
  case NX_SendingAuth:
    return SendUsernameAndPassword( pSession, pTransport );
    break;

  case NX_Authenticated:
    if (StartSession( pSession, pTransport )) 
	{
    return SendParameters( pSession, pTransport );
	} else {
		return false;
	}
    break;

  case NX_SendingParameters:
//    return SendParameters( pSession, pTransport );
    break;

  case NX_ServerParameters:
    if( SetParametersFromServer( pSession, pTransport )) {
       return SendBye(pSession, pTransport );
   }
    break;

  case NX_ConnectionEnd:
    if ( !CheckConnectionError( pSession, pTransport )) 
    {
       if (pSession->GetParameters().GetBool( NX_EnableEncryption, false ))
       { 
          SendSwitch(pSession, pTransport);
       }
       return true;
    } else {
       return false;
    }  
    break;

  default:
    return true;
  }
}

bool Protocol::ConnectionErrorInLine( Session* pSession, int NXCode, string NXString )
{
  bool connection_error = false;

  if( NXCode > 0 )
  {
    if( NXCode == 999 )
      pSession->SetState( NX_ConnectionEnd );

    if( NXCode >= 300 && NXCode <= 699 )
      connection_error = true;
  }


  if( !connection_error )
  {
    for( unsigned int i = 0 ; i < SSHErrorsNumber; i++ )
    {
      if( NXString.find( SSHErrors[i] ) != string::npos )
      {
        NXCode = NX_SSHError;
        connection_error = true;
        break;
      }
    }
  }

  if( connection_error  )
  {
    NX_LOG_LOGERROR( "NXProtocol: error found." );
    stringstream ss;
    ss << "NXProtocol: error code " << NXCode << " = "
         << NXString << "." << endl << flush;
    NX_LOG_LOGERROR(ss.str());
    pSession->SetException( NXCode, NXString );
  }

  return connection_error;
}


bool Protocol::CheckConnectionError( Session* pSession, Transport* pTransport )
{
  NX_LOG_LOGDEBUG( "NXProtocol: searching for error... " );

  string sBuffer = DecodeUrl( pTransport->GetBuffer() );
  StringUtilities::ReplaceAll( sBuffer, "\r", "" );
  list<string> lines;
  int NXCode = -1;
  string NXString = "";

  StringUtilities::SplitString( sBuffer, '\n', lines );

  for( list<string>::const_iterator it = lines.begin(); it != lines.end() ; ++it )
  {
    ParseLineFromServer( *it, NXCode, NXString );

    if( ConnectionErrorInLine( pSession, NXCode, NXString ) )
      return true;
  }

  NX_LOG_LOGDEBUG( "NXProtocol: error not found." );

  return false;
}

bool Protocol::SetParametersFromServer( Session* pSession, Transport* pTransport )
{
  NX_LOG_LOGDEBUG( "NXProtocol: setting parameters negotiated with server." );

  string sBuffer = DecodeUrl( pTransport->GetBuffer() );
  StringUtilities::ReplaceAll( sBuffer, "\r", "" );
  list<string> lines;
  int NXCode = 0;
  string NXString = "";
  bool sRet = false;

  StringUtilities::SplitString( sBuffer, '\n', lines );

  for( list<string>::const_iterator it = lines.begin(); it != lines.end() ; ++it )
  {
    if( ParseLineFromServer( *it, NXCode, NXString ) )
    {
      NX_LOG_LOGDEBUG( "NXProtocol: setting parameter from server" );
      if( SetParameterFromServer( pSession, NXCode, NXString ) )
        sRet = true;
    }
  }

  pSession->SetState( NX_ReadyForBye );
  NX_LOG_LOGDEBUG( "NXProtocol: parameters stored." );
  return sRet;
}

string Protocol::GetParameterFromResponse( string NXString )
{
  list<string> tokens;
  StringUtilities::SplitString( NXString, ':', tokens );
  string sRet = tokens.back();
  StringUtilities::ReplaceAll( sRet, " ", "" );
  return sRet;
}

bool Protocol::SetParameterFromServer( Session* pSession, int NXCode, string NXString )
{

if (NXCode < 700) return false;

  string sTmp = GetParameterFromResponse( NXString );


   NX_LOG_LOGDEBUG( "NXProtocol: set parameter = " + sTmp );

  switch( NXCode )
  {
  case 700:  // SessionId
    NX_LOG_LOGDEBUG("NXProtocol: set ID = " + sTmp );
    pSession->GetParameters().SetString( NX_SessionId, sTmp );
    break;
  case 701:  // Proxy Cookie
    NX_LOG_LOGDEBUG( "NXProtocol: set Cookie = " + sTmp + "." );
    pSession->GetParameters().SetString( NX_ProxyCookie, sTmp );
    break;
  case 702:  // Proxy accept connection from IP
    NX_LOG_LOGDEBUG( "NXProtocol: set Server = " + sTmp + "." );
    pSession->GetParameters().SetString( NX_ProxyServer, sTmp );
    break;
  case 703:  // Session Type
    NX_LOG_LOGDEBUG( "NXProtocol: set Session Type = " + sTmp + "." );
    pSession->GetParameters().SetString( NX_SessionType, sTmp );
    break;
  case 704:  // Not useful since proxy 1.2.2
  /*
#if defined( NX_PROTOCOL_DEBUG )
    cout << "NXProtocol: set Cache Type = " << sTmp << "." << endl << flush;
#endif
    pSession->GetParameters().SetString( NX_ProxyCacheType, sTmp );
 */
    break;
  case 705:  // Session display port
    NX_LOG_LOGDEBUG( "NXProtocol: set Port = " + sTmp + "." );
    pSession->GetParameters().SetString( NX_ProxyPort, sTmp );
    break;
  case 706:  // Agent Cookie
    NX_LOG_LOGDEBUG( "NXProtocol: set Agent Cookie = " + sTmp + "." );
    pSession->GetParameters().SetString( NX_AgentCookie, sTmp );
    break;
  case 707:  // SSL tunneling
    NX_LOG_LOGDEBUG( "NXProtocol: set SSL Tunneling = " + sTmp + "." );
    pSession->GetParameters().SetString( NX_EnableEncryption, sTmp );
    break;
  default:
    return false;
  }
  return true;
}

bool Protocol::SendHello( Session *pSession, Transport *pTransport )
{
  ParametersList& parameters = pSession->GetParameters();

  string sToSend = parameters.GetString( NX_Hello, "" );
  if( sToSend.empty() )
  {
    NX_LOG_LOGDEBUG( "NXProtocol: standard 'Hello' created " );
    sToSend = "hello NXCLIENT - Version ";
    sToSend += Protocol::GetVersion();
    parameters.SetString( NX_Hello, sToSend );
  }

  NX_LOG_LOGDEBUG( "NXProtocol: sending Hello... " );

  sToSend += string( "\n" );

  if( !pTransport->Write( sToSend ) )
  {
    NX_LOG_LOGERROR( "NXProtocol: write Hello failed!");
    return false;
  }

  NX_LOG_LOGDEBUG( "NXProtocol: send hello success!" );

  StringUtilities::ReplaceAll( sToSend, "\n", "" );
  NX_LOG_LOGDEBUG( "NXProtocol: " + sToSend );

  pSession->SetState( NX_AcceptingVersion );
  return true;
}

bool Protocol::SendConfirmation( Session *pSession, Transport *pTransport )
{
  NX_LOG_LOGDEBUG( "NXProtocol: confirming host... " );
  string sToSend( "yes\n" );

  if( !pTransport->Write( sToSend ) )
  {
    NX_LOG_LOGERROR( "NXProtocol: Confirm Host failed!" );
    return false;
  }

  NX_LOG_LOGDEBUG( "NXProtocol: Confirm Host success!" );
  pSession->SetState( NX_Connecting );
  return true;
}

bool Protocol::SendSetModes( Session *pSession, Transport *pTransport )
{
   string sToSend = "SET SHELL_MODE SHELL";

  NX_LOG_LOGDEBUG( "NXProtocol: sending Set Mode... " );

  sToSend += string( "\n" );

  if( !pTransport->Write( sToSend ) )
  {
    NX_LOG_LOGERROR( "NXProtocol: Set Mode failed!" );
    return false;
  }

  NX_LOG_LOGDEBUG( "set mode success!" );

  StringUtilities::ReplaceAll( sToSend, "\n", "" );
  NX_LOG_LOGDEBUG( "NXProtocol: " + sToSend );

  return true;
}


bool Protocol::SendBye( Session *pSession, Transport *pTransport )
{
  NX_LOG_LOGDEBUG( "NXProtocol: Sending Bye... " );
#if defined (WIN32)
  string sToSend( "bye\n" );
#else
  string sToSend( "bye\n" );
#endif
  if( !pTransport->Write( sToSend ) )
  {
    NX_LOG_LOGERROR( "NXProtcol: Send bye failed!" );
    return false;
  }

  pSession->SetState( NX_SessionAccepted );
  return true;
}

bool Protocol::SendSwitch( Session *pSession, Transport *pTransport )
{
  NX_LOG_LOGDEBUG( "NXProtocol: Sending Switch... " );
  string sTmpHost = pSession->GetParameters().GetString(NX_ProxyServer, "");
  string sTmpPort = pSession->GetParameters().GetString(NX_EncryptionPort, "");
  string sTmpCookie = pSession->GetParameters().GetString(NX_ProxyCookie, "");
  string sToSend( "NX> 299 Switching connection to: ");
	  sToSend += sTmpHost + ":" + sTmpPort + " cookie: " + sTmpCookie + "\n";

	  if( !pTransport->Write( sToSend ) )
	  {
	    NX_LOG_LOGERROR( "NXProtocol: SendSwitch failed!" );
	    return false;
	  }

	  return true;
	}

	bool Protocol::StartAuth( Session *pSession, Transport *pTransport )
	{
	  NX_LOG_LOGDEBUG( "NXProtocol: starting authentication... " );
	  string sToSend( "login\n" );

	  if( !pTransport->Write( sToSend ) )
	  {
	    NX_LOG_LOGERROR( "NXProtocol: Start Auth failed!" );
	    return false;
	  }

	  pSession->SetState( NX_WaitingForAuth );
	  return true;
	}

	bool Protocol::SendUsernameAndPassword( Session *pSession, Transport *pTransport )
	{
	  NX_LOG_LOGDEBUG( "NXProtocol: sending authentication... " );

	  const ParametersList& parameters = pSession->GetParameters();

	  string sToSend = parameters.GetString( NX_Username, "" );
	  sToSend += string( "\n" );

	  string sPassword = parameters.GetString( NX_Password, "" );
	  if( sPassword.empty() )
	  {
	    sToSend += string( "\n" );
	    sToSend += parameters.GetString( NX_PasswordMD5, "" );
	  }
	  else
	    sToSend += sPassword;

	  sToSend += string( "\n" );

	  if( !pTransport->Write( sToSend ) )
	  {
	    NX_LOG_LOGERROR( "NXProtocol: Send Auth failed!" );
	    return false;
	  }

	  pSession->SetState( NX_Authenticating );
	  return true;
	}

	bool Protocol::StartSession( Session *pSession, Transport *pTransport )
	{
	  NX_LOG_LOGDEBUG( "NXProtocol: starting session... " );

	string sToSend;
	if (pSession->GetParameters().GetString(NX_ProxySessionId, "").length() > 0)
	{
	 sToSend =  "restoresession " ;
	} else {
	 sToSend =  "startsession " ;
	}

	  if( !pTransport->Write( sToSend ) )
	  {
	    NX_LOG_LOGERROR( "NXProtocol: start session failed!" );
	    return false;
	  }

	  pSession->SetState( NX_StartingSession );
	  return true;
	}

	bool Protocol::SendParameters( Session *pSession, Transport *pTransport )
	{
	  string sToSend = "";
          sToSend += GetServerOptions( pSession->GetParameters() );

	  sToSend += string( "\n" );

	  NX_LOG_LOGDEBUG( "NXProtocol: sending parameters..." );

	  if( !pTransport->Write( sToSend ) )
	  {
	    NX_LOG_LOGERROR( "NXProtocol: sending parameters... failed!" );
	    return false;
	  }

	  StringUtilities::ReplaceAll( sToSend, "\n", "" );
	  NX_LOG_LOGDEBUG( "NXProtocol: parameters sent '" + sToSend + "'." );
	  pSession->SetState( NX_Negotiating );
	  return true;
	}

	bool Protocol::CloseConnection( Session* pSession, Transport* pTransport )
	{
	  NX_LOG_LOGDEBUG( "NXProtocol: closing connection... " );

	  string sToSend( "exit\n" );

	  if( !pTransport->Write( sToSend ) )
	  {
	     NX_LOG_LOGERROR( "NXProtocol: CloseConnection failed!" );
	    return false;
	  }

	  if( pSession->GetState() != NX_SessionAccepted && pSession->GetState() != NX_Exception )
	    pSession->SetState( NX_ConnectionEnd );

	  return true;
	}

	bool Protocol::SetStandardSessionLogPath( ParametersList& parameters )
	{
	  string log_path = "";
	  string sTmp = "";

	  sTmp = parameters.GetString( NX_SessionDirectory, "" );
	  if( sTmp.empty() )
	  {
	    if( !SetStandardSessionDirectory( parameters ) )
	    {
	      NX_LOG_LOGERROR( "NXProtocol: cannot create standard session directory." );
	      return false;
	    }
	    else
	      sTmp = parameters.GetString( NX_SessionDirectory, "" );
	  }

	  log_path += sTmp;
	  log_path += string( "/session" );

	  Utilities::ConvertSeparatorsInPath( log_path );

	  parameters.SetString( NX_ProxyLogPath, log_path );

	  return true;
	}

	bool Protocol::SetStandardProxyOptionsPath( ParametersList& parameters )
	{
	  string options_path = "";
	  string sTmp = "";

	  sTmp = parameters.GetString( NX_SessionDirectory, "" );
	  if( sTmp.empty() )
	  {
	    if( !SetStandardSessionDirectory( parameters ) )
	    {
	      NX_LOG_LOGERROR( "NXProtocol: cannot create standard session directory." );
	      return false;
	    }
	    else
	      sTmp = parameters.GetString( NX_SessionDirectory, "" );
	  }

	  options_path += sTmp;
	  options_path += string( "/options" );

	  Utilities::ConvertSeparatorsInPath( options_path );

	  parameters.SetString( NX_ProxyOptionsPath, options_path );

	  return true;
	}

	bool Protocol::SetStandardSessionDirectory( ParametersList& parameters )
	{
	  string dir_path = "";
	  string sTmp = "";

	  sTmp = parameters.GetString( NX_PersonalDirectory, "" );
	  if( sTmp.empty() )
	  {
	    NX_LOG_LOGERROR( "NXProtocol: parameter 'NX_PersonalDirectory' is empty." );
	    return false;
	  }

	  dir_path += sTmp;
	  dir_path += string( "/" );

	  sTmp = parameters.GetString( NX_ProxyMode, "" );
	  if( sTmp.empty() )
	  {
	    NX_LOG_LOGERROR( "NXProtocol: parameter 'NX_ProxyMode' is empty." );
	    return false;
	  }

	  dir_path += sTmp;
	  dir_path += string( "-" );

	  sTmp = parameters.GetString( NX_ProxySessionId, "" );
	  if( sTmp.empty() )
	  {
	    NX_LOG_LOGDEBUG( "NXProtocol: parameter 'NX_ProxySessionId' is empty." );
	    sTmp = parameters.GetString( NX_SessionId, "" );
	    if( sTmp.empty() )
	    {
	      NX_LOG_LOGERROR( "NXProtocol: parameter 'NX_SessionId' is empty." );
	      return false;
	    }
	  }

	  dir_path += sTmp;

	  Utilities::ConvertSeparatorsInPath( dir_path );

	  parameters.SetString( NX_SessionDirectory, dir_path );

	  return true;
	}

	string Protocol::GetProxyOptions( ParametersList& pl, bool force_creation )
	{
	  string sRet = "";

	  sRet = pl.GetString( NX_ProxyOptions, "" );
	  if( !force_creation && !sRet.empty() )
	  {
	    NX_LOG_LOGDEBUG( "NXProtocol: getting preset options for proxy connection." );

	    NX_LOG_LOGDEBUG( "NXProtocol: options for proxy = '" + sRet + "'." );
	    return sRet;
	  }
	  NX_LOG_LOGDEBUG( "NXProtocol: creating options for proxy connection." );

	  string sTmp = GetOptions( pl, ",", NX_Proxy_FirstParameter, NX_Proxy_LastParameter, false );

	  if( !sTmp.empty() )
	  {
	    sRet = "nx,";
	    sRet += sTmp;
	  }
	  else
	    sRet = "nx";

	  NX_LOG_LOGDEBUG( "NXProtocol: options for proxy = '" + sRet + "'." );

	  return sRet;
	}

	bool Protocol::ParseLineFromProxy( Session* pSession, const string& NXResponse )
	{
	  string ProxyErrorToken = "Error: ";
	  int iErrorPos = NXResponse.find( ProxyErrorToken );
	  if( iErrorPos != string::npos )
	  {
	    string NXError = NXResponse.substr( iErrorPos + ProxyErrorToken.size(), NXResponse.size() -1 );
	    StringUtilities::ReplaceAll( NXError, "\n", " " );
	    StringUtilities::ReplaceAll( NXError, "\r", " " );
	    NX_LOG_LOGERROR( "NXProtocol: proxy error '" + NXError + "' found." );
	    pSession->SetException( NX_ProxyError, NXError );
	    return false;
	  }

	  string ProxyInfoToken = "Info: ";
	  int iInfoPos = NXResponse.find( ProxyInfoToken );
	  if( iInfoPos != string::npos )
	  {
	    string NXInfo = NXResponse.substr( iInfoPos + ProxyInfoToken.size(), NXResponse.size() -1 );
	    StringUtilities::ReplaceAll( NXInfo, "\n", " " );
	    StringUtilities::ReplaceAll( NXInfo, "\r", " " );
	    NX_LOG_LOGDEBUG( "NXProtocol: " + NXInfo );
	  }
	  else
	    NX_LOG_LOGDEBUG( "NXProtocol: " + NXResponse);

	  if( NXResponse.find( "Established X server connection" ) != string::npos )
	    pSession->SetState( NX_ProxyConnected );


	  return true;
	}

	bool Protocol::ParseResponseFromProxy( Session* pSession, const string& NXResponse )
	{
	  if( !NXResponse.empty() )
	  {
	    list<string> lines;
	    StringUtilities::SplitString( NXResponse, '\n', lines );

	    for( list<string>::const_iterator it = lines.begin(); it != lines.end(); ++it )
	    {
	      if( !ParseLineFromProxy( pSession, *it ) )
		return false;
	    }
	  }

	  return true;
	}

	int Protocol::CheckProxyStatus( const string& buffer, Session* pSession, Proxy* pProxy )
	{
	  int connection_state = pSession->GetState();

	  if( connection_state == NX_Exception || connection_state == NX_ProxyConnected )
	    return connection_state;  // nothing to do...

	  if( buffer.empty() )
	    return connection_state;

	  pSession->SetState( NX_ProxyNegotiating );

	  ParseResponseFromProxy( pSession, buffer );

	  return pSession->GetState();
	}

	} /* NX */

