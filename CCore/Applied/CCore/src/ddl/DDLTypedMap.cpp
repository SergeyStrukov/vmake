/* DDLTypedMap.cpp */
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
//  Copyright (c) 2022 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#include <CCore/inc/ddl/DDLTypedMap.h>

#include <CCore/inc/Exception.h>

namespace CCore {
namespace DDL {

/* functions */

void GuardBadStructure()
 {
  Printf(Exception,"CCore::DDL::TypedMap : bad structure");
 };

void GuardMapNoConst(StrLen name)
 {
  Printf(Exception,"CCore::DDL::TypedMap : constant #; is not found",name);
 }

void GuardMapBadConstType(StrLen name)
 {
  Printf(Exception,"CCore::DDL::TypedMap : constant #; has an unexpected type",name);
 }

/* struct EmptyTypeSet */

DDL::MapSizeInfo EmptyTypeSet::structSizeInfo(DDL::StructNode *)
 {
  DDL::MapSizeInfo ret;

  Printf(Exception,"Unknown structure");

  return ret;
 }

void EmptyTypeSet::guardFieldTypes(DDL::StructNode *) const
 {
  Printf(Exception,"Unknown structure");
 }

} // namespace DDL
} // namespace CCore


