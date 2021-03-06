/* DDLSemantic.cpp */
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

#include <CCore/inc/ddl/DDLSemantic.h>

#include <CCore/inc/Crc.h>

#include <CCore/inc/algon/SortUnique.h>

namespace CCore {
namespace DDL {

/* struct Context */

NameId Context::generateIds()
 {
  struct Rec
   {
    NameIdNode *name;
    uint32 hash;

    explicit Rec(NameIdNode &name_)
     {
      name=&name_;

      Crc32 crc;

      crc.addRange(name_.getStr());

      hash=crc;
     }

    void set(NameId id)
     {
      name->name_id=id;
     }

    CmpResult operator <=> (const Rec &obj) const noexcept
     {
      if( auto ret=Cmp(hash,obj.hash) ) return ret;

      return StrCmp(name->getStr(),obj.name->getStr());
     }

    bool operator == (const Rec &obj) const noexcept
     {
      if( hash!=obj.hash ) return false;

      return name->getStr().equal(obj.name->getStr());
     }
   };

  DynArray<Rec> buf(DoReserve,name_id_list.count);

  for(NameIdNode &obj : name_id_list ) buf.append_fill(obj);

  NameId id=0;

  Algon::SortThenApplyUniqueRange(Range(buf), [&] (PtrLen<Rec> range)
                                                  {
                                                   ++id;

                                                   for(Rec &rec : range ) rec.set(id);
                                                  }
                                 );

  if( !id ) return 1;

  return id;
 }

struct Context::MapScopeRec : NoCopy
 {
  MapScopeRec *next;

  BodyNode *body_node;

  explicit MapScopeRec(BodyNode *body_node_) : body_node(body_node_) {}

  void addMaps(ElementPool &pool,NameId max_id,BaseList<NameLinkMap> &map_list)
   {
    auto *map=body_node->map;

    for(ScopeNode &node : body_node->scope_list )
      {
       node.body->map=map->add(pool,max_id,node,map_list);
      }
   }
 };

struct Context::MapScopeExpand
 {
  template <class Add>
  void operator () (MapScopeRec *rec,Add add)
   {
    for(ScopeNode &node : rec->body_node->scope_list ) add(node.body);
   }
 };

void Context::mapScopes(BodyNode *body_node,NameId max_id,BaseList<NameLinkMap> &map_list)
 {
  Enlist<MapScopeRec> enlist(MapScopeExpand(),body_node);

  for(auto &rec : enlist ) rec.addMaps(pool,max_id,map_list);
 }

template <class Node>
bool Context::fillMap(NameLinkMap *map,NameId max_id,Node &node)
 {
  if( ScopeNode *parent=node.parent ) map=parent->body->map;

  return map->add(error,max_id,node);
 }

bool Context::buildMaps(BodyNode *body_node,NameId max_id)
 {
  BaseList<NameLinkMap> map_list;

  NameLinkMap *map=pool.create<NameLinkMap>();

  map_list.init(map);

  body_node->map=map;

  mapScopes(body_node,max_id,map_list);

  AndFlag ret;

  for(AliasNode &node : body_node->alias_list ) ret+=fillMap(map,max_id,node);

  for(ConstNode &node : body_node->const_list ) ret+=fillMap(map,max_id,node);

  for(StructNode &node : body_node->struct_list ) ret+=fillMap(map,max_id,node);

  for(NameLinkMap &map : map_list ) ret+=map.check(error);

  return ret;
 }

bool Context::setAliasTypes(AliasNode &node,ulen mark)
 {
  if( node.result_type ) return true;

  AliasNode *alias=&node;

  for(;;)
    {
     alias->index=mark;

     if( AliasNode *next=alias->getNext() )
       {
        if( next->index==mark )
          {
           error("Cyclic type definition : #;",node.name);

           return false;
          }

        alias=next;

        if( alias->result_type ) break;
       }
     else
       {
        break;
       }
    }

  TypeNode *type;

  if( alias->result_type )
    type=alias->result_type;
  else
    type=alias->type_node;

  alias=&node;

  for(;;)
    {
     alias->result_type=type;

     if( AliasNode *next=alias->getNext() )
       {
        alias=next;

        if( alias->result_type ) break;
       }
     else
       {
        break;
       }
    }

  return true;
 }

bool Context::setAliasTypes(BodyNode *body_node)
 {
  for(AliasNode &node : body_node->alias_list ) node.index=0;

  AndFlag ret;

  ulen mark=0;

  for(AliasNode &node : body_node->alias_list ) ret+=setAliasTypes(node,++mark);

  return ret;
 }

struct Context::AliasDescend
 {
  TypeNode *type;

