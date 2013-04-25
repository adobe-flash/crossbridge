/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HM_INSTR_H_
#define HM_INSTR_H_

namespace halfmoon {

using avmplus::MethodSignature;  // Used by class DeoptSafepointInstr

// ------------------------- templates start ---------------------------------

/// A FixedArgInstr has a fixed number of
/// input uses (aka args) and output defs.
///
template<int USEC, int DEFC>
class FixedArgInstr : public Instr {
  friend class InfoManager;
  friend class InstrFactory;

  static InstrInfo createInfo(InstrKind kind, const Type** insig,
                              const Type** outsig, InstrGraph* ir) {
    return InstrInfo(USEC, DEFC,
                     offsetof(FixedArgInstr, uses),
                     offsetof(FixedArgInstr, defs),
                     kind, insig, outsig, ir);
  }

public:
  Use& use(int i) {
    assert (i >= 0 && i < USEC);
    return uses[i];
  }

protected:
  FixedArgInstr(const InstrInfo* info) :
      Instr(info) {
  }

  Use uses[USEC];
  Def defs[DEFC];
};

/// A FixedArgStmt is a FixedArgInstr whose
/// first input and output are effect values.
///
/// Note: FixedArgStmt is parameterized by its
/// value (i.e., non-effect) use and def counts.
///
template<int VUSEC, int VDEFC>
class FixedArgStmt : public FixedArgInstr<VUSEC + 1, VDEFC + 1> {
protected:
  FixedArgStmt(const InstrInfo* info) :
      FixedArgInstr<VUSEC + 1, VDEFC + 1>(info) {
  }

public:
  Use& effect_in() {
    return this->uses[0];
  }

  Def* effect_out() {
    return &this->defs[0];
  }

  Def* value_out() {
    assert(VDEFC >= 1);
    return &this->defs[1];
  }
};

/// A VarArgInstr has a variable (but nonvolatile)
/// number of uses (args) above a fixed minimum
/// number of uses, and a fixed number of defs.
///
/// NOTE: uses are stored 'off the end', requiring
/// custom allocation (see InstrFactory) and
/// precluding structural extension by subclasses.
///
template<int USEMIN, int DEFC, class BASE>
class VarArgInstr : public Instr {
  friend class InfoManager;
  friend class InstrFactory;

  static InstrInfo createInfo(InstrKind kind, int argc, const Type** insig,
                              const Type** outsig, InstrGraph* ir) {
    return InstrInfo(USEMIN + argc, DEFC,
                     sizeof(BASE),
                     offsetof(VarArgInstr, defs),
                     kind, insig, outsig, ir);
  }

protected:
  VarArgInstr(const InstrInfo* info) :
      Instr(info) {
  }

  Use* uses() {
    return (Use*)((intptr_t)this + info->uses_off);
  }

  Def defs[DEFC];

public:
  /// number of variable args
  int vararg_count() const {
    return this->info->num_uses - USEMIN;
  }

  /// array of variable args, excludes all fixed args
  Use* varargs() {
    return &uses()[USEMIN];
  }

  /// ith variable arg
  Use& vararg(int i) {
    assert(i >= 0 && i < vararg_count());
    return varargs()[i];
  }

  Use& use(int i) {
    assert (i >= 0 && i < info->num_uses);
    return uses()[i];
  }
};


/// VarArgStmt is a VarArgInstr with a fixed minimum number
/// of arguments, whose first input and output are effects,
/// and whose second output is a data result. VarArgStmt is
/// parameterized on its fixed minimum number of value args.
/// the "arg" range of a VarArgStmt maps exactly to VarArgInstr's
/// "vararg" range.
template<int ARGMIN>
class NaryStmt : public VarArgInstr<ARGMIN + 1, 2, class NaryStmt<ARGMIN> > {
protected:
  NaryStmt(const InstrInfo* info) :
    VarArgInstr<ARGMIN + 1, 2, class NaryStmt<ARGMIN> >(info) {
  }

public:
  Def* effect_out() {
    return &this->defs[0];
  }

  Def* value_out() {
    return &this->defs[1];
  }

  Use& effect_in() {
    return this->uses()[0];
  }

  int arg_count() const {
    return this->vararg_count();
  }

