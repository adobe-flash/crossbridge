/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/***
 * The equivalent of jit-calls.h in CodegenLIR but specific
 * methods ONLY for halfmoon
 */

namespace halfmoon {
using namespace avmplus;

union AnyVal {
  Atom atom; // SST_atom
  const Namespace* ns; // SST_namespace
  String* str; // SST_string
  ScriptObject* obj; // SST_scriptobject
  int32_t i; // SST_int32
  uint32_t u; // SST_uint32
  int32_t b; // SST_bool32
  double d; // SST_double
};

Atom makeatom(AvmCore* core, AnyVal* native, SlotStorageType tag) {
  switch (tag) {
    default:
      AvmAssert(false);
    case SST_atom:
      AvmAssert(atomKind(native->atom) != 0);
      return native->atom;
    case SST_namespace:
      AvmAssert(atomKind(native->atom) == 0);
      return native->ns->atom();
    case SST_string:
      AvmAssert(atomKind(native->atom) == 0);
      AvmAssert(uintptr_t(native->str) > 1000 || native->str == 0);
      return native->str->atom();
    case SST_scriptobject:
      AvmAssert(atomKind(native->atom) == 0);
      return native->obj->atom();
    case SST_int32:
      return core->intToAtom(native->i);
    case SST_uint32:
      return core->uintToAtom(native->u);
    case SST_bool32:
      AvmAssert(native->b == 0 || native->b == 1);
      return native->b ? trueAtom : falseAtom;
    case SST_double:
      return core->doubleToAtom(native->d);
  }
}

void buildInterpreterFrame(AnyVal* local_vars, SlotStorageType* type_tags,
                           Atom* scope, Atom* operand, Atom* locals,
                           MethodEnv* env, AvmCore* core, int abc_pc) {
  MethodInfo* method = env->method;
  MethodSignaturep method_signature = method->getMethodSignature();
  BailoutData* deoptData = JitManager::init(method->pool())->ensureMethodData(
      method)->bailout_data;

  int sp = deoptData->getStackPointer(abc_pc);
  int scope_depth = deoptData->getScopeStack(abc_pc);
  int local_count = method_signature->local_count();
  int scope_base = method_signature->scope_base();
  int operand_base = method_signature->stack_base();

  for (int i = 0; i < local_count; i++) {
    locals[i] = makeatom(core, &local_vars[i], type_tags[i]);
  }

  for (int i = 0; i <= scope_depth - scope_base; i++) {
    int index = i + scope_base;
    scope[i] = makeatom(core, &local_vars[index], type_tags[index]);
  }

  for (int i = 0; i <= sp - operand_base; i++) {
    int index = i + operand_base;
    operand[i] = makeatom(core, &local_vars[index], type_tags[index]);
  }

  deoptData->clean();
}

/// Unbox the value, but pass it as GPR sized value
uintptr_t unboxValue(Atom retVal, MethodSignaturep method_signature) {
  switch (method_signature->returnTraitsBT()) {
    case BUILTIN_int:
      return (uintptr_t) AvmCore::integer_i(retVal);
    case BUILTIN_uint:
      return (uintptr_t) AvmCore::integer_u(retVal);
    case BUILTIN_boolean:
      return uintptr_t(retVal == trueAtom);
    case BUILTIN_string:
      return (uintptr_t) AvmCore::atomToString(retVal);
    case BUILTIN_array: {
      assert(atomKind(retVal) == kObjectType);
      return (uintptr_t) AvmCore::atomToScriptObject(retVal);
    }
    case BUILTIN_namespace:
    case BUILTIN_any:
    case BUILTIN_object:
    case BUILTIN_void:
      return (uintptr_t) retVal;
    case BUILTIN_number: // should have called deopt double
    default:
      assert(false && "Unknown return type");
      return (uintptr_t) retVal;
  }
}

void checkBailoutFrame(MethodSignaturep method_signature) {
  assert(method_signature->local_count() < 32);
  assert(method_signature->max_scope() < 16);
  assert(method_signature->max_stack() < 16);
  (void) method_signature;
}

Atom bailoutInterpreter(AvmCore* core, MethodEnv* env, int abc_pc,
                        AnyVal* local_vars, SlotStorageType* type_tags) {
  MethodInfo* method = env->method;
  JitManager* jit = JitManager::init(method->pool());
  BailoutData* bailout = jit->ensureMethodData(method)->bailout_data;
  int sp = bailout->getStackPointer(abc_pc);
  int scope_depth = bailout->getScopeStack(abc_pc);

  // Alloc here so we have cheap cleanup when we leave this method.
  Atom scope[16];
  Atom operand[16];
  Atom locals[32];

  buildInterpreterFrame(local_vars, type_tags, scope, operand, locals, env,
                        core, abc_pc);
  BaseExecMgr::resetMethodInvokers(env);
  return interpBoxedAtLocation(abc_pc, locals, operand, scope, sp, scope_depth,
                               env);
}

uintptr_t gprBailout(AvmCore* core, MethodEnv* env, int abc_pc,
                     AnyVal* local_vars, SlotStorageType* type_tags) {
  MethodInfo* method = env->method;
  checkBailoutFrame(method->getMethodSignature());
  return unboxValue(
      bailoutInterpreter(core, env, abc_pc, local_vars, type_tags),
      method->getMethodSignature());
}
FUNCTION(uintptr_t(gprBailout), SIG5(P,P,P,I,P,P), gprBailout)

double fprBailout(AvmCore* core, MethodEnv* env, int abc_pc,
                           AnyVal* local_vars, SlotStorageType* type_tags) {
  MethodInfo* method = env->method;
  checkBailoutFrame(method->getMethodSignature());
  Atom ret_val = bailoutInterpreter(core, env, abc_pc, local_vars, type_tags);
  return core->atomToDouble(ret_val);
}
FUNCTION(uintptr_t(fprBailout), SIG5(D,P,P,I,P,P), fprBailout)

ArrayObject* scriptObjectToArrayObject(ScriptObject* val) {
  // Has to be a virtual call, that's why we don't inline it into LIR
  return val->toArrayObject();
}
FUNCTION(uintptr_t(scriptObjectToArrayObject), SIG1(P,P),
         scriptObjectToArrayObject)

} // end namespace halfmoon
