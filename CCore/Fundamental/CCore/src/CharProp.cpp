/* CharProp.cpp */
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

#include <CCore/inc/CharProp.h>

namespace CCore {

/* CutLine() */

StrLen CutLine(StrLen &text)
 {
  SplitLine split(text);

  text=split.rest;

  return split.line;
 }

/* struct SplitLine */

SplitLine::SplitLine(StrLen text)
 {
  for(StrLen next=text; +next ;++next)
    {
     char ch=*next;

     if( ch=='\r' || ch=='\n' )
       {
        line=text.prefix(next);

        ++next;

        if( ch=='\r' && +next && *next=='\n' ) ++next;

        rest=next;
        eol=true;

        return;
       }
    }

  line=text;
  rest=Empty;
  eol=false;
 }

/* class ASCIICode */

const bool ASCIICode::IsSpaceTable[256]=
 {
  false,false,false,false,false,false,false,false,false,true ,true ,true ,true ,true ,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  true ,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false
 };

const bool ASCIICode::IsPunctTable[256]=
 {
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,true ,true ,true ,true ,true ,true ,true ,true ,true ,true ,true ,true ,true ,true ,true ,
  false,false,false,false,false,false,false,false,false,false,true ,true ,true ,true ,true ,true ,
  true ,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,true ,true ,true ,true ,false,
  true ,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,true ,true ,true ,true ,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false
 };

const bool ASCIICode::IsSpaceOrPunctTable[256]=
 {
  false,false,false,false,false,false,false,false,false,true ,true ,true ,true ,true ,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  true ,true ,true ,true ,true ,true ,true ,true ,true ,true ,true ,true ,true ,true ,true ,true ,
  false,false,false,false,false,false,false,false,false,false,true ,true ,true ,true ,true ,true ,
  true ,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,true ,true ,true ,true ,false,
  true ,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,true ,true ,true ,true ,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
  false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false
 };

const signed char ASCIICode::DecValueTable[256]=
 {
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
 };

const signed char ASCIICode::HexValueTable[256]=
 {
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
   -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
   -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
 };

/* char sets */

const char * GetSpaceChars() { return " \t\f\v\r\n"; }

const char * GetPunctChars() { return "!\"#$%&'()*+,-./:;<=>?@[\\]^`{|}~"; }

const char * GetDigitChars() { return "0123456789"; }

const char * GetHexDigitChars() { return "0123456789abcdefABCDEF"; }

const char * GetCLetterChars() { return "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"; }

} // namespace CCore


