/* IntScan.h */
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

#ifndef CCore_inc_scanf_IntScan_h
#define CCore_inc_scanf_IntScan_h

#include <CCore/inc/scanf/ScanTools.h>

namespace CCore {

/* consts */

inline constexpr unsigned MinScanIntBase =  2 ;
inline constexpr unsigned MaxScanIntBase = 16 ;

/* classes */

//enum IntScanBase;

struct IntScanOpt;

template <UIntType UInt> class UIntDigitAcc;

template <SIntType SInt> class SIntDigitAcc;

template <ulen Len> class DetectIntFormat;

template <SUIntType SUInt,class Acc> struct IntScanAlgo;

template <UIntType UInt> class UIntScan;

template <SIntType SInt> class SIntScan;

/* enum IntScanBase */

enum IntScanBase
 {
  IntScanNone,

  IntScanHex,
  IntScanBin,
  IntScan0X,

  IntScanDefault = IntScanNone
 };

template <CharPeekType Dev>
bool Parse_try(Dev &dev,IntScanBase &ret)
 {
  typename Dev::Peek peek(dev);

  if( !peek )
    {
     return false;
    }
  else
    {
     switch( *peek )
       {
        case 'h' : case 'H' : ret=IntScanHex; ++dev; return true;
        case 'b' : case 'B' : ret=IntScanBin; ++dev; return true;
        case 'x' : case 'X' : ret=IntScan0X;  ++dev; return true;
       }

     return false;
    }
 }

/* struct IntScanOpt */

struct IntScanOpt
 {
  unsigned base; // 2..16, 0 for any
  IntScanBase scan_base;

  void setDefault()
   {
    base=0;
    scan_base=IntScanDefault;
   }

  IntScanOpt() { setDefault(); }

  IntScanOpt(const char *ptr,const char *lim);

  //
  // [.base|.x|.X|.b|.B|.h|.H]
  //
 };

/* class UIntDigitAcc<UInt> */

template <UIntType UInt>
class UIntDigitAcc : NoCopy
 {
   UInt value;

   UInt a;
   UInt b;
   bool minus;

  public:

   static constexpr unsigned Bits = Meta::UIntBits<UInt> ;

   UIntDigitAcc(int dig,unsigned base,bool minus_)
    {
     value=UInt(dig);

     const UInt MaxVal=UIntFunc<UInt>::MaxUnsigned;

     a=UInt(MaxVal/base);
     b=UInt(MaxVal%base);
     minus=minus_;
    }

   bool next(int dig_,unsigned base)
    {
     unsigned dig=(unsigned)dig_;

     if( value>a || ( value==a && dig>b ) ) return false;

     value=UInt(base*value+dig);

     return true;
    }

   using ValueType = UInt ;

   operator UInt() const { return minus? UIntFunc<UInt>::Neg(value) : value ; }
 };

/* class SIntDigitAcc<SInt> */

template <SIntType SInt>
class SIntDigitAcc : NoCopy
 {
   using UInt = typename SIntFunc<SInt>::UInt ;

   UInt value;

   UInt a;
   UInt b;
   bool minus;

  public:

   static constexpr unsigned Bits = Meta::UIntBits<UInt> ;

   SIntDigitAcc(int dig,unsigned base,bool minus_)
    {
     value=UInt(dig);

     const UInt MaxVal=( minus_? SIntFunc<SInt>::MinNegativeAbs : SIntFunc<SInt>::MaxPositiveAbs );

     a=UInt(MaxVal/base);
     b=UInt(MaxVal%base);
     minus=minus_;
    }

   bool next(int dig_,unsigned base)
    {
     unsigned dig=(unsigned)dig_;

     if( value>a || ( value==a && dig>b ) ) return false;

     value=UInt(base*value+dig);

     return true;
    }

   using ValueType = SInt ;

   operator SInt() const { return minus? SIntFunc<SInt>::NegAbs(value) : SIntFunc<SInt>::PosAbs(value) ; }
 };

/* class DetectIntFormat<ulen Len> */

template <ulen Len>
class DetectIntFormat : NoCopy
 {
   char buf[Len];
   IntScanBase scan_base;

   PtrLen<char> cur;
   bool fail_flag = false ;

  private:

   void put(char ch)
    {
     if( +cur )
       {
        *cur=ch;

        ++cur;
       }
     else
       {
        fail();
       }
    }

   IntScanBase finish(int max_dig,ScannerType auto &inp);

   IntScanBase detect_0X(ScannerType auto &inp);

   IntScanBase detect_0(ScannerType auto &inp);

   IntScanBase detect_nosign(ScannerType auto &inp);

   IntScanBase detect(ScannerType auto &inp);

  public:

   explicit DetectIntFormat(ScannerType auto &inp);

   using ScanInpType = DetectIntFormat<Len> & ;

   ScanInpType scanRef() { return *this; }

   IntScanBase getFormat() const { return scan_base; }

   // cursor

   ulen operator + () const { return +cur; }

   bool operator ! () const { return !cur; }

