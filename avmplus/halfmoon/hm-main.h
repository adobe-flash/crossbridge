/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// This is the main header file for the compiler implementation.
//

#ifndef HM_MAIN_H
#define HM_MAIN_H

#include "avmplus.h"

#ifdef VMCFG_HALFMOON
#include "CodegenLIR.h"
#include "../core/Interpreter.h"

//#define DOPROF
#include "../vprof/vprof.h"

// Forward declarations.  Keep each group in alphabetical order.
namespace halfmoon {
struct Context;
class Def;
class Use;
class InfoManager;
class InstrGraph;
class InstrInfo;
class Lattice;

// Ordinary Instructions
class Instr;
class BinaryExpr;
class BinaryStmt;
class CallStmt1;
class CallStmt2;
class CallStmt3;
class CallStmt4;
class Hasnext2Stmt;
class NaryExpr;
class NaryStmt0;
class NaryStmt1;
class NaryStmt2;
class NaryStmt3;
class NaryStmt4;
class ConstantExpr;
class VoidStmt;
class SafepointInstr;
class DeoptSafepointInstr;  // DEOPT
class DeoptFinishInstr;  // DEOPT
class DeoptFinishCallInstr;  // DEOPT
class SetlocalInstr;
class UnaryExpr;
class UnaryStmt;
class DebugInstr;

// CFG-related Instrs
class BlockStartInstr;
class   ArmInstr;
class   LabelInstr;
class BlockEndInstr;
class   CondInstr;
class     IfInstr;
class     SwitchInstr;
class   GotoInstr;
class StartInstr;
class CatchBlockInstr;
class StopInstr;
}

namespace profiler {
  class MethodProfile;
  class MethodProfileMgr;
}

#include "halfmoon.h"            // External api for this module

/// Enumerator for boolean values passed as it.  Doing it this way
/// allows overloading, whereas a typedef of int would not.
enum BoolKind {
  kBoolFalse,
  kBoolTrue
};

#include "profiler/profiler-types.h"
#include "hm-algorithms.h"
#include "hm-util.h"
#include "hm-types.h"
#include "hm-constraints.h"
#include "hm-instrfactory.h"
#include "hm-check.h"
#include "hm-instrgraph.h"
#include "hm-debug.h"
#include "hm-debug-inlines.h"
#include "hm-models.h"
#include "hm-deoptimizer.h"  // DEOPT -- must precede hm-instr.h
#include "hm-instr.h"
#include "hm-dispatch.h"
#include "generated/Stub_protos.hh"
#include "hm-typeanalyzer.h"
#include "hm-prettyprint.h"
#include "hm-dominatortree.h"
#include "hm-schedulers.h"
#include "hm-abcgraph.h"
#include "hm-abcbuilder.h"
#include "hm-interpreter.h"
#include "hm-liremitter.h"
#include "hm-specializer.h"
#include "hm-inline.h"
#include "hm-identityanalyzer.h"
#include "hm-cleaner.h"
#include "hm-dead.h"
#include "hm-profiler.h"
#include "hm-bailouts.h"
#include "hm-typeinference.h"
#include "hm-jitmanager.h"
#include "hm-valnum.h"