  Use* args() {
    return this->varargs();
  }

  Use& arg(int i) {
    assert(i >= 0 && i < arg_count());
    return this->args()[i];
  }
};

/// CallStmt is a VarArgInstr with a fixed minimum number
/// of arguments, whose first input and output are effects,
/// and whose second output is a data result. CallStmt is
/// parameterized on its fixed minimum number of value args.
///
/// NOTE: the term 'arg' in CallStmt's API means
/// the combination of the last fixed arg plus variable args, designed
/// to be congruent with VM calling conventions.  The only difference
/// between CallStmt and NaryStmt is the definition of the 'arg' range.
///
template<int ARGMIN>
class CallStmt : public VarArgInstr<ARGMIN + 1, 2, class CallStmt<ARGMIN> > {
protected:
  CallStmt(const InstrInfo* info) :
    VarArgInstr<ARGMIN + 1, 2, class CallStmt<ARGMIN> >(info) {
  }

public:
  Def* effect_out() {
    return &this->defs[0];
  }

  Def* value_out() {
    return &this->defs[1];
  }

  Use& effect_in() {
    return this->uses()[0];
  }

  int arg_count() const {
    return 1 + this->vararg_count();
  }

  Use* args() {
    assert(ARGMIN >= 1);
    return this->varargs() - 1;
  }

  Use& arg(int i) {
    assert(i >= 0 && i < arg_count());
    return this->args()[i];
  }
};

// ------------------------- templates end ---------------------------------

/// NaryStmt
///
class NaryStmt0 : public NaryStmt<0> {
  friend class InstrFactory;
  friend class Copier;

  NaryStmt0(const InstrInfo* info) : NaryStmt<0>(info) {
  }

public:
  static const InstrShape shape = NARYSTMT0_SHAPE;
};

class NaryStmt1 : public NaryStmt<1> {
  friend class InstrFactory;
  friend class Copier;

  NaryStmt1(const InstrInfo* info) : NaryStmt<1>(info) {
  }

public:
  // for newfunction
  Use& info_in() { return uses()[1]; }

public:
  static const InstrShape shape = NARYSTMT1_SHAPE;
};

class NaryStmt2 : public NaryStmt<2> {
  friend class InstrFactory;
  friend class Copier;

  NaryStmt2(const InstrInfo* info) : NaryStmt<2>(info) {
  }

public:
  // for findproperty opcodes
  Use& name_in() { return uses()[1]; }
  Use& env_in() { return uses()[2]; }

  // for newclass
  Use& traits_in() { return uses()[1]; }
  Use& base_in() { return uses()[2]; }

public:
  static const InstrShape shape = NARYSTMT2_SHAPE;
};

class NaryStmt3 : public NaryStmt<3> {
  friend class InstrFactory;
  friend class Copier;

  NaryStmt3(const InstrInfo* info) : NaryStmt<3>(info) {
  }

public:
  // for findproperty opcodes
  Use& name_in() { return uses()[1]; }
  Use& env_in() { return uses()[2]; }
  Use& index_in() { return uses()[3]; } // index variable

public:
  static const InstrShape shape = NARYSTMT3_SHAPE;
};

class NaryStmt4 : public NaryStmt<4> {
  friend class InstrFactory;
  friend class Copier;

  NaryStmt4(const InstrInfo* info) : NaryStmt<4>(info) {
  }

public:
  // for findproperty opcodes
  Use& name_in() { return uses()[1]; }
  Use& env_in() { return uses()[2]; }

public:
  static const InstrShape shape = NARYSTMT4_SHAPE;
};

/// CallStmt2 is for instructions that take a name parameter,
/// an object, and some arguments.
///
/// callprop: name, obj, args...
/// newclass: class_traits, base_class, scopes...
/// set: name, obj, arg
/// get: name, obj
///
class CallStmt2 : public CallStmt<2> {
  friend class InstrFactory;
  friend class Copier;

  CallStmt2(const InstrInfo* info) :
      CallStmt<2>(info) {
  }

public:
  static const InstrShape shape = CALLSTMT2_SHAPE;

