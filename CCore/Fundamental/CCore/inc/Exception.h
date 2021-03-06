/* Exception.h */
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

#ifndef CCore_inc_Exception_h
#define CCore_inc_Exception_h

#include <CCore/inc/Printf.h>
#include <CCore/inc/PlanInit.h>

namespace CCore {

/* GetPlanInitNode_...() */

PlanInitNode * GetPlanInitNode_Exception();

/* classes */

class ReportException;

class SilentReportException;

template <class P> class ReportExceptionTo;

class PrintException;

template <class T> class CustomThrow;

template <class T> class PrintCustomException;

/* class ReportException */

class ReportException : NoCopy
 {
   DLink<ReportException> link;

   bool nok = false ;

   using Algo = DLink<ReportException>::LinearAlgo<&ReportException::link> ;

  private:

   static ReportException * Top();

   static void SetTop(ReportException *top);

   static ReportException * Start();

   static void Add(ReportException *report,const char *ptr,ulen len);

   static void End(ReportException *report);

   friend class PrintException;

  protected:

   static void Print(StrLen str);

   virtual void print(StrLen str);

   virtual void start(ExceptionType ex);

   virtual void add(const char *ptr,ulen len);

   virtual void end();

  public:

   ReportException() noexcept;

   ~ReportException();

   bool operator + () const { return !nok; }

   bool operator ! () const { return nok; }

   void clear() { nok=false; }

   void guard() { if( nok ) throw CatchType(); }

   static void Clear();
 };

/* class SilentReportException */

class SilentReportException : public ReportException
 {
  private:

   virtual void print(StrLen str);

   virtual void start(ExceptionType ex);

   virtual void add(const char *ptr,ulen len);

   virtual void end();

  public:

   SilentReportException();

   ~SilentReportException();
 };

/* class ReportExceptionTo<P> */

template <class P>
class ReportExceptionTo : public ReportException
 {
   P &out;

  public:

   explicit ReportExceptionTo(P &out_) : out(out_) {}

   ~ReportExceptionTo() {}

   virtual void print(StrLen str)
    {
     SilentReportException report;

     try
       {
        out.put(str.ptr,str.len);
       }
     catch(...) {}
    }
 };

/* class PrintException */

class PrintException : public PrintBase
 {
   ExceptionType ex;
   ReportException *report;

   char buf[TextBufLen];
   bool do_throw;

  private:

   virtual PtrLen<char> do_provide(ulen hint_len);

   virtual void do_flush(char *ptr,ulen len);

   void start();

  protected:

   PrintException();

  public:

   explicit PrintException(ExceptionType ex);

   ~PrintException() noexcept(false);
 };

template <>
struct PrintOutAdapter<ExceptionType>
 {
  using PrintOutType = PrintException ;
 };

/* class CustomThrow<T> */

template <class T>
class CustomThrow : NoCopy
 {
   T obj;

  public:

   template <class ... SS>
   explicit CustomThrow(SS && ... ss) : obj( std::forward<SS>(ss)... ) {}

   ~CustomThrow() noexcept(false) { throw obj; }
 };

/* class PrintCustomException<T> */

template <class T>
class PrintCustomException : public CustomThrow<T> , public PrintException
 {
  public:

   template <class ... SS>
   explicit PrintCustomException(SS && ... ss) : CustomThrow<T>( std::forward<SS>(ss)... ) {}

   ~PrintCustomException() noexcept(false) {}
 };

} // namespace CCore

#endif


