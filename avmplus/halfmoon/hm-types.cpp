/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace halfmoon {
using namespace avmplus;


//
// global Type values
//

const Type* TOP = &TopType::INSTANCE;
const Type* BOT = &BottomType::INSTANCE;
const Type* TOPDATA = &TopDataType::INSTANCE;
const Type* EFFECT = &EffectType::INSTANCE;
const Type* STATE = &StateType::INSTANCE;

const Type* TRAITS = &TraitsType::BASE;
const Type* ORDINAL = &OrdinalType::BASE;
const Type* NAME = &NameType::BASE;
const Type* METHOD = &MethodType::BASE;
const Type* ENV = &EnvType::BASE;

// ----------------------------------------------------------------------

// Type

/**
 * buildName - prints type name into AVM StringBuffer,
 * then makes an Allocated copy.
 * NOTE: currently printName() may not print anything,
 * in which case we return the empty (not NULL) string.
 */
const char* Type::buildName(Lattice& lat) const {
  StringBuffer buf(lat.core());
  printName(buf, lat.core());
  const char* vmstr = buf.c_str();

  if (vmstr != NULL) {
    char* name = new (lat.alloc) char[VMPI_strlen(buf.c_str()) + 1];
    VMPI_strcpy(name, buf.c_str());
    for(int i = 0; name[i]; ++i)
      if(name[i] == '\"')
        name[i] = '\'';
    return name;
  } else {
    return "";
  }
}

// ----------------------------------------------------------------------

// Base types

const TopType TopType::INSTANCE;
const BottomType BottomType::INSTANCE;
const TopDataType TopDataType::INSTANCE;
const EffectType EffectType::INSTANCE;
const StateType StateType::INSTANCE;

// ----------------------------------------------------------------------

// VM types

template<class SELF_CLASS, TypeKind TYPE_KIND, typename VALUE_REP>
const Type* VMType<SELF_CLASS, TYPE_KIND, VALUE_REP>::copy(Lattice& lat) const {
  if (!is_const)
    return &SELF_CLASS::BASE;
  Type* t = new (lat.alloc) SELF_CLASS(const_value);
  assert(t->name == NULL);
  t->name = buildName(lat);
  return t;
}

const TraitsType TraitsType::BASE;
const OrdinalType OrdinalType::BASE;
const NameType NameType::BASE;
const MethodType MethodType::BASE;
const EnvType EnvType::BASE;

const Type* EnvType::copy(Lattice& lat) const {
  if (method == NULL)
    return &BASE;
  EnvType* t = new (lat.alloc) EnvType(method);
  assert(t->name == NULL);
  t->name = buildName(lat);
  return t;
}

// -------------------------------------------------------------------

// DataType

/**
 *
 */
PrintWriter& DataType::printConstQual(PrintWriter& console, AvmCore* core) const {
  return console << '=' << asAtom(getAtomConst(core));
}

/**
 * true if value set includes null, but we exclude it by constraint
 */
bool DataType::excludesNull() const {
  return Lattice::isNullable(traits) && !is_nullable;
}

/**
 * for most but not all data types, the default model is determined by the traits.
 */
ModelKind DataType::defaultModelKind() const {
  return halfmoon::defaultModelKind(traits);
}


// --------------------------------------------------------------------

// SimpleDataType

template<class SELF_CLASS, TypeKind TYPE_KIND, ModelKind MODEL_KIND, typename VALUE_REP>
PrintWriter& SimpleDataType<SELF_CLASS, TYPE_KIND, MODEL_KIND, VALUE_REP>
             ::printName(PrintWriter& console, AvmCore* core) const {
#ifdef AVMPLUS_VERBOSE

  console << traits;

  if (is_const)
    printConstQual(console, core);
  else if (excludesNull())
    console << "~";

  if (model != defaultModelKind())
    console << "[" << modelDesc(model) << "]";

#else
  (void)core;
#endif
  return console;
}

template<class SELF_CLASS, TypeKind TYPE_KIND, ModelKind MODEL_KIND, typename VALUE_REP>
const Type* SimpleDataType<SELF_CLASS, TYPE_KIND, MODEL_KIND, VALUE_REP>
            ::copy(Lattice& lat) const {
  Type *t = new (lat.alloc) SELF_CLASS(model, traits, is_nullable, is_const, const_value);
  assert(t->name == NULL);
  t->name = buildName(lat);
  return t;
}

template<class SELF_CLASS, TypeKind TYPE_KIND, ModelKind MODEL_KIND, typename VALUE_REP>
const DataType* SimpleDataType<SELF_CLASS, TYPE_KIND, MODEL_KIND, VALUE_REP>
                ::changeModel(Lattice &lat, ModelKind m) const {
  if (model == m) {
    return this;
  } else if (isCompatibleModel(m)) {
    return (DataType*)lat.makeType(SELF_CLASS(m, traits, is_nullable, is_const, const_value));
  } else if (isNull() && isPointer(m)) {
    switch (m) {
    case kModelString:
      return (DataType*)lat.makeType(StringType(lat, m, (String*)NULL));
    case kModelNamespace:
      return (DataType*)lat.makeType(NamespaceType(lat, m, (Namespace*)NULL));
    case kModelScriptObject:
      return lat.null_type;
    default:
      assert(false && "non-nullable model");
      return this; // fail
    }
  } else {
    assert(false && "incompatible model");
    return this; // fail
  }
}

template<class SELF_CLASS, TypeKind TYPE_KIND, ModelKind MODEL_KIND, typename VALUE_REP>
const Type* SimpleDataType<SELF_CLASS, TYPE_KIND, MODEL_KIND, VALUE_REP>
                ::makeNotNull(Lattice &lat) const {
  if (!is_nullable)
    return this;
  if (is_const && AvmCore::isNullOrUndefined(atomVal(this, 0)))
    return BOT; // once you take null out of the set {null}, you have {}
  assert(!is_const);  // is_const && !isNull should imply !is_nullable
  return lat.makeType(
    SELF_CLASS(model, traits, false, is_const, const_value));
}

template<class SELF_CLASS, TypeKind TYPE_KIND, ModelKind MODEL_KIND, typename VALUE_REP>
const SELF_CLASS* SimpleDataType<SELF_CLASS, TYPE_KIND, MODEL_KIND, VALUE_REP>
                  ::makeUnion(const SELF_CLASS& t, Lattice& lat) const {

  ModelKind union_model = model == t.model ? model : kModelAtom;

  if (is_const && t.is_const && const_value == t.const_value)
    return (SELF_CLASS*)lat.makeType(
      SELF_CLASS(union_model, traits, is_nullable, true, const_value));

  Traits* union_traits = Verifier::findCommonBase(traits, t.traits);
  bool union_nullable = is_nullable || t.is_nullable;

  return (SELF_CLASS*)lat.makeType(
    SELF_CLASS(union_model, union_traits, union_nullable, false, (VALUE_REP)0));
}

template<class SELF_CLASS, TypeKind TYPE_KIND, ModelKind MODEL_KIND, typename VALUE_REP>
const Type* SimpleDataType<SELF_CLASS, TYPE_KIND, MODEL_KIND, VALUE_REP>
            ::makeUnion(const Type& t, Lattice& lat) const {

  if (isSubtypeOf(t) && submodelof(this, &t))
    return &t;

  if (t.isSubtypeOf(*this) && submodelof(&t, this))
    return this;

  if (t.kind == TYPE_KIND)
    return makeUnion((SELF_CLASS&)t, lat);

  if (isDataType(t)) {
    // if we're null, reverse the test so we only need to handle the other-is-null case
    if (isNull() && halfmoon::kind(this) != kTypeVoid) {
      return t.makeUnion(*this, lat);
    }

    bool union_nullable = is_nullable || t.is_nullable;
    Traits* union_traits = Verifier::findCommonBase(traits, getTraits(&t));
    switch (builtinType(union_traits)) {
    case BUILTIN_any:
      if (union_traits == NULL) {
        // null unioned with t just returns a nullable t, except for void
        if (builtinType(getTraits(&t)) == BUILTIN_null  && builtinType(traits) != BUILTIN_void) {
          return lat.makeType(SELF_CLASS(model, traits, true, false, (VALUE_REP)0));
        }
      }
      return lat.makeType(AnyType(union_traits, union_nullable));
    case BUILTIN_object:
      return lat.makeType(ObjectType(union_traits, union_nullable));
    default:
      if (union_traits == traits) {
        assert(builtinType(getTraits(&t)) == BUILTIN_null);
        return lat.makeType(SELF_CLASS(model, traits, true, false, (VALUE_REP)0));
      } else {
        assert(halfmoon::defaultModelKind(union_traits) == kModelScriptObject);
        return lat.makeType(ScriptObjectType(kModelScriptObject, union_traits, union_nullable));
      }
    }
  }

  return t.makeUnion(*this, lat);
}

// --------------------------------------------------------------------

// AnyType

AnyType::AnyType(Traits* traits, bool is_nullable) :
  SimpleDataType<AnyType, kTypeAny, kModelAtom, Atom>
    (traits, kModelAtom, is_nullable, false, Atom(0)) {
  assert(builtinType(traits) == BUILTIN_any);
}

// --------------------------------------------------------------------

// ObjectType

ObjectType::ObjectType(Traits* traits, bool is_nullable) :
  SimpleDataType<ObjectType, kTypeObject, kModelAtom, Atom>
    (traits, kModelAtom, is_nullable, false, Atom(0)) {
  assert(builtinType(traits) == BUILTIN_object);
}

// --------------------------------------------------------------------

// VoidType

VoidType::VoidType(Traits* traits) :
  SimpleDataType<VoidType, kTypeVoid, kModelAtom, Atom>
    (traits, kModelAtom, true, true, undefinedAtom) {
  assert(builtinType(traits) == BUILTIN_void);
}

// --------------------------------------------------------------------

// ScriptObjectType

ScriptObjectType::ScriptObjectType(ModelKind model, Traits* traits,
                                   bool is_nullable) :
  SimpleDataType<ScriptObjectType, kTypeScriptObject, kModelScriptObject, ScriptObject*>
    (traits, model, is_nullable, false, NULL) {
  assert(isCompatibleModel(model));
}

ScriptObjectType::ScriptObjectType(ModelKind model, Traits* traits,
                                   ScriptObject* const_value) :
  SimpleDataType<ScriptObjectType, kTypeScriptObject, kModelScriptObject, ScriptObject*>
    (traits, model, const_value == NULL, true, const_value) {
  assert(isCompatibleModel(model));
}

ScriptObjectType::ScriptObjectType(ModelKind model, Traits* traits,
                                   bool is_nullable, bool is_const,
                                   ScriptObject* const_value) :
  SimpleDataType<ScriptObjectType, kTypeScriptObject, kModelScriptObject, ScriptObject*>
    (traits, model, is_nullable, is_const, const_value) {
  assert(isCompatibleModel(model));
}

PrintWriter& ScriptObjectType::printName(PrintWriter& console, AvmCore* core) const {
#ifdef AVMPLUS_VERBOSE

  if (builtinType(traits) == BUILTIN_any)
    console << "ScriptObject";
  else
    console << traits;

  if (is_const)
    printConstQual(console, core);
  else if (excludesNull())
    console << "~";

  if (model != defaultModelKind())
    console << "[" << modelDesc(model) << "]";

#else
  (void)core;
#endif
  return console;
}

// --------------------------------------------------------------------

// NamespaceType

NamespaceType::NamespaceType(Lattice& lat, ModelKind model, bool is_nullable) :
  SimpleDataType<NamespaceType, kTypeNamespace, kModelNamespace, Namespace*>
    (lat.namespace_traits, model, is_nullable, false, NULL) {
  assert(isCompatibleModel(model));
}

NamespaceType::NamespaceType(Lattice& lat, ModelKind model, Namespace* const_value) :
  SimpleDataType<NamespaceType, kTypeNamespace, kModelNamespace, Namespace*>
    (lat.namespace_traits, model, const_value == NULL, true, const_value) {
  assert(isCompatibleModel(model));
}

NamespaceType::NamespaceType(ModelKind model, Traits* traits, bool is_nullable,
                             bool is_const, Namespace* const_value) :
  SimpleDataType<NamespaceType, kTypeNamespace, kModelNamespace, Namespace*>
    (traits, model, is_nullable, is_const, const_value) {
  assert(isCompatibleModel(model));
}

// --------------------------------------------------------------------

// StringType

StringType::StringType(Lattice& lat, ModelKind model, bool is_nullable) :
  SimpleDataType<StringType, kTypeString, kModelString, String*>
    (lat.string_traits, model, is_nullable, false, NULL) {
  assert(isCompatibleModel(model));
}

StringType::StringType(Lattice& lat, ModelKind model, String* const_value) :
  SimpleDataType<StringType, kTypeString, kModelString, String*>
    (lat.string_traits, model, const_value == NULL, true, const_value) {
  assert(isCompatibleModel(model));
}

StringType::StringType(ModelKind model, Traits* traits, bool is_nullable,
                       bool is_const, String* const_value) :
  SimpleDataType<StringType, kTypeString, kModelString, String*>
    (traits, model, is_nullable, is_const, const_value) {
  assert(isCompatibleModel(model));
}

// --------------------------------------------------------------------

// BooleanType

BooleanType::BooleanType(Lattice& lat, ModelKind model) :
  SimpleDataType<BooleanType, kTypeBoolean, kModelInt, bool>
    (lat.boolean_traits, model, false, false, false) {
  assert(isCompatibleModel(model));
}

BooleanType::BooleanType(Lattice& lat, bool const_value, ModelKind model) :
  SimpleDataType<BooleanType, kTypeBoolean, kModelInt, bool>
    (lat.boolean_traits, model, false, true, const_value) {
  assert(isCompatibleModel(model));
}

BooleanType::BooleanType(ModelKind model, Traits* traits, bool is_nullable,
                         bool is_const, bool const_value) :
  SimpleDataType<BooleanType, kTypeBoolean, kModelInt, bool>
    (traits, model, is_nullable, is_const, const_value) {
  assert(isCompatibleModel(model));
}

// --------------------------------------------------------------------

// NumberType

const NumberConstraint NumberType::TRIV;

NumberType::NumberType(Lattice& lat, ModelKind model) :
  DataType(kTypeNumber, model, lat.number_traits, false, false),
  constraint(TRIV) {
  assert(isCompatibleModel(model));
}

NumberType::NumberType(Lattice& lat, ModelKind model, bool is_int, bool is_uint) :
  DataType(kTypeNumber, model, lat.number_traits, false, false),
  constraint(is_int, is_uint) {
  assert(isCompatibleModel(model));
}

NumberType::NumberType(Lattice& lat, double d, ModelKind model) :
  DataType(kTypeNumber, model, lat.number_traits, false, true),
  constraint(d) {
  assert(isCompatibleModel(model));
}

NumberType::NumberType(ModelKind model, Traits* traits, NumberConstraint constraint) :
  DataType(kTypeNumber, model, traits, false, constraint.isConst()),
  constraint(constraint) {
  assert(isCompatibleModel(model));
}

const Type* NumberType::copy(Lattice& lat) const {
  Type *t = new (lat.alloc) NumberType(model, traits, constraint);
  assert(t->name == NULL);
  t->name = buildName(lat);
  return t;
}

const NumberType* NumberType::changeModel(Lattice &lat, ModelKind m) const {
  assert(isCompatibleModel(m));
  return model == m ? this :
    (NumberType*)lat.makeType(NumberType(m, traits, constraint));
}

const NumberType* NumberType::makeUnion(const NumberType& t, Lattice& lat) const {
  NumberConstraint union_constraint = constraint.makeUnion(t.constraint);
  ModelKind union_model = (model == kModelAtom || t.model == kModelAtom) ? kModelAtom :
      (union_constraint.isInt() || union_constraint.isUInt()) ? kModelInt :
      kModelDouble;
  return (NumberType*)lat.makeType(NumberType(union_model, traits, union_constraint));
}

const Type* NumberType::makeUnion(const Type& t, Lattice& lat) const {

  if (isSubtypeOf(t) && submodelof(this, &t))
    return &t;

  if (t.isSubtypeOf(*this) && submodelof(&t, this))
    return this;

  if (t.kind == kTypeNumber)
    return makeUnion((NumberType&)t, lat);

  if (isDataType(t)) {
    Traits* union_traits = Verifier::findCommonBase(traits, getTraits(&t));
    BuiltinType bt = builtinType(union_traits);
    if (bt == BUILTIN_any) {
      return lat.makeType(AnyType(union_traits, t.is_nullable));
    } else {
      assert(bt == BUILTIN_object);
      return lat.makeType(ObjectType(union_traits, t.is_nullable));
    }
  }

  return t.makeUnion(*this, lat);
}

// -------------------------------------------------------------------

///
/// type functions
///

/**
 * Return true iff value_set(t1) is a subset of value_set(t2)
 */
bool subtypeof(const Type* t1, const Type* t2) {
  return t1->isSubtypeOf(*t2);
}

bool subtypeof(const Type& t1, const Type& t2) {
  return t1.isSubtypeOf(t2);
}

// CAUTION: wiggly, will change
bool submodelof(const Type* t1, const Type* t2) {
  return model(t1) == model(t2) ||
      isBottom(t1) ||
      isTop(t2) ||
      (isTopData(t2) && isDataType(*t1));
}

// --------------------------------------------------------------------

bool maybeIndex(const Type* name_type, const Type* index_type) {
  if (isConst(name_type) && isNumber(index_type)) {
    const Multiname* name = nameVal(name_type);
    return !name->isAttr() && name->isRuntime() && name->containsAnyPublicNamespace();
  }
  return false;
}

/**
 * Lattice
 */
Lattice::Lattice(AvmCore* core, Allocator& alloc) :
    alloc(alloc), core_(core), types(alloc) {

  BuiltinTraits& builtin = core->traits;
  int_traits = builtin.int_itraits;
  uint_traits = builtin.uint_itraits;
  number_traits = builtin.number_itraits;
  boolean_traits = builtin.boolean_itraits;
  namespace_traits = builtin.namespace_itraits;
  string_traits = builtin.string_itraits;
  object_traits = builtin.object_itraits;
  class_traits = builtin.class_itraits;
  void_traits = builtin.void_itraits;

  null_type = makeAtomConst(builtin.null_itraits, nullObjectAtom);
  void_type = makeAtomConst(void_traits, undefinedAtom);

  boolean_type = makeType(boolean_traits);
  double_type = makeType(number_traits);

  int_type = (NumberType*)makeType(NumberType(*this, kModelInt, true, false));
  uint_type = (NumberType*)makeType(NumberType(*this, kModelInt, false, true));

  makeNullableTypes(object_traits, object_type);
  makeNullableTypes(string_traits, string_type);
  makeNullableTypes(namespace_traits, namespace_type);
  makeNullableTypes(builtin.error_itraits, error_type);
  makeNullableTypes(builtin.array_itraits, array_type);
  makeNullableTypes(builtin.vectorint_itraits, vectorint_type);
  makeNullableTypes(builtin.vectoruint_itraits, vectoruint_type);
  makeNullableTypes(builtin.vectordouble_itraits, vectordouble_type);
  makeNullableTypes(builtin.vectorobj_itraits, vectorobject_type);
  makeNullableTypes(class_traits, class_type);
  makeNullableTypes(builtin.function_itraits, function_type);

  atom_type[kTypeNullable] = makeType(NULL, true, kModelAtom);
  atom_type[kTypeNotNull] = makeType(NULL, false, kModelAtom);

  method_type = METHOD;   // TODO remove

  scriptobject_type[kTypeNullable] =
    (ScriptObjectType*)makeType(ScriptObjectType(kModelScriptObject, NULL, true));
  scriptobject_type[kTypeNotNull] =
    (ScriptObjectType*)makeType(ScriptObjectType(kModelScriptObject, NULL, false));
}

/**
 *
 */
void Lattice::makeNullableTypes(Traits* traits, const DataType* types[2]) {
  types[kTypeNullable] = makeType(traits, true);
  types[kTypeNotNull] = makeType(traits, false);
}

/**
 * canBeNull() is true if the given type allows null or undefined.
 * False for primitives and true for String, Namespace, Object, user class
 * and interface types, and activation object types.
 */
bool Lattice::isNullable(Traits* t) {
  switch (Traits::getBuiltinType(t)) {
    case BUILTIN_number:
    case BUILTIN_int:
    case BUILTIN_uint:
    case BUILTIN_boolean:
      return false;
    default:
      return true;
  }
}

const Type* Lattice::makeParamType(int p, MethodSignaturep signature) {
  if (p == 0)
    return makeType(signature->paramTraits(0), false);
  if (p <= signature->param_count())
    return makeType(signature->paramTraits(p));
  return atom_type[kTypeNullable];
}

/**
 * Look up the given type in the hashtable, add it if necessary, then
 * return the canonical type.  This function always returns a pointer to
 * an arena allocated Type.
 */
const Type* Lattice::makeType(const Type& type) {
  // TODO figure this out, then remove
  assert(!type.is_const || !isDataType(type) || model(&type) != kModelInvalid);

  const Type* interned = types.get(TypeKey(&type));
  if (interned == NULL) {
    const Type* fresh = type.copy(*this);
    types.put(TypeKey(fresh), fresh);
    interned = fresh;
  }

  return interned;
}

/**
 * Create a type value for the given traits and nullable hint.
 * TODO final param
 */
const DataType* Lattice::makeType(Traits* traits, bool nullable, ModelKind model) {
  assert(isNullable(traits) || !nullable);
  switch (builtinType(traits)) {
    case BUILTIN_int:
      return int_type;
    case BUILTIN_uint:
      return uint_type;
    case BUILTIN_null:
      return null_type;
    case BUILTIN_void:
      return void_type;
    case BUILTIN_string:
      return (DataType*)makeType(StringType(*this, model, nullable));
    case BUILTIN_boolean:
      return (DataType*)makeType(BooleanType(*this, model));
    case BUILTIN_namespace:
      return (DataType*)makeType(NamespaceType(*this, model, nullable));
    case BUILTIN_number:
      return (DataType*)makeType(NumberType(*this, model));
    case BUILTIN_object:
      assert(model == kModelAtom);
      return (DataType*)makeType(ObjectType(traits, nullable));
    case BUILTIN_any:
      assert(model == kModelAtom);
      return (DataType*)makeType(AnyType(traits, nullable));
    default:
      return (DataType*)makeType(ScriptObjectType(model, traits, nullable));
  }
}

/**
 * Second guess the verifier.  If the verifier derived the type int
 * or uint, it really means Number with an int32 or uint32 representation.
 */
const Type* Lattice::makeType(const FrameValue& value) {
  return makeType(value.traits,
                  isNullable(value.traits) ? !value.notNull : false);
}

/**
 * Create a non-null type value given a type-value that might be null.
 */
const Type* Lattice::makeNotNull(const Type* type) {
  assert(isDataType(*type));
  return ((DataType*)type)->makeNotNull(*this);
}

/**
 * Create a constant of the given type and value.
 * TODO API with just Atom, figure out traits using
 * chunk of toplevel toTraits (TODO should factor w/that)
 */
const DataType* Lattice::makeAtomConst(Traits* traits, Atom val) {
  bool nullOrUndefined = AvmCore::isNullOrUndefined(val);
  (void)nullOrUndefined;  // used only in asserts

  switch (builtinType(traits)) {
    case BUILTIN_int:
    case BUILTIN_uint: {
      assert(false); // should have been mapped to number
      break;
    }

    case BUILTIN_number: {
      assert(AvmCore::isNumber(val));
      return makeNumberConst(AvmCore::number_d(val), kModelAtom);
    }

    case BUILTIN_any: {
      assert(nullOrUndefined);
      return AvmCore::isNull(val) ?
        makeObjectConst(traits, (ScriptObject*)atomPtr(val), kModelAtom) :
        (DataType*)makeType(VoidType(void_traits));
    }

    case BUILTIN_void: {
      assert(val == undefinedAtom);
      return (DataType*)makeType(VoidType(traits));
    }

    case BUILTIN_string: {
      assert(AvmCore::isString(val));
      return makeStringConst((String*)atomPtr(val), kModelAtom);
    }

    case BUILTIN_boolean: {
      assert(AvmCore::isBoolean(val));
      return makeBoolConst(val == trueAtom, kModelAtom);
    }

    case BUILTIN_namespace: {
      assert(AvmCore::isNamespace(val));
      return makeNamespaceConst((Namespace*)atomPtr(val), kModelAtom);
    }

    case BUILTIN_null: {
      assert(AvmCore::isNull(val));
      return makeObjectConst(traits, (ScriptObject*)atomPtr(val), kModelAtom);
    }

    case BUILTIN_object:
    default: {
      assert(AvmCore::isNull(val) || AvmCore::isObject(val));
      return makeObjectConst(traits, (ScriptObject*)atomPtr(val), kModelAtom);
    }
  }

  assert(false);
  return NULL;
}

//
//  TODO
//

/** Create an int32 constant in -2^31..2^31-1 */
const DataType* Lattice::makeIntConst(int32_t val, bool boxed) {
  ModelKind m = boxed ? kModelAtom : kModelInt;
  return (DataType*)makeType(NumberType(*this, val, m));
}

/** Create a uint32 in 0..2^32-1 */
const DataType* Lattice::makeUIntConst(uint32_t val, bool boxed) {
  ModelKind m = boxed ? kModelAtom : kModelInt;
  return (DataType*)makeType(NumberType(*this, val, m));
}

/** Create a double constant. */
const DataType* Lattice::makeDoubleConst(double val, bool boxed) {
  ModelKind m = boxed ? kModelAtom : kModelDouble;
  return (DataType*)makeType(NumberType(*this, val, m));
}

const DataType* Lattice::makeNumberConst(double val, bool boxed) {
  if (isWhole(val)) {
    if (int32_t(val) == val)
      return makeIntConst(int32_t(val), boxed);
    if (uint32_t(val) == val)
      return makeUIntConst(uint32_t(val), boxed);
  }
  return makeDoubleConst(val, boxed);
}

const BooleanType* Lattice::makeBoolConst(bool val, ModelKind m) {
  return (BooleanType*)makeType(BooleanType(*this, val, m));
}

const StringType* Lattice::makeStringConst(PoolObject* pool, uint32_t index) {
  return (StringType*)makeType(
      StringType(*this, StringType::NATIVE_MODEL, pool->getString(index)));
}

const StringType* Lattice::makeStringConst(String* str, ModelKind m) {
  return (StringType*)makeType(StringType(*this, m, str));
}

const NamespaceType* Lattice::makeNamespaceConst(Namespace* val, ModelKind m) {
  return (NamespaceType*)makeType(NamespaceType(*this, m, val));
}

const ScriptObjectType* Lattice::makeObjectConst(Traits* traits, ScriptObject* val, ModelKind m) {
  return (ScriptObjectType*)makeType(ScriptObjectType(m, traits, val));
}

avmplus::Binding Lattice::toBinding(const Type* obj_type,
                                    const Type* name_type) const {
  // fixme - should isDataType(bottom) = true?  if so, remove this early
  // return and keep assert.
  if (isBottom(obj_type))
    return BIND_NONE;
  return toBinding(getTraits(obj_type), name_type);
}

avmplus::Binding Lattice::toBinding(Traits* traits, const Type* name_type) const {
  // This code is based on getBinding() in instr.cpp, but without the need
  // for Toplevel* or the possibility to throw any exceptions.
  const Multiname* name;
  if (!traits || !isConst(name_type) ||
      !(name = nameVal(name_type))->isBinding())
    return BIND_NONE;
  TraitsBindingsp tb = traits->getTraitsBindings();
  return
      name->isNsset() ? tb->findBinding(name->getName(), name->getNsset()) :
          tb->findBinding(name->getName(), name->getNamespace());
}

/** Return a type for an outer scope */
const Type* Lattice::getOuterScopeType(MethodInfo* method, int scope_index) {
  Traits* scope_type = method->declaringScope()->getScopeTraitsAt(scope_index);
  return makeType(scope_type, false, kModelAtom);
}

const Type* Lattice::getSlotType(const Type* obj_type, int slot) {
  return makeType(getSlotTraits(obj_type, slot));
}

uint32_t Lattice::getSlotOffset(const Type* obj_type, int slot) {
  Traits* obj_traits = getTraits(obj_type);
  assert(obj_traits && "getSlotType requires a type with slots");
  TraitsBindingsp tb = obj_traits->getTraitsBindings();
  return tb->getSlotOffset(slot);
}

Traits* Lattice::getSlotTraits(const Type* obj_type, int slot) {
  Traits* obj_traits = getTraits(obj_type);
  assert(obj_traits && "getSlotType requires a type with slots");
  TraitsBindingsp tb = obj_traits->getTraitsBindings();
  return tb->getSlotTraits(slot);
}

bool Lattice::isResolved(const Type* obj_type) {
  return getTraits(obj_type)->isResolved();
}

/**
 * obj_type is the Type of an object
 * disp_id is the vtable slot corresponding to a method
 * returns the MethodInfo for the given slot of a vtable
 */
MethodInfo* Lattice::getSlotMethod(const Type* obj_type, int slot) {
  TraitsBindingsp tb = getTraits(obj_type)->getTraitsBindings();
  return tb->getMethod(slot);
}

MethodInfo* Lattice::getInitializer(const Type* obj_type) {
  return getTraits(obj_type)->init;
}

const Type* Lattice::finddefType(PoolObject* pool, const Type* name_type) {
  const Multiname* name = nameVal(name_type);
  DomainMgr* domainMgr = core()->domainMgr();
  MethodInfo* script = domainMgr->findScriptInPoolByMultiname(pool, *name);
  if (isValidBinding((avmplus::Binding) script))
    return makeType(script->declaringTraits(), false);
  return object_type[kTypeNotNull];
}

/**
 * at the moment, we don't correctly handle interfaces
 * eg class A impl I, class B impl I, a and b's union should be the type I
 * However, verifier::findCommonBase, given the traits of a and b,
 * does not correctly find I, and so makeUnion currently 
 * loosens the type to object. Thus, we have to forcibly keep
 * coercions from a -> I by stating nothing is a sub type of t if t is an interface 
 * see check halfmoon/test/functionscripts/classes/interfaceMerge.as
 */
const Type* Lattice::makeUnion(const Type* t1, const Type* t2) {
  return t1->makeUnion(*t2, *this);
}

const DataType* Lattice::getInstanceType(const Type* class_type) {
  // TODO should we be asserting?
  if (!isDataType(*class_type))
    return NULL;

  Traits* class_traits = getTraits(class_type);
  return class_traits && class_traits->itraits ?
    makeType(class_traits->itraits) :
    NULL;
}

} // namespace avmplus
#endif // VMCFG_HALFMOON
