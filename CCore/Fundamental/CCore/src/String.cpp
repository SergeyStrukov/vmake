/* String.cpp */
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

#include <CCore/inc/String.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* class String */

class String::CharBuilder
 {
   PtrLen<const Char> str;
   ulen len;

  public:

   explicit CharBuilder(PtrLen<const Char> str_)
    : str(str_)
    {
     ulen total=0;

     for(Char ch : str )
       {
        Symbol sym=ToSymbol(ch);

        total=LenAdd(total,SymbolLen(sym));
       }

     len=total;
    }

   ulen getLen() const { return len; }

   PtrLen<char> operator () (Place<void> place) const
    {
     char *ptr=place;

     OutBuf<char> out(ptr);

     for(Char ch : str )
       {
        Symbol sym=ToSymbol(ch);

        out(SymbolRange(sym));
       }

     return Range(ptr,len);
    }
 };

String::String(PtrLen<const Char> str) : data(DoBuild,CharBuilder(str)) {}

/* class PrintString */

PtrLen<char> PrintString::do_provide(ulen hint_len)
 {
  if( !buf )
    {
     ulen len=data.getLen();

     if( len>max_len ) return Nothing;

     buf=data.extend_raw( Min<ulen>(Max(frame_len,hint_len)-1,max_len-len) + 1 );
    }

  return buf;
 }

void PrintString::do_flush(char *,ulen len)
 {
  buf+=len;
 }

PrintString::PrintString(ulen max_len_,ulen frame_len_)
 : max_len(max_len_),
   frame_len(Max(frame_len_,DefaultFrameLen)),
   data(frame_len)
 {
 }

String PrintString::close()
 {
  flush();

  data.shrink(buf.len);

  buf=Empty;

  if( data.getLen()>max_len )
    {
     Printf(Exception,"CCore::PrintString::close() : overflow");
    }

  String ret;

  data.copyTo(ret);

  return ret;
 }

void PrintString::cleanup()
 {
  flush();

  data.erase();

  buf=Empty;
 }

} // namespace CCore


