/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {
using avmplus::atomGetIntptr;
using avmplus::atomIsIntptr;
using avmplus::BIND_NONE;
using avmplus::CodeMgr;
using avmplus::DomainMgr;
using avmplus::ScopeTypeChain;
using avmplus::TraitsBindingsp;
using avmplus::MethodInfo;

/// Builder for customized inliners.
/// todo: move sharable code from here and AbcBuilder into InstrGraphBuilder.
/// it may be that everything here should be folded into InstrGraphBuilder,
/// but its hard to tell, so I've left it here for now in a form that is
/// taylored to the needs of Specializer.
///
class SpecialBuilder: public InstrGraphBuilder {
public:
  SpecialBuilder(InstrGraph* ir, InstrFactory* factory, Instr* pos, const Use& effect_in)
  : InstrGraphBuilder(ir, factory, pos)
  , ir_(ir)
  , lattice_(&ir->lattice)
  , effect_(def(effect_in)) {
  }

  SpecialBuilder(InstrGraph* ir, InstrFactory* factory, Instr* pos)
  : InstrGraphBuilder(ir, factory, pos)
  , ir_(ir)
  , lattice_(&ir->lattice)
  , effect_(0) {
  }

  Def* addOrdinal(int ordinal) {
    return addConst(lattice_->makeOrdinalConst(ordinal));
  }

  Def* addCallStmt2(InstrKind kind, Def* name, Def* obj) {
    CallStmt2* stmt = factory().newCallStmt2(kind, effect_, name, obj, 0, 0);
    addInstr(stmt);
    effect_ = stmt->effect_out();
    return stmt->value_out();
  }

  Def* addConst(const Type* t) {
    ConstantExpr* instr = factory().newConstantExpr(HR_const, t);
    addInstr(instr);
    return instr->value();
  }

  Def* coerceExpr(Traits* traits, Def* value) {
    if (coerceIsNop(lattice_, value, traits))
      return value;
    Def* traits_in = addConst(lattice_->makeTraitsConst(traits));
    return addBinaryStmt(coerceKind(traits), traits_in, value);
  }

  Def* cknull(Def* ptr) {
    if (!isNullable(type(ptr)))
      return ptr;
    UnaryStmt* stmt = factory().newUnaryStmt(HR_cknull, effect_, ptr);
    addInstr(stmt);
    effect_ = stmt->effect_out();
    return stmt->value_out();
  }

  /// Convert the given CallStmt2 to a different opcode, then re-evaluate
  /// the new instruction's type.
  ///
  void toCallStmt2(InstrKind k, CallStmt2* instr) {
    factory().toCallStmt2(k, instr);
    TypeAnalyzer a(ir_);
    a.computeTypes(instr); // new opcode, so compute new types.
  }

  Def* effect() {
    return effect_;
  }

  Def* addExpr(InstrKind k, Def* v1) {
    return addExpr(factory().newUnaryExpr(k, v1));
  }
  Def* addExpr(InstrKind k, Def* v1, Def* v2) {
    return addExpr(factory().newBinaryExpr(k, v1, v2));
  }

  template <class STMT>
  Def* addStmt(STMT* stmt) {
    addInstr(stmt);
    effect_ = stmt->effect_out();
    return stmt->value_out();
  }

  template <class EXPR>
  Def* addExpr(EXPR* expr) {
    addInstr(expr);
    return expr->value_out();
  }

private:
  // FIXME: it goes against the grain to take opcode + Defs here, our
  // intent was to avoid api's like this to builder classes.  However,
  // this is convenient for now.
  Def* addBinaryStmt(InstrKind kind, Def* lhs, Def* rhs) {
    BinaryStmt* stmt = factory().newBinaryStmt(kind, effect_, lhs, rhs);
    addInstr(stmt);
    effect_ = stmt->effect_out();
    return stmt->value_out();
  }

private:
  InstrGraph* ir_;          // What IR are we adding code to.
  Lattice* const lattice_;  // its lattice, here for convenience.
  Def* effect_;             // points to the current EFFECT input.
};

/// Helper: if definer of v is a UnaryExpr k(x:t), return x
///
Def* matchUnaryExpr(const Use& v, InstrKind k, const Type* t) {
  assert(InstrFactory::isUnaryExpr(k));
  Def* v2;
  Instr* i = definer(v);
  return (kind(i) == k &&
      subtypeof(type(v2 = def(cast<UnaryExpr>(i)->value_in())), t)) ?
          v2 : 0;
}

/// Helper: if definer of v is a UnaryExpr k(x), return x
///
Def* matchUnaryExpr(const Use& v, InstrKind k) {
  assert(InstrFactory::isUnaryExpr(k));
  Instr* i = definer(v);
  return kind(i) == k ? def(cast<UnaryExpr>(i)->value_in()) : 0;
}

ScopeKind findScope(Lattice* lattice_, NaryStmt3* instr, int* index_) {
  assert(kind(instr) == HR_abc_findproperty ||
         kind(instr) == HR_abc_findpropstrict);
  const Type* name_type = type(instr->name_in());
  assert(isConst(name_type) && isName(name_type));
  int scope_count = instr->vararg_count();
  Def* env = def(instr->env_in());
  const Use* scopes = instr->varargs();
  MethodInfo* caller_method = getMethod(type(env));
  PoolObject* pool = caller_method->pool();
  if (!nameVal(name_type)->isBinding())
    return kScopeNotFound; // not an ordinary known lexical name.

  // Don't short circuit dynamic lookups
  assert(isOrdinal(type(instr->index_in())));
  int withbase = ordinalVal(type(instr->index_in()));
  if (withbase != -1)
    return kScopeNotFound;

  // Search local scopes.
  const ScopeTypeChain* scope = caller_method->declaringScope();
  int base = (scope->size == 0) ? 1 : 0; // Don't try to early bind to global.
  for (int index = scope_count - 1; index >= base; --index) {
    Binding b = lattice_->toBinding(type(scopes[index]), name_type);
    if (isValidBinding(b))
      return (*index_ = index), kScopeLocal;
    if (false /* fixme: if is with scope */)
      return kScopeNotFound;
  }

  // Search outer scopes.
  for (int index = scope->size - 1; index > 0; --index) {
    Binding b = lattice_->toBinding(scope->getScopeTraitsAt(index), name_type);
    if (isValidBinding(b))
      return (*index_ = index), kScopeOuter;
    if (scope->getScopeIsWithAt(index))
      return kScopeNotFound; // Don't look past enclosing with block.
  }

  // Search domain.
  DomainMgr* domain = pool->core->domainMgr();
  const Multiname* multiname = nameVal(name_type);
  MethodInfo* script = domain->findScriptInPoolByMultiname(pool, *multiname);
  if (isValidBinding((Binding) script)) {
    if (script != caller_method)
      return kScopeDomain; // use finddef.
    if (scope->size == 0)
      return (*index_ = 0), kScopeLocal;
    return (*index_ = 0), kScopeOuter;
  }
  return kScopeNotFound;
}

CallAnalyzer::CallAnalyzer(const Type* object_type, const Type* name_type,
                           Lattice* lattice, int extra_argc)
: binding(avmplus::BIND_NONE) {
  Binding b = lattice->toBinding(object_type, name_type);
  if (isMethod(b) && lattice->isResolved(object_type)) {
    MethodInfo* callee = lattice->getSlotMethod(object_type, toMethod(b));
    MethodSignaturep ms = callee->getMethodSignature();

    if (ms->argcOk(extra_argc)) { 
      this->signature = ms;
      if (isInterface(callee))
        this->disp_id = -1;
      else
        this->disp_id = toMethod(b);
      this->is_interface = isInterface(callee);
      this->binding = b;
      this->method = callee;
    }
  } else if (isSlot(b)) {
    this->binding = b;
  }
}

/*
TODO:
* muld -1 * x => -x
* muld x * -1 => -x
* mixed int/uint comparisons - sign extend on 64bit
*/

Specializer::Specializer(InstrGraph* ir)
: ir_(ir)
, factory_(ir)
, lattice_(&ir->lattice) {
}

void Specializer::specialize(Instr* instr) {
  do_instr(this, instr);
}

InstrKind Specializer::getLoadEnvKind(const Type* object, bool is_interface) {
  switch (kind(object)) {
  default: assert(false && "Unknown load env type");
  case kTypeScriptObject: return is_interface ? HR_loadenv_interface : HR_loadenv;
  case kTypeString: return HR_loadenv_string;
  case kTypeNumber: return HR_loadenv_number;
  case kTypeBoolean: return HR_loadenv_boolean;
  case kTypeNamespace: return HR_loadenv_namespace;
  case kTypeAny:
  case kTypeVoid:
  case kTypeObject:
    return HR_loadenv_atom;
  }
}

void Specializer::do_abc_getprop(CallStmt2* instr) {
  assert(instr->vararg_count() == 0);
  const Use& object_in = instr->object_in();
  Binding b = toBinding(lattice_, object_in, instr->param_in());
  if (isSlot(b)) {
    factory_.toCallStmt2(HR_abc_getprop_slot, instr);
  } else if (hasGetter(b) && lattice_->isResolved(type(object_in))) {
    // Early bind to a getter.
    // Fixme: if we create a toGetterIndex opcode then this can be a template.
    SpecialBuilder builder(ir_, &factory_, instr, instr->effect_in());
    Def* object_def = builder.cknull(def(object_in));
    uint32_t getter_index = toGetterIndex(b);

    Traits* object_traits = getTraits(type(object_in));
    const TraitsBindingsp objtd = object_traits->getTraitsBindings();
    MethodInfo* method = objtd->getMethod(getter_index);
    bool is_interface = object_traits->isInterface();
    assert (method != NULL);

    InstrKind env_kind = getLoadEnvKind(type(object_in), is_interface);
    assert (kind(type(object_in)) != kTypeAny);
    Def* env_param = is_interface ?
      builder.addConst(lattice_->makeMethodConst(method)) :
      builder.addOrdinal(toGetterIndex(b)); 

    Def* env_def = builder.addExpr(env_kind, env_param, object_def);
    instr->object_in() = object_def;
    instr->param_in() = env_def;

    InstrKind callKind = is_interface ? HR_callinterface : HR_callmethod;
    builder.toCallStmt2(callKind, instr);
  }
}

struct IndexOpcodes {
  InstrKind uint_kind, int_kind, double_kind;
};

struct IndexOpcodes2 {
  IndexOpcodes vectorint_ops,
               vectoruint_ops,
               vectordouble_ops,
               array_ops,
               default_ops;
};

/** Specialized forms of HR_getpropx */
static const IndexOpcodes2 get_ops = {
  //  [uint]           [int]            [Number]
    { HR_getpropx_viu, HR_getpropx_vii, HR_getpropx_vid }, // Vector.<int>
    { HR_getpropx_vuu, HR_getpropx_vui, HR_getpropx_vud }, // Vector.<uint>
    { HR_getpropx_vdu, HR_getpropx_vdi, HR_getpropx_vdd }, // Vector.<Number>
    { HR_getpropx_au,  HR_getpropx_ai,  HR_getpropx_ad },  // Array
    { HR_getpropx_u,   HR_getpropx_i,   HR_getpropx_d }    // *
};

/** Specialized forms of HR_setpropx */
static const IndexOpcodes2 set_ops = {
  //  [uint]           [int]            [Number]
    { HR_setpropx_viu, HR_setpropx_vii, HR_setpropx_vid }, // Vector.<int>
    { HR_setpropx_vuu, HR_setpropx_vui, HR_setpropx_vud }, // Vector.<uint>
    { HR_setpropx_vdu, HR_setpropx_vdi, HR_setpropx_vdd }, // Vector.<Number>
    { HR_setpropx_au,  HR_setpropx_ai,  HR_setpropx_ad },  // Array
    { HR_setpropx_u,   HR_setpropx_i,   HR_setpropx_d }    // *
};

/** Choose an opcode based on the object type and index type */
InstrKind choose(Lattice* l_, const Type* index_type, const Type* object_type,
                 const IndexOpcodes2& ops2) {
  const IndexOpcodes& ops =
    subtypeof(object_type, l_->vectorint_type[kTypeNotNull]) ? ops2.vectorint_ops :
    subtypeof(object_type, l_->vectoruint_type[kTypeNotNull]) ? ops2.vectoruint_ops :
    subtypeof(object_type, l_->vectordouble_type[kTypeNotNull]) ? ops2.vectordouble_ops :
    subtypeof(object_type, l_->array_type[kTypeNotNull]) ? ops2.array_ops :
    /* else */ ops2.default_ops;
  return isUInt(index_type) ? ops.uint_kind :
         isInt(index_type) ? ops.int_kind :
         /* else */ ops.double_kind;
}

void Specializer::do_abc_getpropx(CallStmt3* instr) {
  const Type* index_type = type(instr->index_in());
  if (maybeIndex(type(instr->param_in()), index_type)) {
    factory_.toCallStmt3(choose(lattice_, index_type, type(instr->object_in()),
                                get_ops),
                         instr);
  }
}

void Specializer::do_abc_setpropx(CallStmt3* instr) {
  const Type* index_type = type(instr->index_in());
  if (maybeIndex(type(instr->param_in()), index_type)) {
    factory_.toCallStmt3(choose(lattice_, index_type, type(instr->object_in()),
                                set_ops),
                         instr);
  }
}

void Specializer::do_abc_setprop(CallStmt2* instr) {
  const Use& obj_in = instr->object_in();
  const Use& name_in = instr->param_in();
  Binding b = toBinding(lattice_, obj_in, name_in);
  if (isVarSlot(b))
    factory_.toCallStmt2(HR_abc_setprop_slot, instr);
}

void Specializer::do_abc_initprop(CallStmt2* instr) {
  // treat initprop as the more restrictive setslot.  Do not optimize
  // initprop for const slots here, because we don't know the caller.
  return do_abc_setprop(instr);
//  const Type* object_type = type(instr->object_in());
//  const Type* name_type = type(instr->param_in());
//  Traits* object_traits = getTraits(object_type);
//  if (!object_traits || !isConst(name_type))
//    return;
//  TraitsBindingsp tb = object_traits->getTraitsBindings();
//  Traits* declarer;
//  Binding b = tb->findBindingAndDeclarer(*nameVal(name_type), declarer);
//  if (isVarSlot(b) || (isConstSlot(b) && declarer->init == ir_->method())) {
//    // this is a) a slot access, and b) a legal assignment if the slot is const.
//    factory_.toCallStmt2(HR_setprop_slot, instr);
//  }
}

/// Try to specialize a comparison operator based on the operand types.
///
void Specializer::doCompare(BinaryExpr* instr, InstrKind int_kind,
                            InstrKind uint_kind, InstrKind double_kind) {
  const Type* t1 = type(instr->lhs_in());
  const Type* t2 = type(instr->rhs_in());
  if (isInt(t1) && isInt(t2))
    factory_.toBinaryExpr(int_kind, instr);
  else if (isUInt(t1) && isUInt(t2))
    factory_.toBinaryExpr(uint_kind, instr);
  else if (isNumber(t1) && isNumber(t2))
    factory_.toBinaryExpr(double_kind, instr);
}

/// Change each use of old_def to point to new_def.
///
void connectUsesToDef(const Def& old_def, Def* new_def) {
  if (&old_def == new_def)
    return;
  for (UseRange u(old_def); !u.empty(); u.popFront())
    if (user(u.front()) != definer(new_def))
      u.front() = new_def;
}

bool Specializer::specializeSlotCallProp(CallStmt2* instr, CallAnalyzer* call_analyzer) {
  const TraitsBindingsp tb = getTraits(type(instr->object_in()))->getTraitsBindings();
  int slot_id = AvmCore::bindingToSlotId(call_analyzer->binding);
  Traits* slotType = tb->getSlotTraits(slot_id);

  // So far only callprop_string is specialized based on slot type
  // although we may need one for every builtin type.
  if (slotType == lattice_->core()->traits.string_ctraits) {
    factory_.toCallStmt2(HR_callprop_string, instr);
    return true;
  }

  return false;
}

bool Specializer::specializeMethodCallProp(CallStmt2* instr, CallAnalyzer* call_analyzer) {
  // Early bind virtual call.
  Use& obj = instr->object_in();
  Use& name = instr->param_in();
  int extra_argc = instr->vararg_count();

  SpecialBuilder builder(ir_, &factory_, instr, instr->effect_in());
  //so name is not a name anymore.. it's an ordinal.. presumably index of virtual function slot (matz)
  Def* object_def = builder.coerceExpr(getParamTraits(call_analyzer->signature, 0), def(obj));
  obj = object_def;
  InstrKind env_kind = getLoadEnvKind(type(object_def), call_analyzer->is_interface);

  Def* env_param = call_analyzer->is_interface ? 
    builder.addConst(lattice_->makeMethodConst(call_analyzer->method)) :
    builder.addOrdinal(call_analyzer->disp_id);

  name = builder.addExpr(env_kind, env_param, object_def);

  for (int i = 0; i < extra_argc; ++i) {
    Use& arg = instr->vararg(i);
    arg = builder.coerceExpr(getParamTraits(call_analyzer->signature, i + 1), def(arg));
  }

  InstrKind methodKind = call_analyzer->is_interface ? HR_callinterface : HR_callmethod;
  instr->effect_in() = builder.effect();
  builder.toCallStmt2(methodKind, instr);
  return true;
}

void Specializer::do_abc_callprop(CallStmt2* instr) {
  Use& obj = instr->object_in();
  Use& name = instr->param_in();
  int extra_argc = instr->vararg_count();

  CallAnalyzer b(type(obj), type(name), lattice_, extra_argc);
  if (isMethod(b.binding)) {
    specializeMethodCallProp(instr, &b);
    return;
  }

  if (isSlot(b.binding) && (extra_argc == 1)) {
    if (specializeSlotCallProp(instr, &b)) return;
  }
}

void Specializer::do_constructsuper(CallStmt2* instr) {
  // this could be in a template if the template could delegate varargs.
  // constructsuper(env, [args]) => callmethod(loadsuperinitenv(env), args)
  Use& env_in = instr->param_in();
  SpecialBuilder builder(ir_, &factory_, instr, instr->effect_in());
  env_in = builder.addExpr(HR_loadsuperinitenv, def(env_in));
  factory_.toCallStmt2(HR_callmethod, instr);
}

void Specializer::do_abc_constructprop(CallStmt2* instr) {
  const Use& obj = instr->object_in();
  const Use& name = instr->param_in();
  const Type* obj_type = type(obj);

  if (isDataType(*obj_type)) { // See regression bug_638233
    // A null converted to a not null is bottom
    // can't call to binding on a non datatype
    Binding b = lattice_->toBinding(obj_type, type(name));
    if (isSlot(b)) {
      // replace
      //   constructproperty(name, obj, args...)
      // with
      //   construct(getslot(obj, slot), null, args...)
      // fixme: template system cant handle variadic cases
      SpecialBuilder builder(ir_, &factory_, instr, instr->effect_in());
      Def* slot = builder.addOrdinal(toSlot(b));
      Def* constructor = builder.addCallStmt2(HR_getslot, slot, def(obj));
      instr->effect_in() = builder.effect();
      instr->param_in() = constructor;
      instr->object_in() = builder.addConst(lattice_->null_type);
      factory_.toCallStmt2(HR_construct, instr);
    }
  }
}

/// Return true if we are able to bind to constructor of the given class type.
bool canBindConstructor(Traits* ctraits, int actual_argc) {
  // fixme: this is copied from CodegenLIR::emitConstruct(); factor it out instead.
  Traits* itraits;
  MethodInfo* init;
  return ctraits && !ctraits->hasCustomConstruct
      && (itraits = ctraits->itraits) != NULL && (init = itraits->init) != NULL
      && init->isResolved()
      && init->getMethodSignature()->argcOk(actual_argc - 1);
}

void Specializer::do_construct(CallStmt2* instr) {
  Def* ctor = def(instr->param_in());
  Traits* ctraits = getTraits(type(ctor));
  int actual_argc = instr->arg_count();
  if (!canBindConstructor(ctraits, actual_argc))
    return;
  SpecialBuilder builder(ir_, &factory_, instr, instr->effect_in());
  // inline early-bind code, copied from CodegenLIR::emitConstruct().
  ctor = builder.coerceExpr(ctraits, ctor); // Atom -> ScriptObject*
  ctor = builder.cknull(ctor);
  Def* object = builder.addExpr(HR_newinstance, ctor);
  Def* env = builder.addExpr(HR_loadinitenv, object);
  MethodSignaturep signature = ctraits->itraits->init->getMethodSignature();
  Allocator scratch;
  int arg_count = instr->arg_count();
  Def** args = new (scratch) Def*[arg_count];
  for (int i = 1, n = instr->arg_count(); i < n; ++i)
    args[i] = builder.coerceExpr(getParamTraits(signature, i),
                                          def(instr->arg(i)));
  CallStmt2* call = factory_.newCallStmt2(HR_callmethod, builder.effect(),
                                          env, object, arg_count - 1, args + 1);
  builder.addStmt(call);
  connectUsesToDef(*instr->value_out(), object);
  connectUsesToDef(*instr->effect_out(), call->effect_out());
}

void Specializer::doFindStmt(NaryStmt3* instr) {
  (void)instr;
#if 0
  // Can't in place replace an NaryStmt2 with a BinaryStmt, so just
  // disable this for now.  It will be deleted anyway.
  int index;
  switch (findScope(lattice_, instr, &index)) {
    case kScopeOuter: {
      SpecialBuilder builder(ir_, &factory_, instr, instr->effect_in());
      instr->name_in() = builder.addConst(lattice_->makeOrdinalConst(index));
      factory_.toBinaryStmt(HR_findprop2getouter, instr);
      assert(def(instr->effect_in()) == builder.effect());
      break;
    }
    case kScopeDomain:
      factory_.toBinaryStmt(HR_findprop2finddef, instr);
      break;
  }
#endif
}

void Specializer::do_toint(UnaryStmt* instr) {
  const Type* t = type(instr->value_in());
  if (isUInt(t))
    factory_.toUnaryStmt(HR_uinttoint, instr);
  else if (isNumber(t))
    factory_.toUnaryStmt(HR_numbertoint, instr);
}

void Specializer::do_touint(UnaryStmt* instr) {
  const Type* t = type(instr->value_in());
  if (isInt(t))
    factory_.toUnaryStmt(HR_inttouint, instr);
  else if (isNumber(t))
    factory_.toUnaryStmt(HR_numbertouint, instr);
}

void Specializer::do_toboolean(UnaryExpr* instr) {
  const Type* t = type(instr->value_in());
  if (isNumber(t))
    factory_.toUnaryExpr(HR_d2b, instr);
}

void Specializer::do_coerce(BinaryStmt* instr) {
  const Type* to_traits_type = type(instr->lhs_in());
  if (!isConst(to_traits_type))
    return;
  Traits* traits = traitsVal(to_traits_type);
  factory_.toBinaryStmt(coerceKind(traits), instr);
}

void Specializer::do_abc_add(BinaryStmt* instr) {
  const Type* t0 = type(instr->lhs_in());
  const Type* t1 = type(instr->rhs_in());
  if (isUInt(t0) && isInt(t1))
    factory_.toBinaryStmt(HR_add_ui, instr);
  else if (isNumber(t0) && isNumber(t1))
    factory_.toBinaryStmt(HR_add_nn, instr);
  else if (isString(t0) && isString(t1))
    factory_.toBinaryStmt(HR_add_ss, instr);
}

void Specializer::do_abc_modulo(BinaryStmt* instr) {
  const Type* lhs_type = type(instr->lhs_in());
  const Type* rhs_type = type(instr->rhs_in());
  if (isUInt(lhs_type) && isConst(rhs_type) && isUInt(rhs_type)) {
    uint32_t rhs = uintVal(rhs_type);
    if (rhs && exactlyOneBit(rhs))
      factory_.toBinaryStmt(HR_abc_modulo_and, instr);
  }
}

/**
 * Return true if values with this traits cannot be compared with pointer identity.
 */
bool hasComplexEq(Traits* t) {
  return !t || t->hasComplexEqualityRules();
}

/**
 * Decide if we can compare pointers directly.  This is based on the logic
 * in CodegenLir::cmpEq().  TODO: expand to support string and namespace.
 */
bool canComparePointers(const Type* x, const Type* y) {
  Traits* xt = getTraits(x);
  Traits* yt = getTraits(y);
  return (!hasComplexEq(xt) && !hasComplexEq(yt)) ||
         (isNull(x) && !hasComplexEq(yt)) ||
         (isNull(y) && !hasComplexEq(xt));
}

void Specializer::do_abc_strictequals(BinaryExpr* instr) {
  const Type* lht = type(instr->lhs_in());
  const Type* rht = type(instr->rhs_in());
  if (isInt(lht) && isInt(rht))
    factory_.toBinaryExpr(HR_eqi, instr);
  else if (isUInt(lht) && isUInt(rht))
    factory_.toBinaryExpr(HR_equi, instr);
  else if (isNumber(lht) && isNumber(rht))
    factory_.toBinaryExpr(HR_eqd, instr);
  else if (canComparePointers(lht, rht))
    factory_.toBinaryExpr(HR_eqp, instr);
  else if (isString(lht) && isString(rht))
    factory_.toBinaryExpr(HR_eqs, instr);
}

void Specializer::do_abc_equals(BinaryStmt* instr) {
  const Type* lht = type(instr->lhs_in());
  const Type* rht = type(instr->rhs_in());
  if (isBoolean(lht) && isBoolean(rht))
    factory_.toBinaryStmt(HR_abc_equals_bb, instr);
  else if (isInt(lht) && isInt(rht))
    factory_.toBinaryStmt(HR_abc_equals_ii, instr);
  else if (isUInt(lht) && isUInt(rht))
    factory_.toBinaryStmt(HR_abc_equals_uu, instr);
  else if (isNumber(lht) && isNumber(rht))
    factory_.toBinaryStmt(HR_abc_equals_nn, instr);
  else if (canComparePointers(lht, rht))
    factory_.toBinaryStmt(HR_abc_equals_pp, instr);
  else if (isString(lht) && isString(rht))
    factory_.toBinaryStmt(HR_abc_equals_ss, instr);
}

bool isTruncatedInt(Def* d) {
  const Type* t = type(d);
  return isInt(t) || isUInt(t);
}

Def* truncateInt(Def* d, SpecialBuilder* builder) {
  const Type* t = type(d);
  if (isInt(t))
    return d;
  assert(isUInt(t));
  return builder->addExpr(HR_u2i, d);
}

bool Specializer::specializeArithmetic(InstrKind k1, InstrKind k2,
                                       UnaryExpr* instr) {
  Instr* definer_in = definer(instr->value_in());
  if (kind(definer_in) == k1) {
    // TODO - simply and just convert to integer operations.
    BinaryExpr* old_expr = cast<BinaryExpr>(definer_in);
    Def* v1 = def(old_expr->lhs_in());
    Def* v2 = def(old_expr->rhs_in());
    if (isTruncatedInt(v1) && isTruncatedInt(v2)) {
      // fixme: we need a no-effect template for this; fatadd_i takes effect.
      SpecialBuilder builder(ir_, &factory_, instr);
      v1 = truncateInt(v1, &builder);
      v2 = truncateInt(v2, &builder);
      Def* new_def = builder.addExpr(factory_.newBinaryExpr(k2, v1, v2));
      copyUses(instr->value_out(), new_def);
      return true;
    }
  }
  return false;
}

void Specializer::do_doubletoint32(UnaryExpr* instr) {
  // convert double math to int math, based on logic from
  // CodegenLIR::coerceNumberToInt and arithOpcodeD2I().
  if (specializeArithmetic(HR_addd, HR_addi, instr))
    return;
  if (specializeArithmetic(HR_subd, HR_subi, instr))
    return;
  if (specializeArithmetic(HR_muld, HR_muli, instr))
    return;
}

void Specializer::do_if(IfInstr* cond) {
  Use& selector_use = cond->selector();
  Instr* sel_instr = definer(selector_use);
  while (kind(sel_instr) == HR_not) {
    // swap arms: if (not(c)) { T } else { F } => if (c) { F } else { T }
    selector_use = cast<UnaryExpr>(sel_instr)->value_in();
    ir_->swapArms(cond);
    sel_instr = definer(selector_use); // maybe we can fold another HR_not?
  }
}

/***
 * If type inference finds out that we're actually a precise type
 * don't speculate anymore
 */
void Specializer::removeSpeculate(BinaryExpr* instr) {
  Def* lhs = def(instr->lhs_in());
  if (!isAny(type(lhs))) {
    Def* state = def(instr->rhs_in());
	  (void) state;
    assert (isState(type(state)));
    assert (kind(definer(state)) == HR_safepoint);
    connectUsesToDef(*instr->value_out(), lhs);
  }
}

} // namespace halfmoon
#endif // VMCFG_HALFMOON