  /// input parameter.  What this actually is depends on kind(this).
  Use& param_in() { return uses()[1]; }
  Use& object_in() { return uses()[2]; }
};

/// CallStmt3 is for instructions that take a name
/// parameter, an index or namespace, an object, and some arguments.
///
class CallStmt3 : public CallStmt<3> {
  friend class InstrFactory;
  friend class Copier;

  CallStmt3(const InstrInfo* info) :
      CallStmt<3>(info) {
  }

public:
  static const InstrShape shape = CALLSTMT3_SHAPE;

  Use& param_in()  { return uses()[1]; }
  Use& index_in()  { return uses()[2]; } // for -x opcodes
  Use& ns_in()     { return uses()[2]; } // same as index_in, but for -ns opcodes
  Use& object_in() { return uses()[3]; } // same as arg(0)
};

/// CallStmt4 is for instructions that take a name
/// parameter, a namespace, an index, an object, and
/// some arguments.
///
class CallStmt4 : public CallStmt<4> {
  friend class InstrFactory;
  friend class Copier;

  CallStmt4(const InstrInfo* info) :
      CallStmt<4>(info) {
  }

public:
  static const InstrShape shape = CALLSTMT4_SHAPE;

  Use& param_in()  { return uses()[1]; }
  Use& ns_in()     { return uses()[2]; }
  Use& index_in()  { return uses()[3]; }
  Use& object_in() { return uses()[4]; } // same as arg(0)
};

/// A ConstantExpr is a 0-input instruction which
/// produces a constant (strongly-typed) value.
///
/// NOTE: should really be FixedArgInstr<0, 1>, but
/// unfortunately this hits the zero-length member
/// array prohibition.
///
class ConstantExpr : public Instr {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  static InstrInfo createInfo(InstrKind kind, const Type** insig,
                              const Type** outsig, InstrGraph* ir) {
    return InstrInfo(0, 1, 0, offsetof(ConstantExpr, def), kind, insig, outsig, ir);
  }

  ConstantExpr(const InstrInfo* info) :
      Instr(info) {
  }

public:
  static const InstrShape shape = CONSTANTEXPR_SHAPE;

  Def* value() {
    return &def;
  }

  const Type* type() {
    return halfmoon::type(def);
  }

private:
  Def def;
};

/// VoidStmt is for a statement with only side effects,
/// but no data input or output.
///
class VoidStmt : public FixedArgStmt<0, 0> {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  VoidStmt(const InstrInfo* info) :
      FixedArgStmt<0, 0>(info) {
  }

public:
  static const InstrShape shape = VOIDSTMT_SHAPE;
};

/// UnaryExpr is a simple pure unary operator,
/// with no side effects.
///
class UnaryExpr : public FixedArgInstr<1, 1> {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  UnaryExpr(const InstrInfo* info) :
      FixedArgInstr<1, 1>(info) {
  }

public:
  static const InstrShape shape = UNARYEXPR_SHAPE;

  Use& value_in() {
    return this->uses[0];
  }

  Def* value_out() {
    return &this->defs[0];
  }
};

/// UnaryStmt has (in addition to effect in/out)
/// one input and one output.
///
class UnaryStmt : public FixedArgStmt<1, 1> {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  UnaryStmt(const InstrInfo* info) :
      FixedArgStmt<1, 1>(info) {
  }

public:
  static const InstrShape shape = UNARYSTMT_SHAPE;

  Use& value_in() {
    return this->uses[1];
  }
};

/// BinaryExpr has two inputs and one output,
/// and no side effects.
///
class BinaryExpr : public FixedArgInstr<2, 1> {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  BinaryExpr(const InstrInfo* info) :
      FixedArgInstr<2, 1>(info) {
  }

public:
  static const InstrShape shape = BINARYEXPR_SHAPE;

  Use& lhs_in() {
    return this->uses[0];
  }

  Use& rhs_in() {
    return this->uses[1];
  }

  Def* value_out() {
    return &this->defs[0];
  }
};

/// BinaryStmt has (in addition to effect in/out)
/// two inputs and one output
///
class BinaryStmt : public FixedArgStmt<2, 1> {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  BinaryStmt(const InstrInfo* info) :
    FixedArgStmt<2, 1>(info) {
  }

public:
  static const InstrShape shape = BINARYSTMT_SHAPE;

  Use& lhs_in() {
    return this->uses[1];
  }

