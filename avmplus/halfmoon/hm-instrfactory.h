/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INSTR_FACTORY_H_
#define INSTR_FACTORY_H_

namespace halfmoon {

/// generated instruction enums, descriptors
///
#include "generated/InstrFactory_defs_proto.hh"

/// Choose which coerce opcode to use, depending on target traits.
///
InstrKind coerceKind(Traits* traits);

/// InstrFactory - API for creating and converting instructions.
///
class InstrFactory {
public:
  InstrFactory(Allocator& alloc, Lattice* lattice, InfoManager* infos);
  explicit InstrFactory(InstrGraph* ir);

  /// generated predicate prototypes
  #include "generated/InstrFactory_preds_proto.hh"

  // nary stmt
  NaryStmt0* newNaryStmt0(InstrKind kind, Def* effect, int argc, Def* args[]);
  NaryStmt0* toNaryStmt0(InstrKind kind, Instr*);

  NaryStmt1* newNaryStmt1(InstrKind kind, Def* effect, Def* param,
                          int argc, Def* args[]);

  NaryStmt2* newNaryStmt2(InstrKind kind, Def* effect, Def* param1,
                          Def* param2, int varargc, Def* varargs[]);

  NaryStmt3* newNaryStmt3(InstrKind kind, Def* effect, Def* param1, Def* param2,
                          Def* param3, int varargc, Def* varargs[]);

  NaryStmt4* newNaryStmt4(InstrKind kind, Def* effect, Def* param1, Def* param2,
                          Def* param3, Def* param4, int varargc, Def* varargs[]);
  NaryStmt4* newNaryStmt4(InstrKind kind, Def* effect, Def* param1, Def* param2,
                          Def* param3, Def* param4, Def* param5, int varargc, Def* varargs[]);

  // call stmt with multiname
  CallStmt2* newCallStmt2(InstrKind kind, Def* effect, Def* param,
                        Def* obj, int varargc, Def* varargs[]);
  CallStmt2* newCallStmt2(InstrKind kind, Def* effect, Def* param,
                          Def* obj, Def* arg);
  CallStmt2* newCallStmt2(InstrKind kind, Def* effect, Def* param,
                          Def* obj);
  CallStmt2* toCallStmt2(InstrKind kind, Instr* instr);

  // call stmt with multiname, and either namespace or index
  CallStmt3* newCallStmt3(InstrKind kind, Def* effect, Def* param, Def* index,
                          Def* obj, int varargc, Def* varargs[]);
  CallStmt3* newCallStmt3(InstrKind kind, Def* effect, Def* param, Def* index,
                          Def* obj, Def* arg);
  CallStmt3* toCallStmt3(InstrKind kind, Instr* instr);

  // call stmt with multiname, namespace, and index
  CallStmt4* newCallStmt4(InstrKind kind, Def* effect, Def* param, Def* ns, Def* index,
                          Def* obj, int varargc, Def* varargs[]);

  // constant expr
  ConstantExpr* newConstantExpr(InstrKind kind, const Type* type);

  /// unary expr
  UnaryExpr* newUnaryExpr(InstrKind k, Def* val);
  UnaryExpr* toUnaryExpr(InstrKind k, Instr* instr);

  /// binary expr
  BinaryExpr* newBinaryExpr(InstrKind k, Def* lhs, Def* rhs);
  BinaryExpr* toBinaryExpr(InstrKind k, Instr* instr);

  /// unary stmt
  UnaryStmt* newUnaryStmt(InstrKind k, Def* effect, Def* val);
  UnaryStmt* toUnaryStmt(InstrKind k, Instr* instr);

  /// binary stmt
  BinaryStmt* newBinaryStmt(InstrKind k, Def* effect, Def* lhs, Def* rhs);
  BinaryStmt* toBinaryStmt(InstrKind k, Instr* instr);

  // hasnext2 statement
  Hasnext2Stmt* newHasnext2Stmt(Def* effect, Def* obj, Def* counter);

  // void stmt
  VoidStmt* newVoidStmt(InstrKind kind, Def* pred);

  // label instr
  LabelInstr* newLabelInstr(int param_count);

  // start instr
  StartInstr* newStartInstr(MethodInfo* method);
  StartInstr* newStartInstr(InstrKind kind, int num_params, const Type* param_types[]);

  // catchblock instr
  CatchBlockInstr* newCatchBlockInstr(int num_params);

