/* RangeDel.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Simple
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_RangeDel_h
#define CCore_inc_RangeDel_h

#include <CCore/inc/Swap.h>

namespace CCore {

/* functions */

bool RangeCopyDel(NothrowCopyableType auto list[],ulen len,ulen ind)
 {
  if( ind>=len ) return false;

  for(len--; ind<len ;ind++) list[ind]=list[ind+1];

  return true;
 }

bool RangeCopyDel(PtrLen<NothrowCopyableType auto> range,ulen ind)
 {
  return RangeCopyDel(range.ptr,range.len,ind);
 }

bool RangeSwapDel(SwappableType auto list[],ulen len,ulen ind)
 {
  if( ind>=len ) return false;

  for(len--; ind<len ;ind++) Swap(list[ind],list[ind+1]);

  return true;
 }

bool RangeSwapDel(PtrLen<SwappableType auto> range,ulen ind)
 {
  return RangeSwapDel(range.ptr,range.len,ind);
 }

void RangeCopyDel_guarded(NothrowCopyableType auto list[],ulen len,ulen ind)
 {
  GuardIndex(ind,len);

  for(len--; ind<len ;ind++) list[ind]=list[ind+1];
 }

void RangeCopyDel_guarded(PtrLen<NothrowCopyableType auto> range,ulen ind)
 {
  RangeCopyDel_guarded(range.ptr,range.len,ind);
 }

void RangeSwapDel_guarded(SwappableType auto list[],ulen len,ulen ind)
 {
  GuardIndex(ind,len);

  for(len--; ind<len ;ind++) Swap(list[ind],list[ind+1]);
 }

void RangeSwapDel_guarded(PtrLen<SwappableType auto> range,ulen ind)
 {
  RangeSwapDel_guarded(range.ptr,range.len,ind);
 }

/* GuardIndexCount() */

bool GuardIndexCount(ulen ind,ulen count,ulen len);

/* range functions */

ulen RangeCopyDel(NothrowCopyableType auto list[],ulen len,ulen ind,ulen count)
 {
  if( ind>=len || !count ) return 0;

  ulen rest=len-ind;

  if( count<rest )
    {
     for(rest-=count; rest ;rest--,ind++) list[ind]=list[ind+count];

     return count;
    }
  else
    {
     return rest;
    }
 }

ulen RangeCopyDel(PtrLen<NothrowCopyableType auto> range,ulen ind,ulen count)
 {
  return RangeCopyDel(range.ptr,range.len,ind,count);
 }

ulen RangeSwapDel(SwappableType auto list[],ulen len,ulen ind,ulen count)
 {
  if( ind>=len || !count ) return 0;

  ulen rest=len-ind;

  if( count<rest )
    {
     for(rest-=count; rest ;rest--,ind++) Swap(list[ind],list[ind+count]);

     return count;
    }
  else
    {
     return rest;
    }
 }

ulen RangeSwapDel(PtrLen<SwappableType auto> range,ulen ind,ulen count)
 {
  return RangeSwapDel(range.ptr,range.len,ind,count);
 }

void RangeCopyDel_guarded(NothrowCopyableType auto list[],ulen len,ulen ind,ulen count)
 {
  if( GuardIndexCount(ind,count,len) ) return;

  ulen rest=len-ind;

  if( count<rest )
    {
     for(rest-=count; rest ;rest--,ind++) list[ind]=list[ind+count];
    }
 }

void RangeCopyDel_guarded(PtrLen<NothrowCopyableType auto> range,ulen ind,ulen count)
 {
  RangeCopyDel_guarded(range.ptr,range.len,ind,count);
 }

void RangeSwapDel_guarded(SwappableType auto list[],ulen len,ulen ind,ulen count)
 {
  if( GuardIndexCount(ind,count,len) ) return;

  ulen rest=len-ind;

  if( count<rest )
    {
     for(rest-=count; rest ;rest--,ind++) Swap(list[ind],list[ind+count]);
    }
 }

void RangeSwapDel_guarded(PtrLen<SwappableType auto> range,ulen ind,ulen count)
 {
  RangeSwapDel_guarded(range.ptr,range.len,ind,count);
 }

} // namespace CCore

#endif

