/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

#include "profiler/profiler-main.h"

namespace halfmoon {
  using avmplus::CodeMgr;
  using avmplus::DomainMgr;
  using avmplus::ScopeTypeChain;
  using profiler::MethodProfile;
  using profiler::MethodProfileMgr;

JitManager::JitManager(Allocator& mgr_alloc, PoolObject* pool)
  : mgr_alloc_(mgr_alloc)
  , mgr_alloc0_(mgr_alloc)
  , lattice_(pool->core, mgr_alloc)
  , infos_(mgr_alloc)
  , meta_alloc_(&(pool->core->config.njconfig))
  , method_count_(pool->methodCount())
  , methods_(new (mgr_alloc0_) MethodData*[pool->methodCount()])
  , profile_mgr_(new (mgr_alloc) MethodProfileMgr(mgr_alloc)) {
}

JitManager* JitManager::init(PoolObject* pool) {
  CodeMgr* mgr = initCodeMgr(pool);
  JitManager* j2 = mgr->jit_mgr;
  if (!j2) {
    Allocator& mgr_alloc = mgr->allocator;
    mgr->jit_mgr = j2 = new (mgr_alloc) JitManager(mgr_alloc, pool);
  }
  return j2;
}

const InterpreterData& JitManager::interpreter(MethodEnv* env) {
  JitManager* j2 = env->method->pool()->codeMgr->jit_mgr;
  return *j2->methods_[env->method->method_id()]->interp_data;
}

void JitManager::set_interp(MethodInfo* method, InstrGraph* ir) {
  MethodData* data = ensureMethodData(method);
  data->interp_data = new (mgr_alloc_) InterpreterData(mgr_alloc_, ir);
}

void JitManager::set_lir(MethodInfo* method, InstrGraph* ir, 
                        ProfiledInformation* profiled_info) {
  MethodData* data = ensureMethodData(method);

  Context cxt(method);
  //TODO: Pass both results via pointers for consistency.
  data->code = LirEmitter(&cxt, ir, profiled_info).emit(&data->deopt_data);
}

void JitManager::set_ir(MethodInfo* method, InstrGraph* ir) {
  ensureMethodData(method)->ir = ir;
}

InstrGraph* JitManager::ir(MethodInfo* method) {
  return ensureMethodData(method)->ir;
}

GprMethodProc JitManager::getImpl(MethodInfo* method) {
  int method_id = method->method_id();
  if (haveCode(method_id))
    return methods_[method_id]->code;
  if (haveInterp(method_id))
    switch (method->getMethodSignature()->returnTraitsBT()) {
      case BUILTIN_number:  return (GprMethodProc)Interpreter::fprStub;
      case BUILTIN_int:     return Interpreter::intStub;
      case BUILTIN_uint:
      case BUILTIN_boolean: return Interpreter::uintStub;
      default: return Interpreter::gprStub;
    }
  return 0;
}

// Allocate a new MethodData object.
JitManager::MethodData* JitManager::ensureMethodData(MethodInfo* method) {
  assert(checkMethodId(method->method_id()));
  int method_id = method->method_id();
  MethodData* method_data = methods_[method_id];
  if (!method_data) {
    MethodSignaturep method_signature = method->getMethodSignature();
    int scope_base = method_signature->scope_base();
    int operand_base = method_signature->stack_base();

    BailoutData* meta_data = new (mgr_alloc_)
                               BailoutData(mgr_alloc_, method,
                               scope_base - 1, operand_base - 1);
    methods_[method_id] = method_data = new (mgr_alloc_) MethodData(meta_data);
  }

  return method_data;
}

MethodProfile* JitManager::getProfile(MethodInfo* info) {
  CodeMgr* mgr = info->pool()->codeMgr;
  if (!mgr || !mgr->jit_mgr)
    return 0;
  return mgr->jit_mgr->profile_mgr()->getMethodProfile(info);
}

} // end namespace halfmoon
#endif // VMCFG_HALFMOON