  Use& rhs_in() {
    return this->uses[2];
  }
};

/// Hasnext2Stmt is a 2-input, 3-output Statement, used solely for
/// HR_hasnext2, which is for the ABC OP_hasnext2 instruction.
///
class Hasnext2Stmt : public FixedArgStmt<2, 3> {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  Hasnext2Stmt(const InstrInfo* info) :
    FixedArgStmt<2, 3>(info) {
  }

public:
  static const InstrShape shape = HASNEXT2STMT_SHAPE;

  Use& counter_in() {
    return this->uses[1];
  }

  Use& object_in() {
    return this->uses[2];
  }

  Def* counter_out() {
    return &this->defs[2];
  }

  Def* object_out() {
    return &this->defs[3];
  }
};

/// A Safepoint instruction represents a specific point in the ABC bytecode
/// where an exception can occur.  It holds a reference to a chain of
/// Setlocals which enable recreation of a state vector for the locals.
///
/// We place it in our IR to keep those values alive in case exceptions.
/// Because it captures the virtual pc and the abstract state value, it is
/// in the control flow dependency chain.
///
/// We don't save sp or scopep in this safepoint because it is for exceptions,
/// and an exception resets sp and scopep to known values.
/// Is the above statement still true for speculative deopt? Then we need
/// sp and scopep
///
/// Exception safepoints are placed in the IR at positions that can reach
/// a local catch handler.  So they dont exist outside of try blocks and
/// don't exist in methods that don't have catch handlers.
///
/// Other kinds of safepoints:
/// 1. Bailout - these would need to save sp and scopep as well, if we
///    are bailing out to the interpreter.  Also, all frame values need to be
///    referenced, not just locals.  The stack and scopes are still live in
///    code we bail out to.
///
/// 2. Conditional Exception.  This would look just like a conditional branch;
///   it would have an explicit control edge to a catch block, and the catch
///   block would start with a block and phis for all incoming paths and
///   live values.  This supports full dataflow analysis and would be used
///   if we want to optimize the try code and catch code at the same time.
///
/// Safepoint MUST output state, because it has an implicit setlocal
/// It implicitly saves the current abc
class SafepointInstr : public VarArgInstr<1, 2, class SafepointInstr> {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  SafepointInstr(const InstrInfo* info) :
    VarArgInstr<1, 2, class SafepointInstr>(info) {
    assert(info->uses_off == sizeof(SafepointInstr));
  }

  static const int USEMIN = 1;  // effect in
  static const int DEFC = 2;    // effect out, state out

public:
  static const InstrShape shape = SAFEPOINTINSTR_SHAPE;

  int vpc; // Points into abc bytecode.  fixme: should be uint8_t*.
  int sp; // points to top of operand stack
  int scopep; // points to top of abc scope stack.

  Def* effect_out() {
    return &this->defs[0];
  }

  Def* state_out() {
    return &this->defs[1];
  }

  Use& effect_in() {
    return this->uses()[0];
  }
};

/// A setlocal instruction updates one element of the abstract "local state"
/// tuple (called kStateIn and kStateOut).
/// input:  kStateIn  (v0, ..., vk,     ... vN),  newval, index (k)
/// output: kStateOut (v0, ..., newval, ... vN)
///
class SetlocalInstr : public FixedArgInstr<2, 1> {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  SetlocalInstr(const InstrInfo* info, int index) :
      FixedArgInstr<2, 1>(info), index(index) {
  }

public:
  static const InstrShape shape = SETLOCALINSTR_SHAPE;

  int index; // Fixme: should this be a plain ordinal input?

  Use& state_in() {
    return this->uses[0];
  }

  Use& value_in() {
    return this->uses[1];
  }

  Def* state_out() {
    return &this->defs[0];
  }
};

/// DEOPT: New-style safepoint.
/// Force a use of a list of definitions, typically represenenting
/// some subset of the ABC frame state, to extend their lifetimes
/// and to make them accessible for metadata generation.
/// NOTE: Due to "off the end" allocation of the uses, we cannot
/// extend VarArgInstr here, as we need to add additional slots.
class DeoptSafepointInstr : public Instr {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;          // Need custom copier due to custom fields.

  static const int USEMIN = 1;  // effect in
  static const int DEFC = 1;    // effect out

