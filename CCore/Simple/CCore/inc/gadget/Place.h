/* Place.h */
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

#ifndef CCore_inc_gadget_Place_h
#define CCore_inc_gadget_Place_h

#include <CCore/inc/gadget/Nothing.h>
#include <CCore/inc/gadget/Len.h>
#include <CCore/inc/gadget/Replace.h>
#include <CCore/inc/gadget/Classification.h>

namespace CCore {

/* type byte */

using byte = unsigned char ;

/* raw pointer functions */

inline byte * CastPtr(void *ptr) { return static_cast<byte *>(ptr); }

inline const byte * CastPtr(const void *ptr) { return static_cast<const byte *>(ptr); }


inline void * PtrAdd(void *ptr,ulen delta) { return CastPtr(ptr)+delta; }

inline const void * PtrAdd(const void *ptr,ulen delta) { return CastPtr(ptr)+delta; }


inline void * PtrSub(void *ptr,ulen delta) { return CastPtr(ptr)-delta; }

inline const void * PtrSub(const void *ptr,ulen delta) { return CastPtr(ptr)-delta; }


inline ulen PtrDist(const void *from,const void *to) { return ulen( CastPtr(to)-CastPtr(from) ); }

/* classes */

template <VoidType Void> class Place;

/* class Place<Void> */

template <VoidType Void>
class Place
 {
   Void *ptr;

  public:

   explicit Place(Void *ptr_=0) noexcept : ptr(ptr_) {}

   Place(NothingType) : Place() {}

   Place<Void> operator + (UIntType auto delta) const { return Place<Void>(PtrAdd(ptr,delta)); }

   Place<Void> operator - (UIntType auto delta) const { return Place<Void>(PtrSub(ptr,delta)); }

   Place<Void> operator += (UIntType auto delta) { return Replace(*this,*this+delta); }

   Place<Void> operator -= (UIntType auto delta) { return Replace(*this,*this-delta); }

   template <class T>
   operator T * () const { return static_cast<T *>(ptr); }
 };

/* PlaceAt() */

inline Place<void> PlaceAt(void *ptr) { return Place<void>(ptr); }

inline Place<const void> PlaceAt(const void *ptr) { return Place<const void>(ptr); }

} // namespace CCore

/* placement new */

inline void * operator new(std::size_t,CCore::Place<void> place) { return place; }

inline void operator delete(void *,CCore::Place<void>) noexcept {}

#endif


