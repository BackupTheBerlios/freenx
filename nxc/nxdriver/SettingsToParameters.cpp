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


#include "SettingsToParameters.h"
#include "SessionInterface.h"
#include "StringUtilities.h"
#include "Utilities.h"
#include "Logger.h"


SettingsToParameters::SettingsToParameters( NXSettings* p_sets, NXSessionPtr p_session )
{
  sets = p_sets;
  pSession = p_session;
}

SettingsToParameters::~SettingsToParameters()
{}

void SettingsToParameters::SetAuth()
{
  string sPrevGroup = sets->GetGroup();
  sets->SetGroup( "Login" );

  string sTmp = sets->GetString( "User", "" );
  NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_Username' to ' " + sTmp + "'.");
  SetStringParameter( pSession, NX_Username, sTmp.c_str() );

  sTmp = sets->GetString( "Password", "" );
  NX_LOG_LOGDEBUG("SettingsToParameters : setting 'NX_PasswordMD5' to 'xxxxx'.");
  SetStringParameter( pSession, NX_PasswordMD5, sTmp.c_str() );
  sTmp = sets->GetString( "ClearPassword", "" );
  SetStringParameter( pSession, NX_Password, sTmp.c_str() );
  sets->SetGroup( sPrevGroup );
}

void SettingsToParameters::SetResolution()
{
  string sPrevGroup = sets->GetGroup();

  sets->SetGroup( "General" );

  string resolution = "";

  string sTmp = StringUtilities::GetLowerString( sets->GetString( "Resolution", "available" ) );

  int w = 800, h = 600;

  if( sTmp == "available" )
  {
    resolution = sets->GetString( "Available area", "" );
    if( resolution.empty() )
    {
      NX_LOG_LOGERROR( "Info: available area info is not present... fullscreen set." );
      resolution = "fullscreen";
    }
  }
  else if( sTmp == "fullscreen" )
  {
     resolution = sTmp;
  }
  else
  {
    if( sTmp == "800x600")
    {
      w = 800;
      h = 600;
    }
    else if( sTmp == "640x480")
    {
      w = 640;
      h = 480;
    }
    else if( sTmp == "1024x768")
    {
      w = 1024;
      h = 768;
    }
    else
    {
      w = sets->GetNumber( "Resolution width", 800 );
      h = sets->GetNumber( "Resolution height", 600 );
    }

#ifndef WIN32
    int offsetW = 0;
    int offsetH = 0;
    resolution = StringUtilities::NumberToString( w ) + "x" +
                 StringUtilities::NumberToString( h ) + "+" +
                 StringUtilities::NumberToString( offsetW ) + "+" +
                 StringUtilities::NumberToString( offsetH );
#else
    resolution = StringUtilities::NumberToString( w ) + "x" +
                 StringUtilities::NumberToString( h );
#endif
  }

#if defined( WIN32 )
// FIXME : use resolution for XServer and pass fullscreen to server
  resolution = "fullscreen";
#endif

  NX_LOG_LOGDEBUG("Info: setting 'NX_DesktopGeometry' to '" + resolution + "'.");
  SetStringParameter( pSession, NX_DesktopGeometry, resolution.c_str() );

  sets->SetGroup( sPrevGroup );
}

string GetExstensionKeyboard( int item )
{
  switch(item)
  {
  case 0: return string("it");
  case 1: return string("de");
  case 2: return string("af");
  case 3: return string("sq");
  case 4: return string("ar");
  case 5: return string("eu");
  case 6: return string("be");
  case 7: return string("bg");
  case 8: return string("ca");
  case 9: return string("ch");
  case 10:return string("hr");
  case 11:return string("cs");
  case 12:return string("da");
  case 13:return string("nl");
  case 14:return string("us");
  case 15:return string("et");
  case 16:return string("fo");
  case 17:return string("fa");
  case 18:return string("fi");
  case 19:return string("fr");
  case 20:return string("el");
  case 21:return string("he");
  case 22:return string("hu");
  case 23:return string("is");
  case 24:return string("in");
  case 25:return string("jp");
  case 26:return string("ko");
  case 27:return string("lv");
  case 28:return string("lt");
  case 29:return string("no");
  case 30:return string("pl");
  case 31:return string("pt");
  case 32:return string("ro");
  case 33:return string("ru");
  case 34:return string("sk");
  case 35:return string("sl");
  case 36:return string("es");
  case 37:return string("sw");
  case 38:return string("th");
  case 39:return string("tr");
  case 40:return string("vi");
  case 41:return string("uk");
  default:
    return string("");
  };
}