  static InstrInfo createInfo(InstrKind kind, int argc, const Type** insig,
                              const Type** outsig, InstrGraph* ir) {
    return InstrInfo(USEMIN + argc, DEFC,
                     sizeof(DeoptSafepointInstr),
                     offsetof(DeoptSafepointInstr, defs),
                     kind, insig, outsig, ir);
  }

protected:
    DeoptSafepointInstr(const InstrInfo* info) :
      Instr(info) {
  }

  Use* uses() {
    return (Use*)(this + 1);
  }

  Def defs[DEFC];

public:
  static const InstrShape shape = DEOPTSAFEPOINTINSTR_SHAPE;

  SafepointKind kind;
  int vpc;    // points into abc bytecode
  int scopep; // points to top of abc scope stack
  int stackp; // points to top of operand stack
  // Only applicable to call safepoints.
  int vlen;
  int nargs;
  SlotStorageType rtype;  // result type if function call
  // Only applicable to inline safepoints.
  MethodInfo* minfo;

  Def* effect_out() {
    return &this->defs[0];
  }

  Use& effect_in() {
    return this->uses()[0];
  }

   // number of captured variables
  int values_count() const {
    return this->info->num_uses - USEMIN;
  }

  // array of captured variables
  Use* values_in() {
    return &uses()[USEMIN];
  }

  // ith captured variable
  // sequence does not include scopes above scopep or operands above sp
  Use& value_in(int i) {
    assert(i >= 0 && i < values_count());
    return values_in()[i];
  }

  Use& use(int i) {
    assert (i >= 0 && i < info->num_uses);
    return uses()[i];
  }
};

class DeoptFinishInstr : public FixedArgStmt<0, 0> {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

public:
  static const InstrShape shape = DEOPTFINISHINSTR_SHAPE;

  DeoptSafepointInstr* safepoint;

  DeoptFinishInstr(const InstrInfo* info) :
    FixedArgStmt<0, 0>(info) {
  }
};


class DeoptFinishCallInstr : public FixedArgStmt<1, 0> {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  DeoptFinishCallInstr(const InstrInfo* info) :
      FixedArgStmt<1, 0>(info) {
  }

public:
  static const InstrShape shape = DEOPTFINISHCALLINSTR_SHAPE;

  DeoptSafepointInstr* safepoint;

  Use& value_in() {
    return this->uses[1];
  }
};


class DebugInstr : public FixedArgStmt<1, 0> {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  DebugInstr(const InstrInfo* info) :
      FixedArgStmt<1, 0>(info) {
  }

public:
  static const InstrShape shape = DEBUGINSTR_SHAPE;

  Use& value_in() {
    return this->uses[1];
  }
};


// ------------------------------- IR5 start ---------------------------------

/// BlockStartInstr is the common superclass of all IR5
/// block start delimiters: StartInstr, LabelInstr,
/// ArmInstr.
///
/// All block start instructions carry a parameter
/// list of Defs. Predecessor blocks end with block
/// end delimiters carrying congruent argument lists.
///
/// Design note: we use an allocated array for params,
/// rather than inline off-the-end storage, so that
/// we can factor out a nonvirtual superclass of
/// block start delimiters.
///
class BlockStartInstr : public Instr {
protected:
  static InstrInfo createInfo(InstrKind kind, const Type** insig,
                              const Type** outsig, InstrGraph* ir) {
    return InstrInfo(0, -1, 0, -1, kind, insig, outsig, ir);
  }

  BlockStartInstr(const InstrInfo* info, Def* params) :
    Instr(info), params(params), blockid(-1) {
  }

public:
  Def* params; // block parameters
  int blockid;
};

/// A BlockHeaderInstr begins a block that is a unique
/// successor - i.e., none of its predecessors has any
/// other successor. As such it owns its parameter count,
/// which determines the argument counts of its predecessors.
///
class BlockHeaderInstr : public BlockStartInstr {
protected:
  BlockHeaderInstr(const InstrInfo* info, int paramc, Def* params) :
    BlockStartInstr(info, params), paramc(paramc) {
  }

public:
  int paramc; // parameter count
};

/// BlockEndInstr is the common superclass of all IR5
/// block end delimiters: GotoInstr, CondInstr,
/// StopInstr.
///
/// All block end instructions carry an argument
/// list of Uses. Successor blocks begin with block
/// start delimiters carrying congruent parameter
/// lists.
///
/// Design note: we use an allocated array for args,
/// rather than inline off-the-end storage, so that
/// we can factor out a nonvirtual superclass of
/// block end delimiters.
///

class ExceptionEdge {
 public:
  ExceptionEdge(BlockStartInstr* f, CatchBlockInstr* t): from(InstrGraph::blockEnd(f)), to(t), next_exception(NULL), prev_exception(NULL) {}

