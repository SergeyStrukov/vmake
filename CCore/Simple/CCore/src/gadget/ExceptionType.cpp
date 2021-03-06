/* ExceptionType.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Simple Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/gadget/ExceptionType.h>

namespace CCore {

/* enum ExceptionType */

const char * GetTextDesc(ExceptionType ex)
 {
  switch( ex )
    {
     case NoException : return "Error : ";
     case Exception   : return "Exception : ";

     default: return "??? : ";
    }
 }

} // namespace CCore


