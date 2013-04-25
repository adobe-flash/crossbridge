/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef HM_INTERPRETER_H_
#define HM_INTERPRETER_H_

namespace halfmoon {
using avmplus::MethodFrame;

/// A Value is a variant of all the possible values encountered by
/// the IR interpreter.  It has Atom, the various unboxed VM types,
/// plus more variants for internal VM types like MethodInfo* and Traits*.
///
class Value {
public:
  Value() {} // so we can allocate arrays of values.
  Value(String* s) : string_(s) {}
  Value(Namespace* ns) : namespace_(ns) {}
  Value(ScriptObject* o) : object_(o) {}
  Value(int32_t i) : int_(i) {}
  Value(uint32_t u) : int_(u) {}
  Value(BoolKind b) : int_(b) {}
  Value(bool b) : int_(b) {}
  Value(MethodInfo* method) : method_(method) {}
  Value(Traits* t) : traits_(t) {}
  Value(double d) : double_(d) {}
  Value(const Multiname* name) : name_(name) {}
  Value(MethodEnv* env) : env_(env) {}

protected:
  union {
    Atom atom_;
    Namespace* namespace_;
    String* string_;
    ScriptObject* object_;
    int ordinal_;
    int32_t int_;
    double double_;
    MethodInfo* method_;
    Traits* traits_;
    const Multiname* name_;
    MethodEnv* env_;
  };

  friend class Interpreter;
  friend class InterpreterData;
  friend class StateValue;
};

/// AtomValue is an ephemeral subclass of Value, for creating Value from Atom.
/// It avoids the ambiguity between Value(Atom) and Value(int32_t) since
/// Atom is typedef int32_t.
///
class AtomValue: public Value {
public:
  AtomValue(Atom a) { atom_ = a; }
};

class OrdValue: public Value {
public:
  OrdValue(int o) { ordinal_ = o; }
};

class StateValue: public Value {
public:
  StateValue() { double_ = 0; }
};

/// Interpreter holds all the extra data we need for interpreting
/// an InstrGraph:
/// - How many values are there.
/// - The offset in the values array to store each instruction's results.
/// - A pointer to the InstrGraph.
/// - max # of args in any single instruction, for storing outgoing arguments.
///
class InterpreterData {
public:
  InterpreterData(Allocator& alloc, InstrGraph*);
  int num_values() const { return ir_->def_count(); }
private:
  int max_argc_; // Max argc of any instruction.
  InstrGraph* const ir_; // The IR to interpret.

  friend class Interpreter;
};

/// InterpFrame holds the stack-allocated state of one interpreter
/// invocation.
///
class Interpreter: public KindAdapter<Interpreter, void> {
public:
  Interpreter(const InterpreterData& interpreter,
              MethodEnv* env, int argc, uint32_t* args,
              Value* values, uint32_t* args_out, Value* state)
  : values_(values)
  , args_out_(args_out)
  , env_(env)
  , signature_(env->method->getMethodSignature())
  , argc_(argc)
  , args_in_(args)
  , interpreter_(interpreter)
  , next_block_(0)
  , saved_state_(state)
  , frame_() {
    frame_.enter(core(), env);
  }

  ~Interpreter() {
    frame_.exit(core());
  }

  /** Get the Value of a Def. */
  const Value& get(const Def* d) {
    return values_[defId(d)];
  }

  /** Get the Value of def(u). */
  const Value& get(const Use& u) {
    return get(def(u));
  }

  /** Get the Atom value of def(u). */
  Atom getAtom(const Use& u) {
    return get(u).atom_;
  }

  /** Get the bool value of def(u). */
  BoolKind getBool(const Use& u) {
    return get(u).int_ ? kBoolTrue : kBoolFalse;
  }

  /** Get the String* value of def(u). */
  String* getString(const Use& u) {
    return get(u).string_;
  }

  Namespace* getNs(const Use& u) {
    return get(u).namespace_;
  }

