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

#ifndef WIN32
using namespace std;
#include <unistd.h>
#endif

#include "Utilities.h"
#include "NXDriver.h"
#include "Logger.h"
#include "ConsoleCallback.h"

static void printUsage(const char* progname) {
  fprintf( stdout, "Usage: %s 'session file path' [-u user] [-p password] [-i] [-q] [l loglevel]\n-i : Interactive.  It will prompt for user and password if they are not set.\n-q: quiet.  Suppress the messages that are printed to STDOUT and STDERR.\n-l: loglevel is a string that includes a combination of the following:\n   c : log to console\n   f : log to file\n   e : log errors\n   d : log debug info\n   i : log info\n   example:   -l cie - log errors and info to the console\n   default is to log nothing to the console\n", progname );
  return ;
}



static bool ParseCommandLine( int argc_, char** argv_, string& file_path, string& user, string& password, bool &suppresscallback_flag)
{
  if( argc_ > 1 )
  {
    file_path = argv_[1];

    if( file_path != "--help" && file_path != "-h" )
    {
       // Look for optional parameters
       int i = 2;
       bool interactive_flag = false;
       while (i < argc_) {
          string tmp = argv_[i++];
          if (tmp == "-u") {
             if (argc_ > i) {
                user = argv_[i++];
             } else {
                printUsage(argv_[0]);
                return false;
             }
          }
          else if (tmp == "-p") {
             if (argc_ > i) {
                password = argv_[i++];
             } else {
                printUsage(argv_[0]);
                return false;
             }
          }
          else if (tmp == "-i") {
             interactive_flag = true;
          }
          else if (tmp == "-q") {
             suppresscallback_flag = true;
          }
          else if (tmp == "-l") {
             if (argc_ > i) {
                  NX_LOG_SETFLAGS(argv_[i]);
                  i++; 
             } else {
                printUsage(argv_[0]);
                return false;
             }
          } else {
             printUsage(argv_[0]);
             return false;
          }
       }
      if (interactive_flag) {
        if( user.empty() )
        {
           cout << "Login: " << flush;
           cin >> user;
        }
        if( password.empty() )
        {
           string prompt = user;
           prompt += "'s password: ";
#if defined (WIN32)
           cout << prompt << flush;
           cin >> password ;
#else
           password = getpass( prompt.c_str() );
#endif
        }
      }
      return true;
    }
  }
  printUsage(argv_[0]);
  return false;
}

static void SetConnectionError( string error )
{
  if( !error.empty() && error[(error.size()-1)] != '.' )
    error += ".";
  cerr << "Error: " << error << endl << flush;
  NX_LOG_LOGERROR(error);
}

int main( int argc, char** argv )
{

  string NX_ConfigFilePath = "";
  string NX_User = "";
  string NX_ClearPassword = "";
  bool suppresscallback_flag = false;
  if( !ParseCommandLine( argc, argv, NX_ConfigFilePath, NX_User, NX_ClearPassword, suppresscallback_flag ) )
    return 1;
  if( !NX::Utilities::FileExist( NX_ConfigFilePath ) )
  {
    SetConnectionError( NX_ConfigFilePath + " file not found" );
    return 1;
  }

  NXDriver dr;
  dr.SetConfigFilePath(NX_ConfigFilePath);
  if (!NX_User.empty())  dr.SetUserName(NX_User);
  if (!NX_ClearPassword.empty()) dr.SetClearPassword(NX_ClearPassword);
  if (!suppresscallback_flag)
     dr.SetCallback(new ConsoleCallback());
  dr.Run();
}

