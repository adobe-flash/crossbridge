/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON
#include "hm-templatebuilder.h"

namespace halfmoon {
using avmplus::BUILTIN_any;
using avmplus::BUILTIN_object;

/// generated initializers for instruction definitions
#include "generated/InstrFactory_defs_impl.hh"

/// generated predicate impls
#include "generated/InstrFactory_preds_impl.hh"

/// generated signature builders
#include "generated/InstrFactory_signatures_impl.hh"

/// template builder
///
InstrGraph* InstrFactory::buildTemplate(InstrKind kind) {
  TemplateBuilder tb(this);
  PrintWriter& console = lattice_.console();

  if (enable_verbose || enable_typecheck)
    console << "\n=== build template for " << instr_attrs[kind].name << " ===\n";

  switch (kind) {
#include "generated/InstrFactory_buildTemplate_cases.hh"

  default: {
    assert(false && "unknown opcode");
    return NULL;
  }
  }

  if (enable_verbose || enable_typecheck)
    console << "=== done " << instr_attrs[kind].name << " ===\n\n";

  return tb.ir();
}

/// For each fixed-arg Instr leaf subclass,
/// we have an InfoManager method get(kind, factory)
/// which returns an InstrInfo object for the given kind.
/// If no cached info is found, a new one is created using
/// the given factory.
///
template<class INSTR>
InstrInfo* InfoManager::get(InstrKind kind, InstrFactory* factory) {
  InfoKey key = { kind, 0 };
  InstrInfo* info = instr_infos_.get(key);
  if (!info) {
    InstrGraph* ir = factory->hasTemplate(kind) ?
        factory->buildTemplate(kind) : NULL;
    const Type** insig = factory->buildInputSignature(kind);
    const Type** outsig = factory->buildOutputSignature(kind);
    info = new (alloc_)
        InstrInfo(INSTR::createInfo(kind, insig, outsig, ir));
    instr_infos_.put(key, info);
  }
  return info;
}

/// For each variable-arg Instr leaf subclass, we
/// have an InfoManager method get(kind, argc, factory)
/// which returns an InstrInfo object for the given kind
/// and argument count. If no cached info is found, a new
/// one is created using the given factory.
///
template<class INSTR>
InstrInfo* InfoManager::get(InstrKind kind, int argc, InstrFactory* factory) {
  InfoKey key = { kind, argc };
  InstrInfo* info = (InstrInfo*) instr_infos_.get(key);
  if (!info) {
    InstrGraph* ir = factory->hasTemplate(kind) ?
        factory->buildTemplate(kind) : NULL;
    const Type** insig = factory->buildInputSignature(kind);
    const Type** outsig = factory->buildOutputSignature(kind);
    info = new (alloc_)
        InstrInfo(INSTR::createInfo(kind, argc, insig, outsig, ir));
    instr_infos_.put(key, info);
  }
  return info;
}

InstrFactory::InstrFactory(Allocator& alloc, Lattice* lattice,
                           InfoManager* infos) :
  alloc_(alloc), lattice_(*lattice), infos_(*infos) {
}

InstrFactory::InstrFactory(InstrGraph* ir) :
  alloc_(ir->alloc()), lattice_(ir->lattice), infos_(*ir->infos()) {
}

/// private helper: copy a Type* array into allocated space
///
const Type** InstrFactory::copySig(int len, const Type** sig) {
  const Type** sig2 = new (alloc_) const Type*[len];
  for (int i = 0; i < len; ++i)
    sig2[i] = sig[i];
  return sig2;
}

/// private helper: copy a Type* into allocated singleton array
const Type** InstrFactory::copySig(const Type* t) {
  const Type** sig = new (alloc_) const Type*[1];
  sig[0] = t;
  return sig;
}

/// create an n-ary statement with the given inputs
///
NaryStmt0* InstrFactory::newNaryStmt0(InstrKind kind, Def* effect, int argc,
                                    Def* args[]) {
  assert(isNaryStmt0(kind) && "invalid opcode for n-ary statement");
  InstrInfo* info = infos_.get<NaryStmt0>(kind, argc, this);
  NaryStmt0* stmt = new (alloc_, info->num_uses, sizeof(Use)) NaryStmt0(info);
  // placement new to initialize uses.
  new (&stmt->effect_in()) Use(stmt, effect);
  for (int i = 0; i < argc; ++i)
    new (&stmt->arg(i)) Use(stmt, args[i]); // placement new
  // placement new to initialize defs.
  new (stmt->effect_out()) Def(stmt);
  new (stmt->value_out()) Def(stmt);
  return stmt;
}

/// Convert an instruction to the specified kind of nary statement.
///
NaryStmt0* InstrFactory::toNaryStmt0(InstrKind k, Instr* instr) {
  assert(isNaryStmt0(kind(instr)) && "unsupported source opcode for statement conversion");
  assert(isNaryStmt0(k) && "invalid target opcode for statement conversion");
  NaryStmt0* stmt = cast<NaryStmt0>(instr);
  stmt->info = infos_.get<NaryStmt0>(k, stmt->arg_count(), this);
  return stmt;
}

/// create an n-ary statement with the given inputs
///
NaryStmt1* InstrFactory::newNaryStmt1(InstrKind kind, Def* effect, Def* param,
                                      int argc, Def* args[]) {
  assert(isNaryStmt1(kind) && "invalid opcode for n-ary statement");
  InstrInfo* info = infos_.get<NaryStmt1>(kind, argc, this);
  NaryStmt1* stmt = new (alloc_, info->num_uses, sizeof(Use)) NaryStmt1(info);
  // placement new to initialize uses.
  new (&stmt->effect_in()) Use(stmt, effect);
  new (&stmt->info_in()) Use(stmt, param);
  for (int i = 0; i < argc; ++i)
    new (&stmt->vararg(i)) Use(stmt, args[i]); // placement new
  // placement new to initialize defs.
  new (stmt->effect_out()) Def(stmt);
  new (stmt->value_out()) Def(stmt);
  return stmt;
}

/// Create an nary2 statement with the given inputs.
///
NaryStmt2* InstrFactory::newNaryStmt2(InstrKind kind, Def* effect, Def* param1,
                                      Def* param2, int varargc, Def* varargs[]) {
  assert(isNaryStmt2(kind) && "invalid opcode");
  InstrInfo* info = infos_.get<NaryStmt2>(kind, varargc, this);
  NaryStmt2* stmt = new (alloc_, info->num_uses, sizeof(Use)) NaryStmt2(info);
  // placement new to initialize Uses
  new (&stmt->effect_in()) Use(stmt, effect);
  new (&stmt->name_in()) Use(stmt, param1);
  new (&stmt->env_in()) Use(stmt, param2);
  for (int i = 0; i < varargc; ++i)
    new (&stmt->vararg(i)) Use(stmt, varargs[i]);
  // placement new to initialize Defs
  new (stmt->effect_out()) Def(stmt);
  new (stmt->value_out()) Def(stmt);
  return stmt;
}

/// Create an nary3 statement with the given inputs.
///
NaryStmt3* InstrFactory::newNaryStmt3(InstrKind kind, Def* effect, Def* param1,
                                      Def* param2, Def* param3, int varargc,
                                      Def* varargs[]) {
  assert(isNaryStmt3(kind) && "invalid opcode");
  InstrInfo* info = infos_.get<NaryStmt3>(kind, varargc, this);
  NaryStmt3* stmt = new (alloc_, info->num_uses, sizeof(Use)) NaryStmt3(info);
  // placement new to initialize Uses
  new (&stmt->effect_in()) Use(stmt, effect);
  new (&stmt->uses()[1]) Use(stmt, param1);
  new (&stmt->uses()[2]) Use(stmt, param2);
  new (&stmt->uses()[3]) Use(stmt, param3);
  for (int i = 0; i < varargc; ++i)
    new (&stmt->vararg(i)) Use(stmt, varargs[i]);
  // placement new to initialize Defs
  new (stmt->effect_out()) Def(stmt);
  new (stmt->value_out()) Def(stmt);
  return stmt;
}

/// Create an nary4 statement with the given inputs.
///
NaryStmt4* InstrFactory::newNaryStmt4(InstrKind kind, Def* effect, Def* param1,
                                      Def* param2, Def* param3, Def* param4,
                                      int varargc, Def* varargs[]) {
  assert(isNaryStmt4(kind) && "invalid opcode");
  InstrInfo* info = infos_.get<NaryStmt4>(kind, varargc, this);
  NaryStmt4* stmt = new (alloc_, info->num_uses, sizeof(Use)) NaryStmt4(info);
  // placement new to initialize Uses
  new (&stmt->effect_in()) Use(stmt, effect);
  new (&stmt->uses()[1]) Use(stmt, param1);
  new (&stmt->uses()[2]) Use(stmt, param2);
  new (&stmt->uses()[3]) Use(stmt, param3);
  new (&stmt->uses()[4]) Use(stmt, param4);
  for (int i = 0; i < varargc; ++i)
    new (&stmt->vararg(i)) Use(stmt, varargs[i]);
  // placement new to initialize Defs
  new (stmt->effect_out()) Def(stmt);
  new (stmt->value_out()) Def(stmt);
  return stmt;
}

/// Create an nary5 statement with the given inputs.
///
NaryStmt4* InstrFactory::newNaryStmt4(InstrKind kind, Def* effect, Def* param1,
                                      Def* param2, Def* param3, Def* param4, Def* param5,
                                      int varargc, Def* varargs[]) {
  assert(isNaryStmt4(kind) && "invalid opcode");
  varargc++;
  InstrInfo* info = infos_.get<NaryStmt4>(kind, varargc, this);
  NaryStmt4* stmt = new (alloc_, info->num_uses, sizeof(Use)) NaryStmt4(info);
  // placement new to initialize Uses
  new (&stmt->effect_in()) Use(stmt, effect);
  new (&stmt->uses()[1]) Use(stmt, param1);
  new (&stmt->uses()[2]) Use(stmt, param2);
  new (&stmt->uses()[3]) Use(stmt, param3);
  new (&stmt->uses()[4]) Use(stmt, param4);
  new (&stmt->vararg(0)) Use(stmt, param5);
  for (int i = 1; i < varargc; ++i)
    new (&stmt->vararg(i)) Use(stmt, varargs[i - 1]);
  // placement new to initialize Defs
  new (stmt->effect_out()) Def(stmt);
  new (stmt->value_out()) Def(stmt);
  return stmt;
}

/// helper - create and initialize a CallStmt2, up to but not including
/// initializing the varargs
///
CallStmt2* InstrFactory::createCallStmt2(InstrKind kind, Def* effect, Def* param,
                                         Def* obj, int varargc) {
  assert(isCallStmt2(kind) && "invalid opcode for call statement");
  InstrInfo* info = infos_.get<CallStmt2>(kind, varargc, this);
  CallStmt2* call = new (alloc_, info->num_uses, sizeof(Use)) CallStmt2(info);
  // placement new to initialize Uses
  new (&call->effect_in()) Use(call, effect);
  new (&call->param_in()) Use(call, param);
  new (&call->object_in()) Use(call, obj);
  // placement new to initialize Defs
  new (call->effect_out()) Def(call);
  new (call->value_out()) Def(call);
  return call;
}

/// Create a CallStmt2 with the given inputs.
///
CallStmt2* InstrFactory::newCallStmt2(InstrKind kind, Def* effect, Def* param,
                                      Def* obj, int varargc, Def* varargs[]) {
  CallStmt2* call = createCallStmt2(kind, effect, param, obj, varargc);
  for (int i = 0; i < varargc; ++i)
    new (&call->vararg(i)) Use(call, varargs[i]);
  return call;
}

/// Create a call statement with the given inputs -
/// convenience override; takes a single argument
/// in addition to the fixed args.
///
CallStmt2* InstrFactory::newCallStmt2(InstrKind kind, Def* effect, Def* param,
                                      Def* obj, Def* arg) {
  CallStmt2* call = createCallStmt2(kind, effect, param, obj, 1);
  new (&call->vararg(0)) Use(call, arg);
  return call;
}

/// Create a call statement with the given inputs -
/// convenience override; takes fixed args only.
///
CallStmt2* InstrFactory::newCallStmt2(InstrKind kind, Def* effect, Def* param,
                                      Def* obj) {
  return createCallStmt2(kind, effect, param, obj, 0);
}

/// Convert an instruction to the specified kind of call statement.
///
CallStmt2* InstrFactory::toCallStmt2(InstrKind k, Instr* instr) {
  assert(isCallStmt2(kind(instr)) && "invalid source opcode for conversion");
  assert(isCallStmt2(k) && "invalid target opcode for conversion");
  CallStmt2* stmt = cast<CallStmt2>(instr);
  stmt->info = infos_.get<CallStmt2>(k, stmt->vararg_count(), this);
  return stmt;
}

/// helper - create and initialize a CallStmt3, up to but not including
/// initializing the varargs
///
CallStmt3* InstrFactory::createCallStmt3(InstrKind kind, Def* effect, Def* param,
                                      Def* index, Def* obj, int varargc) {
  assert(isCallStmt3(kind) && "invalid opcode for call statement");
  InstrInfo* info = infos_.get<CallStmt3>(kind, varargc, this);
  CallStmt3* call = new (alloc_, info->num_uses, sizeof(Use)) CallStmt3(info);
  // placement new to initialize Uses
  new (&call->effect_in()) Use(call, effect);
  new (&call->param_in()) Use(call, param);
  new (&call->index_in()) Use(call, index);
  new (&call->object_in()) Use(call, obj);
  // placement new to initialize Defs
  new (call->effect_out()) Def(call);
  new (call->value_out()) Def(call);
  return call;
}

/// Create a call statement with the given inputs.
///
CallStmt3* InstrFactory::newCallStmt3(InstrKind kind, Def* effect, Def* param,
                                      Def* index, Def* obj, int varargc,
                                      Def* varargs[]) {
  CallStmt3* call = createCallStmt3(kind, effect, param, index, obj, varargc);
  for (int i = 0, n = varargc; i < n; ++i)
    new (&call->vararg(i)) Use(call, varargs[i]);
  return call;
}

/// Create a call statement with the given inputs -
/// convenience override, takes a single argument
/// in addition to the fixed args.
///
CallStmt3* InstrFactory::newCallStmt3(InstrKind kind, Def* effect, Def* param,
                                      Def* index, Def* obj, Def* arg) {
  CallStmt3* call = createCallStmt3(kind, effect, param, index, obj, 1);
  new (&call->vararg(0)) Use(call, arg);
  return call;
}

/// Convert an instruction to the specified kind of call statement.
///
CallStmt3* InstrFactory::toCallStmt3(InstrKind k, Instr* instr) {
  assert(isCallStmt3(kind(instr)) && "invalid source opcode for conversion");
  assert(isCallStmt3(k) && "invalid target opcode for conversion");
  CallStmt3* stmt = cast<CallStmt3>(instr);
  stmt->info = infos_.get<CallStmt3>(k, stmt->vararg_count(), this);
  return stmt;
}

/// Create a call statement with the given inputs.
///
CallStmt4* InstrFactory::newCallStmt4(InstrKind kind, Def* effect, Def* param,
                                      Def* ns, Def* index, Def* obj,
                                      int varargc, Def* varargs[]) {
  assert(isCallStmt4(kind) && "invalid opcode for call statement 4");
  InstrInfo* info = infos_.get<CallStmt4>(kind, varargc, this);
  CallStmt4* call = new (alloc_, info->num_uses, sizeof(Use)) CallStmt4(info);
  // placement new to initialize Uses
  new (&call->effect_in()) Use(call, effect);
  new (&call->param_in()) Use(call, param);
  new (&call->ns_in()) Use(call, ns);
  new (&call->index_in()) Use(call, index);
  new (&call->object_in()) Use(call, obj);
  for (int i = 0; i < varargc; ++i)
    new (&call->vararg(i)) Use(call, varargs[i]);
  // placement new to initialize defs.
  new (call->effect_out()) Def(call);
  new (call->value_out()) Def(call);
  return call;
}

/// create a constant (0-arg, pure) instruction with the given kind
/// and output type
///
ConstantExpr* InstrFactory::newConstantExpr(InstrKind kind, const Type* type) {
  assert(isConstantExpr(kind) && "invalid opcode for constant expr");
  ConstantExpr* expr = new (alloc_)
      ConstantExpr(infos_.get<ConstantExpr>(kind, this));
  new (expr->value()) Def(expr, type);
  return expr;
}

/// creates new instance of UnaryExpr with given args
///
UnaryExpr* InstrFactory::newUnaryExpr(InstrKind k, Def* val) {
  assert(isUnaryExpr(k) && "invalid opcode for UnaryExpr");
  UnaryExpr* instr = new (alloc_) UnaryExpr(infos_.get<UnaryExpr>(k, this));
  new (&instr->value_in()) Use(instr, val);
  new (instr->value_out()) Def(instr);
  return instr;
}

/// performs in-place InstrKind conversion on instance of UnaryExpr
///
UnaryExpr* InstrFactory::toUnaryExpr(InstrKind k, Instr* instr) {
  assert(isUnaryExpr(kind(instr)) && "invalid source opcode for UnaryExpr conversion");
  assert(isUnaryExpr(k) && "invalid target opcode for UnaryExpr conversion");
  UnaryExpr* cvt = (UnaryExpr*)instr;
  cvt->info = infos_.get<UnaryExpr>(k, this);
  return cvt;
}

/// creates new instance of BinaryExpr with given args
///
BinaryExpr* InstrFactory::newBinaryExpr(InstrKind k, Def* lhs, Def* rhs) {
  assert(isBinaryExpr(k) && "invalid opcode for BinaryExpr");
  BinaryExpr* instr = new (alloc_) BinaryExpr(infos_.get<BinaryExpr>(k, this));
  new (&instr->lhs_in()) Use(instr, lhs);
  new (&instr->rhs_in()) Use(instr, rhs);
  new (instr->value_out()) Def(instr);
  return instr;
}

/// performs in-place InstrKind conversion on instance of BinaryExpr
///
BinaryExpr* InstrFactory::toBinaryExpr(InstrKind k, Instr* instr) {
  assert(isBinaryExpr(kind(instr)) && "invalid source opcode for BinaryExpr conversion");
  assert(isBinaryExpr(k) && "invalid target opcode for BinaryExpr conversion");
  BinaryExpr* cvt = (BinaryExpr*)instr;
  cvt->info = infos_.get<BinaryExpr>(k, this);
  return cvt;
}

/// creates new instance of UnaryStmt with given args
///
UnaryStmt* InstrFactory::newUnaryStmt(InstrKind k, Def* effect, Def* val) {
  assert(isUnaryStmt(k) && "invalid opcode for UnaryStmt");
  UnaryStmt* instr = new (alloc_) UnaryStmt(infos_.get<UnaryStmt>(k, this));
  new (&instr->effect_in()) Use(instr, effect);
  new (&instr->value_in()) Use(instr, val);
  new (instr->effect_out()) Def(instr);
  new (instr->value_out()) Def(instr);
  return instr;
}

/// performs in-place InstrKind conversion on instance of UnaryStmt
///
UnaryStmt* InstrFactory::toUnaryStmt(InstrKind k, Instr* instr) {
  assert(isUnaryStmt(kind(instr)) && "invalid source opcode for UnaryStmt conversion");
  assert(isUnaryStmt(k) && "invalid target opcode for UnaryStmt conversion");
  UnaryStmt* cvt = (UnaryStmt*)instr;
  cvt->info = infos_.get<UnaryStmt>(k, this);
  return cvt;
}

/// creates new instance of BinaryStmt with given args
///
BinaryStmt* InstrFactory::newBinaryStmt(InstrKind k, Def* effect, Def* lhs, Def* rhs) {
  assert(isBinaryStmt(k) && "invalid opcode for BinaryStmt");
  BinaryStmt* instr = new (alloc_) BinaryStmt(infos_.get<BinaryStmt>(k, this));
  new (&instr->effect_in()) Use(instr, effect);
  new (&instr->lhs_in()) Use(instr, lhs);
  new (&instr->rhs_in()) Use(instr, rhs);
  new (instr->effect_out()) Def(instr);
  new (instr->value_out()) Def(instr);
  return instr;
}

/// performs in-place InstrKind conversion on instance of BinaryStmt
///
BinaryStmt* InstrFactory::toBinaryStmt(InstrKind k, Instr* instr) {
  assert(isBinaryStmt(kind(instr)) && "invalid source opcode for BinaryStmt conversion");
  assert(isBinaryStmt(k) && "invalid target opcode for BinaryStmt conversion");
  BinaryStmt* cvt = (BinaryStmt*)instr;
  cvt->info = infos_.get<BinaryStmt>(k, this);
  return cvt;
}

/// create a hasnext2 statement with the given inputs
///
Hasnext2Stmt* InstrFactory::newHasnext2Stmt(Def* effect, Def* object,
                                            Def* counter) {
  Hasnext2Stmt* stmt = new (alloc_)
      Hasnext2Stmt(infos_.get<Hasnext2Stmt>(HR_abc_hasnext2, this));
  // placement new to initialize uses.
  new (&stmt->effect_in()) Use(stmt, effect);
  new (&stmt->counter_in()) Use(stmt, counter);
  new (&stmt->object_in()) Use(stmt, object);
  // placement new to initialize defs.
  new (stmt->effect_out()) Def(stmt);
  new (stmt->value_out()) Def(stmt);
  new (stmt->counter_out()) Def(stmt);
  new (stmt->object_out()) Def(stmt);
  return stmt;
}

/// Create Start instruction with given param types
///
StartInstr* InstrFactory::newStartInstr(InstrKind kind, int paramc,
                                        const Type* param_types[]) {
  Def* params = new (alloc_) Def[paramc];
  StartInstr* start = new (alloc_) StartInstr(infos_.get<StartInstr>(kind, this),
                                              paramc, false, params);
  for (int i = 0; i < paramc; ++i)
    new (&start->params[i]) Def(start, param_types[i]);
  return start;
}

/// create start instruction for the given method
/// TODO factor with above
///
StartInstr* InstrFactory::newStartInstr(MethodInfo* method) {
  MethodSignaturep sig = method->getMethodSignature();

  int fixedc = 1 /* effect */ +
               1 /* env */ +
               1 /* this */ +
               sig->param_count();
  bool rest = method->needRestOrArguments() != 0;
  int paramc = fixedc + (rest ? 1 : 0);

  Def* params = new (alloc_) Def[paramc];
  StartInstr* start = new (alloc_)
      StartInstr(infos_.get<StartInstr>(HR_start, this), paramc, rest, params);

  const Type** param_types = new (alloc_) const Type*[paramc];
  param_types[0] = EFFECT;
  param_types[1] = lattice_.makeEnvType(method);
  for (int i = 2, n = fixedc; i < n; ++i)
    param_types[i] = lattice_.makeParamType(i - 2, sig);
  if (rest)
    param_types[fixedc] = lattice_.array_type[kTypeNotNull];

  for (int i = 0; i < paramc; ++i)
    new (&start->params[i]) Def(start, param_types[i]);

  return start;
}

/// Create CatchBlock instruction with given param types
///
CatchBlockInstr* InstrFactory::newCatchBlockInstr(int paramc) {
  Def* params = new (alloc_) Def[paramc];
  CatchBlockInstr* start = new (alloc_) CatchBlockInstr(infos_.get<CatchBlockInstr>(HR_catchblock, this),
                                                        paramc, params);
  return start;
}

/// private helper: initialize an ArmInstr within a given owner
/// NOTE: static, takes an allocator so it can be called by Info::clone()
/// TODO: refactor so clone just calls a helper in here for the whole CondInstr
///
ArmInstr* InstrFactory::initArm(Allocator& alloc, int arm_pos, CondInstr* owner,
                           const InstrInfo* info) {
  ArmInstr* arm = new (alloc) ArmInstr(info);
  arm->owner = owner;
  arm->arm_pos = arm_pos;
  int param_count = owner->argc;
  arm->params = new (alloc) Def[param_count];
  for (int i = 0; i < param_count; i++)
    new (&arm->params[i]) Def(arm);
  return arm;
}

/// private helper: initialize a CondInstr
///
void InstrFactory::initCondInstr(Allocator& alloc, CondInstr* instr, Def* sel,
                                 int argc, Def* default_arg, const InstrInfo* arm_info) {
  instr->argc = argc;
  instr->uses = new (alloc) Use[argc + 1];
  instr->args = &instr->uses[1];
  new (&instr->selector()) Use(instr, sel);
  for (int i = 0; i < argc; i++)
    new (&instr->args[i]) Use(instr, default_arg);
  instr->arms = new (alloc) ArmInstr*[instr->armc];
  for (int i = 0, n = instr->armc; i < n; ++i)
    instr->arms[i] = initArm(alloc, i, instr, arm_info);
}

/// create new if instr over a given condition, number of args,
/// and args array.
///
IfInstr* InstrFactory::newIfInstr(Def* cond, int argc, Def* args[]) {
  IfInstr* instr = new (alloc_) IfInstr(infos_.get<IfInstr>(HR_if, this));
  InstrInfo *info = infos_.get<ArmInstr>(HR_arm, this);
  initCondInstr(alloc_, instr, cond, argc, NULL, info);
  for (int i = 0; i < argc; i++)
    new (&instr->args[i]) Use(instr, args[i]);
  return instr;
}

/// create new if instr over a given condition and single arg
///
IfInstr* InstrFactory::newIfInstr(Def* cond, Def* arg) {
  IfInstr* instr = new (alloc_) IfInstr(infos_.get<IfInstr>(HR_if, this));
  InstrInfo *info = infos_.get<ArmInstr>(HR_arm, this);
  initCondInstr(alloc_, instr, cond, 1, arg, info);
  return instr;
}

/// create new if instr over a given condition and pair of args
///
IfInstr* InstrFactory::newIfInstr(Def* cond, Def* arg0, Def* arg1) {
  IfInstr* instr = new (alloc_) IfInstr(infos_.get<IfInstr>(HR_if, this));
  InstrInfo *info = infos_.get<ArmInstr>(HR_arm, this);
  initCondInstr(alloc_, instr, cond, 2, NULL, info);
  new (&instr->args[0]) Use(instr, arg0);
  new (&instr->args[1]) Use(instr, arg1);
  return instr;
}

/// create new if instr over a given condition, number of args,
/// and default Def for the args to use.
///
IfInstr* InstrFactory::newIfInstr(Def* cond, int argc, Def* default_arg) {
  IfInstr* instr = new (alloc_) IfInstr(infos_.get<IfInstr>(HR_if, this));
  InstrInfo *info = infos_.get<ArmInstr>(HR_arm, this);
  initCondInstr(alloc_, instr, cond, argc, default_arg, info);
  return instr;
}

/// create new if instr over a given condition and number of args.
/// args are initialized to NULL.
///
IfInstr* InstrFactory::newIfInstr(Def* cond, int argc) {
  IfInstr* instr = new (alloc_) IfInstr(infos_.get<IfInstr>(HR_if, this));
  InstrInfo *info = infos_.get<ArmInstr>(HR_arm, this);
  initCondInstr(alloc_, instr, cond, argc, NULL, info);
  return instr;
}

/// create new switch instr with the given selector, number of cases,
/// number of args, and default Def for args to use.
/// (note: num_cases does not include default case)
///
SwitchInstr* InstrFactory::newSwitchInstr(Def* sel, int num_cases, int argc,
                                          Def* default_arg) {
  const InstrInfo* sw_info = infos_.get<SwitchInstr>(HR_switch, this);
  const InstrInfo* arm_info = infos_.get<ArmInstr>(HR_arm, this);
  SwitchInstr* instr = new (alloc_) SwitchInstr(sw_info, num_cases);
  initCondInstr(alloc_, instr, sel, argc, default_arg, arm_info);
  return instr;
}

/// create a VoidStmt with the given effect input.
///
VoidStmt* InstrFactory::newVoidStmt(InstrKind kind, Def* pred) {
  assert(isVoidStmt(kind));
  VoidStmt* block = new (alloc_) VoidStmt(infos_.get<VoidStmt>(kind, this));
  new (&block->effect_in()) Use(block, pred);
  new (block->effect_out()) Def(block);
  return block;
}

/// Initialize goto's target, with housekeeping of Label's goto list.
/// TODO: a generalized "changeTarget" function must ensure the args/params
/// have compatible arity.  It would be wrong to set goto->target = null,
/// then to a label, because in the mean time we lose track of goto's arity.
///
void initGotoTarget(GotoInstr* G, LabelInstr* to) {
  assert(!G->target && to);
  G->target = to;
  GotoInstr* N = to->preds;
  if (!N) {
    to->preds = G;
    G->next_goto = G->prev_goto = G;
  } else {
    GotoInstr* P = N->prev_goto;
    G->next_goto = N;
    G->prev_goto = P;
    N->prev_goto = G;
    P->next_goto = G;
  }
}

/// Create new goto statement with the given target.
/// Size of Goto's argument list is set to the size
/// of target's param list, with each use pointing to
/// default_def argument.
///
GotoInstr* InstrFactory::newGotoStmt(LabelInstr* target, Def* default_def) {
  int argc = target->paramc;
  const InstrInfo* info = infos_.get<GotoInstr>(HR_goto, this);
  GotoInstr* instr = new (alloc_) GotoInstr(info, new (alloc_) Use[argc]);
  for (int i = 0; i < argc; ++i)
    new (&instr->args[i]) Use(instr, default_def);
  initGotoTarget(instr, target);
  return instr;
}

/// create a LabelInstr with no predecessors and param_count Defs.
///
LabelInstr* InstrFactory::newLabelInstr(int paramc) {
  const InstrInfo* info = infos_.get<LabelInstr>(HR_label, this);
  LabelInstr* label = new (alloc_) LabelInstr(info, paramc, new (alloc_) Def[paramc]);
  for (int i = 0; i < paramc; ++i)
    new (&label->params[i]) Def(label);
  label->paramc = paramc;
  return label;
}

/// private helper - create a StopInstr
/// but leave arg array uninitialized
///
StopInstr* InstrFactory::createStopInstr(InstrKind k, int argc) {
  assert(isStopInstr(k) && "invalid opcode for StopInstr");
  return new (alloc_) StopInstr(infos_.get<StopInstr>(k, this), argc,
                                new (alloc_) Use[argc]);
}

/// creates new instance of StopInstr with given pair of args
StopInstr* InstrFactory::newStopInstr(InstrKind k, Def* effect, Def* data) {
  StopInstr* stop = createStopInstr(k, 2);
  new (&stop->args[0]) Use(stop, effect);
  new (&stop->args[1]) Use(stop, data);
  return stop;
}

/// creates new instance of StopInstr with given
/// effect arg and array of data args
///
StopInstr* InstrFactory::newStopInstr(InstrKind k, Def* effect,
                                      int data_argc, Def* data_args[]) {
  assert(isStopInstr(k) && "invalid opcode for StopInstr");
  int argc = data_argc + 1;
  StopInstr* stop = createStopInstr(k, argc);
  new (&stop->args[0]) Use(stop, effect);
  for (int i = 0; i < data_argc; ++i)
    new (&stop->args[i + 1]) Use(stop, data_args[i]);
  return stop;
}

/// create a new safepoint instr with the given inputs
///
SafepointInstr* InstrFactory::newSafepointInstr(Def* effect, int argc, Def* args[]) {
  InstrInfo* info = infos_.get<SafepointInstr>(HR_safepoint, argc, this);
  SafepointInstr* stmt =
    new (alloc_, info->num_uses, sizeof(Use)) SafepointInstr(info);
  // placement new to initialize uses.
  new (&stmt->effect_in()) Use(stmt, effect);
  for (int i = 0; i < argc; ++i)
      new (&stmt->vararg(i)) Use(stmt, args[i]); // placement new
  // placement new to initialize defs.
  new (stmt->effect_out()) Def(stmt);
  new (stmt->state_out()) Def(stmt);
  return stmt;
}

/// create a new setlocal instr with the given tuple index,
/// state tuple, and value to set
///
SetlocalInstr* InstrFactory::newSetlocalInstr(int index, Def* state, Def* val) {
  SetlocalInstr* setlocal = new (alloc_)
      SetlocalInstr(infos_.get<SetlocalInstr>(HR_setlocal, this), index);
  // placement new to initialize uses
  new (&setlocal->state_in()) Use(setlocal, state);
  new (&setlocal->value_in()) Use(setlocal, val);
  // placement new to initialize defs
  new (setlocal->state_out()) Def(setlocal);
  return setlocal;
}

/// Create a new new-style safepoint statement with the given inputs.
///
DeoptSafepointInstr* InstrFactory::newDeoptSafepointInstr(Def* effect, int argc, Def* args[]) {
  InstrInfo* info = infos_.get<DeoptSafepointInstr>(HR_deopt_safepoint, argc, this);
  DeoptSafepointInstr* stmt =
    new (alloc_, info->num_uses, sizeof(Use)) DeoptSafepointInstr(info);
  // placement new to initialize uses.
  new (&stmt->effect_in()) Use(stmt, effect);
  for (int i = 0; i < argc; ++i)
      new (&stmt->value_in(i)) Use(stmt, args[i]); // placement new
  // placement new to initialize defs.
  new (stmt->effect_out()) Def(stmt);
  return stmt;
}

/// Capture the inferred type of an expression, which should be a preceding call.
/// Used with new-style safepoints.
///
DeoptFinishCallInstr* InstrFactory::newDeoptFinishCallInstr(Def* effect, Def* val) {
  InstrInfo* info = infos_.get<DeoptFinishCallInstr>(HR_deopt_finishcall, this);
  DeoptFinishCallInstr* stmt = new (alloc_) DeoptFinishCallInstr(info);
  new (&stmt->effect_in()) Use(stmt, effect);
  new (&stmt->value_in()) Use(stmt, val);
  new (stmt->effect_out()) Def(stmt);
  return stmt;
}

/// Bracket the end of the side-effecting instructions emitted for a safepointed
/// instruction. At present, the main purpose of this is to allow us to detect
/// attempts to look up metadata for addresses for which no safepoint was recorded.
/// In the future, it may facilitate recovery of frame slots from registers.
///
DeoptFinishInstr* InstrFactory::newDeoptFinishInstr(Def* effect) {
  InstrInfo* info = infos_.get<DeoptFinishInstr>(HR_deopt_finish, this);
  DeoptFinishInstr* stmt = new (alloc_) DeoptFinishInstr(info);
  new (&stmt->effect_in()) Use(stmt, effect);
  new (stmt->effect_out()) Def(stmt);
  return stmt;
}

DebugInstr* InstrFactory::newDebugInstr(InstrKind kind, Def* effect, Def* val) {
  DebugInstr* debuginstr = new (alloc_)
      DebugInstr(infos_.get<DebugInstr>(kind, this));
  // placement new to initialize uses
  new (&debuginstr->effect_in()) Use(debuginstr, effect);
  new (&debuginstr->value_in()) Use(debuginstr, val);
  // placement new to initialize defs
  new (debuginstr->effect_out()) Def(debuginstr);
  return debuginstr;
}

/// create a new InstrGraph, using our infos and lattice
///
InstrGraph* InstrFactory::createGraph() {
  return new (alloc_) InstrGraph(this, &infos_);
}

/// helper: return the HR coercion opcode corresponding
/// to the given traits.
///
InstrKind coerceKind(Traits* traits) {
  switch (builtinType(traits)) {
    case BUILTIN_any:
      return HR_coerce_any;
    case BUILTIN_object:
      return HR_coerce_object;
    case BUILTIN_number:
      return HR_coerce_number;
    case BUILTIN_int:
      return HR_coerce_int;
    case BUILTIN_uint:
      return HR_coerce_uint;
    case BUILTIN_boolean:
      return HR_coerce_boolean;
    case BUILTIN_string:
      return HR_coerce_string;
    case BUILTIN_namespace:
      return HR_coerce_ns;
    default:
      return HR_cast;
  }
}

Copier::Copier(InstrGraph* from_ir, InstrGraph* to_ir)
: scratch0_(scratch_)
, map_(new (scratch0_) Instr*[from_ir->size()])
, to_ir_(to_ir)
, to_alloc_(to_ir->alloc()){
}

/// Make a deep copy of instr by creating a new one and copying any
/// instr specific parameters into it, then assigning input arguments
/// to deep copies of input instructions, and then finally by copying output
/// types using a shallow copy of each type.
///
/// \param instr thing to be copied
/// \param map a working array caching already copied instructions
/// \new_ir destination: graph created by the copy.
///
Instr* Copier::deepCopy(Instr* instr) {
  if (!instr)
    return 0;
  Instr* new_instr = map_[instr->id];
  if (new_instr)
    return new_instr;
  if (kind(instr) == HR_arm) {
    // Arms get copied when the owner gets copied, so do that first.
    deepCopy(((ArmInstr*) instr)->owner);
    return map_[instr->id];
  }
  new_instr = map_[instr->id] = do_shape(this, instr);
  if (isBlockStart(new_instr))
    to_ir_->assignBlockId((BlockStartInstr*)new_instr);
  else
    to_ir_->assignId(new_instr);

  // placement-new each Use in the new instruction before recursing.
  switch (kind(instr)) {
    case HR_goto:
    case HR_if:
    case HR_switch:
      // uses on these instructions were placement-new'd in their clone() function.
      break;
    default:
      for (ArrayRange<Use> r = useRange(new_instr); !r.empty(); r.popFront())
        new (&r.front()) Use(new_instr); // placement new
      break;
  }

  // placement-new each Def in the new instruction before recursing.
  for (ArrayRange<Def> r = defRange(new_instr); !r.empty(); r.popFront())
    new (&r.front()) Def(new_instr);

  // Copy types from other graph.
  // TODO: if we're cloning into a more specific context,
  //       then we should compute new types instead.
  copyRange(defRange(instr), defRange(new_instr));

  switch (kind(instr)) {
    case HR_goto: {
      GotoInstr* oldgoto = cast<GotoInstr>(instr);
      GotoInstr* newgoto = cast<GotoInstr>(new_instr);
      newgoto->target = copy(oldgoto->target);
      newgoto->next_goto = copy(oldgoto->next_goto);
      newgoto->prev_goto = copy(oldgoto->prev_goto);
      break;
    }
    case HR_label: {
      LabelInstr* oldlabel = cast<LabelInstr>(instr);
      LabelInstr* newlabel = cast<LabelInstr>(new_instr);
      newlabel->preds = copy(oldlabel->preds);
      break;
    }
    case HR_if:
    case HR_switch: {
      CondInstr* c = (CondInstr*) instr;
      CondInstr* newc = (CondInstr*) new_instr;
      // make entries in map_ before we recurse copying arm's next/prev pointers.
      for (ArrayRange<ArmInstr*> r = armRange(c), newr = armRange(newc);
          !r.empty(); r.popFront(), newr.popFront())
        map_[r.front()->id] = newr.front();
      // now copy each arm recursively
      for (ArrayRange<ArmInstr*> r = armRange(c), newr = armRange(newc);
          !r.empty(); r.popFront(), newr.popFront())
        copyArm(r.front(), newr.front());
      break;
    }
    case HR_deopt_finish: {
      // DEOPT: HR_deopt_finish points to its corresponding HR_deopt_safepoint
      DeoptFinishInstr* oldfinish = cast<DeoptFinishInstr>(instr);
      DeoptFinishInstr* newfinish = cast<DeoptFinishInstr>(new_instr);
      newfinish->safepoint = copy(oldfinish->safepoint);
      break;
    }
    case HR_deopt_finishcall: {
      // DEOPT: HR_deopt_finishcall points to its corresponding HR_deopt_safepoint
      DeoptFinishCallInstr* oldfinish = cast<DeoptFinishCallInstr>(instr);
      DeoptFinishCallInstr* newfinish = cast<DeoptFinishCallInstr>(new_instr);
      newfinish->safepoint = copy(oldfinish->safepoint);
      break;
    }
    default:
      break;
  }

  // hook up prev/next pointers
  new_instr->next_ = copy(instr->next_);
  new_instr->prev_ = copy(instr->prev_);

  // hook up inputs
  ArrayRange<Use> old_in = useRange(instr);
  ArrayRange<Use> new_in = useRange(new_instr);
  for (; !old_in.empty(); old_in.popFront(), new_in.popFront()) {
    Def* old_def = def(old_in.front());
    Instr* new_definer = copy(definer(old_def));
    new_in.front() = &getDefs(new_definer)[pos(old_def)];
  }

  //todo: follow the outputs too
  return new_instr;
}

/// Assign newarm an id, Copy def types from oldarm to newarm, and copy
/// its prev/next pointers too.
///
void Copier::copyArm(ArmInstr* oldarm, ArmInstr* newarm) {
  to_ir_->assignBlockId(newarm);
  newarm->arm_pos = oldarm->arm_pos;
  copyRange(defRange(oldarm), defRange(newarm));
  newarm->next_ = copy(oldarm->next_);
  newarm->prev_ = copy(oldarm->prev_);
}

Instr* Copier::do_default(Instr* /*instr*/) {
  assert(false && "InstrShape not supported");
  return 0;
}

Instr* Copier::do_ConstantExpr(ConstantExpr* i) {
  return new (to_alloc_) ConstantExpr(i->info);
}

Instr* Copier::do_StartInstr(StartInstr* start) {
  int paramc = start->paramc;
  return new (to_alloc_)
    StartInstr(start->info, paramc, start->rest, new (to_alloc_) Def[paramc]);
}

Instr* Copier::do_ArmInstr(ArmInstr* i) {
  ArmInstr* a = new (to_alloc_) ArmInstr(i->info);
  a->arm_pos = i->arm_pos;
  a->owner = i->owner;
  assert(false && "If owner can be a shallow copy, then remove this assert, otherwise fix this method!!!");
  return a;
}

Instr* Copier::do_IfInstr(IfInstr* oldif) {
  IfInstr* newif = new (to_alloc_) IfInstr(oldif->info);
  InstrFactory::initCondInstr(to_alloc_, newif, NULL, oldif->argc, NULL, oldif->arms[0]->info);
  return newif;
}

Instr* Copier::do_SwitchInstr(SwitchInstr* oldsw) {
  SwitchInstr* newsw = new (to_alloc_) SwitchInstr(oldsw->info, oldsw->num_cases());
  InstrFactory::initCondInstr(to_alloc_, newsw, NULL, oldsw->argc, NULL, oldsw->arms[0]->info);
  return newsw;
}

///
/// TODO FACTOR ALL OF THESE WITH InstrFactory::new*()
///
Instr* Copier::do_StopInstr(StopInstr* stop) {
  int argc = stop->argc;
  StopInstr* stop2 = new (to_alloc_) StopInstr(stop->info, argc,
                                            new (to_alloc_) Use[argc]);
  for (int i = 0; i < argc; ++i)
    new (&stop2->args[i]) Use(stop2);
  return stop2;
}

Instr* Copier::do_SafepointInstr(SafepointInstr* other) {
  SafepointInstr* vmstate = new (to_alloc_) SafepointInstr(other->info);
  vmstate->vpc = other->vpc;
  return vmstate;
}

// DEOPT: new-style safepoint
Instr* Copier::do_DeoptSafepointInstr(DeoptSafepointInstr* other) {
  DeoptSafepointInstr* vmstate =
    new (to_alloc_, other->info->num_uses, sizeof(Use)) DeoptSafepointInstr(other->info);
  vmstate->kind = other->kind;
  vmstate->vpc = other->vpc;
  vmstate->scopep = other->scopep;
  vmstate->stackp = other->stackp;
  vmstate->vlen = other->vlen;
  vmstate->nargs = other->nargs;
  vmstate->rtype = other->rtype;
  vmstate->minfo = other->minfo;
  return vmstate;
}

// DEOPT: bracket end of safepointed ABC instruction in generated code
Instr* Copier::do_DeoptFinishInstr(DeoptFinishInstr* other) {
  DeoptFinishInstr* instr = new (to_alloc_) DeoptFinishInstr(other->info);
  instr->safepoint = other->safepoint;
  return instr;
}

// DEOPT: capture result expression of call, used with new-style safepoints
Instr* Copier::do_DeoptFinishCallInstr(DeoptFinishCallInstr* other) {
  DeoptFinishCallInstr* instr = new (to_alloc_) DeoptFinishCallInstr(other->info);
  instr->safepoint = other->safepoint;
  return instr;
}

Instr* Copier::do_GotoInstr(GotoInstr* go) {
  int argc = go->target->paramc;
  GotoInstr* go2 = new (to_alloc_) GotoInstr(go->info, new (to_alloc_) Use[argc]);
  for (int i = 0; i < argc; ++i)
    new (&go2->args[i]) Use(go2); // placement new
  return go2;
}

Instr* Copier::do_LabelInstr(LabelInstr* label) {
  int paramc = label->paramc;
  return new (to_alloc_) LabelInstr(label->info, paramc, new (to_alloc_) Def[paramc]);
}

Instr* Copier::do_SetlocalInstr(SetlocalInstr* oldset) {
  return new (to_alloc_) SetlocalInstr(oldset->info, oldset->index);
}

} // namespace halfmoon
#endif // VMCFG_HALFMOON
