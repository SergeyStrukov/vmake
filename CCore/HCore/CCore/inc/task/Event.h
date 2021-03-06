/* Event.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: HCore Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_task_Event_h
#define CCore_inc_task_Event_h

#include <CCore/inc/task/FastMutex.h>

namespace CCore {

/* classes */

class Event;

/* class Event */

class Event : public Funchor_nocopy
 {
   FastMutex mutex;
   bool flag;
   ulen wait_count;

   SlowSem sem;

  private:

   void init(bool flag);

   bool begin_wait();

   bool end_wait();

   bool begin_trigger(bool &ret);

  public:

   // constructors

   explicit Event(bool flag=false);

   explicit Event(TextLabel name,bool flag=false);

   explicit Event(const char *name) : Event(TextLabel(name)) {}

   ~Event();

   // trigger

   bool trigger();

   // wait

   bool try_wait();

   void wait();

   bool wait(MSec timeout);

   bool wait(TimeScope time_scope)
    {
     auto timeout=time_scope.get();

     if( !timeout ) return false;

     return wait(timeout);
    }

   // functions

   void trigger_void() { trigger(); }

   Function<void (void)> function_trigger() { return FunctionOf(this,&Event::trigger_void); }
 };

} // namespace CCore

#endif


