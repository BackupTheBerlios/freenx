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

#ifndef NX_LOGGER_H
#define NX_LOGGER_H

#include <iostream>
#include <fstream>
#include <string>

namespace NX
{

class Logger 
{
private:
  unsigned int m_loglevel;
  bool m_logtofile;
  string m_logfilename;
public:
  enum NXLOGFLAGS { NX_LOGNONE = 0x0000, NX_LOGINFO = 0x0001 , NX_LOGDEBUG = 0x0002 , NX_LOGERR = 0x0004 };
  static Logger &GetLogger() { static Logger l; return l;}
  ~Logger() {}
  void SetLogFilePath(const string &logfilepath) { m_logfilename = logfilepath; }
  void SetLogLevelFlag( const unsigned int loglevel ) { m_loglevel |= loglevel;}
  void SetLogLevelFlags(const string &loglevel) { 
					char logflags[6];
					strncpy(logflags, loglevel.c_str(), 6); 
					int i = 0;
					while ((i < 6) && (logflags[i] > 0)) {
						switch (logflags[i])
						{ 
						case 'c': m_logtofile = false; 
							break; 
						case 'f': 
							m_logtofile = true; 
							break; 
						case 'i': SetLogLevelFlag(NX_LOGINFO); 
							break;
						case 'd': SetLogLevelFlag(NX_LOGDEBUG); 
							break; 
						case 'e': SetLogLevelFlag(NX_LOGERR); 
							break; 
						} 
					i++; 
					}  
  } 
  void LogToFile(const bool logtofile) { m_logtofile = logtofile;}
  unsigned int GetLogLevelFlags() { return m_loglevel;}
  void WriteLog(unsigned int loglevel, const string &logstring) {
    string logtype= "UNDEFINED: ";
	  if (!(m_loglevel & loglevel)) return;
    if (isLogError(loglevel)) { logtype = "ERROR: ";}
    if (isLogDebug(loglevel)) { logtype = "DEBUG: ";}
    if (isLogInfo(loglevel)) { logtype = "INFO: ";}
    if (!m_logtofile) {
       if (isLogError(loglevel)) {
           cerr << logtype << logstring << endl;
       } else {

           cout << logtype << logstring << endl;
       } 
     } else {
      ofstream os;
      os.open(m_logfilename.c_str(), ios::app);
      os << logtype << logstring << endl;
      os.close();
    }

  }
private:
  Logger() {m_loglevel = NX_LOGNONE; m_logtofile = false; m_logfilename = "nxcompsh.log"; }
  bool isLogError(unsigned int flag) { return (flag & NX_LOGERR) > 0;}
  bool isLogDebug(unsigned int flag) { return (flag & NX_LOGDEBUG) > 0;}
  bool isLogInfo(unsigned int flag) { return (flag & NX_LOGINFO) > 0;}
};

} /* NX */

#ifdef NX_ENABLE_LOGGING
   #define NX_LOG_SETFLAGS(flags) NX::Logger::GetLogger().SetLogLevelFlags(flags)
   #define NX_LOG_LOGERROR(logstring) NX::Logger::GetLogger().WriteLog(NX::Logger::NX_LOGERR, logstring)
   #define NX_LOG_LOGDEBUG(logstring) NX::Logger::GetLogger().WriteLog(NX::Logger::NX_LOGDEBUG, logstring)
   #define NX_LOG_LOGINFO(logstring) NX::Logger::GetLogger().WriteLog(NX::Logger::NX_LOGINFO, logstring)
#else
   #define NX_LOG_SETFLAGS(flags)
   #define NX_LOG_LOGERROR(logstring)
   #define NX_LOG_LOGDEBUG(logstring)
   #define NX_LOG_LOGINFO(logstring)
#endif

#endif /* NX_LOGGER_H */

