/* PrintFile.cpp */
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

#include <CCore/inc/print/PrintFile.h>

#include <CCore/inc/Exception.h>

namespace CCore {

/* class RawFileToPrint */

FileError RawFileToPrint::open(StrLen file_name,FileOpenFlags oflags)
 {
  if( opened ) return FileError_NoMethod;

  if( FileError fe=file.open(file_name,oflags) ) return fe;

  opened=true;
  preserve_file=false;

  return FileError_Ok;
 }

void RawFileToPrint::close(FileMultiError &errout)
 {
  if( opened )
    {
     opened=false;

     file.close(errout,preserve_file);
    }
  else
    {
     errout.add(FileError_NoMethod);
    }
 }

FileError RawFileToPrint::write(const uint8 *buf,ulen len)
 {
  if( !opened ) return FileError_NoMethod;

  auto result=file.write(buf,len);

  if( result.error ) return result.error;

  if( result.len!=len ) return FileError_WriteLenMismatch;

  return FileError_Ok;
 }

/* class PrintFile */

PtrLen<char> PrintFile::do_provide(ulen)
 {
  if( !isOpened() )
    {
     Printf(Exception,"CCore::PrintFile::do_provide(...) : file is not opened");
    }

  return Range(buf);
 }

void PrintFile::do_flush(char *ptr,ulen len)
 {
  if( FileError error=file.write(MutatePtr<const uint8>(ptr),len) )
    {
     if( no_flush_exception )
       {
        flush_error=error;
       }
     else
       {
        Printf(Exception,"CCore::PrintFile::do_flush(...) : #;",error);
       }
    }
 }

PrintFile::PrintFile() noexcept
 : buf(BufLen)
 {
 }

PrintFile::PrintFile(StrLen file_name,FileOpenFlags oflags)
 : PrintFile()
 {
  open(file_name,oflags);
 }

PrintFile::~PrintFile()
 {
  if( isOpened() )
    {
     FileMultiError errout;

     close(errout);

     if( +errout )
       {
        Printf(NoException,"CCore::PrintFile::~PrintFile() : #;",errout);
       }
    }
 }

void PrintFile::open(StrLen file_name,FileOpenFlags oflags)
 {
  if( FileError error=file.open(file_name,oflags) )
    {
     Printf(Exception,"CCore::PrintFile::open(#.q;,#;) : #;",file_name,oflags,error);
    }

  no_flush_exception=false;
 }

void PrintFile::disableExceptions()
 {
  if( !no_flush_exception )
    {
     no_flush_exception=true;

     flush_error=FileError_Ok;
    }
 }

void PrintFile::close(FileMultiError &errout)
 {
  if( isOpened() )
    {
     disableExceptions();

     flush();

     errout.add(flush_error);

     file.close(errout);
    }
  else
    {
     errout.add(FileError_NoMethod);
    }
 }

void PrintFile::close()
 {
  FileMultiError errout;

  close(errout);

  if( +errout )
    {
     Printf(Exception,"CCore::PrintFile::close() : #;",errout);
    }
 }

} // namespace CCore


