/* TextTools.h */
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

#ifndef CCore_inc_TextTools_h
#define CCore_inc_TextTools_h

#include <CCore/inc/CharProp.h>
#include <CCore/inc/SymCount.h>
#include <CCore/inc/Symbol.h>

namespace CCore {

/* AllChars() */

void AllChars(FuncArgType<char> auto func)
 {
  for(int i=-128; i<128 ;i++)
    {
     char ch=char(i);

     func(ch);
    }
 }

/* classes */

struct TextPos;

class TextPosCounter;

template <class CharClass,CharClass DefaultClass> class CharPropTable;

class ExtCharCode;

/* struct TextPos */

struct TextPos
 {
  ulen line;
  ulen col;

  TextPos() : line(1),col(1) {}

  void nextPos() { col++; }

  void nextLine() { col=1; line++; }

  void update(ulen len) { col+=len; }

  void update(StrLen text); // must not break "\r\n"

  // print object

  void print(PrinterType auto &out) const
   {
    Printf(out,"(line=#;,col=#;)",line,col);
   }
 };

/* class TextPosCounter */

class TextPosCounter : NoCopy
 {
   TextPos pos;
   SymDetector det;
   bool flag = false ;

  private:

   void newLine();

  public:

   TextPosCounter() {}

   operator TextPos() const { return pos; }

   void reset() { pos={}; det.reset(); flag=false; }

   void put(char ch);

   void put(char ch,ulen len)
    {
     for(; len ;len--) put(ch);
    }

   void put(const char *str,ulen len)
    {
     for(; len ;len--,str++) put(*str);
    }
 };

/* class CharPropTable<CharClass,CharClass DefaultClass> */

template <class CharClass,CharClass DefaultClass>
class CharPropTable : NoCopy
 {
   CharClass table[256];

  private:

   static uint8 Index(char ch) { return uint8(ch); }

  protected:

   void set(char ch,CharClass cc)
    {
     table[Index(ch)]=cc;
    }

   void setSet(const char *zstr,CharClass cc)
    {
     for(; char ch=*zstr ;zstr++) set(ch,cc);
    }

   void setIf(FuncType<bool,char> auto cond,CharClass cc)
    {
     AllChars( [this,cond,cc] (char ch) { if( cond(ch) ) set(ch,cc); } );
    }

  public:

   CharPropTable() { Range(table).set(DefaultClass); }

   CharClass operator [] (char ch) const { return table[Index(ch)]; }
 };

/* class ExtCharCode */

class ExtCharCode
 {
   Symbol code;

  public:

   explicit ExtCharCode(const Symbol &code_) : code(code_) {}

   explicit ExtCharCode(IsType<Char> auto ch) : code(ToSymbol(ch)) {}

   // print object

   void print(PrinterType auto &out) const
    {
     if( SymbolLen(code)==1 )
       Putobj(out,CharCode(code[0]));
     else
       Putobj(out,code);
    }
 };

} // namespace CCore

#endif


