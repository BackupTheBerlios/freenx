/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * ***** END LICENSE BLOCK ***** */

#include <iostream>

#include "ConsoleCallback.h"
#include "CallbackMessageTypes.h"

using namespace std;

void ConsoleCallback::Callback(unsigned int msg, void *ptr)
{
   switch (msg)
   {
      case CB_MSG_STATUS:
         cout << "I> " << ((const char *)ptr) << endl << flush;
      break;
      case CB_MSG_SESSION:
         cout << "S> " << ((const char *)ptr) << endl << flush;
		 break;
      case CB_MSG_COMPLETE:
         cout << "C> Complete" << endl << flush;
      break;
      case CB_MSG_ERROR:
         cerr << "E> " << ((const char *)ptr) << endl << flush;
      break;
   } 
}

