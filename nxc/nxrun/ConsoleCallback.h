#ifndef CONSOLECALLBACK_H
#define CONSOLECALLBACK_H

#include "CallbackInterface.h"

class ConsoleCallback: public CallbackInterface
{
public:
   virtual void Callback(unsigned int iMsg, void *ptr);
};
#endif