void SettingsToParameters::SetSessionType()
{
  // Default
  string sessionType = "unix-application";
  string applicationPath = "xterm";
  string agentServer = "";
  string agentUser = "";
  string agentPassword = "";
  bool useVirtualDesktop = true;

  string sPrevGroup = sets->GetGroup();

  sets->SetGroup( "General" );

  string sTmp = StringUtilities::GetLowerString( sets->GetString( "Session", "unix" ) );
  bool bTmp = false;

  if( sTmp == "unix")
    SetSessionTypeUnix( sessionType, applicationPath, agentServer, agentUser, agentPassword, useVirtualDesktop );
  else if( sTmp == "windows" )
    SetSessionTypeWindows( sessionType, applicationPath, agentServer, agentUser, agentPassword, useVirtualDesktop );
  else if( sTmp == "vnc" )
    SetSessionTypeVnc( sessionType, applicationPath, agentServer, agentUser, agentPassword, useVirtualDesktop );

  NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_SessionType' to '" + sessionType + "'.");
  SetStringParameter( pSession, NX_SessionType, sessionType.c_str() );

  if( !applicationPath.empty() )
  {
    NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_ApplicationPath' to '" + applicationPath + "'." );
    SetStringParameter( pSession, NX_ApplicationPath, applicationPath.c_str() );
  }

  if( !agentServer.empty() )
  {
    NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_AgentServer' to ''" + agentServer + "'." );
    SetStringParameter( pSession, NX_AgentServer, agentServer.c_str() );
  }

  if( !agentUser.empty() )
  {
    NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_AgentUser' to ''" + agentUser + "'.");
    SetStringParameter( pSession, NX_AgentUser, agentUser.c_str() );
  }

  if( !agentPassword.empty() )
  {
    NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_AgentPassword'.");
    SetStringParameter( pSession, NX_AgentPassword, agentPassword.c_str() );
  }

  sets->SetGroup( "Environment" );

  if( sets->GetBool( "Current keyboard", true ) )
    sTmp = NX::Utilities::GetCurrentKeyboardLayout();
  else
    sTmp = GetExstensionKeyboard( sets->GetNumber( "Other keyboard", 0 ) );

  if( sessionType == "windows" )
    sTmp = NX::Utilities::ConvertKeyboardLayoutToHex( sTmp );

  NX_LOG_LOGDEBUG(  "SettingsToParameters : setting 'NX_Keyboard' to '%s'." + sTmp);
  SetStringParameter( pSession, NX_Keyboard, sTmp.c_str() );

#if defined( WIN32 )
  if( (applicationPath.length() > 0 ) && !useVirtualDesktop )
  {
    NX_LOG_LOGDEBUG("SettingsToParameters : setting 'NX_Keymap' to '%s'." + sTmp);
    SetStringParameter( pSession, NX_Keymap, sTmp.c_str() );
  }
#endif

  sets->SetGroup( "Services" );
  // MEDIA and SAMBA: not implemented
  SetStringParameter( pSession, NX_EnableMedia, "0" );
  SetStringParameter( pSession, NX_EnableSamba, "0" );
  sets->SetGroup( sPrevGroup );
}

