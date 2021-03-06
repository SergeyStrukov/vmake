/* DDLEval.h */
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

#ifndef CCore_inc_ddl_DDLEval_h
#define CCore_inc_ddl_DDLEval_h

#include <CCore/inc/StepEval.h>
#include <CCore/inc/PrintStem.h>

#include <CCore/inc/ddl/DDLSemantic.h>
#include <CCore/inc/ddl/DDLValue.h>

namespace CCore {
namespace DDL {

/* consts */

inline constexpr ulen PrintTypeCap = 100 ;

inline constexpr ulen PrintOffCap = 200 ;

/* functions */

template <class T,class ... SS>
void ExtIntType(T &obj,TypeNode::Base::Type type,SS && ... ss)
 {
  switch( type )
    {
     case TypeNode::Base::Type_sint   : obj.template op<imp_sint>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_uint   : obj.template op<imp_uint>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_ulen   : obj.template op<imp_ulen>( std::forward<SS>(ss)... ); break;

     case TypeNode::Base::Type_sint8  : obj.template op<imp_sint8>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_uint8  : obj.template op<imp_uint8>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_sint16 : obj.template op<imp_sint16>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_uint16 : obj.template op<imp_uint16>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_sint32 : obj.template op<imp_sint32>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_uint32 : obj.template op<imp_uint32>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_sint64 : obj.template op<imp_sint64>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_uint64 : obj.template op<imp_uint64>( std::forward<SS>(ss)... ); break;
    }
 }

template <class T,class ... SS>
void ExtBaseType(T &obj,TypeNode::Base::Type type,SS && ... ss)
 {
  switch( type )
    {
     case TypeNode::Base::Type_sint   : obj.template op<imp_sint>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_uint   : obj.template op<imp_uint>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_ulen   : obj.template op<imp_ulen>( std::forward<SS>(ss)... ); break;

     case TypeNode::Base::Type_sint8  : obj.template op<imp_sint8>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_uint8  : obj.template op<imp_uint8>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_sint16 : obj.template op<imp_sint16>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_uint16 : obj.template op<imp_uint16>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_sint32 : obj.template op<imp_sint32>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_uint32 : obj.template op<imp_uint32>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_sint64 : obj.template op<imp_sint64>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_uint64 : obj.template op<imp_uint64>( std::forward<SS>(ss)... ); break;

     case TypeNode::Base::Type_text   : obj.template op<Text>( std::forward<SS>(ss)... ); break;
     case TypeNode::Base::Type_ip     : obj.template op<IP>( std::forward<SS>(ss)... ); break;
    }
 }

template <class T,class ... SS>
void UseIntType(T &obj,SS && ... ss)
 {
  ExtIntType(obj,obj.type, std::forward<SS>(ss)... );
 }

template <class T,class ... SS>
void UseBaseType(T &obj,SS && ... ss)
 {
  ExtBaseType(obj,obj.type, std::forward<SS>(ss)... );
 }

/* classes */

struct TypeAdapter;

struct ConstResult;

struct LenResult;

struct EvalResult;

struct PrintName;

struct PrintType;

struct PrintValue;

class EvalContext;

/* struct TypeAdapter */

struct TypeAdapter
 {
  using AnyPtrType = AnyPtr<TypeNode::Base,TypeNode::Ptr,TypeNode::PolyPtr,TypeNode::Array,TypeNode::ArrayLen,StructNode> ;

  AnyPtrType ptr;

  template <class P>
  void operator () (P *type_ptr)
   {
    ptr=type_ptr;
   }

  void operator () (AliasNode *alias_node)
   {
    alias_node->result_type->ptr.apply( FunctorRef(*this) );
   }

  void operator () (StructNode *struct_node)
   {
    ptr=struct_node;
   }

  void operator () (TypeNode::Ref *type_ptr)
   {
    type_ptr->ptr.apply( FunctorRef(*this) );
   }

  void operator () (TypeNode::Struct *type_ptr)
   {
    ptr=type_ptr->struct_node;
   }

  explicit TypeAdapter(TypeNode *type)
   {
    type->ptr.apply( FunctorRef(*this) );
   }

