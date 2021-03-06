/* DDLErrorMsg.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Applied
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2021 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_ddl_DDLErrorMsg_h
#define CCore_inc_ddl_DDLErrorMsg_h

#include <CCore/inc/Printf.h>

namespace CCore {
namespace DDL {

/* consts */

inline constexpr unsigned ErrorMsgCap = 100 ;

/* classes */

class ErrorMsg;

/* class ErrorMsg */

class ErrorMsg : NoCopy
 {
   PrintBase &msg;

   unsigned error_cap;

  public:

   explicit ErrorMsg(PrintBase &msg_) : msg(msg_) { reset(); }

   bool operator + () const { return error_cap!=ErrorMsgCap; }

   bool operator ! () const { return error_cap==ErrorMsgCap; }

   bool tooMany() const { return error_cap==0; }

   PrintBase & getMsg() { return msg; }

   void reset()
    {
     error_cap=ErrorMsgCap;
    }

   template <class ... SS>
   void operator () (const char *format,SS && ... ss)
    {
     if( error_cap )
       {
        error_cap--;

        Printf(msg,format, std::forward<SS>(ss)... );

        Putch(msg,'\n');
       }
    }
 };

} // namespace DDL
} // namespace CCore

#endif