  explicit AliasDescend(AliasNode *node) : type(node->result_type) {}

  struct Rec : NoCopy
   {
    Rec *next;

    TypeNode *type;

    explicit Rec(TypeNode *type_) : type(type_) {}
   };

  template <class Func>
  struct Expand
   {
    Func func;

    explicit Expand(const Func &func_) : func(func_) {}

    template <class Add>
    struct DispFunc
     {
      Func func;
      Add add;

      DispFunc(const Func &func_,const Add &add_) : func(func_),add(add_) {}

      template <class T>
      void operator () (T *) {}

      void operator () (TypeNode::Ref *ptr)
       {
        if( AliasNode *node=ptr->ptr.castPtr<AliasNode>() ) func(node);
       }

      void operator () (TypeNode::Ptr *ptr)
       {
        add(ptr->type_node);
       }

      void operator () (TypeNode::Array *ptr)
       {
        add(ptr->type_node);
       }

      void operator () (TypeNode::ArrayLen *ptr)
       {
        add(ptr->type_node);
       }

      void operator () (TypeNode::PolyPtr *ptr)
       {
        for(auto &node : *(ptr->type_list) ) add(node.type_node);
       }
     };

    template <class Add>
    void operator () (Rec *rec,Add add)
     {
      TypeNode *type=rec->type;

      type->ptr.apply( DispFunc<Add>(func,add) );
     }
   };

  template <class Func>
  void operator () (Func func) // func(AliasNode *node)
   {
    Enlist<Rec> enlist(Expand<Func>(func),type);
   }
 };

bool Context::checkLoop1(BodyNode *body_node)
 {
  return CheckLoop<AliasDescend>(body_node->alias_list, [this] (AliasNode *node) { error("Cyclic type definition : #;",node->name); } );
 }

struct Context::StructDescend
 {
  StructNode *node;

  explicit StructDescend(StructNode *node_) : node(node_) {}

  struct Rec : NoCopy
   {
    Rec *next;

    AnyPtr<StructNode,TypeNode> ptr;

    explicit Rec(StructNode *struct_node) : ptr(struct_node) {}

    explicit Rec(TypeNode *type) : ptr(type) {}
   };

  template <class Func>
  struct Expand
   {
    Func func;

    explicit Expand(const Func &func_) : func(func_) {}

    template <class Add>
    struct DispFunc
     {
      Func func;
      Add add;

      DispFunc(const Func &func_,const Add &add_) : func(func_),add(add_) {}

      template <class T>
      void operator () (T *) {}

      void operator () (AliasNode *node)
       {
        add(node->result_type);
       }

      void operator () (StructNode *node)
       {
        func(node);
       }

      void operator () (TypeNode::Ref *ptr)
       {
        ptr->ptr.apply(*this);
       }

      void operator () (TypeNode::Array *ptr)
       {
        add(ptr->type_node);
       }

      void operator () (TypeNode::ArrayLen *ptr)
       {
        add(ptr->type_node);
       }
     };

    template <class Add>
    struct RecFunc
     {
      DispFunc<Add> func;

      RecFunc(const Func &func_,const Add &add) : func(func_,add) {}

      void operator () (StructNode *struct_node)
       {
        for(FieldNode &node : struct_node->field_list )
          {
           node.type_node->ptr.apply(func);
          }
       }

      void operator () (TypeNode *type)
       {
        type->ptr.apply(func);
       }
     };

    template <class Add>
    void operator () (Rec *rec,Add add)
     {
      rec->ptr.apply( RecFunc<Add>(func,add) );
     }
   };