  template <class Ret,class FuncInit>
  static Ret Binary(TypeNode *a,TypeNode *b,FuncInit func_init)
   {
    return AnyPtrType::Binary<Ret>(TypeAdapter(a).ptr,TypeAdapter(b).ptr,func_init);
   }
 };

/* IsStructType() */

inline StructNode * IsStructType(TypeNode *type) { return TypeAdapter(type).ptr.castPtr<StructNode>(); }

/* struct ConstResult */

struct ConstResult
 {
  TypeNode *type;
  ConstNode *node;
  Value value;

  void set(ConstNode *node_,const Value &value_)
   {
    type=node_->type_node;
    node=node_;
    value=value_;
   }

  struct PrintOptType
   {
    EvalResult &result;

    PrintOptType(EvalResult &result_) : result(result_) {}
   };

  void print(PrinterType auto &out,PrintOptType opt) const;
 };

/* struct LenResult */

struct LenResult
 {
  LenNode *node;
  ulen_type value;

  void set(LenNode *node_,ulen_type value_)
   {
    node=node_;
    value=value_;
   }

  void print(PrinterType auto &out) const
   {
    Printf(out,"#; = #;",*node,value);
   }
 };

/* struct EvalResult */

struct EvalResult : NoCopy
 {
  DynArray<ConstResult> const_table;
  DynArray<LenResult> len_table;

  EvalResult() {}

  void erase()
   {
    const_table.erase();
    len_table.erase();
   }

  ulen_type getLen(const LenNode &node) const { return len_table[node.index].value; }
 };

/* struct PrintName */

struct PrintName
 {
  ScopeNode *scope;
  ulen depth;

  StrLen name;

  template <class Node>
  explicit PrintName(Node *node) : scope(node->parent),depth(node->depth),name(node->name.getStr()) {}

  void print(PrinterType auto &out) const
   {
    TempArray<ScopeNode *,25> temp(depth);

    ScopeNode *node=scope;

    for(ScopeNode * &ptr : temp )
      {
       ptr=node;
       node=node->parent;
      }

    for(ScopeNode * ptr : RangeReverse(temp) )
      {
       Putch(out,'#');
       Putobj(out,ptr->name.getStr());
      }

    Putch(out,'#');
    Putobj(out,name);
   }
 };

/* struct PrintType */

struct PrintType
 {
  EvalResult &result;
  TypeNode *type;
  ulen cap;

  PrintType(EvalResult &result_,TypeNode *type_,ulen cap_=PrintTypeCap) : result(result_),type(type_),cap(cap_) {}

  void operator () (PrinterType auto &out,TypeNode::Base *type_ptr) const
   {
    Putobj(out,type_ptr->type);
   }

  void operator () (PrinterType auto &out,TypeNode::Ptr *type_ptr) const
   {
    if( cap )
      {
       Printf(out,"#; *",PrintType(result,type_ptr->type_node,cap-1));
      }
    else
      {
       Putobj(out,"... *"_c);
      }
   }

  void operator () (PrinterType auto &out,TypeNode::PolyPtr *type_ptr) const
   {
    if( cap )
      {
       Putobj(out,"{"_c);

       PrintFirst stem(" "_c," , "_c);

       for(auto &node : *type_ptr->type_list )
         {
          Putobj(out,*(stem++),PrintType(result,node.type_node,cap-1));
         }

       Putobj(out," } *"_c);
      }
    else
      {
       Putobj(out,"{ ... } *"_c);
      }
   }

  void operator () (PrinterType auto &out,TypeNode::Array *type_ptr) const
   {
    if( cap )
      {
       Printf(out,"#; []",PrintType(result,type_ptr->type_node,cap-1));
      }
    else
      {
       Printf(out,"... []");
      }
   }

  void operator () (PrinterType auto &out,TypeNode::ArrayLen *type_ptr) const
   {
    auto len=result.getLen(type_ptr->len_node);

    if( cap )
      {
       Printf(out,"#; [#;]",PrintType(result,type_ptr->type_node,cap-1),len);
      }
    else
      {
       Printf(out,"... [#;]",len);
      }
   }

  void operator () (PrinterType auto &out,StructNode *struct_node) const
   {
    Printf(out,"struct #;",PrintName(struct_node));
   }

