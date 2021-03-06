/* ScanFile.cpp */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: HCore Mini
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/scan/ScanFile.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* class RawFileToScan */

FileError RawFileToScan::open(StrLen file_name)
 {
  if( opened ) return FileError_NoMethod;

  if( FileError fe=file.open(file_name,Open_ToRead) ) return fe;

  opened=true;

  return FileError_Ok;
 }

void RawFileToScan::close(FileMultiError &errout)
 {
  if( opened )
    {
     opened=false;

     file.close(errout);
    }
  else
    {
     errout.add(FileError_NoMethod);
    }
 }

Sys::File::IOResult RawFileToScan::read(uint8 *buf,ulen len)
 {
  if( !opened ) return {0,FileError_NoMethod};

  return file.read(buf,len);
 }

/* class ScanFile */

PtrLen<const char> ScanFile::underflow()
 {
  char *ptr=buf.getPtr();

  auto result=file.read(MutatePtr<uint8>(ptr),buf.getLen());

  if( result.error )
    {
     fail();

     if( no_underflow_exception )
       {
        underflow_error=result.error;
       }
     else
       {
        Printf(Exception,"CCore::ScanFile::underflow() : #;",result.error);
       }

     return Empty;
    }
  else
    {
     return Range_const(ptr,result.len);
    }
 }

ScanFile::ScanFile() noexcept
 : buf(BufLen)
 {
 }

ScanFile::ScanFile(StrLen file_name,bool disable_exceptions)
 : ScanFile()
 {
  open(file_name,disable_exceptions);
 }

ScanFile::~ScanFile()
 {
  if( isOpened() )
    {
     FileMultiError errout;

     close(errout);

     if( +errout )
       {
        Printf(NoException,"CCore::ScanFile::~ScanFile() : #;",errout);
       }
    }
 }

void ScanFile::open(StrLen file_name,bool disable_exceptions)
 {
  if( FileError error=file.open(file_name) )
    {
     Printf(Exception,"CCore::ScanFile::open(#.q;,...) : #;",file_name,error);
    }

  reset();

  no_underflow_exception=false;

  if( disable_exceptions ) disableExceptions();

  pump();
 }

void ScanFile::disableExceptions()
 {
  if( !no_underflow_exception )
    {
     underflow_error=FileError_Ok;

     no_underflow_exception=true;
    }
 }

void ScanFile::close(FileMultiError &errout)
 {
  if( isOpened() )
    {
     if( no_underflow_exception ) errout.add(underflow_error);

     file.close(errout);

     reset();
    }
  else
    {
     errout.add(FileError_NoMethod);
    }
 }

void ScanFile::close()
 {
  FileMultiError errout;

  close(errout);

  if( +errout )
    {
     Printf(Exception,"CCore::ScanFile::close() : #;",errout);
    }
 }

} // namespace CCore