  template <class Func>
  void operator () (Func func) // func(StructNode *node)
   {
    Enlist<Rec> enlist(Expand<Func>(func),node);
   }
 };

bool Context::checkLoop2(BodyNode *body_node)
 {
  if( CheckLoop<StructDescend>(body_node->struct_list, [this] (StructNode *node) { error("Cyclic struct definition : #;",node->name); } ) )
    {
     for(auto &node : body_node->struct_list ) node.dep_index=node.index;

     return true;
    }

  return false;
 }

bool Context::setDomainTypes()
 {
  AndFlag ret;

  for(DomainRefNode &node : domain_ref_list ) ret+=node.ptr->setType(error);

  return ret;
 }

Context::Context(PrintBase &msg,ulen mem_cap)
 : error(msg),
   pool(MemPool::DefaultBlockLen,mem_cap)
 {
  name_id_list.init();
  domain_ref_list.init();
 }

Context::~Context()
 {
 }

void Context::reset()
 {
  error.reset();

  pool.erase();

  name_id_list.init();
  domain_ref_list.init();
 }

BodyNode * Context::complete(BodyNode *body_node)
 {
  NameId max_id=generateIds();

  AndFlag ret;

  ret+=body_node->setMap(this,max_id);

  ret+=buildMaps(body_node,max_id);

  ret+=body_node->doLink(LinkContext(error,body_node->map));

  if( ret && setAliasTypes(body_node)
          && checkLoop1(body_node)
          && checkLoop2(body_node)
          && setDomainTypes() )
    return body_node;

  error("\nName link error\n");

  return 0;
 }

/* struct ScopeNode */

void ScopeNode::init(ExtContext ctx,const Token &token,BodyNode *body_)
 {
  parent=0;

  name.init(ctx,token);

  body=body_;

  body_->foldin(this);
 }

/* class ScopeList */

ScopeList::ScopeList(From from)
 : TempArray<ScopeNode *,100>(from.depth)
 {
  for(auto r=RangeReverse(*this); +r ;++r)
    {
     *r=from.scope;

     from.back();
    }
 }

ScopeList::~ScopeList()
 {
 }

/* struct TypeNode */

bool TypeNode::doLink(From from,LinkContext ctx)
 {
  if( !lists ) return true;

  for(auto &node : lists->from_list ) node.from=from;

  AndFlag ret;

  for(RefListNode &node : lists->ref_list ) ret+=ctx.doLink(from,node.ptr);

  return ret;
 }

const char * GetTextDesc(TypeNode::Base::Type base_type)
 {
  switch( base_type )
    {
     case TypeNode::Base::Type_none   : return "none";

     case TypeNode::Base::Type_sint   : return "sint";
     case TypeNode::Base::Type_uint   : return "uint";
     case TypeNode::Base::Type_ulen   : return "ulen";

     case TypeNode::Base::Type_sint8  : return "sint8";
     case TypeNode::Base::Type_uint8  : return "uint8";
     case TypeNode::Base::Type_sint16 : return "sint16";
     case TypeNode::Base::Type_uint16 : return "uint16";
     case TypeNode::Base::Type_sint32 : return "sint32";
     case TypeNode::Base::Type_uint32 : return "uint32";
     case TypeNode::Base::Type_sint64 : return "sint64";
     case TypeNode::Base::Type_uint64 : return "uint64";

     case TypeNode::Base::Type_text   : return "text";
     case TypeNode::Base::Type_ip     : return "ip";

     default: return "???";
    }
 }

/* struct ExprNode::Domain */

bool ExprNode::Domain::setType(ErrorMsg &error)
 {
  if( TypeNode::Base *base_ptr=ptr->result_type->ptr.castPtr<TypeNode::Base>() )
    {
     if( base_ptr->isIntegral() )
       {
        type=base_ptr->type;

        return true;
       }
    }

  error("Bad domain type : #;",*name_ref);

  return false;
 }

/* struct ExprNode */

bool ExprNode::doLink(From from,LinkContext ctx)
 {
  if( !lists ) return true;

  AndFlag ret;

  for(RefListNode &node : lists->ref_list ) ret+=ctx.doLink(from,node.ptr);

  return ret;
 }

bool ExprNode::doLinkQName(From from,LinkContext ctx)
 {
  if( !lists ) return true;

  AndFlag ret;

  for(RefListNode &node : lists->ref_list ) ret+=ctx.doLinkQName(from,node.ptr);

  return ret;
 }

/* struct AliasNode */

struct AliasNode::PickNextAlias
 {
  AliasNode * ret = 0;

  operator AliasNode * () const { return ret; }

  template <class T>
  void operator () (T *) {}

