#ifndef NXDRIVER_H
#define NXDRIVER_H

#include "NXSettings.h"
#include "CallbackInterface.h"
#include "NXConnection.h"

class NXDriver
{
public:
   NXDriver();
   ~NXDriver();
   void SetCallback(CallbackInterface *cb) { m_cb = cb; }
   void SetConfigFilePath(string path) { m_configFilePath = path; }
   void SetRestoreSessionId(string sessionid) { m_restoreSessionId = sessionid; }
   void SetSessionName(string sessionname) { m_sessionName = sessionname; }
   void SetUserName(string username) {m_username = username;}
   void SetClearPassword(string clearpassword) {m_clearpassword = clearpassword;}
    int Run();  
    void HandleCloseAll(int my_signal);
private:
   void GetLogin(NXSettings *sets);
   virtual void SetConnectionError(string error);
   virtual void SetConnectionMessage(string message);
    string Welcome();

   // members
   string m_message;
   CallbackInterface *m_cb;
   string m_configFilePath;
   string m_restoreSessionId;
   string m_sessionName;
   string m_username;
   string m_clearpassword;
   NXConnection *pConnection;
   unsigned int m_running;
};   

#endif /* NXDRIVER_H */