  // if instr
  IfInstr* newIfInstr(Def* cond, int argc, Def* args[]);      // n-arg array
  IfInstr* newIfInstr(Def* cond, Def* arg);                   // single arg
  IfInstr* newIfInstr(Def* cond, Def* arg0, Def* arg1);       // pair of args
  IfInstr* newIfInstr(Def* cond, int argc, Def* default_arg); // n-arg default
  IfInstr* newIfInstr(Def* cond, int argc);                   // n-arg NULL

  // switch instr
  SwitchInstr* newSwitchInstr(Def* sel, int num_cases, int argc, Def* default_arg);

  // goto statement
  GotoInstr* newGotoStmt(LabelInstr* target, Def* default_def = 0);

  /// stop instruction
  StopInstr* newStopInstr(InstrKind k, Def* effect, Def* data);
  StopInstr* newStopInstr(InstrKind k, Def* effect, int data_argc, Def* data_args[]);

  // safepoint instr
  SafepointInstr* newSafepointInstr(Def* effect, int argc, Def* args[]);

  // DEOPT: new-style safepoint instr
  DeoptSafepointInstr* newDeoptSafepointInstr(Def* effect, int argc, Def* args[]);
  DeoptFinishInstr* newDeoptFinishInstr(Def* effect);
  DeoptFinishCallInstr* newDeoptFinishCallInstr(Def* effect, Def* val);

  // debugfile and debugline
  DebugInstr* newDebugInstr(InstrKind kind, Def* effect, Def* val);

  // setlocal instr
  SetlocalInstr* newSetlocalInstr(int index, Def* state, Def* val);

  // create a new InstrGraph over our infos and lattice
  InstrGraph* createGraph();

private:
  // private helper - create a CallStmt2, up to vararg initialization
  CallStmt2* createCallStmt2(InstrKind kind, Def* effect, Def* param,
                             Def* obj, int varargc);

  // private helper - create a CallStmt3, up to vararg initialization
  CallStmt3* createCallStmt3(InstrKind kind, Def* effect, Def* param, Def* index,
                             Def* obj, int varargc);

  // private helper - initialize a CondInstr in place
  static void initCondInstr(Allocator&, CondInstr* instr, Def* sel,
                            int argc, Def* default_arg, const InstrInfo* arm_info);

  // private helper - initialize ArmInstr in place
  static ArmInstr* initArm(Allocator&, int arm_pos, CondInstr* owner,
                      const InstrInfo* info);

  /// private helper - create and partially initialize StopInstr
  StopInstr* createStopInstr(InstrKind k, int argc);

private:
  friend class InfoManager;
  friend class Copier;

  /// Build a template's ir graph.
  /// Info ctors call buildTemplate() to obtain subgraphs
  /// for instrs that have templates.
  InstrGraph* buildTemplate(InstrKind kind);

  /// helpers - make allocated copies of Type* arrays and singletons
  const Type** copySig(int len, const Type** sig);
  const Type** copySig(const Type* t);

  /// helpers - build an instruction's signatures
  const Type** buildInputSignature(InstrKind kind);
  const Type** buildOutputSignature(InstrKind kind);

public:
  Allocator& alloc() {
    return alloc_;
  }

  Lattice& lattice() {
    return lattice_;
  }

private:
  Allocator& alloc_;
  Lattice& lattice_;
  InfoManager& infos_;
  bool dead;
};

/// InfoManager creates and holds InstrInfo singletons uniquely identified
/// by InstrKind, (InstrKind, argc) or (InstrKind, argc, argmax).
///
class InfoManager {
private:
  // InfoKey is a POD object used as a key to intern InstrInfo instances.
  struct InfoKey {
    InstrKind kind;
    int argc;
    bool operator==(const InfoKey& that) const {
      return this->kind == that.kind && this->argc == that.argc;
    }
    bool operator!=(const InfoKey& that) const {
      return !(*this == that);
    }
    // for HashMap
    static size_t hash(const InfoKey& k) {
      return (k.kind << 2) + k.argc;
    }
  };

public:
  InfoManager(Allocator& alloc) :
      alloc_(alloc), instr_infos_(alloc, HR_MAX) {
  }

  // get or build Info instance for given kind[, arg count[, max args]].
  // instr_infos_ is cache.
  template<class INSTR> InstrInfo* get(InstrKind kind, InstrFactory*);
  template<class INSTR> InstrInfo* get(InstrKind kind, int argc, InstrFactory*);

private:
  Allocator& alloc_;
  HashMap<InfoKey, InstrInfo*, InfoKey, UseOperEqual<InfoKey> > instr_infos_;
};

} // namespace halfmoon
#endif