  void operator () (AliasNode *ptr)
   {
    ret=ptr;
   }

  void operator () (TypeNode::Ref *ptr)
   {
    ElaborateAnyPtr(*this,ptr->ptr);
   }
 };

AliasNode * AliasNode::getNext()
 {
  PickNextAlias ret;

  ElaborateAnyPtr(ret,type_node->ptr);

  return ret;
 }

/* class FieldMap */

void FieldMap::add(ErrorMsg &error,NameId max_id,FieldNode *node)
 {
  Algo::PrepareIns prepare(root,node->name.name_id,1,max_id);

  if( prepare.found )
    {
     error("Field name duplication : #;\n  Found field is : #;",node->name,prepare.found->name);

     ok=false;
    }
  else
    {
     prepare.complete(node);
    }
 }

FieldMap::FieldMap(ErrorMsg &error,NameId max_id,StructNode *node)
 : ok(true)
 {
  for(FieldNode &field : node->field_list ) add(error,max_id,&field);
 }

/* struct StructNode */

bool StructNode::setMap(Context *ctx,NameId max_id)
 {
  FieldMap *map=ctx->pool.create<FieldMap>(ctx->error,max_id,this);

  if( !*map ) return false;

  field_map=map;

  return true;
 }

bool StructNode::doLink(LinkContext ctx)
 {
  From from=getFieldFrom();

  AndFlag ret;

  for(FieldNode &node : field_list ) ret+=node.doLink(from,ctx);

  return ret;
 }

/* class NameLinkMap */

template <class T>
void NameLinkMap::fill(Collector<Rec> &col,T *root)
 {
  if( root )
    {
     col.append_fill(root);

     fill(col,Algo<T>::Link(root).lo);
     fill(col,Algo<T>::Link(root).hi);
    }
 }

bool NameLinkMap::check(ErrorMsg &error,RecPtr a,RecPtr b)
 {
  if( a.hasType<StructNode>() && b.hasType<NameLinkMap>() ) return true;

  if( b.hasType<StructNode>() && a.hasType<NameLinkMap>() ) return true;

  error("Same name of different kind : #; and #;",a,b);

  return false;
 }

bool NameLinkMap::check(ErrorMsg &error,PtrLen<Rec> range)
 {
  unsigned count=0;

  for(; +range ;++range)
    {
     auto r=range;

     for(++r; +r ;++r)
       {
        if( !check(error,range->ptr,r->ptr) ) count++;

        if( count>=ErrorMsgCap ) return false;
       }
    }

  return count==0;
 }

NameLinkMap * NameLinkMap::add(ElementPool &pool,NameId max_id,ScopeNode &node,BaseList<NameLinkMap> &map_list)
 {
  Algo<NameLinkMap>::PrepareIns prepare(map_root,node.name.name_id,1,max_id);

  if( prepare.found )
    {
     return prepare.found;
    }
  else
    {
     NameLinkMap *map=pool.create<NameLinkMap>(node);

     prepare.complete(map);

     map_list.add(map);

     return map;
    }
 }

bool NameLinkMap::add(ErrorMsg &error,NameId max_id,AliasNode &node)
 {
  Algo<AliasNode>::PrepareIns prepare(alias_root,node.name.name_id,1,max_id);

  if( prepare.found )
    {
     error("Type name duplication : #; , \n  Found object is : #;",node.name,prepare.found->name);

     return false;
    }

  prepare.complete(&node);

  return true;
 }

bool NameLinkMap::add(ErrorMsg &error,NameId max_id,ConstNode &node)
 {
  Algo<ConstNode>::PrepareIns prepare(const_root,node.name.name_id,1,max_id);

  if( prepare.found )
    {
     error("Constant name duplication : #; , \n  Found object is : #;",node.name,prepare.found->name);

     return false;
    }

  prepare.complete(&node);

  return true;
 }

bool NameLinkMap::add(ErrorMsg &error,NameId max_id,StructNode &node)
 {
  Algo<StructNode>::PrepareIns prepare(struct_root,node.name.name_id,1,max_id);

  if( prepare.found )
    {
     error("Structure name duplication : #; , \n  Found object is : #;",node.name,prepare.found->name);

     return false;
    }

  prepare.complete(&node);

  return true;
 }

bool NameLinkMap::check(ErrorMsg &error)
 {
  Collector<Rec> col;

  fill(col,alias_root.root);
  fill(col,const_root.root);
  fill(col,struct_root.root);
  fill(col,map_root.root);

  AndFlag ret;

  Algon::IncrSortThenApplyUniqueRangeBy(col.flat(), [] (const Rec &rec) { return rec.name->name_id; } ,
                                                    [&] (PtrLen<Rec> range)
                                                        {
                                                         ret+=check(error,range);
                                                        }
                                       );

  return ret;
 }

template <class Error>
NameLinkMap * NameLinkMap::findMap(Error &error,From from,NameNodeList *name_list)
 {
  ScopeList list(from);

  NameLinkMap *map=this;

  for(ScopeNode *node : list )
    {
     map=map->findMap(node->name);

     if( !map )
       {
        error("Undefined scope : #;",node->name);

        return 0;
       }
    }

  for(NameNode &node : NoLast<NameNode>(name_list->list) )
    {
     map=map->findMap(node.name);

     if( !map )
       {
        error("Undefined scope : #;",node.name);

        return 0;
       }
    }

  return map;
 }

NameLinkMap * NameLinkMap::tryFindMap(From from,NameNodeList *name_list)
 {
  NoError error;

  return findMap(error,from,name_list);
 }

template <class Func>
bool NameLinkMap::tryFindNode(From from,NameNodeList *name_list,Func func)
 {
  if( auto *map=tryFindMap(from,name_list) )
    {
     if( func(map,name_list->getLastName()) ) return true;
    }

  return false;
 }

template <class Error,class Func>
void NameLinkMap::findNode(Error &error,From from,NameNodeList *name_list,Func func)
 {
  if( auto *map=findMap(error,from,name_list) )
    {
     if( func(map,name_list->getLastName()) ) return;
    }

  error("Undefined name : #;",*name_list);
 }

template <class Error,class Func>
void NameLinkMap::findNode(Error &error,From from,NameRefNode *name_ref,Func func)
 {
  auto *name_list=name_ref->name_list;

  switch( name_ref->type )
    {
     case NameRefNode::Type_rel :
      {
       do
         {
          if( tryFindNode(from,name_list,func) ) return;
         }
       while( from.back() );

       error("Undefined name : #;",*name_ref);
      }
     break;

     case NameRefNode::Type_abs :
      {
       findNode(error,From(),name_list,func);
      }
     break;

     case NameRefNode::Type_this :
      {
       findNode(error,from,name_list,func);
      }
     break;

     case NameRefNode::Type_dots :
      {
       for(ulen cnt=name_ref->dots_count-1; cnt ;cnt--)
         if( !from.back() )
           {
            error("Undefined name, too many dots : #;",*name_ref);

            return;
           }

       findNode(error,from,name_list,func);
      }
     break;
    }
 }

template <class Error>
ConstNode * NameLinkMap::findConst(Error &error,From from,NameRefNode *name_ref)
 {
  ConstNode *ret=0;

  findNode(error,from,name_ref, [&] (NameLinkMap *map,const NameIdNode &name)
                                    {
                                     ret=map->findConst(name);

                                     return ret;
                                    }
          );

  return ret;
 }

AliasNode * NameLinkMap::findAlias(ErrorMsg &error,From from,NameRefNode *name_ref)
 {
  AliasNode *ret=0;

  findNode(error,from,name_ref, [&] (NameLinkMap *map,const NameIdNode &name)
                                    {
                                     ret=map->findAlias(name);

                                     return ret;
                                    }
          );

  return ret;
 }

AnyPtr<AliasNode,StructNode> NameLinkMap::findAliasStruct(ErrorMsg &error,From from,NameRefNode *name_ref)
 {
  AnyPtr<AliasNode,StructNode> ret;

  findNode(error,from,name_ref, [&] (NameLinkMap *map,const NameIdNode &name)
                                    {
                                     if( auto *ptr=map->findAlias(name) )
                                       {
                                        ret=ptr;

                                        return true;
                                       }

                                     if( auto *ptr=map->findStruct(name) )
                                       {
                                        ret=ptr;

                                        return true;
                                       }

                                     return false;
                                    }
          );

  return ret;
 }

bool NameLinkMap::doLink(ErrorMsg &error,From from,TypeNode::Ref *ptr)
 {
  auto *name_ref=ptr->name_ref;

  auto result=findAliasStruct(error,from,name_ref);

  if( +result )
    {
     ptr->ptr=result;

     return true;
    }

  return false;
 }

bool NameLinkMap::doLink(ErrorMsg &error,From from,ExprNode::Domain *ptr)
 {
  auto *name_ref=ptr->name_ref;

  if( name_ref->name_list->qname_flag )
    {
     error("QName is not allowed here : #;",*name_ref);

     return false;
    }

  if( auto *result=findAlias(error,from,name_ref) )
    {
     ptr->ptr=result;

     return true;
    }

  return false;
 }

bool NameLinkMap::doLink(ErrorMsg &error,From from,ExprNode::Ref *ptr)
 {
  auto *name_ref=ptr->name_ref;

  if( name_ref->name_list->qname_flag )
    {
     error("QName is not allowed here : #;",*name_ref);

     return false;
    }

  if( auto *result=findConst(error,from,name_ref) )
    {
     ptr->ptr=result;

     return true;
    }

  return false;
 }

bool NameLinkMap::doLinkQName(ErrorMsg &error,From from,ExprNode::Ref *ptr)
 {
  auto *name_ref=ptr->name_ref;

  if( name_ref->name_list->qname_flag ) return true;

  if( auto *result=findConst(error,from,name_ref) )
    {
     ptr->ptr=result;

     return true;
    }

  return false;
 }

ConstNode * NameLinkMap::doLink(From from,ExprNode::Ref *ptr)
 {
  if( ConstNode *ret=ptr->ptr ) return ret;

  NoError error;

  return findConst(error,from,ptr->name_ref);
 }

/* struct LinkContext */

struct LinkContext::Func
 {
  LinkContext *ctx;
  From from;
  bool ret;

