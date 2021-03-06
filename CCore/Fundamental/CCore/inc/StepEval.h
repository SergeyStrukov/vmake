/* StepEval.h */
//----------------------------------------------------------------------------------------
//
//  Project: CCore 4.01
//
//  Tag: Fundamental
//
//  License: Boost Software License - Version 1.0 - August 17th, 2003
//
//            see http://www.boost.org/LICENSE_1_0.txt or the local copy
//
//  Copyright (c) 2020 Sergey Strukov. All rights reserved.
//
//----------------------------------------------------------------------------------------

#ifndef CCore_inc_StepEval_h
#define CCore_inc_StepEval_h

#include <CCore/inc/FunctorType.h>
#include <CCore/inc/List.h>
#include <CCore/inc/NewDelete.h>

namespace CCore {
namespace StepEvalPrivate {

/* classes */

struct StepId;

template <class T> struct RetStep;

template <class T,class StepEval> struct CallFinal;

template <class T,class StepEval> struct CallMain;

template <class Ctx> class StepEval;

/* struct StepId */

struct StepId
 {
  void *ptr;
 };

/* struct RetStep<T> */

template <class T>
struct RetStep
 {
  T &obj;
  StepId id;

  RetStep(T &obj_,StepId id_) : obj(obj_),id(id_) {}
 };

/* concept Has_final<T,S> */

template <class T,class S>
concept Has_final = requires()
 {
  Ground< void (T::*)(S &) >( &T::final );
 } ;

/* struct CallFinal<T,StepEval> */

template <class T,class StepEval> requires ( Has_final<T,StepEval> )
struct CallFinal<T,StepEval>
 {
  static void Do(T &obj,StepEval &eval) { obj.final(eval); }
 };

template <class T,class StepEval>
struct CallFinal
 {
  static void Do(T &,StepEval &) {}
 };

/* struct CallMain<T,StepEval> */

template <class T,class StepEval>
struct CallMain
 {
  static void Do(T &obj,StepEval &,StepId) { obj(); }
 };

template <class T,class StepEval> requires ( FuncArgType<T,StepEval &> )
struct CallMain<T,StepEval>
 {
  static void Do(T &obj,StepEval &eval,StepId) { obj(eval); }
 };

template <class T,class StepEval> requires ( FuncArgType<T,StepEval &,StepId> )
struct CallMain<T,StepEval>
 {
  static void Do(T &obj,StepEval &eval,StepId dep) { obj(eval,dep); }
 };

/* class StepEval<Ctx> */

template <class Ctx>
class StepEval : public Ctx
 {
  private:

   StepEval(const StepEval &) = delete ;

   void operator = (const StepEval &) = delete ;

  private:

   struct NodeBase : NoCopy
    {
     DLink<NodeBase> link;

     ulen lock_count;
     NodeBase *dep;
     bool gated;

     NodeBase(StepId dep_,bool gated_)
      {
       lock_count=0;
       dep=static_cast<NodeBase *>(dep_.ptr);
       gated=gated_;
      }

     virtual ~NodeBase() {}

     StepId getDep() const { return {dep}; }

     bool notLocked() const { return !lock_count && !gated ; }

     bool incLock()
      {
       bool ret=notLocked();

       lock_count++;

       return ret;
      }

     bool decLock()
      {
       --lock_count;

       return notLocked();
      }

     virtual Space objSpace()=0;

     virtual void call(StepEval &eval)=0;

     virtual void final(StepEval &eval)=0;
    };

   template <class T>
   struct Node : NodeBase
    {
     T obj;

     template <class Init>
     Node(Init init,StepId dep,bool gated) : NodeBase(dep,gated),obj(init) {}

     virtual ~Node() {}

     virtual Space objSpace() { return Space(this,sizeof (*this)); }

     virtual void call(StepEval &eval) { CallMain<T,StepEval>::Do(obj,eval,this->getDep()); }

     virtual void final(StepEval &eval) { CallFinal<T,StepEval>::Do(obj,eval); }

     RetStep<T> getRetStep() { return RetStep<T>(obj,{(NodeBase *)this}); }
    };

  public:

   class Gate;

