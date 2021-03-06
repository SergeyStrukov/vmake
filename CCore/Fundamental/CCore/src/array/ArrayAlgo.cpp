/* ArrayAlgo.cpp */
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

#include <CCore/inc/array/ArrayAlgo.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* GuardArrayOverflow() */

void GuardArrayOverflow(ulen len,ulen maxlen,ulen extra_len)
 {
  Printf(Exception,"CCore::GuardArrayOverflow(len=#;,maxlen=#;,extra_len=#;) : cannot extend array",len,maxlen,extra_len);
 }

} // namespace CCore