   char operator * () const { return *cur; }

   void operator ++ () { ++cur; }

   // error

   bool isOk() const { return !fail_flag; }

   bool isFailed() const { return fail_flag; }

   void fail() { cur=Nothing; fail_flag=true; }
 };

template <ulen Len>
IntScanBase DetectIntFormat<Len>::finish(int max_dig,ScannerType auto &inp)
 {
  bool bin_flag=false;

  for(; +inp ;++inp)
    {
     char ch=*inp;
     int dig=charHexValue(ch);

     if( dig<0 )
       {
        if( ch=='h' || ch=='H' )
          {
           put(ch);

           ++inp;

           return IntScanHex;
          }

        break;
       }
     else
       {
        bin_flag=( max_dig<2 && ( ch=='b' || ch=='B' ) );

        put(ch);

        Replace_max(max_dig,dig);
       }
    }

  if( bin_flag ) return IntScanBin;

  if( max_dig>=10 ) inp.fail();

  return IntScanNone;
 }

template <ulen Len>
IntScanBase DetectIntFormat<Len>::detect_0X(ScannerType auto &inp)
 {
  if( +inp )
    {
     char ch=*inp;

     if( ch=='0' )
       {
        put(ch);

        ++inp;

        SkipAllOfChar(inp, [] (char ch) { return ch=='0'; } );
       }
     else
       {
        if( charHexValue(ch)>=0 )
          {
           put(ch);

           ++inp;
          }
        else
          {
           inp.fail();

           return IntScan0X;
          }
       }

     for(char ch; +inp && charHexValue(ch=*inp)>=0 ;++inp) put(ch);
    }
  else
    {
     inp.fail();
    }

  return IntScan0X;
 }

template <ulen Len>
IntScanBase DetectIntFormat<Len>::detect_0(ScannerType auto &inp)
 {
  if( +inp )
    {
     char ch=*inp;

     switch( ch )
       {
        case 'x' : case 'X' :
         {
          put(ch);

          ++inp;

          return detect_0X(inp);
         }

        default:
         {
          SkipAllOfChar(inp, [] (char ch) { return ch=='0'; } );

          return finish(0,inp);
         }
       }
    }
  else
    {
     return IntScanNone;
    }
 }

template <ulen Len>
IntScanBase DetectIntFormat<Len>::detect_nosign(ScannerType auto &inp)
 {
  if( +inp )
    {
     char ch=*inp;

     if( ch=='0' )
       {
        put(ch);

        ++inp;

        return detect_0(inp);
       }
     else
       {
        int dig=charHexValue(ch);

        if( dig<0 )
          {
           inp.fail();
          }
        else
          {
           put(ch);

           ++inp;

           return finish(dig,inp);
          }
       }
    }
  else
    {
     inp.fail();
    }

  return IntScanNone;
 }

template <ulen Len>
IntScanBase DetectIntFormat<Len>::detect(ScannerType auto &inp)
 {
  if( +inp )
    {
     char ch=*inp;

     if( ch=='+' || ch=='-' )
       {
        put(ch);

        ++inp;
       }

     return detect_nosign(inp);
    }
  else
    {
     inp.fail();
    }

  return IntScanNone;
 }

template <ulen Len>
DetectIntFormat<Len>::DetectIntFormat(ScannerType auto &inp)
 {
  cur=Range(buf);

  scan_base=detect(inp);

  if( isOk() ) cur=Range(buf).prefix(cur); else inp.fail();
 }

/* struct IntScanAlgo<SUInt,Acc> */

template <SUIntType SUInt,class Acc>
struct IntScanAlgo
 {
  // Hex

  static void ScanHex(ScannerType auto &inp,SUInt &ret,bool minus)
   {
    if( +inp )
      {
       int dig=charHexValue(*inp);

       if( dig<0 )
         {
          inp.fail();
         }
       else
         {
          Acc acc(dig,16,minus);

          for(++inp; +inp ;++inp)
            {
             char ch=*inp;

             dig=charHexValue(ch);

             if( dig<0 )
               {
                if( ch=='h' || ch=='H' )
                  {
                   ++inp;

                   ret=acc;
                  }
                else
                  {
                   inp.fail();
                  }

                return;
               }
             else
               {
                if( !acc.next(dig,16) )
                  {
                   inp.fail();

                   return;
                  }
               }
            }

          inp.fail();
         }
      }
    else
      {
       inp.fail();
      }
   }

  static void ScanHex(ScannerType auto &inp,SUInt &ret)
   {
    if( +inp )
      {
       switch( *inp )
         {
          case '+' : ++inp; [[fallthrough]];

          default: ScanHex(inp,ret,false); break;

          case '-' : ++inp; ScanHex(inp,ret,true);
         }
      }
    else
      {
       inp.fail();
      }
   }

  // Bin

