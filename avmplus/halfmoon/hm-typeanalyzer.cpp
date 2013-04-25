/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {
using avmplus::atomFromIntptrValue;
using avmplus::atomGetIntptr;
using avmplus::atomIsIntptr;
using avmplus::atomIsValidIntptrValue;

TypeAnalyzer::TypeAnalyzer(InstrGraph* ir)
: ir_(ir), lattice_(&ir->lattice) {
}

/// Force the model of each result to the model defined in the output signature
///
void TypeAnalyzer::coerceOutputModels(Instr* instr) {
  if (hasOutputSignature(instr)) {
    SigRange sig = outputSigRange(instr);
    ArrayRange<Def> d = defRange(instr);
    for (int i = 0; !d.empty(); d.popFront(), sig.popFront(), ++i) {
      const Type* result_type = sig.front();
      const Type* data_type = type(&d.front());
      if (isDataType(*result_type) && !submodelof(data_type, result_type))
        setType(&d.front(), lattice_->changeModel(data_type, model(result_type)));
    }
  }
}

void TypeAnalyzer::computeTypes(Instr* instr) {
  // TODO consider carefully replacing the switch with the default case.
  // the effect will be the same, but masking off block starts because 
  // their params start with no type points to a deeper issue with where
  // in an instr's lifetime computeTypes() should be called.
  switch (kind(instr)) {
    case HR_goto:
    case HR_catchblock:
    case HR_label:
    case HR_arm:
      break;
    default: {
      if (numUses(instr) > 0) {
        // For ordinary instructions, any UN input gives UN results.
        for (ArrayRange<Use> r = useRange(instr); !r.empty(); r.popFront())
          if (isBottom(type(r.front()))) {
            resetTypes(instr);
            return;
          }
      }
      break;
    }
  }

  assert(checkTypes(instr, false /* check_model */));
  do_instr(this, instr);
  coerceOutputModels(instr);
  assert(checkResultTypes(instr, false /* check_model */));
}

/// default handler, called if TA defines no overload
/// for a given InstrKind:
/// 1. delegates fat instructions to subgraph propagation.
///    TODO should only happen on nontrivial fats.
/// 2. sets output types of skinny instructions
///    with non-dependent output types from
///    generated signatures
///
/// TODO:
/// 1. factor setOutputSignature
/// 2. rewrite custom handlers to call sOS in default cases
///
void TypeAnalyzer::do_default(Instr* instr) {
  if (hasSubgraph(instr)) {
    // for now, we always calculate template results via subgraph
    return doTemplateInstr(instr);

  } else if (hasOutputSignature(instr)) {
    // in the default case, our output types are fixed by our
    // signature. Note that we trap for certain placeholder
    // types which may appear in signatures but must not
    // appear in actual instruction instances.
    SigRange sig = outputSigRange(instr);
    ArrayRange<Def> d = defRange(instr);
    for (int i = 0; !d.empty(); d.popFront(), sig.popFront(), ++i) {
      const Type* result_type = sig.front();
#if 0
      if (result_type == ALL || result_type == TOPDATA) {
        printf("instr = %s, result_type = %s\n", name(instr), typeName(result_type));
        assert(false && "illegal result type");
      }
#endif
      setType(&d.front(), result_type);
    }

  } else {
    // instr has no outputs
    assert(numDefs(instr) == 0 && "missing output signature");
  }
}

/// Calculate the output types of a template
/// instruction instance, by propagating the input
/// types of the instance through the template subgraph,
/// and transferring the resulting subgraph output types
/// back to the instance.
///
void TypeAnalyzer::doTemplateInstr(Instr* instr) {
  propagateTypes(instr);
  StopInstr* ret = subgraph(instr)->returnStmt();
  assert(numDefs(instr) == numUses(ret));
  ArrayRange<Def> d = defRange(instr);
  ArrayRange<Use> u = useRange(ret);
  for (; !d.empty(); d.popFront(), u.popFront())
    setType(&d.front(), type(u.front()));
}

/// if we can resolve the definition, set
/// data result type to its type.
///
void TypeAnalyzer::do_abc_finddef(BinaryStmt* instr) {
  // fixme: this is busted if the finddef instruction was inlined from
  // a different pool.  Code origin needs to be sticky, somehow.
  MethodInfo* caller = getMethod(type(instr->rhs_in()));
  const Type* name_type = type(instr->lhs_in());
  if (isConst(name_type))
    return setStmtType(instr,
                       lattice_->finddefType(caller->pool(), name_type));
  // we don't know what type finddef returns but we know it will be some
  // global object, represented as ScriptObject*.
  return do_default(instr);
}

/// if we can resolve the called method, set
/// data result type to the resolved method's
/// return type.
///
void TypeAnalyzer::do_callmethod(CallStmt2* instr) {
  const Type* object_type = type(instr->object_in());
  if (getTraits(object_type)) {
    const Type* env_type = type(instr->param_in());
    MethodInfo* callee = getMethod(env_type);
    if (callee) {
      Traits* return_traits = callee->getMethodSignature()->returnTraits();
      return setStmtType(instr, lattice_->makeType(return_traits));
    }
  }
  return do_default(instr);
}

/// if we can resolve the called method, set
/// data result type to the resolved method's
/// return type.
///
void TypeAnalyzer::do_callstatic(CallStmt2* instr) {
  MethodInfo* method = getMethod(type(instr->param_in()));
  const Type* return_type = lattice_->makeType(method->getMethodSignature()->returnTraits());
  return setStmtType(instr, return_type);
}

/// Always set the output type to the interface method signature
void TypeAnalyzer::do_callinterface(CallStmt2* instr) {
  MethodInfo* method = getMethod(type(instr->param_in()));
  const Type* return_type = lattice_->makeType(method->getMethodSignature()->returnTraits());
  return setStmtType(instr, return_type);
}

/// if we have a constant dispatch id and
/// can resolve a method info, set data result
/// type to resolved method's env.
///
void TypeAnalyzer::do_loadenv(BinaryExpr* instr) {
  const Type* disp_id_type = type(instr->lhs_in());
  if (isConst(disp_id_type)) {
    const Type* object_type = type(instr->rhs_in());
    if (getTraits(object_type)) {
      int disp_id = ordinalVal(disp_id_type);
      MethodInfo* callee = lattice_->getSlotMethod(object_type, disp_id);
      return setType(instr->value_out(), lattice_->makeEnvType(callee));
    }
  }
  do_default(instr);
}

void TypeAnalyzer::do_loadenv_interface(BinaryExpr* instr) {
  MethodInfo* method_signature = methodVal(type(instr->lhs_in()));
  setType(instr->value_out(), lattice_->makeEnvType(method_signature));
}

/// if we can resolve an initializer,
/// return its env.
///
void TypeAnalyzer::do_loadinitenv(UnaryExpr* instr) {
  const Type* object_type = type(instr->value_in());
  if (getTraits(object_type)) {
    MethodInfo* callee = lattice_->getInitializer(object_type);
    return setType(instr->value_out(), lattice_->makeEnvType(callee));
  }
  do_default(instr);
}

/// if we can result a super initializer,
/// return its env.
///
void TypeAnalyzer::do_loadsuperinitenv(UnaryExpr* instr) {
  const Type* env_in = type(instr->value_in());
  MethodInfo* info_in = getMethod(env_in);
  if (info_in) {
    MethodInfo* info_out = info_in->declaringTraits()->base->init;
    return setType(instr->value_out(), lattice_->makeEnvType(info_out));
  }
  do_default(instr);
}

void TypeAnalyzer::do_loadenv_env(BinaryExpr* instr) {
  const Type* disp_id_type = type(instr->lhs_in());
  assert(isConst(disp_id_type));
  int disp_id = ordinalVal(disp_id_type);

  MethodInfo* caller = getMethod(type(instr->rhs_in()));
  MethodInfo* callee = caller->pool()->getMethodInfo(disp_id);
  return setType(instr->value_out(), lattice_->makeEnvType(callee));
}

// adapters to convert C++ bool to BoolKind into and out of stubs
inline BoolKind b2k(bool b) { return b ? kBoolTrue : kBoolFalse; }
inline bool k2b(BoolKind k) { return k == kBoolTrue; }

const Type* callstub(Lattice* l, const Type* c, Stub_I_I stub) {
  return l->makeIntConst(stub(0, intVal(c)));
}
const Type* callstub(Lattice* l, const Type* c, Stub_I_U stub) {
  return l->makeIntConst(stub(0, uintVal(c)));
}
const Type* callstub(Lattice* l, const Type* c, Stub_U_I stub) {
  return l->makeUIntConst(stub(0, intVal(c)));
}
const Type* callstub(Lattice* l, const Type* c, Stub_D_D stub) {
  return l->makeDoubleConst(stub(0, doubleVal(c)));
}
const Type* callstub(Lattice* l, const Type* c, Stub_D_B stub) {
  return l->makeBoolConst(k2b(stub(0, doubleVal(c))));
}
const Type* callstub(Lattice* l, const Type* c, Stub_D_I stub) {
  return l->makeIntConst(stub(0, doubleVal(c)));
}
const Type* callstub(Lattice* l, const Type* c1, const Type* c2, Stub_II_I stub) {
  return l->makeIntConst(stub(0, intVal(c1), intVal(c2)));
}
const Type* callstub(Lattice* l, const Type* c1, const Type* c2, Stub_DD_D stub) {
  return l->makeDoubleConst(stub(0, doubleVal(c1), doubleVal(c2)));
}
const Type* callstub(Lattice* l, const Type* c1, const Type* c2, Stub_II_U stub) {
  return l->makeUIntConst(stub(0, intVal(c1), intVal(c2)));
}
const Type* callstub(Lattice* l, const Type* c1, const Type* c2, Stub_II_B stub) {
  return l->makeBoolConst(k2b(stub(0, intVal(c1), intVal(c2))));
}
const Type* callstub(Lattice* l, const Type* c1, const Type* c2, Stub_UU_B stub) {
  return l->makeBoolConst(k2b(stub(0, uintVal(c1), uintVal(c2))));
}
const Type* callstub(Lattice* l, const Type* c1, const Type* c2, Stub_DD_B stub) {
  return l->makeBoolConst(k2b(stub(0, doubleVal(c1), doubleVal(c2))));
}
const Type* callstub(Lattice* l, const Type* c1, const Type* c2, Stub_BB_B stub) {
  return l->makeBoolConst(k2b(stub(0, b2k(boolVal(c1)), b2k(boolVal(c2)))));
}

template<typename STUB>
void TypeAnalyzer::fold(BinaryExpr* instr, STUB stub) {
  const Type* t1 = type(instr->lhs_in());
  const Type* t2 = type(instr->rhs_in());
  if (isConst(t1) && isConst(t2)) {
    const Type* t = callstub(lattice_, t1, t2, stub);
    return setType(instr->value_out(), t);
  }
  return do_default(instr);
}

template<typename STUB>
void TypeAnalyzer::fold(UnaryExpr* instr, STUB stub) {
  const Type* t1 = type(instr->value_in());
  if (isConst(t1))
    return setType(instr->value_out(), callstub(lattice_, t1, stub));
  return do_default(instr);
}

void TypeAnalyzer::do_toprimitive(UnaryStmt* instr) {
  // fixme: type is any Atom but not a ScriptObject*
  // fixme: carefully inspect ScriptObject::defaultValue for nullability.
  setStmtType(instr, lattice_->atom_type[kTypeNullable]);
}

const Type* computeCoercedType(Lattice* lattice, const Use& value,
                               const Type* to_type) {
  const Type* t = type(value);
  if (subtypeof(t, lattice->makeNotNull(to_type)))
    return t;
  if (!isNullable(t))
    return lattice->makeNotNull(to_type);
  return to_type;
}

void TypeAnalyzer::doCoerceInstr(UnaryExpr* instr, const Type* to_type) {
  setType(instr->value_out(),
          computeCoercedType(lattice_, instr->value_in(), to_type));
}

void TypeAnalyzer::doCoerceInstr(UnaryStmt* instr, const Type* to_type) {
  setStmtType(instr, computeCoercedType(lattice_, instr->value_in(), to_type));
}

void TypeAnalyzer::doCoerceInstr(BinaryStmt* instr) {
  const Type* to_traits_type = type(instr->lhs_in());
  if (!isConst(to_traits_type))
    return setStmtType(instr, lattice_->atom_type[kTypeNullable]);
  const Type* to_type = lattice_->makeType(traitsVal(to_traits_type));
  const Type* t = computeCoercedType(lattice_, instr->rhs_in(), to_type);
  setStmtType(instr, t);
}

void TypeAnalyzer::do_toint(UnaryStmt* instr) {
  const Type* t = type(instr->value_in());
  if (*t == *lattice_->void_type || isNull(t))
    return setStmtType(instr, lattice_->makeIntConst(0));
  doCoerceInstr(instr, lattice_->int_type);
}

void TypeAnalyzer::do_touint(UnaryStmt* instr) {
  const Type* t = type(instr->value_in());
  const Type* uint_type = lattice_->uint_type;
  setStmtType(instr,
              subtypeof(t, uint_type) ? t :
              isConst(t) && isInt(t) ? lattice_->makeUIntConst(uint32_t(intVal(t))) :
              isConst(t) && isNumber(t) ? lattice_->makeUIntConst(AvmCore::doubleToInt32(doubleVal(t))) :
              uint_type);
}

/// an arm takes its param types directly from
/// the arg types of its cond 'caller'
///
void TypeAnalyzer::do_arm(ArmInstr* arm) {
  CondInstr* cond = arm->owner;
  ArrayRange<Use> u(argRange(cond));
  ArrayRange<Def> d(defRange(arm));
  for (; !u.empty(); u.popFront(), d.popFront())
    setType(&d.front(), type(u.front()));
}

#ifdef DEBUG
bool checkUnion(Lattice* lattice, const Type* t1, const Type* t2) {
  const Type* u1 = lattice->makeUnion(t1, t2);
  const Type* u2 = lattice->makeUnion(t2, t1);
  assert(*u1 == *u2);
  assert(*u2 == *u1);
  return true;
}
#endif

/// Helper function just to assert that makeUnion is commutative.
///
const Type* phi_reduce(Lattice* lattice, const Type* t1, const Type* t2) {
  assert(checkUnion(lattice, t1, t2));
  return lattice->makeUnion(t1, t2);
}

/// each label param's type is the union of
/// the types of its corresponding arguments
/// in the label's predecessor gotos
///
void TypeAnalyzer::do_label(LabelInstr* label) {
  for (int i = 0, n = label->paramc; i < n; ++i) {
    const Type* t = BOT;
    Def* d = &label->params[i];
    for (LabelArgRange r(label, i); !r.empty(); r.popFront()) {
      if (def(r.front()) != d) { // ignore self. 
        t = phi_reduce(lattice_, t, type(r.front()));
      }
    }
    const Type* new_type = lattice_->makeType(*t);
    if (isDataType(*new_type))
      new_type = lattice_->changeModel(new_type, halfmoon::promoteModelKind(new_type));
    setType(d, new_type);
  }
}

void TypeAnalyzer::do_applytype(NaryStmt0* instr) {
  // fixme: we could be much smarter at this one.
  setStmtType(instr, lattice_->makeType(0, false));
}

void TypeAnalyzer::do_newclass(NaryStmt2* instr) {
  Traits* class_traits = traitsVal(type(instr->traits_in()));
  return setStmtType(instr, lattice_->makeType(class_traits, false));
}

/// if we have a binding, set data output type accordingly.
///
void TypeAnalyzer::do_abc_constructprop(CallStmt2* instr) {
  Binding b = toBinding(lattice_, instr->object_in(), instr->param_in());
  if (isSlot(b)) {
    const Type* class_type = lattice_->getSlotType(type(instr->object_in()),
                                                   toSlot(b));
    const Type* instance_type = lattice_->getInstanceType(class_type);
    if (instance_type)
      return setStmtType(instr, lattice_->makeNotNull(instance_type));
  }
  do_default(instr);
}

const Type* instanceType(Lattice* lattice, const Use& ctor) {
  const Type* t = lattice->getInstanceType(type(ctor));
  return t ? lattice->makeNotNull(t) : lattice->atom_type[kTypeNotNull];
}

void TypeAnalyzer::do_construct(CallStmt2* instr) {
  setStmtType(instr, instanceType(lattice_, instr->param_in()));
}

void TypeAnalyzer::do_newinstance(UnaryExpr* instr) {
  setType(instr->value_out(), instanceType(lattice_, instr->value_in()));
}

/// if we can resolve a slot, set result type
/// to constant ordinal.
///
void TypeAnalyzer::do_toslot(BinaryExpr* instr) {
  const Type* object_type = type(instr->lhs_in());
  const Type* name_type = type(instr->rhs_in());
  if (isName(name_type)) {
    // this is only valid if it really is a slot binding.
    Binding b = lattice_->toBinding(object_type, name_type);
    if (isSlot(b))
      return setType(instr->value_out(), lattice_->makeOrdinalConst(toSlot(b)));
  }
  do_default(instr);
}

/// if we can resolve a slot, set result type
/// to the corresponding traits type.
///
void TypeAnalyzer::do_slottype(BinaryExpr* instr) {
  const Type* object_type = type(instr->lhs_in());
  const Type* slot_num_type = type(instr->rhs_in());
  // slottype(obj, slotnum) => Traits* for the slot
  if (isConst(slot_num_type)) {
    int slot = ordinalVal(slot_num_type);
    Traits* slot_traits = lattice_->getSlotTraits(object_type, slot);
    return setType(instr->value_out(), lattice_->makeTraitsConst(slot_traits));
  }
  // Unknown slot number.
  do_default(instr);
}

/// if we find a local definition, set data result type
/// to its type.
///
void TypeAnalyzer::doFindInstr(NaryStmt3* instr) {
  int index;
  if (findScope(lattice_, instr, &index) == kScopeLocal)
    return setStmtType(instr, type(instr->vararg(index)));
  do_default(instr);
}

void TypeAnalyzer::do_abc_add(BinaryStmt* instr) {
  const Type* string_nn_type = lattice_->string_type[kTypeNotNull];
  const Type* lhs = type(instr->lhs_in());
  const Type* rhs = type(instr->rhs_in());
  if (subtypeof(lhs, string_nn_type) && subtypeof(rhs, string_nn_type))
    return setStmtType(instr, string_nn_type);
  if (isNumber(lhs) && isNumber(rhs))
    return setStmtType(instr, lattice_->double_type);
  setStmtType(instr, lattice_->object_type[kTypeNotNull]);
}

/// if we can resolve the property, set data result type accordingly.
///
void TypeAnalyzer::do_abc_getprop(CallStmt2* instr) {
  const Use& object = instr->object_in();
  Binding b = toBinding(lattice_, object, instr->param_in());
  if (isSlot(b)) {
    // This is a getslot, really.
    if (enable_verbose)
      printf("getprop -> getslot %s%d\n", kInstrPrefix, instr->id);
    return setStmtType(instr, lattice_->getSlotType(type(object), toSlot(b)));
  }
  do_default(instr);
}

/**
 * If we know we have a vector and the index is known to be numeric, then
 * return the vector element type.  The logic here is based on the
 * Verifier code for OP_getproperty.
 */
bool knownPropertyType(Lattice* l, const Type* object_type, const Type** t) {
  if (object_type == l->vectorint_type[kTypeNotNull])
    return (*t = l->int_type), true;
  if (object_type == l->vectoruint_type[kTypeNotNull])
    return (*t = l->uint_type), true;
  if (object_type == l->vectordouble_type[kTypeNotNull])
    return (*t = l->double_type), true;
  // The verifier only returns T for Vector.<T> if
  // bugCompatibility()->bugzilla678952 == true.  We do it always, because
  // doing the optimization here cannot affect verification.
  if (subtypeof(object_type, l->vectorobject_type[kTypeNotNull])) {
    // ObjectVector always uses Atom to store values.
    Traits* prop_traits = getTraits(object_type)->m_paramTraits;
    return (*t = l->makeAtom(l->makeType(prop_traits))), true;
  }
  return false;
}

void TypeAnalyzer::do_abc_getpropx(CallStmt3* instr) {
  const Type* name_type = type(instr->param_in());
  const Type* index_type = type(instr->index_in());
  const Type* object_type = type(instr->object_in());
  if (maybeIndex(name_type, index_type)) {
    const Type* prop_type;
    if (knownPropertyType(lattice_, object_type, &prop_type))
      return setStmtType(instr, prop_type);
  }
  do_default(instr);
}

void TypeAnalyzer::doGetpropertylate(BinaryStmt* instr) {
  const Type* object_type = type(instr->rhs_in());
  const Type* prop_type;
  if (knownPropertyType(lattice_, object_type, &prop_type))
    return setStmtType(instr, prop_type);
  do_default(instr);
}

/// if we can resolve the slot, set data result type accordingly.
///
void TypeAnalyzer::do_getslot(CallStmt2* instr) {
  const Type* obj_type = type(instr->object_in());
  const Type* slot_type = type(instr->param_in());
  if (isConst(slot_type))
    return setStmtType(instr,
                       lattice_->getSlotType(obj_type, ordinalVal(slot_type)));
  do_default(instr);
}

void TypeAnalyzer::do_ckfilter(UnaryExpr* instr) {
  setType(instr->value_out(), type(instr->value_in()));
}

/// if we can resolve slot, set result type accordingly.
///
void TypeAnalyzer::do_getouterscope(BinaryExpr* instr) {
  // Value is index ordinal.
  const Type* ord_type = type(instr->lhs_in());
  if (isConst(ord_type)) {
    MethodInfo* caller = getMethod(type(instr->rhs_in()));
    const Type* t = lattice_->getOuterScopeType(caller, ordinalVal(ord_type));
    return setType(instr->value_out(), t);
  }
  do_default(instr);
}

void TypeAnalyzer::do_cknull(UnaryStmt* instr) {
  const Type* value = type(instr->value_in());
  const Type* not_null = lattice_->makeNotNull(value);
  if (isBottom(not_null)) {
      // Don't allow this type to go to bottom, so make up something wide enough to hold it
      return setStmtType(instr, lattice_->makeNotNull(lattice_->makeUnion(value, lattice_->atom_type[kTypeNullable])));
  }
  return setStmtType(instr, not_null);
}

void TypeAnalyzer::do_newactivation(UnaryStmt* instr) {
  // fixme: opcode table says these can throw, but can they?
  const Type* env = type(instr->value_in());
  MethodInfo* method = getMethod(env);
  if (method)
    return setStmtType(instr, lattice_->makeType(method->activationTraits(), false));
  do_default(instr);
}

void TypeAnalyzer::do_newcatch(UnaryStmt* instr) {
  // fixme: opcode table says these can throw, but can they?
  const Type* value = type(instr->value_in());
  setStmtType(instr, lattice_->makeType(traitsVal(value), false));
}

void TypeAnalyzer::do_ltui(BinaryExpr* instr) {
  const Type* t1 = type(instr->lhs_in());
  const Type* t2 = type(instr->rhs_in());
  if (isConst(t1) && isConst(t2))
    return fold(instr, Stubs::do_ltui);
  if (isConst(t2) && uintVal(t2) == 0) {
    // x:uint < 0 == false for all x
    return setType(instr->value_out(), lattice_->makeBoolConst(false));
  }
  do_default(instr);
}

/** If this call appears to be a cast, return true and the target type */
bool isCastCall(Lattice* l, const Type* object_type, uint32_t slot,
                const Type** t) {
  Traits* slot_traits = l->getSlotTraits(object_type, slot);
  BuiltinTraits& builtin = l->core()->traits;
  if (slot_traits == builtin.int_ctraits)
    return (*t = l->int_type), true;
  if (slot_traits == builtin.uint_ctraits)
    return (*t = l->uint_type), true;
  if (slot_traits == builtin.number_ctraits)
    return (*t = l->double_type), true;
  if (slot_traits == builtin.boolean_ctraits)
    return (*t = l->boolean_type), true;
  if (slot_traits == builtin.string_ctraits)
    return (*t = l->string_type[kTypeNotNull]), true;
  if (slot_traits && slot_traits->base == builtin.class_itraits &&
      slot_traits->getCreateClassClosureProc() == NULL) {
    // if this class C is user-defined then C(1+ args) means coerce<C>
    assert(slot_traits->itraits != NULL && "Class with unknown instance traits");
    return (*t = l->makeType(slot_traits->itraits)), true;
  }
  return false;
}

void TypeAnalyzer::do_abc_callprop(CallStmt2* instr) {
  const Type* object_type = type(instr->object_in());
  CallAnalyzer b(object_type, type(instr->param_in()),
                 lattice_, instr->vararg_count());
  if (isMethod(b.binding)) {
    const Type* return_type = lattice_->makeType(b.signature->returnTraits());
    return setStmtType(instr, lattice_->makeAtom(return_type));
  }
  if (isSlot(b.binding) && instr->arg_count() == 2) {
    // If we're calling a known class like a function, this call becomes a cast.
    // Logic based on Verifier::emitCallpropertySlot().
    const Type* to_type;
    if (isCastCall(lattice_, object_type, toSlot(b.binding), &to_type))
      return setStmtType(instr,
                         computeCoercedType(lattice_, instr->arg(1), to_type));
  }
  do_default(instr);
}

void TypeAnalyzer::doChangeModel(UnaryExpr* instr, ModelKind m) {
  const Type* t = lattice_->changeModel(type(instr->value_in()), m);
  setType(instr->value_out(), t);
}
} // namespace halfmoon
#endif // VMCFG_HALFMOON