  BlockEndInstr* from;
  CatchBlockInstr* to;
  ExceptionEdge *next_exception, *prev_exception;
};

class BlockEndInstr : public Instr {
protected:
  static InstrInfo createInfo(InstrKind kind, const Type** insig,
                              const Type** outsig, InstrGraph* ir) {
    return InstrInfo(-1, 0, -1, 0, kind, insig, outsig, ir);
  }

  BlockEndInstr(const InstrInfo* info, Use* args) :
    Instr(info), args(args), catch_blocks(NULL) {
  }

public:
  Use* args; // arguments for successor

  SeqBuilder<ExceptionEdge*>* catch_blocks;
};


/// A BlockFooterInstr ends a block that is a unique
/// predecessor - i.e., none of its successors has any
/// other predecessor. As such it owns its argument count,
/// which determines the parameter counts of its successors.
///
class BlockFooterInstr : public BlockEndInstr {
protected:
  BlockFooterInstr(const InstrInfo* info, int argc, Use* args) :
    BlockEndInstr(info, args), argc(argc) {
  }

public:
  int argc; // argument count

  int vararg_count() const {
    return argc;
  }

  Use& vararg(int i) {
    assert((i >= 0) & (i < argc));
    return args[i];
  }
};

/// A StartInstr begins the initial block of a
/// function or template. Legal opcodes are
/// HR_start and HR_template. The parameters
/// of a StartInstr are function or template
/// params.
///
/// HR_start:    (effect env [args])
/// HR_template: (effect [args]
class StartInstr : public BlockHeaderInstr {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  StartInstr(const InstrInfo* info, int paramc, bool rest, Def* params) :
      BlockHeaderInstr(info, paramc, params),
      rest(rest) {
  }

public:
  static const InstrShape shape = STARTINSTR_SHAPE;

  /**
   * True if this is the start instruction of an AS3 function with
   * rest parameters.  Extra arguments passed to the function by the
   * caller are received by the last parameter as an array.
   */
  bool rest;

  /** Number of parameters, not counting effect */
  int data_param_count() const {
    return paramc - 1;
  }

  /** get data param i */
  Def* data_param(int i) {
    assert(i >= 0 && i < data_param_count());
    return &params[1 + i];
  }

  Def* effect_out() {
    return &params[0];
  }

  bool has_rest() const {
    return rest;
  }

  /** get the rest array parameter */
  Def* rest_out() {
    assert(has_rest());
    return &params[paramc - 1];
  }
};

/// A StopInstr ends a block by returning or throwing
/// from the current function. Legal opcodes are
/// HR_return and HR_throw. The arguments of a
/// StopInstr are values returned by the function.
///
class StopInstr : public BlockFooterInstr {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  StopInstr(const InstrInfo* info, int argc, Use* args) :
      BlockFooterInstr(info, argc, args) {
  }

public:
  static const InstrShape shape = STOPINSTR_SHAPE;

  /// convenience method, for use in conventional,
  /// return-one-value-from-effectful-code contexts.
  /// note assert.
  Use& value_in() {
    assert(argc == 2);
    return args[1];
  }

  Use& use(int i) {
    assert(i >= 0 && i < argc);
    return args[i];
  }
};

/// A LabelInstr starts a block targeted by multiple predecessors.
/// Every incoming edge must come from a GotoInstr.
///
class LabelInstr : public BlockHeaderInstr {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;


protected:
  LabelInstr(const InstrInfo* info, int paramc, Def* params) :
      BlockHeaderInstr(info, paramc, params), preds(0) {
  }

public:
  static const InstrShape shape = LABELINSTR_SHAPE;