  void print(PrinterType auto &out) const
   {
    ElaborateAnyPtr(*this,out,TypeAdapter(type).ptr);
   }
 };

/* struct PrintValue */

struct PrintValue
 {
  EvalResult &result;
  TypeNode *type;
  Value value;
  ulen off;

  PrintValue(EvalResult &result_,TypeNode *type_,const Value &value_,ulen off_=0)
   : result(result_),type(type_),value(value_),off(off_) {}

  template <class T>
  void op(PrinterType auto &out,TypeNode::Base::Type type) const
   {
    Printf(out,"#;(#;) #;",RepeatChar(off,' '),type,value.get<T>());
   }

  void operator () (PrinterType auto &out,TypeNode::Base *type_ptr) const
   {
    ExtBaseType(*this,type_ptr->type,out,type_ptr->type);
   }

  static void PrintPtr(PrinterType auto &out,EvalResult &result,PtrNode *ptr_node,ulen cap=PrintTypeCap)
   {
    if( PtrNode *parent=ptr_node->parent )
      {
       if( cap )
         PrintPtr(out,result,parent,cap-1);
       else
         Putobj(out,"..."_c);

       if( StructNode *struct_node=IsStructType(parent->type) )
         {
          FieldNode &field=struct_node->getField(ptr_node->index);

          Printf(out,".#;",field.name.getStr());
         }
       else
         {
          Printf(out,"[#;]",ptr_node->index);
         }
      }
    else
      {
       Printf(out,"#;",PrintName(result.const_table[ptr_node->index].node));
      }
   }

  void printPtr(PrinterType auto &out) const
   {
    Ptr ptr=value.get<Ptr>();

    Putobj(out,RepeatChar(off,' '));

    if( !ptr )
      {
       Printf(out,"(??? *) null");
      }
    else if( ptr.null )
      {
       Printf(out,"(#; *) null",PrintType(result,ptr.getType()));
      }
    else
      {
       Putobj(out,"-> "_c);

       PrintPtr(out,result,ptr.ptr_node);
      }
   }

  void operator () (PrinterType auto &out,TypeNode::Ptr *) const
   {
    printPtr(out);
   }

  void operator () (PrinterType auto &out,TypeNode::PolyPtr *) const
   {
    printPtr(out);
   }

  void printArrayInner(PrinterType auto &out,TypeNode *type,PtrLen<Value> data,ulen off) const
   {
    PrintFirst stem(""_c,",\n"_c);

    for(const Value &obj : data ) Putobj(out,*(stem++),PrintValue(result,type,obj,off));
   }

  void printArray(PrinterType auto &out,TypeNode *type) const
   {
    PtrLen<Value> data=value.get<Block>().data;

    if( +data )
      {
       if( off )
         {
          if( off>PrintOffCap )
            {
             Putobj(out,RepeatChar(off,' '),"{ ... }"_c);

             return;
            }

          Putobj(out,RepeatChar(off,' ')," {\n"_c);

          printArrayInner(out,type,data,off+2);

          Putch(out,'\n');
          Putobj(out,RepeatChar(off,' ')," }"_c);
         }
       else
         {
          Putobj(out,"\n {\n"_c);

          printArrayInner(out,type,data,off+2);

          Putobj(out,"\n }"_c);
         }
      }
    else
      {
       Putobj(out,RepeatChar(off,' '),"{ }"_c);
      }
   }

  void operator () (PrinterType auto &out,TypeNode::Array *type_ptr) const
   {
    printArray(out,type_ptr->type_node);
   }

  void operator () (PrinterType auto &out,TypeNode::ArrayLen *type_ptr) const
   {
    printArray(out,type_ptr->type_node);
   }

  void printStructFields(PrinterType auto &out,StructNode *struct_node,PtrLen<Value> data,ulen off) const
   {
    PrintFirst stem(""_c,",\n"_c);

    for(FieldNode &field : struct_node->field_list )
       {
        Putobj(out,*(stem++),PrintValue(result,field.type_node,*data,off));

        ++data;
       }
   }

