/* NoMutex.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_task_NoMutex_h
#define CCore_inc_task_NoMutex_h

#include <CCore/inc/TextLabel.h>

namespace CCore {

/* classes */

class NoMutex;

/* class NoMutex */

class NoMutex : NoCopy
 {
  public:

   NoMutex() {}

   explicit NoMutex(TextLabel) {}

   ~NoMutex() {}

   void lock() {}

   void unlock() {}

   struct Lock : NoCopy
    {
     explicit Lock(NoMutex &) {}
    };
 };

} // namespace CCore

#endif