  Func(LinkContext *ctx_,From from_)
   : ctx(ctx_),
     from(from_),
     ret(false)
   {
   }

  operator bool() const { return ret; }

  template <class T>
  void operator () (T *ptr)
   {
    ret=ctx->doLink(from,ptr);
   }
 };

struct LinkContext::FuncQName
 {
  LinkContext *ctx;
  From from;
  bool ret;

  FuncQName(LinkContext *ctx_,From from_)
   : ctx(ctx_),
     from(from_),
     ret(false)
   {
   }

  operator bool() const { return ret; }

  template <class T>
  void operator () (T *ptr)
   {
    ret=ctx->doLinkQName(from,ptr);
   }
 };

bool LinkContext::doLink(From from,AnyPtr<TypeNode::Ref,ExprNode::Domain,ExprNode::Ref> ptr)
 {
  Func ret(this,from);

  ElaborateAnyPtr(ret,ptr);

  return ret;
 }

bool LinkContext::doLinkQName(From from,AnyPtr<TypeNode::Ref,ExprNode::Domain,ExprNode::Ref> ptr)
 {
  FuncQName ret(this,from);

  ElaborateAnyPtr(ret,ptr);

  return ret;
 }

/* struct BodyNode */

void BodyNode::add(TypeNode *type_node)
 {
  if( auto *lists=type_node->lists )
    {
     lists->fillFromList();

     len_list.join(lists->len_list);

     for(StructNode &node : lists->struct_list ) add(&node.scope_node);

     struct_list.join(lists->struct_list);
    }
 }

void BodyNode::foldin(ScopeNode *scope)
 {
  for(AliasNode &node : alias_list ) node.setParent(scope);

  for(ConstNode &node : const_list ) node.setParent(scope);

  for(StructNode &node : struct_list ) node.setParent(scope);

  for(ScopeNode &node : scope_list ) node.setParent(scope);
 }

bool BodyNode::setMap(Context *ctx,NameId max_id)
 {
  AndFlag result;

  for(StructNode &node : struct_list ) result+=node.setMap(ctx,max_id);

  return result;
 }

bool BodyNode::doLink(LinkContext ctx)
 {
  AndFlag ret;

  for(AliasNode &node : alias_list ) ret+=node.doLink(ctx);

  for(ConstNode &node : const_list ) ret+=node.doLink(ctx);

  for(StructNode &node : struct_list ) ret+=node.doLink(ctx);

  return ret;
 }

} // namespace DDL
} // namespace CCore