  private:

   struct EmptyStep
    {
     EmptyStep() {}

     void operator () () {}
    };

   template <class FuncInit>
   struct GateStep
    {
     FuncInit func_init;
     Gate *gate;

     GateStep(const FuncInit &func_init_,Gate *gate_) : func_init(func_init_),gate(gate_) {}

     struct FunctorType : NoCopy
      {
       using Type = FunctorTypeOf<FuncInit> ;

       Type obj;
       Gate *gate;

       explicit FunctorType(GateStep<FuncInit> init) : obj(init.func_init),gate(init.gate) {}

       void operator () (StepEval &eval)
        {
         CallMain<Type,StepEval>::Do(obj,eval,{0});

         gate->open();
        }

       void final(StepEval &eval)
        {
         CallFinal<Type,StepEval>::Do(obj,eval);
        }
      };
    };

   template <class T>
   struct GateNode : NodeBase
    {
     T obj;
     Gate *gate;

     template <class Init>
     GateNode(Init init,StepId dep,Gate *gate_) : NodeBase(dep,false),obj(init),gate(gate_) {}

     virtual ~GateNode() {}

     virtual Space objSpace() { return Space(this,sizeof (*this)); }

     virtual void call(StepEval &eval)
      {
       CallMain<T,StepEval>::Do(obj,eval,this->getDep());

       gate->gate_node=0;
      }

     virtual void final(StepEval &eval) { CallFinal<T,StepEval>::Do(obj,eval); }
    };

   using Algo = typename DLink<NodeBase>::template LinearAlgo<&NodeBase::link> ;

   typename Algo::Top ready_list;
   typename Algo::Top locked_list;

  private:

   using Ctx::getAlloc;

   template <class FuncInit>
   auto createNode(const FuncInit &func_init,StepId dep,bool gated)
    {
     return New< Node< FunctorTypeOf<FuncInit> > >(getAlloc(),func_init,dep,gated);
    }

   template <class FuncInit>
   auto createGateNode(const FuncInit &func_init,StepId dep,Gate *gate)
    {
     return New< GateNode< FunctorTypeOf<FuncInit> > >(getAlloc(),func_init,dep,gate);
    }

   void destroy(NodeBase *node)
    {
     Delete_dynamic(getAlloc(),node);
    }

   void lockStep(NodeBase *node);

   void unlockStep(NodeBase *node);

   void boostStep(NodeBase *node);

  public:

   class Gate : public NoCopy
    {
      SLink<Gate> link;

      StepEval *eval;
      NodeBase *gate_node;

      typename Algo::Top list;
      bool opened;

      friend class StepEval;

     public:

      explicit Gate(StepEval *eval);

      ~Gate();

      template <class FuncInit>
      auto createStep(const FuncInit &func_init,StepId dep={0}); // dep executes after

      void delay(StepId dep) { createStep(EmptyStep(),dep); }

      void open();

      void boost() { eval->boostStep(gate_node); }
    };

  private:

   using GateAlgo = typename SLink<Gate>::template LinearAlgo<&Gate::link> ;

   typename GateAlgo::Top gate_list;

  private:

   class Guard : NoCopy
    {
      StepEval *eval;
      NodeBase *node;

     public:

      Guard(StepEval *eval_,NodeBase *node_)
       {
        eval=eval_;
        node=node_;
       }

      ~Guard()
       {
        eval->destroy(node);
       }
    };

   void destroy(Gate *gate)
    {
     Delete(getAlloc(),gate);
    }

   template <class List>
   void destroyList(List &list)
    {
     while( auto *node=list.del() ) destroy(node);
    }

   template <class List>
   void finalList(List &list)
    {
     for(auto cur=list.start(); +cur ;++cur) cur->final(*this);
    }

   template <class FuncInit>
   void createGateStep(const FuncInit &func_init,StepId dep,Gate *gate);

  public:

   template <class ... SS>
   explicit StepEval(SS && ... ss) : Ctx( std::forward<SS>(ss)... ) {}

   ~StepEval();

   Gate * createGate();

   template <class OpenFuncInit,class FuncInit>
   Gate * createGate(const OpenFuncInit &openfunc_init,const FuncInit &func_init);