  void operator () (PrinterType auto &out,StructNode *struct_node) const
   {
    PtrLen<Value> data=value.get<Block>().data;

    if( +data )
      {
       if( off )
         {
          if( off>PrintOffCap )
            {
             Putobj(out,RepeatChar(off,' '),"{ ... }"_c);

             return;
            }

          Putobj(out,RepeatChar(off,' ')," {\n"_c);

          printStructFields(out,struct_node,data,off+2);

          Putch(out,'\n');
          Putobj(out,RepeatChar(off,' ')," }"_c);
         }
       else
         {
          Putobj(out,"\n {\n"_c);

          printStructFields(out,struct_node,data,off+2);

          Putobj(out,"\n }"_c);
         }
      }
    else
      {
       Putobj(out,RepeatChar(off,' '),"{ }"_c);
      }
   }

  void print(PrinterType auto &out) const
   {
    ElaborateAnyPtr(*this,out,TypeAdapter(type).ptr);
   }
 };

void ConstResult::print(PrinterType auto &out,PrintOptType opt) const
 {
  Printf(out,"#; #; = #;",PrintType(opt.result,type),PrintName(node),PrintValue(opt.result,type,value));
 }

/* class EvalContext */

class EvalContext : NoCopy
 {
   ErrorMsg &error;
   ElementPool &pool;
   NameLinkMap *map;

   TypeNode * int_type[TypeNode::Base::Type::TypeIntMax+1];

  private:

   using Eval = StepEval<EvalContext> ;

   struct ConstRec;
   struct LenRec;
   struct FieldRec;

   struct CmpLenStep;
   struct CmpArrayLenStep;
   struct CmpListStep;
   struct JoinLoopStep;
   struct JoinStep;
   struct SortStep;
   struct CmpTypeStep;

   struct CmpToOkStep;
   struct CheckTypeListStep;
   struct CheckTypeStep;

   struct MinusStep;
   struct AddStep;
   struct AddTextStep;
   struct MulStep;
   struct DivStep;
   struct RemStep;

   struct ExpandArray;
   struct NullArrayStep;
   struct NullStep;

   struct CopyStep;

   struct DefArrayStep;
   struct DefStep;

   struct CastIntToIntStep;
   struct CastIntToTextStep;

   struct ArrayLenStep;

   struct PtrOrSLen;
   struct PtrOrInt;

   struct Ops;
   struct OpsSLen;
   struct Trio;

   struct CompleteAddPtrStep;
   struct AddPtrStep;
   struct AddAnyPtrStep;
   struct CompleteSubStep;
   struct SubStep;
   struct SubPtrStep;
   struct SubAnyPtrStep;

   struct OpNumber;

   struct LValue;

   struct CompleteReadStep;
   struct ReadPtrStep;
   struct CastSameTypeStep;
   struct CastValueStep;
   struct DecayStep;
   struct CastStep;

   struct CompleteCheckPtrStep;
   struct CheckPtrStep;

   struct SelectFieldStep;

   struct CastValueToPtrStep;
   struct CastToPtrStep;

   struct CastToSLenOp;
   struct CastValueToSLenStep;
   struct CastToSLenStep;

   struct CastValueToPtrOrSLenStep;
   struct CastToPtrOrSLenStep;

   struct SLenMinusStep;
   struct SLenAddStep;
   struct SLenMulStep;
   struct SLenDivStep;
   struct SLenRemStep;
   struct SLenSubStep;
   struct CastIntToSLenStep;

   template <class T> struct SetPtrStep;
   struct SetSLenStep;

   struct PtrOrSLenAddStep;
   struct PtrOrSLenSubStep;

   struct CastValueToPtrOrIntStep;
   struct CastToPtrOrIntStep;

   struct PtrOrIntAddStep;
   struct PtrOrIntSubStep;

   struct TrioAddStep;
   struct TrioSubStep;

   struct CastValueToTrioStep;
   struct CastToTrioStep;

   struct DuoMinusStep;
   struct DuoAddStep;
   struct DuoMulStep;
   struct DuoDivStep;
   struct DuoRemStep;
   struct CastIntToDuoStep;

   struct DuoSubStep;

   struct CastValueToDuoStep;
   struct CastToDuoStep;

   struct CastValueToStructStep;
   struct CastToStructStep;