  /** Get the ScriptObject* value of def(u). */
  ScriptObject* getObject(const Use& u) {
    return get(u).object_;
  }

  /** Get the ordinal (int) value of def(u). */
  int getOrdinal(const Use& u) {
    return get(u).ordinal_;
  }

  /** Get the MethodInfo* value of def(u). */
  MethodInfo* getMethod(const Use& u) {
    return get(u).method_;
  }

  /** Get the const Multiname* value of def(u). */
  const Multiname* getName(const Use& u) {
    return get(u).name_;
  }

  /** Get the int value of def(u). */
  int getInt(const Use& u) {
    return get(u).int_;
  }

  /** Get the uint value of def(u). */
  uint32_t getUint(const Use& u) {
    return (uint32_t) getInt(u);
  }

  /** Get the double value of def(u). */
  double getDouble(const Use& u) {
    return get(u).double_;
  }

  /** Get the Traits* value of def(u) */
  Traits* getTraits(const Use& u) {
    return get(u).traits_;
  }

  MethodEnv* getEnv(const Use& u) {
    return get(u).env_;
  }

  /** Return a reference to use to assign the value of def */
  Value& resultVal(const Def* def) {
    return values_[defId(def)];
  }

  // Interpreter invocation stubs.
  static uintptr_t intStub(MethodEnv* env, int32_t argc, uint32_t* args);
  static uintptr_t uintStub(MethodEnv* env, int32_t argc, uint32_t* args);
  static uintptr_t gprStub(MethodEnv* env, int32_t argc, uint32_t* args);
  static double fprStub(MethodEnv* env, int32_t argc, uint32_t* args);
  static Value stub(MethodEnv*, int32_t, uint32_t* args);

  // Accessors

  PrintWriter& console() {
    return core()->console;
  }
  bool isThisBoxed() {
    return env_->method->unboxThis() != 0;
  }
  AvmCore* core() {
    return env_->core();
  }
  MethodSignaturep signature() {
    return signature_;
  }
  Lattice& lattice() {
    return interpreter_.ir_->lattice;
  }

public: // dispatch() adapter methods.
  void do_default(Instr*);
  void do_start(StartInstr*);
  void do_const(ConstantExpr*);
  void do_callmethod(CallStmt2*);
  void do_callinterface(CallStmt2*);
  void do_return(StopInstr*) {}
  void do_arm(ArmInstr*);
  void do_label(LabelInstr*);
  void do_goto(GotoInstr*);
  void do_if(IfInstr*);
  void do_setslot(CallStmt2*);
  void do_getslot(CallStmt2*);
  void do_switch(SwitchInstr*);
  void do_abc_hasnext2(Hasnext2Stmt*);

  void do_speculate_number(BinaryExpr*);
  void do_speculate_int(BinaryExpr*);
  void do_newstate(ConstantExpr*);
  void do_setlocal(SetlocalInstr*);
  void do_safepoint(SafepointInstr*);

private:
  void jump(BlockEndInstr*, BlockStartInstr*);
  void coerceArgs(MethodSignaturep, int, const Use*);
  void doCallInterface(MethodInfo* signature_method, MethodEnv* callee_env,
    int argc_out, const Use* call_args, Def* value_out);
  void doCall(MethodEnv* callee_env, int argc_out, const Use* call_args,
              Def* value_out);

private:
  Value* const values_; // Stack space for values.
  uint32_t* const args_out_; // Stack space for outgoing args.
  MethodEnv* const env_;
  MethodSignaturep const signature_;
  int const argc_; // argc for this invocation.
  uint32_t* const args_in_; // arguments for this invocation.
  const InterpreterData& interpreter_;
  BlockStartInstr* next_block_; // What is the next block to run.
  Value* saved_state_;
  MethodFrame frame_;

private:
  typedef void (*StubCall)(Interpreter*, Instr*);
  static const StubCall stub_table[];
  friend class StubCaller;
};

}
#endif