void SettingsToParameters::SetSessionTypeUnix( string& sessionType,
                                               string& applicationPath,
                                               string& agentServer,
                                               string& agentUser,
                                               string& agentPassword,
                                               bool &useVirtualDesktop )
{
  sessionType = "unix-application";
  applicationPath = "xterm";
  agentServer = "";
  agentUser = "";
  agentPassword = "";
  useVirtualDesktop = true;

  string sPrevGroup = sets->GetGroup();
  sets->SetGroup( "General" );

  string sTmp = StringUtilities::GetLowerString( sets->GetString( "Desktop", "KDE" ) );
  sessionType = "unix-" + sTmp;
  applicationPath = "";

  if( sessionType == "unix-custom" || sessionType == "unix-console" )
  {
    sessionType = "unix-application";

    sTmp = sets->GetString( "Command line" , "" );
    if( sTmp.empty() || sets->GetBool( "Only console" , true ) )
      applicationPath = "xterm";
    else
      applicationPath = sTmp;

    if( sets->GetBool( "Run default script", false ) )
    {
      sessionType = "unix-default";
      applicationPath = "";
    }

    if( sets->GetBool( "Virtual desktop", false ) )
    {
      sTmp = "1";
      SetResolution();
    }
    else
    {
      sTmp = "0";

      useVirtualDesktop = false;
      NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_EnableRender' to '" + sets->GetString("Use render", "true") + "'.");
      SetBoolParameter( pSession, NX_EnableRender, sets->GetBool( "Use render", true ) );

      NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_EnableTaint' to '" + sets->GetString("Use taint", "true") + "'.");
      SetBoolParameter( pSession, NX_EnableTaint, sets->GetBool( "Use taint", true ) );
    }

    NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_EnableVirtualDesktop' to ''" + sTmp + "'.");
    SetStringParameter( pSession, NX_EnableVirtualDesktop, sTmp.c_str() );
  }
  else
    SetResolution();

  sets->SetGroup( "Advanced" );

  if( !sets->GetBool( "Use default image encoding", true ) )
  {
    sets->SetGroup( "Images" );

    int iTmp = sets->GetNumber("Image Compression Type", 0 );
    switch( iTmp )
    {
    case 0:
      sTmp = "-1";
      NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_ImageCompressionType' to ''" + sTmp + "'.");
      SetStringParameter( pSession, NX_ImageCompressionType, sTmp.c_str() );
      NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_ImageCompressionLevel' to ''" + sTmp + "'.");
      SetStringParameter( pSession, NX_ImageCompressionLevel, sTmp.c_str() );
      break;

    case 1:
      sTmp = "1";
      NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_ImageCompressionType' to ''" + sTmp + "'." );
      SetStringParameter( pSession, NX_ImageCompressionType, sTmp.c_str() );

      sTmp = StringUtilities::NumberToString( sets->GetNumber( "JPEG Quality", 6 ) );
      NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_ImageCompressionLevel' to ''" + sTmp + "'.");
      SetStringParameter( pSession, NX_ImageCompressionLevel, sTmp.c_str() );
      break;

    case 2:
      sTmp = "0";
      NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_ImageCompressionType' to ''" + sTmp + "'.");
      SetStringParameter( pSession, NX_ImageCompressionType, sTmp.c_str() );
      break;
    };

    if( sets->GetBool("Use PNG Compression", false ) && iTmp == 2 )
    {
      sTmp = "2";
      NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_ImageCompressionType' to ''" + sTmp + "'.");
      SetStringParameter( pSession, NX_ImageCompressionType, sTmp.c_str() );
    }
  }

  sets->SetGroup( sPrevGroup );
}

void SettingsToParameters::SetSessionTypeWindows( string& sessionType,
                                                  string& applicationPath,
                                                  string& agentServer,
                                                  string& agentUser,
                                                  string& agentPassword,
                                                  bool &useVirtualDesktop )

{
  sessionType = "windows";
  applicationPath = "";
  agentServer = "";
  agentUser = "";
  agentPassword = "";
  useVirtualDesktop = true;

  string sPrevGroup = sets->GetGroup();

  sets->SetGroup( "Windows Session" );
  if( sets->GetBool( "Run application", false ) )
    applicationPath = sets->GetString( "Application" ,"" );

  sets->SetGroup( "Advanced" );

  if( !sets->GetBool( "Use default image encoding", true ) )
  {
    int iTmp = 0;

    sets->SetGroup( "Images" );
    if( sets->GetBool( "RDP optimization for low-bandwidth link", false ) )
    {
      NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_EnableLowBandwidth' to '1'.");
      SetBoolParameter( pSession, NX_EnableLowBandwidth, 1 );
    }

    iTmp = sets->GetNumber( "Windows images compression", 0 );
    if( iTmp != 0 )
    {
      char buffer[100];
      sprintf(buffer, "SettingsToParameters : setting 'NX_ImageCompressionType' to '%i'.", iTmp);
      NX_LOG_LOGDEBUG(buffer);
      SetNumberParameter( pSession, NX_ImageCompressionType, iTmp );
    }
  }

  sets->SetGroup( "Windows Session" );
  agentServer = sets->GetString( "Server", "" );

  switch( sets->GetNumber( "Authentication", 1 ) )
  {
  case 0:
    agentUser = sets->GetString( "User", "" );
    agentPassword = "";
    break;
  default:
    agentUser = "";
    agentPassword = "";
  }

  SetResolution();

  sets->SetGroup( sPrevGroup );
}