   struct ExprAddressStep;

   struct ExprSLenStep;
   struct ExprPtrOrSLenStep;
   struct ExprPtrOrIntStep;
   struct ExprDuoStep;
   struct ExprTrioStep;
   struct ExprPtrStep;

   struct ExprStructStep;

   struct ExprStep;
   struct CheckPtrBoundStep;
   struct LockStep;
   struct ExprLockStep;
   struct GateStep;

   static void CastField(Eval &eval,Value *ret,TypeNode *type,Ptr ptr,const Value &src,From from,const PosName &ops,StepEvalStepId dep);

   static void NullStruct(Eval &eval,Value *result,bool *flags,StructNode *struct_node,StepEvalStepId dep);

   static void DefStruct(Eval &eval,Value *result,bool *flags,StructNode *struct_node,From from,StepEvalStepId dep);

  private:

   SimpleArray<ConstRec> const_list;
   SimpleArray<LenRec> len_list;
   SimpleArray<FieldRec> field_list;

  private:

   // utilities

   static bool OR(bool a,bool b) { return a || b ; }

   ConstRec & getConst(Ptr ptr); // ptr is not null

   struct ReadPtrResult
    {
     Value value;
     bool ok;

     ReadPtrResult(NothingType) : ok(false) {}

     ReadPtrResult(const Value &value_) : value(value_),ok(true) {}

     bool operator ! () const { return !ok; }
    };

   ReadPtrResult readPtr(Value val,Ptr ptr); // ptr is not null

   struct NeedLockFunc;

   static bool NeedLock(TypeNode *type,unsigned level=100);

   Text add(Text arg1,Text arg2) { return pool.cat(arg1.str,arg2.str); }

   struct NullBaseOp;

   static Value NullBaseType(TypeNode::Base::Type type);

   Ptr nullPtr(TypeNode *type);

   void guardLen(ulen len);

   PtrLen<Value> block(ulen len) { guardLen(len); return pool.createArray<Value>(len); }

   PtrLen<bool> flags(ulen len) { return pool.createArray<bool>(len); }

   PtrLen<Value> block(LenRec &rec);

   template <class T>
   Text castToText(const T &obj);

   struct NotArrayFunc;

   static bool NotArray(TypeNode *type);

   Ptr add(Ptr ptr,SLen delta,const PosName &ops); // ptr is not null

   static bool SamePtr(PtrNode *a,PtrNode *b);

   SLen sub(Ptr a,Ptr b,const PosName &ops); // a,b are not null

   Ptr ptrToField(Ptr ptr,FieldNode *field);

   Ptr ptrToField(Ptr ptr,const NameIdNode &name,const PosName &ops); // ptr is not null

   struct PtrTypeFunc;

   static AnyPtr<TypeNode,TypeList> GetPointerElemType(TypeNode *ptr_type_expected);

   struct ArrayTypeFunc;

   static TypeNode * GetArrayElemType(TypeNode *array_type_expected);

   struct DecayCase
    {
     AnyPtr<TypeNode,TypeList> type_set;
     TypeNode *type;

     DecayCase(TypeNode *ptr_type_expected,TypeNode *array_type_expected);
    };

   struct DecayAnyCase
    {
     TypeNode *type;

     explicit DecayAnyCase(TypeNode *array_type_expected);
    };

   Ptr decay(Ptr ptr,TypeNode *type);

   ConstNode * doLink(From from,ExprNode::Ref *expr_ptr) { return map->doLink(from,expr_ptr); }

   Ptr ptrTo(ConstNode *node);

   struct NotPtrFunc;

   static bool NotPtr(TypeNode *type);

   Text string(const Token &token);

   static IP ToIP(const LitIP &lit);

   // main

   bool process(Eval &eval,BodyNode *body_node,EvalResult &result);

  public:

   EvalContext(ErrorMsg &error,ElementPool &pool);

   ~EvalContext();

   DefaultHeapAlloc getAlloc() { return DefaultHeapAlloc(); }

   static bool Process(ErrorMsg &error,ElementPool &pool,BodyNode *body_node,EvalResult &result);
 };

} // namespace DDL
} // namespace CCore

#endif