  static void ScanBin(ScannerType auto &inp,SUInt &ret,bool minus)
   {
    if( +inp )
      {
       int dig=charBinValue(*inp);

       if( dig<0 )
         {
          inp.fail();
         }
       else
         {
          Acc acc(dig,2,minus);

          for(++inp; +inp ;++inp)
            {
             char ch=*inp;

             dig=charBinValue(ch);

             if( dig<0 )
               {
                if( ch=='b' || ch=='B' )
                  {
                   ++inp;

                   ret=acc;
                  }
                else
                  {
                   inp.fail();
                  }

                return;
               }
             else
               {
                if( !acc.next(dig,2) )
                  {
                   inp.fail();

                   return;
                  }
               }
            }

          inp.fail();
         }
      }
    else
      {
       inp.fail();
      }
   }

  static void ScanBin(ScannerType auto &inp,SUInt &ret)
   {
    if( +inp )
      {
       switch( *inp )
         {
          case '+' : ++inp; [[fallthrough]];

          default: ScanBin(inp,ret,false); break;

          case '-' : ++inp; ScanBin(inp,ret,true);
         }
      }
    else
      {
       inp.fail();
      }
   }

  // 0X

  static void Scan0X(ScannerType auto &inp,SUInt &ret,bool minus)
   {
    PassChar(inp,'0');
    PassOneOfChar(inp, [] (char ch) { return ch=='x' || ch=='X'; } );

    if( +inp )
      {
       int dig=charHexValue(*inp);

       if( dig<0 )
         {
          inp.fail();
         }
       else
         {
          Acc acc(dig,16,minus);

          for(++inp; +inp && (dig=charHexValue(*inp))>=0 ;++inp)
            {
             if( !acc.next(dig,16) )
               {
                inp.fail();

                return;
               }
            }

          ret=acc;
         }
      }
    else
      {
       inp.fail();
      }
   }

  static void Scan0X(ScannerType auto &inp,SUInt &ret)
   {
    if( +inp )
      {
       switch( *inp )
         {
          case '+' : ++inp; [[fallthrough]];

          default: Scan0X(inp,ret,false); break;

          case '-' : ++inp; Scan0X(inp,ret,true);
         }
      }
    else
      {
       inp.fail();
      }
   }

  // base

  static void ScanBase(ScannerType auto &inp,SUInt &ret,unsigned base,bool minus)
   {
    if( +inp )
      {
       int dig=charBaseValue(*inp,base);

       if( dig<0 )
         {
          inp.fail();
         }
       else
         {
          Acc acc(dig,base,minus);

          for(++inp; +inp && (dig=charBaseValue(*inp,base))>=0 ;++inp)
            {
             if( !acc.next(dig,base) )
               {
                inp.fail();

                return;
               }
            }

          ret=acc;
         }
      }
    else
      {
       inp.fail();
      }
   }

  static void ScanBase(ScannerType auto &inp,SUInt &ret,unsigned base)
   {
    if( +inp )
      {
       switch( *inp )
         {
          case '+' : ++inp; [[fallthrough]];

          default: ScanBase(inp,ret,base,false); break;

          case '-' : ++inp; ScanBase(inp,ret,base,true);
         }
      }
    else
      {
       inp.fail();
      }
   }

  // any

  static void ScanAny(ScannerType auto &inp,SUInt &ret) // Hex | Bin | 0X | Base(10)
   {
    DetectIntFormat<Acc::Bits+10> detect(inp);

    if( inp.isFailed() ) return;

    switch( detect.getFormat() )
      {
       case IntScanHex : ScanHex(detect,ret); break;

       case IntScanBin : ScanBin(detect,ret); break;

       case IntScan0X : Scan0X(detect,ret); break;

       case IntScanNone : ScanBase(detect,ret,10); break;
      }

    if( detect.isFailed() ) inp.fail();
   }

  // opt

  static void Scan(ScannerType auto &inp,IntScanOpt opt,SUInt &ret)
   {
    switch( opt.scan_base )
      {
       case IntScanHex : ScanHex(inp,ret); break;

       case IntScanBin : ScanBin(inp,ret); break;

       case IntScan0X : Scan0X(inp,ret); break;

       case IntScanNone :
        {
         if( opt.base )
           {
            ScanBase(inp,ret,opt.base);
           }
         else
           {
            ScanAny(inp,ret);
           }
        }
      }
   }
 };

/* class UIntScan<UInt> */

template <UIntType UInt>
class UIntScan : NoCopy
 {
   UInt &var;

  public:

   explicit UIntScan(UInt &var_) : var(var_) {}

   void scan(ScannerType auto &inp,IntScanOpt opt)
    {
     var=0;

     IntScanAlgo<UInt,UIntDigitAcc<UInt> >::Scan(inp,opt,var);
    }
 };

/* class SIntScan<SInt> */

template <SIntType SInt>
class SIntScan : NoCopy
 {
   SInt &var;

  public:

   explicit SIntScan(SInt &var_) : var(var_) {}

   void scan(ScannerType auto &inp,IntScanOpt opt)
    {
     var=0;

     IntScanAlgo<SInt,SIntDigitAcc<SInt> >::Scan(inp,opt,var);
    }
 };

} // namespace CCore

#endif