  GotoInstr* preds; // list of incoming gotos
};

/// GotoInstr ends a block with an unconditional jump to
/// a sucessor block start-delimited by a LabelInstr.
///
/// In addition to carrying a pointer to the LabelInstr
/// of the target block, each GotoInstr is a member of
/// a doubly-linked list of all the predecessors of that
/// target.
///
class GotoInstr : public BlockEndInstr {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  GotoInstr(const InstrInfo* info, Use* args) :
    BlockEndInstr(info, args), target(0), next_goto(0), prev_goto(0) {
  }

public:
  static const InstrShape shape = GOTOINSTR_SHAPE;

  LabelInstr* target;
  GotoInstr *next_goto, *prev_goto;
};

/// ArmInstr is a block start that begins a successor
/// block of a CondInstr.
///
/// Note: ArmInstrs are wholly owned by their precessor
/// CondInstrs.
///
class ArmInstr : public BlockStartInstr {
  friend class InfoManager;
  friend class InstrFactory;
  friend class CondInstr;
  friend class Copier;

  /// Note: info arg is optional due to
  /// array init in CondInstr ctor.
  /// InstrFactory fills it in.
  ArmInstr(const InstrInfo* info = NULL) :
      BlockStartInstr(info, 0), owner(0) {
  }

public:
  static const InstrShape shape = ARMINSTR_SHAPE;
  CondInstr* owner;
  int arm_pos; // Which of owner's arm is this.
};

/// CondInstr is a common superclass of IR5 conditional
/// block end delimiters (IfInstr, SwitchInstr).
///
/// CondInstr supplements BlockEndInstr with the additional
/// structure necessary for representing a conditional branch:
///
/// - an additional Use designating the conditional selector
/// - an array of wholly-owned ArmInstrs.
///
/// Subclasses specialize on selector type (boolean for IfInstr,
/// integer for SwitchInstr) and add accessors and verification
/// accordingly.
///
class CondInstr : public BlockFooterInstr {
protected:
  static InstrInfo createInfo(InstrKind kind, const Type** insig,
                              const Type** outsig, InstrGraph* ir) {
    return InstrInfo(-1, 0, -1, 0, kind, insig, outsig, ir);
  }

  // Note: initialization of args/uses left to InstrFactory
  CondInstr(const InstrInfo* info, int armc) :
      BlockFooterInstr(info, 0, NULL), uses(NULL), arms(0), armc(armc) {
  }

public:
  Use* uses; // selector plus args. NOTE: args == &uses[1]
  ArmInstr** arms; // array of wholly-owned arms
  int armc; // arm count

  Use& selector() {
    return uses[0];
  }

  Use& arg(int i) {
    return uses[i + 1];
  }
};

/// IfInstr ends a block with a conditional branch
/// selected by a boolean.
///
class IfInstr : public CondInstr {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  IfInstr(const InstrInfo* info) :
    CondInstr(info, 2) {
  }

public:
  static const InstrShape shape = IFINSTR_SHAPE;

  Use& cond() {
    return selector();
  }

  ArmInstr* arm(bool b) {
    return b ? true_arm() : false_arm();
  }

  ArmInstr* false_arm() {
    return arms[0];
  }

  ArmInstr* true_arm() {
    return arms[1];
  }
};

/// SwitchInstr ends a block with a conditional branch
/// selected by an integer.
///
class SwitchInstr : public CondInstr {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  SwitchInstr(const InstrInfo* info, int num_cases) :
      CondInstr(info, num_cases + 1) {
  }

public:
  static const InstrShape shape = SWITCHINSTR_SHAPE;

  int num_cases() {
    return armc - 1;
  }

  bool is_case(int i) {
    return i >= 0 && i < num_cases();
  }

  ArmInstr* arm(int i) {
    return is_case(i) ? case_arm(i) : default_arm();
  }

  ArmInstr* case_arm(int i) {
    assert(is_case(i));
    return arms[i];
  }

  ArmInstr* default_arm() {
    return arms[num_cases()];
  }
};

/// A CatchBlockInstr begins the initial block of a catch block. Legal
/// opcodes are HR_catchblock. The parameters of a CatchBlockInstr are
/// the incoming state.
///
/// HR_catchblock: ([args]
class CatchBlockInstr : public LabelInstr {
  friend class InfoManager;
  friend class InstrFactory;
  friend class Copier;