   template <class FuncInit>
   auto createStep(const FuncInit &func_init,StepId dep={0}); // dep executes after

   void run();
 };

template <class Ctx>
void StepEval<Ctx>::lockStep(NodeBase *node)
 {
  if( node )
    {
     if( node->incLock() )
       {
        ready_list.del(node);
        locked_list.ins(node);
       }
    }
 }

template <class Ctx>
void StepEval<Ctx>::unlockStep(NodeBase *node)
 {
  if( node )
    {
     if( node->decLock() )
       {
        locked_list.del(node);
        ready_list.ins(node);
       }
    }
 }

template <class Ctx>
void StepEval<Ctx>::boostStep(NodeBase *node)
 {
  if( node )
    {
     if( node->notLocked() )
       {
        ready_list.del(node);
        ready_list.ins(node);
       }
    }
 }

template <class Ctx>
StepEval<Ctx>::Gate::Gate(StepEval *eval_)
 {
  eval=eval_;
  opened=false;
  gate_node=0;
 }

template <class Ctx>
StepEval<Ctx>::Gate::~Gate()
 {
  eval->destroyList(list);
 }

template <class Ctx>
template <class FuncInit>
auto StepEval<Ctx>::Gate::createStep(const FuncInit &func_init,StepId dep)
 {
  if( opened )
    {
     return eval->createStep(func_init,dep);
    }
  else
    {
     auto *node=eval->createNode(func_init,dep,true);

     list.ins(node);

     eval->lockStep(node->dep);

     boost();

     return node->getRetStep();
    }
 }

template <class Ctx>
void StepEval<Ctx>::Gate::open()
 {
  if( !opened )
    {
     opened=true;

     while( auto *node=list.del() )
       {
        node->gated=false;

        if( node->lock_count )
          eval->locked_list.ins(node);
        else
          eval->ready_list.ins(node);
       }
    }
 }

template <class Ctx>
template <class FuncInit>
void StepEval<Ctx>::createGateStep(const FuncInit &func_init,StepId dep,Gate *gate)
 {
  auto *node=createGateNode(func_init,dep,gate);

  ready_list.ins(node);

  lockStep(node->dep);

  gate->gate_node=node;
 }

template <class Ctx>
StepEval<Ctx>::~StepEval()
 {
  destroyList(ready_list);
  destroyList(locked_list);
  destroyList(gate_list);
 }

template <class Ctx>
auto StepEval<Ctx>::createGate() -> Gate *
 {
  Gate *ret=New<Gate>(getAlloc(),this);

  gate_list.ins(ret);

  return ret;
 }

template <class Ctx>
template <class OpenFuncInit,class FuncInit>
auto StepEval<Ctx>::createGate(const OpenFuncInit &openfunc_init,const FuncInit &func_init) -> Gate *
 {
  Gate *ret=createGate();

  auto step=createStep(GateStep<OpenFuncInit>(openfunc_init,ret));

  createGateStep(func_init,step.id,ret);

  return ret;
 }

template <class Ctx>
template <class FuncInit>
auto StepEval<Ctx>::createStep(const FuncInit &func_init,StepId dep)
 {
  auto *node=createNode(func_init,dep,false);

  ready_list.ins(node);

  lockStep(node->dep);

  return node->getRetStep();
 }

template <class Ctx>
void StepEval<Ctx>::run()
 {
  while( auto *node=ready_list.del() )
    {
     Guard guard(this,node);

     node->call(*this);

     unlockStep(node->dep);
    }

  for(auto cur=gate_list.start(); +cur ;++cur) finalList(cur->list);

  finalList(locked_list);
 }

} // namespace StepEvalPrivate

/* type StepEvalStepId */

using StepEvalStepId = StepEvalPrivate::StepId ;

/* type StepEval<Ctx> */

template <class Ctx>
using StepEval = StepEvalPrivate::StepEval<Ctx> ;

/* type StepEvalGate<Ctx> */

template <class Ctx>
using StepEvalGate = typename StepEval<Ctx>::Gate ;

} // namespace CCore

#endif



