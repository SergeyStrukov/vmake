/* DDLChar.h */
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

#ifndef CCore_inc_ddl_DDLChar_h
#define CCore_inc_ddl_DDLChar_h

#include <CCore/inc/Printf.h>
#include <CCore/inc/TextTools.h>

namespace CCore {
namespace DDL {

/* classes */

//enum CharClass;

class CharProp;

struct FileId;

class PrintPos;

/* enum CharClass */

enum CharClass
 {
  Char_Digit,
  Char_Letter,
  Char_QMark,
  Char_Punct,
  Char_Space,

  Char_Other
 };

inline bool IsLetterDigit(CharClass cc) { return cc<=Char_Letter; }

/* class CharProp */

class CharProp : public CharPropTable<CharClass,Char_Other>
 {
  public:

   CharProp();

   static const CharProp Object;
 };

/* char functions */

inline CharClass GetCharClass(char ch) { return CharProp::Object[ch]; }

inline bool charIsLetterDigit(char ch) { return IsLetterDigit(GetCharClass(ch)); }

inline bool charIsHexSuffix(char ch) { return ch=='h' || ch=='H' ; }

inline bool charIsBinSuffix(char ch) { return ch=='b' || ch=='B' ; }

//---------------------------------
//
// From CharProp.h
//
//---------------------------------
//
// bool charIsEOL(char ch);
//
// bool charIsSpace(char ch);
//
// bool charIsPrintable(char ch);
//
// bool charIsBin(char ch);
//
// bool charIsHex(char ch);
//
// bool charIsDec(char ch);
//
//---------------------------------

inline char MapDChar(char ch)
 {
  switch( ch )
    {
     case 'b' : return '\b';
     case 't' : return '\t';
     case 'n' : return '\n';
     case 'r' : return '\r';
     case 'v' : return '\v';
     case 'f' : return '\f';

     default: return ch;
    }
 }

/* struct FileId */

struct FileId
 {
  virtual void printPos(PrintBase &out,TextPos pos);
 };

/* class PrintPos */

class PrintPos
 {
   FileId *file_id;
   TextPos pos;

  public:

   PrintPos(FileId *file_id_,TextPos pos_) : file_id(file_id_),pos(pos_) {}

   void print(PrintBase &out) const
    {
     file_id->printPos(out,pos);
    }
 };

} // namespace DDL
} // namespace CCore

#endif