  CatchBlockInstr(const InstrInfo* info, int paramc, Def* params) :
    LabelInstr(info, paramc, params), catch_preds(NULL) {
  }

public:
  static const InstrShape shape = CATCHBLOCKINSTR_SHAPE;

  /** Number of parameters, not counting effect */
  int data_param_count() const {
    return paramc - 1;
  }

  /** get data param i */
  Def* data_param(int i) {
    assert(i >= 0 && i < data_param_count());
    return &params[1 + i];
  }

  Def* effect_out() {
    return &params[0];
  }

  int vpc;
  ExceptionEdge* catch_preds;

  inline void printCatchPreds();
};

class CatchBlockRange: public SeqRange<ExceptionEdge*> {
 public:
  explicit CatchBlockRange(BlockStartInstr* block): SeqRange<ExceptionEdge*>(*InstrGraph::blockEnd(block)->catch_blocks) {}
  explicit CatchBlockRange(BlockEndInstr* block): SeqRange<ExceptionEdge*>(*block->catch_blocks) {}

  CatchBlockInstr* front() const {
    return SeqRange<ExceptionEdge*>::front()->to;
  }
  CatchBlockInstr* popFront() {
    CatchBlockInstr* t = front();
    SeqRange<ExceptionEdge*>::popFront();
    return t;
  }
};


class ExceptionEdgeRange {
public:
  explicit ExceptionEdgeRange(CatchBlockInstr* catch_block) {
    ExceptionEdge* p = catch_block->catch_preds;
    front_ = p;
    back_ = p ? p->prev_exception : 0;
  }

  bool empty() const {
    return !front_;
  }

  ExceptionEdge* front() const {
    assert(!empty());
    return front_;
  }

  ExceptionEdge* popFront() {
    ExceptionEdge* t = front();
    ExceptionEdge* F = front_;
    front_ = (F == back_) ? (back_ = 0) : F->next_exception;
    return t;
  }

private:
  ExceptionEdge *front_, *back_;
};

inline void CatchBlockInstr::printCatchPreds() {
  printf("exception edges ( ");
  for (ExceptionEdgeRange r(this); !r.empty(); r.popFront())
    printf("i%d ", r.front()->from->id);
  printf(") -> i%d\n", id);
}

// ------------------------------- IR5 end ---------------------------------

/// true if this goto is the only predecessor of its target.
///
inline bool isAlone(GotoInstr* go) {
  assert(go->target);
  // Don't allow start block to merge into main block
  return go->next_goto == go && kind(InstrGraph::blockStart(go)) != HR_start;
}

/// Range over the param at the given position
/// for all arms of a CondInstr
///
class ArmParamRange {
public:
  ArmParamRange(CondInstr* instr, int pos) :
    arms(instr->arms, instr->armc), pos(pos) {
  }

  bool empty() const {
    return arms.empty();
  }

  Def& front() const {
    return arms.front()->params[pos];
  }

  void popFront() {
    arms.popFront();
  }

private:
  ArrayRange<ArmInstr*> arms;
  const int pos;
};

/// Range iterator for the predecessors of a label.
///
class PredRange {
public:
  explicit PredRange(LabelInstr* label) {
    GotoInstr* p = label->preds;
    front_ = p;
    back_ = p ? p->prev_goto : 0;
  }

  bool empty() const {
    return !front_;
  }

  GotoInstr* front() const {
    assert(!empty());
    return front_;
  }

  GotoInstr* popFront() {
    assert(!empty());
    GotoInstr* F = front_;
    front_ = (F == back_) ? (back_ = 0) : F->next_goto;
    return F;
  }

private:
  GotoInstr *front_, *back_;
};

/// Range over the arguments to a specific param of a label.
///
class LabelArgRange {
public:
  LabelArgRange(LabelInstr* label, int k) :
      r(label), k(k) {
  }

  bool empty() const {
    return r.empty();
  }

  Use& front() const {
    return r.front()->args[k];
  }

  void popFront() {
    r.popFront();
  }

private:
  PredRange r;
  const int k;
};

} // namespace halfmoon

#endif // HM_INSTR_H_
