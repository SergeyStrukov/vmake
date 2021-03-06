/* Len.cpp */
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

#include <CCore/inc/gadget/Len.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* guard functions */

void GuardLenAddOverflow(ulen len,ulen extra_len)
 {
  Printf(Exception,"CCore::LenAdd(len=#;,extra_len=#;) : overflow",len,extra_len);
 }

void GuardLenFailed(ulen len,ulen maxlen)
 {
  Printf(Exception,"CCore::GuardLen(len=#;,maxlen=#;) : len>max_len",len,maxlen);
 }

void GuardLenOfOverflow(ulen count,ulen size_of,ulen extra)
 {
  Printf(Exception,"CCore::LenOf(count=#;,size_of=#;,extra=#;) : overflow",count,size_of,extra);
 }

void GuardIndexOutOfRange(ulen index,ulen len)
 {
  Printf(Exception,"CCore::GuardIndex(index=#;,len=#;) : out of range",index,len);
 }

void GuardIsEmpty()
 {
  Printf(Exception,"CCore::GuardIsEmpty(...) : container is empty");
 }

} // namespace CCore