void SettingsToParameters::SetSessionTypeVnc( string& sessionType,
                                              string& applicationPath,
                                              string& agentServer,
                                              string& agentUser,
                                              string& agentPassword,
                                              bool &useVirtualDesktop )
{
  sessionType = "vnc";
  applicationPath = "";
  agentServer = "";
  agentUser = "";
  agentPassword = "";
  useVirtualDesktop = true;

  string sPrevGroup = sets->GetGroup();

  sets->SetGroup( "Advanced" );

  if( !sets->GetBool( "Use default image encoding", true ) )
  {
    string sTmp = "";
    sets->SetGroup( "Images" );
    switch( sets->GetNumber( "VNC images compression", 0 ) )
    {
    case 0:
      sTmp = "1";
      break;
    case 1:
      sTmp = "2";
      break;
    }

    if( sets->GetBool( "Image JPEG Encoding", false ) )
      sTmp = "3";

    NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_ImageCompressionType' to ''" + sTmp + "'." );
    SetStringParameter( pSession, NX_ImageCompressionType, sTmp.c_str() );
  }

  sets->SetGroup( "VNC Session" );
  agentServer = sets->GetString( "Server", "" );
  agentServer += sets->GetString( "Display", ":1" );
  agentPassword = sets->GetString( "Password", "" );

  SetResolution();

  sets->SetGroup( sPrevGroup );
}

void SettingsToParameters::Convert()
{
  string sTmp = "";
  bool bTmp = false;
  int iTmp = 0;

  string sPrevGroup = sets->GetGroup();

  // FIXME sTmp = sets->fileName( true );
  sTmp = sets->GetConfigName();
  StringUtilities::ReplaceAll( sTmp, ".conf", "" );
  NX_LOG_LOGDEBUG("SettingsToParameters : setting 'NX_SessionName' to ''" + sTmp + "'.");
  SetStringParameter( pSession, NX_SessionName, sTmp.c_str() );

  SetAuth();

  sets->SetGroup( "General" );

  sTmp = sets->GetString( "Server host", "" );
  NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_HostName' to ''" + sTmp + "'." );
  SetStringParameter( pSession, NX_HostName, sTmp.c_str() );

  iTmp = sets->GetNumber( "Server port", 22 );
  char buffer[100];
  sprintf(buffer, "SettingsToParameters : setting 'NX_HostPort' to '%d'.", iTmp) ;
  NX_LOG_LOGDEBUG(buffer);
  SetNumberParameter( pSession, NX_HostPort, iTmp );

  sTmp = sets->GetString( "Link speed", "adsl" );
  NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_LinkSpeed' to ''" + sTmp + "'.");
  SetStringParameter( pSession, NX_LinkSpeed, sTmp.c_str() );
  
  SetSessionType();

  sets->SetGroup( "Advanced" );

  NX_LOG_LOGDEBUG("SettingsToParameters : setting 'NX_EnableTcpNoDelay' to ' not " + sets->GetString("Disable TCP no-delay", "true") + "'.");
  SetBoolParameter( pSession, NX_EnableTcpNoDelay,
    !sets->GetBool( "Disable TCP no-delay", true ) );

  NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_EnableStreamCompression' to  not " + sets->GetString("Disable ZLIB stream compression", "true") + "'.");
  SetBoolParameter( pSession, NX_EnableStreamCompression,
    !sets->GetBool( "Disable ZLIB stream compression", true ) );

  bTmp = sets->GetBool( "Enable SSL encryption", false );
  NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_EnableEncryption' to '" + sets->GetString("Enable SSL Encryption", "false") +   "'.");
  SetBoolParameter( pSession, NX_EnableEncryption, bTmp );
  if( bTmp )
  {
    // FIXME int iPort
//    int iPort = 12345;
    int iPort = 33057;
    NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_EncryptionPort' to '33057'.");
    SetNumberParameter( pSession, NX_EncryptionPort, iPort );
  }

  bTmp = sets->GetBool( "Restore cache", true );
  NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_RestoreCache' to '" + sets->GetString("Restore cache", "true") +  "'.");
  SetBoolParameter( pSession, NX_RestoreCache, bTmp );

  // FIXME: cache can be in kbytes also...
  sTmp = StringUtilities::NumberToString( sets->GetNumber( "Cache size", 4 ) ) + "m";
  NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_CacheSizeInMemory' to ''" + sTmp + "'." );
  SetStringParameter( pSession, NX_CacheSizeInMemory, sTmp.c_str() );

  sTmp = StringUtilities::NumberToString( sets->GetNumber( "Cache size on disk", 32 ) ) + "m";
  NX_LOG_LOGDEBUG( "SettingsToParameters : setting 'NX_CacheSizeOnDisk' to ''" + sTmp + "'.");
  SetStringParameter( pSession, NX_CacheSizeOnDisk, sTmp.c_str() );


  sets->SetGroup( sPrevGroup );
}


