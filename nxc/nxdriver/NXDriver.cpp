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


#include "SessionInterface.h"
#include "StringUtilities.h"
#include "Utilities.h"
#include "NXSettings.h"
#include "NXConnection.h"
#include "NXRunVersion.h"
#include "NXDriver.h"
#include "CallbackMessageTypes.h"
#include "Options.h"
#include "Logger.h"

using namespace std;

#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#ifndef WIN32
#include <sys/wait.h>
#endif

#define FALSE 0
#define TRUE 1

NXDriver::NXDriver()
{
   m_message = "";
   m_restoreSessionId = "";
   m_cb = NULL;
   m_configFilePath = "";
   m_username = "";
   m_clearpassword = "";
   m_sessionName = "";
   pConnection = NULL;
   m_running = FALSE;
}

NXDriver::~NXDriver()
{
   if (pConnection) delete pConnection;
}

void NXDriver::GetLogin( NXSettings* sets )
{
  string sPrevGroup = sets->GetGroup();
  sets->SetGroup( "Login" );
  string password = sets->GetString( "Password", "" );
  string username = sets->GetString( "User", "" );
  string keyfile = sets->GetString( "KeyFile", "" );
  if (!keyfile.empty())
  {
     //override the option
     options->SetValue("NXSshKeyFileName", keyfile);
  }

  // If the username or password have been passed, then user them as override
  if (!m_username.empty()) {
     sets->SetString("User", m_username);
  }
  if (!m_clearpassword.empty()) {
     sets->SetString("ClearPassword", m_clearpassword);
  }
  sets->SetGroup( sPrevGroup );
}

void NXDriver::SetConnectionError( string error )
{
  if( !error.empty() && error[(error.size()-1)] != '.' )
    error = "Error: " + error + ".";
  cerr << error << endl << flush;
  NX_LOG_LOGERROR(error);
  if (m_cb) m_cb->Callback(CB_MSG_ERROR, (void *)error.c_str());
}

void NXDriver::SetConnectionMessage( string message )
{

  if( message != m_message )
  {
    m_message = message;
    if( message[(message.size()-1)] != '.' )
      message += ".";

    NX_LOG_LOGINFO(message);
    if (m_cb) m_cb->Callback(CB_MSG_STATUS, (void *)message.c_str());
  }
}

string NXDriver::Welcome()
{
  string welcome = "moznx - version ";
  welcome += MOZ_NX_VERSION;
  welcome += " (based on NXRUN - version ";
  welcome += NX_RUN_VERSION;
  welcome += ")";
  return welcome; 
}

void NXDriver::HandleCloseAll( int my_signal )
{
//  if( pConnection != NULL )
  if (m_running)
  {
    NX_LOG_LOGDEBUG( "Info: Closing all running processes..." );
    NX_LOG_LOGDEBUG( "Info: Session log is '" + pConnection->GetLogPath() + "'.");
    pConnection->Stop();
#ifndef WIN32
    waitpid( -1, NULL, WNOHANG);
#endif
  }
  NX_LOG_LOGINFO("Info: Shutting down");
  return;
}
int NXDriver::Run()
{

   if (m_configFilePath.length() > 0) SetConnectionMessage(m_configFilePath);
//  signal( SIGINT, &this->HandleCloseAll );
   SetConnectionMessage(Welcome());

  if( !NX::Utilities::FileExist( m_configFilePath ) )
  {
    SetConnectionError( m_configFilePath + " file not found" );
    return 1;
  }

  NXSettings sets;
  sets.SetFilePath( m_configFilePath );
  sets.SetConfigName(m_sessionName);
  if( !sets.Read() )
  {
    string msg = "Parsing error in file ";
    msg += m_configFilePath;
    SetConnectionError( msg);
    return 1;
  }

  GetLogin( &sets );

  m_running = TRUE;
  pConnection = new NXConnection();

  if( !pConnection->SetParameters( &sets ) )
  {
    SetConnectionError( pConnection->GetError() );
    return 1;
  }

  pConnection->Start();

  NX_LOG_LOGINFO("Connection Started");

  if( pConnection->ErrorOccurred() )
  {
    SetConnectionError( pConnection->GetError() );
    return 1;
  }

  int CheckTimeout = 250;
  bool error_occurred = false;
  if (m_restoreSessionId.length() > 0)
  {
     pConnection->SetSessionId(m_restoreSessionId);
  } 
  for( ;; )
  {
    pConnection->Advance( CheckTimeout );

    if( pConnection->ErrorOccurred() )
    {
      SetConnectionError( pConnection->GetError() );
      error_occurred = true;
      break;
    }

    string sState = pConnection->GetState();
    if( !sState.empty() )
      SetConnectionMessage( sState);

    if( pConnection->SessionAccepted() )
    {
       if (m_cb) m_cb->Callback(CB_MSG_SESSION, (void *)pConnection->GetSessionId().c_str());
    }
    if( pConnection->Accepted() )
      break;

  }

  if( error_occurred )
  {
    string msg = "Error occurred. For more info check '";
    msg += pConnection->GetLogPath();
    msg += "'";
    SetConnectionMessage( msg );
    return 1;
  }
  else
  {
    SetConnectionMessage( "Session accepted. Please wait for NX window...");
    m_running = FALSE;
    if (m_cb) m_cb->Callback(CB_MSG_COMPLETE, (void *)NULL);
    return 0;
  }
}


