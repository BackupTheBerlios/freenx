#ifndef CALLBACKINTERFACE_H
#define CALLBACKINTERFACE_H

class CallbackInterface
{
public:
   virtual void Callback(unsigned int iMsg, void *ptr)=0;
};
#endif

