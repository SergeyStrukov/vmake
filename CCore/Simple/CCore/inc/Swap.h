/* Swap.h */
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

#ifndef CCore_inc_Swap_h
#define CCore_inc_Swap_h

#include <CCore/inc/Gadget.h>

namespace CCore {

/* functions */

template <CopyableType T> requires ( !NothrowCopyableType<T> )
[[deprecated("throw copyable type")]]
void CopySwap(T &a,T &b) noexcept
 {
  T temp(a);

  a=b;

  b=temp;
 }

template <NothrowCopyableType T>
void CopySwap(T &a,T &b) noexcept
 {
  T temp(a);

  a=b;

  b=temp;
 }

template <MovableType T>
void MoveSwap(T &a,T &b) noexcept
 {
  T temp( std::move(a) );

  a = std::move(b) ;

  b = std::move(temp) ;
 }

/* concept Has_objSwap<T> */

template <class T>
concept Has_objSwap = requires()
 {
  Ground< void (T::*)(T &) >( &T::objSwap );
 } ;

/* concept No_objSwap<T> */

template <class T>
concept No_objSwap = !Has_objSwap<T> ;

/* classes */

template <class T> struct SwapAdapter;

/* struct SwapAdapter<T> */

template <Has_objSwap T>
struct SwapAdapter<T>
 {
  static void Swap(T &a,T &b) { a.objSwap(b); }
 };

template <No_objSwap T> requires ( MovableType<T> )
struct SwapAdapter<T>
 {
  static void Swap(T &a,T &b) { MoveSwap(a,b); }
 };

template <No_objSwap T> requires ( !MovableType<T> )
struct SwapAdapter<T>
 {
  static void Swap(T &a,T &b) { CopySwap(a,b); }
 };

/* concept SwappableType<T> */

template <class T>
concept SwappableType = Has_objSwap<T> || MovableType<T> || CopyableType<T> ;

/* Swap() */

template <SwappableType T>
void Swap(T &a,T &b) noexcept { SwapAdapter<T>::Swap(a,b); }

/* NullBySwap() */

template <NothrowDefaultCtorType T>
void NullBySwap(T &obj) noexcept
 {
  T temp{};

  Swap(temp,obj);
 }

/* MoveBySwap() */

template <NothrowDefaultCtorType T>
void MoveBySwap(T &dst,T &src) noexcept
 {
  if( &dst!=&src )
    {
     Swap(dst,src);

     NullBySwap(src);
    }
 }

} // namespace CCore

#endif


