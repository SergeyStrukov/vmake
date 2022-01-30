/* SysFile.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Target/WIN32
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_sys_SysFile_h
#define CCore_inc_sys_SysFile_h

#include <CCore/inc/sys/SysTypes.h>

#include <CCore/inc/TimeScope.h>
#include <CCore/inc/GenFile.h>

namespace CCore {
namespace Sys {

/* classes */

struct File;

struct AltFile;

/* struct File */

struct File
 {
  // public

  struct IOResult
   {
    ulen len;
    FileError error;
   };

  struct PosResult
   {
    FilePosType pos;
    FileError error;
   };

  // private data

  using Type = handle_t ;

  Type handle;
  FileOpenFlags oflags;

  // private

  struct OpenType
   {
    Type handle;
    FileError error;
   };

  static OpenType Open(StrLen file_name,FileOpenFlags oflags) noexcept;

  static void Close(FileMultiError &errout,Type handle,FileOpenFlags oflags,bool preserve_file) noexcept;

  static IOResult Write(Type handle,FileOpenFlags oflags,const uint8 *buf,ulen len) noexcept;

  static IOResult Read(Type handle,FileOpenFlags oflags,uint8 *buf,ulen len) noexcept;

  static PosResult GetLen(Type handle,FileOpenFlags oflags) noexcept;

  static PosResult GetPos(Type handle,FileOpenFlags oflags) noexcept;

  static FileError SetPos(Type handle,FileOpenFlags oflags,FilePosType pos) noexcept;

  // public

  FileError open(StrLen file_name,FileOpenFlags oflags_)
   {
    OpenType result=Open(file_name,oflags_);

    handle=result.handle;
    oflags=oflags_;

    return result.error;
   }

  void close(FileMultiError &errout,bool preserve_file=false)
   {
    Close(errout,handle,oflags,preserve_file);
   }

  void close()
   {
    FileMultiError errout;

    close(errout);
   }

  IOResult write(const uint8 *buf,ulen len) { return Write(handle,oflags,buf,len); }

  IOResult read(uint8 *buf,ulen len) { return Read(handle,oflags,buf,len); }

  PosResult getLen() { return GetLen(handle,oflags); }

  PosResult getPos() { return GetPos(handle,oflags); }

  FileError setPos(FilePosType pos) { return SetPos(handle,oflags,pos); }
 };

/* struct AltFile */

struct AltFile
 {
  // public

  struct Result
   {
    FilePosType file_len;
    FileError error;
   };

  // private data

  using Type = handle_t ;
  using EventType = handle_t ;

  Type handle;
  EventType h_event;
  FilePosType file_len;
  FileOpenFlags oflags;

  // private

  struct OpenType
   {
    Type handle;
    EventType h_event;
    FilePosType file_len;
    FileError error;
   };

  static OpenType Open(StrLen file_name,FileOpenFlags oflags) noexcept;

  static void Close(FileMultiError &errout,Type handle,EventType h_event,FileOpenFlags oflags,bool preserve_file) noexcept;

  static FileError Write(Type handle,EventType h_event,FileOpenFlags oflags,FilePosType off,const uint8 *buf,ulen len) noexcept;

  static FileError Read(Type handle,EventType h_event,FileOpenFlags oflags,FilePosType off,uint8 *buf,ulen len) noexcept;

  // public

  Result open(StrLen file_name,FileOpenFlags oflags_)
   {
    OpenType result=Open(file_name,oflags_);

    handle=result.handle;
    h_event=result.h_event;
    file_len=result.file_len;
    oflags=oflags_;

    Result ret;

    ret.file_len=result.file_len;
    ret.error=result.error;

    return ret;
   }

  void close(FileMultiError &errout,bool preserve_file=false)
   {
    Close(errout,handle,h_event,oflags,preserve_file);
   }

  void close()
   {
    FileMultiError errout;

    close(errout);
   }

  Result write(FilePosType off,const uint8 *buf,ulen len)
   {
    FileError fe=Write(handle,h_event,oflags,off,buf,len);

    Result ret;

    ret.error=fe;

    if( fe )
      {
       ret.file_len=0;
      }
    else
      {
       Replace_max(file_len,off+len);

       ret.file_len=file_len;
      }

    return ret;
   }

  FileError read(FilePosType off,uint8 *buf,ulen len)
   {
    return Read(handle,h_event,oflags,off,buf,len);
   }
 };

} // namespace Sys
} // namespace CCore

#endif