namespace halfmoon {

/// Replace references to instructions with references to the instructions
/// identity instr (if different).
///
void computeIdentities(InstrGraph*);

/// Peephole-optimize one def.
///
Def* peephole(Def*, InstrGraph*, InstrFactory*);

/// Run all of the optimization passes.
///
void optimize(Context*, InstrGraph*);

/// Parse the ABC code body of method, using the given type lattice,
/// then return the optimized InstrGraph, allocated with ir_alloc.
/// \param abc_env TODO: matz_inline_experiment
///
InstrGraph* parseAbc(MethodInfo*, Lattice*, InfoManager*, Allocator& ir_alloc,
                     AbcGraph*, Toplevel *toplevel, AbcEnv *abc_env,
                     ProfiledInformation* profiled_info, Context &cxt);

/// A Context is a wrapper struct containing a number of things
/// all passes generally need, including the method being optimized,
/// and a console to print messages to.
///
struct Context {
  Context(MethodInfo* m, PrintWriter& out, Toplevel *tl, AbcEnv *abc_env,
          const Context *caller) :
      method(m), out(out), abc_env(abc_env), toplevel(tl),
      caller(caller),
      inline_depth(caller ? caller->inline_depth + 1 : 0) {
  }
  Context(MethodInfo* m) :
      method(m), out(method->pool()->core->console),
      caller(NULL),
      inline_depth(0) {
  }
  MethodInfo* method;
  PrintWriter& out;
  AbcEnv *abc_env; // for inliner
  Toplevel *toplevel; // for inliner
  const Context *caller; // calling context. like stacktrace of inlines in progress.
  const int inline_depth;
};

/// Helper function for early binding.
///
inline Binding toBinding(Lattice* lattice, const Use& object, const Use& name) {
  return lattice->toBinding(type(object), type(name));
}

/// Helper function to analyze AS3 coerce operator.
/// Return true iff HR_coerce would be a no-op for this traits and value.
///
inline bool coerceIsNop(Lattice* lattice, const Def* val, Traits* to_traits) {
  return subtypeof(type(val), lattice->makeType(to_traits));
}

/// Redirect uses of outer_stmt's defs to the defs that are inputs to
/// new_ret.
///
void connectOuterUsesToInnerUses(Instr* ret_stmt, Instr* callsite_stmt);

/// Given two blocks that end in a stop, join them by creating a label,
/// replacing the two stops with just one.  This is a special case of
/// cross jumping.  Returns whichever stop instruction is retained.
///
StopInstr* joinStops(InstrGraph* ir, InstrFactory*, StopInstr* stop1,
                     StopInstr* stop2);

/// If the given value is toatom(x) and x <= to_type, then return x.
///
bool canUnwrapToatom(Def* value_in, const Type* to_type, Def** arg_out);

/// Parse environment variables.
///
void init();

// Configuration.
extern int enable_welcome;    // Print welcome and options.
extern int enable_verbose;    // Generate verbose output.
extern int enable_peephole;   // Enable ABC peephole optimizer.
extern int enable_gml;        // Enable GML graph output.
extern int enable_vmstate;    // Always generate Safepoint instructions.
extern int enable_builtins;   // Optimize builtins too.
extern int enable_try;        // Enable optimizing try/catch functions.
extern int enable_framestate; // Print the frame state during parsing of abc
extern int enable_printir;    // Enable printing of final IR
extern int enable_mode;       // Which execution mode.
extern int enable_trace;      // Enable execution trace.
extern int enable_inline;     // Enable inline optimization.
extern int enable_profiler;   // Enable runtime profiling
extern int enable_typecheck;  // Enable type-check verbosity.
extern int enable_optional;   // Enable optional argument support.

/** Halfmoon execution modes */
enum ExecutionMode {
  kModeNone,        // Don't do anything
  kModeAnalyze,     // build IR and optimize, but don't run it.
  kModeInterpret,   // Use halfmoon::Interpreter to run the IR.
  kModeLirStubs,    // use LirEmitter, always call stubs
  kModeLir,         // Use LirEmitter with inlined code
};

enum ScheduleKind {
  kScheduleNone,
  kScheduleEarly,
  kScheduleLate,
  kScheduleMiddle
};
extern ScheduleKind enable_schedule;   // Scheduler. 0=none, 1=
extern int enable_deopt;  // Enable deoptimization.

/// A Copier is used for cloning instructions from one IR into another one.
/// It keeps a map of old instructions to new ones.  Eacy copy() operation
/// runs depth-first, so any instructions referenced by the copied instruction
/// are also copied recursively.
///
/// Use->Def references are followed recursively, but Def->Use references
/// are *not*.
/// fixme: this belongs in hm-instrfactory.h, but is here to avoid
/// circular type dependencies between ShapeAdapter and Instr.
///
class Copier: public ShapeAdapter<Copier, Instr*> {
public:
  Copier(InstrGraph* from_ir, InstrGraph* to_ir);

  /// Copy one instruction deeply.
  ///
  template<class INSTR> INSTR* copy(INSTR* i) {
    return (INSTR*) deepCopy(i);
  }

public: // do_shape interface

    Instr* do_default(Instr*);
    Instr* do_GotoInstr(GotoInstr* i);
    Instr* do_ArmInstr(ArmInstr* i);
    Instr* do_LabelInstr(LabelInstr* i);
    Instr* do_ConstantExpr(ConstantExpr* i);
    Instr* do_StartInstr(StartInstr* i);
    Instr* do_IfInstr(IfInstr* i);
    Instr* do_SwitchInstr(SwitchInstr* i);
    Instr* do_UnaryExpr(UnaryExpr* i)               { return copyFixedArgInstr(i); }
    Instr* do_BinaryExpr(BinaryExpr* i)             { return copyFixedArgInstr(i); }
    Instr* do_SetlocalInstr(SetlocalInstr* i);
    Instr* do_StopInstr(StopInstr* i);
    Instr* do_VoidStmt(VoidStmt* i)                 { return copyVarArgInstr(i); }
    Instr* do_NaryStmt0(NaryStmt0* i)               { return copyVarArgInstr(i); }
    Instr* do_NaryStmt1(NaryStmt1* i)               { return copyVarArgInstr(i); }
    Instr* do_SafepointInstr(SafepointInstr* i);
    Instr* do_DeoptSafepointInstr(DeoptSafepointInstr* i); // DEOPT
    Instr* do_DeoptFinishInstr(DeoptFinishInstr* i); // DEOPT
    Instr* do_DeoptFinishCallInstr(DeoptFinishCallInstr* i); // DEOPT
    Instr* do_UnaryStmt(UnaryStmt* i)               { return copyFixedArgInstr(i); }
    Instr* do_CallStmt2(CallStmt2* i)               { return copyVarArgInstr(i); }
    Instr* do_NaryStmt2(NaryStmt2* i)               { return copyVarArgInstr(i); }
    Instr* do_BinaryStmt(BinaryStmt* i)             { return copyFixedArgInstr(i); }
    Instr* do_Hasnext2Stmt(Hasnext2Stmt* i)         { return copyFixedArgInstr(i); }
    Instr* do_CallStmt3(CallStmt3* i)               { return copyVarArgInstr(i); }
    Instr* do_NaryStmt3(NaryStmt3* i)               { return copyVarArgInstr(i); }
    Instr* do_CallStmt4(CallStmt4* i)               { return copyVarArgInstr(i); }
    Instr* do_NaryStmt4(NaryStmt4* i)               { return copyVarArgInstr(i); }

private:
  Instr* deepCopy(Instr* instr);
  void copyArm(ArmInstr* oldarm, ArmInstr* newarm);

  template<class INSTR> INSTR* copyFixedArgInstr(INSTR* instr) {
    return new (to_alloc_) INSTR(instr->info);
  }

  template<class INSTR> INSTR* copyVarArgInstr(INSTR* instr) {
    return new (to_alloc_, instr->info->num_uses, sizeof(Use))
        INSTR(instr->info);
  }

private:
  Allocator scratch_;
  Allocator0 scratch0_;
  Instr** const map_;
  InstrGraph* to_ir_;
  Allocator& to_alloc_;
};

/**
 * class JitFriend is a layer of indirection for accessing private
 * avm data structures without avm needing to refer to compiler internal
 * classes.
 */
class JitFriend {

public:
  static uintptr_t getIID(MethodInfo* info) {
    return ImtHolder::getIID(info);
  }

  static uint32_t hashIID(MethodInfo* info) {
    return ImtHolder::hashIID(info);
  }

  static GprMethodProc envImplGpr(MethodEnv* env) {
    return env->_implGPR;
  }

  static FprMethodProc envImplFpr(MethodEnv* env) {
    return env->_implFPR;
  }

  static GprImtThunkProc envImplImtGpr(MethodEnv* env) {
    return env->_implImtGPR;
  }

  static FprImtThunkProc envImplImtFpr(MethodEnv* env) {
    return (FprImtThunkProc) env->_implImtGPR;
  }

  static MethodEnv* imtEntry(VTable* vtable, MethodInfo* info) {
    return reinterpret_cast<MethodEnv*>(vtable->imt.entries[hashIID(info)]);
  }

  static MethodEnv* superInitEnv(MethodEnv* env) {
    return env->vtable()->base->init;
  }

public:
  // Offsets of fields accessed directly by halfmoon JIT.
  static const size_t core_cmf_offset = offsetof(AvmCore, currentMethodFrame);
  static const size_t core_minstack_offset = offsetof(AvmCore, minstack);
  static const size_t core_interrupted_offset = offsetof(AvmCore, interrupted);
  static const size_t mf_env_offset = offsetof(MethodFrame, envOrCodeContext);
  static const size_t mf_dxns_offset = offsetof(MethodFrame, dxns);
  static const size_t env_impl_offset = offsetof(MethodEnvProcHolder, _implGPR);
  static const size_t env_scope_offset = offsetof(MethodEnv, _scope);
  static const size_t scope_vtable_offset = offsetof(ScopeChain, _vtable);
  static const size_t scope_scopes_offset = offsetof(ScopeChain, _scopes);
  static const size_t vtable_imt_offset = offsetof(VTable, imt.entries);
};

} // end namespace halfmoon
#endif // VMCFG_HALFMOON
#endif // HM_MAIN_H
