/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace halfmoon {
using avmplus::AvmCore;
using avmplus::Binding;
using avmplus::BIND_AMBIGUOUS;
using avmplus::BIND_NONE;
using avmplus::BuiltinType;
using avmplus::BUILTIN_any;
using avmplus::BUILTIN_boolean;
using avmplus::BUILTIN_int;
using avmplus::BUILTIN_null;
using avmplus::BUILTIN_object;
using avmplus::BUILTIN_number;
using avmplus::BUILTIN_string;
using avmplus::BUILTIN_uint;
using avmplus::BUILTIN_void;
using avmplus::exactlyOneBit;
using avmplus::FrameValue;
using avmplus::MathUtils;
using avmplus::MethodEnv;
using avmplus::Multiname;
using avmplus::Namespace;
using avmplus::PoolObject;
using avmplus::SlotStorageType;
using avmplus::SST_atom;
using avmplus::SST_bool32;
using avmplus::SST_double;
using avmplus::SST_int32;
using avmplus::SST_namespace;
using avmplus::SST_string;
using avmplus::SST_scriptobject;
using avmplus::SST_uint32;
using avmplus::Traits;
using nanojit::UseOperEqual;

/***************************************************************
 * Type ::=
 *  Top | TopData | Bottom |
 *  Effect | State |
 *  VMType | DataType
 *
 * VMType ::=
 *  Traits(traits_constant?) |
 *  Ordinal(ordinal_constant?) |
 *  Name(name_constant?) |
 *  Method(method_constant?) |
 *  Env(method_constraint)
 *
 * DataType ::=
 *  Any(nullable) |
 *  Object(nullable) |
 *  Void |
 *  Boolean(boolean_constant?, model) |
 *  Number(number_constraint | number_constant, model)
 *  String(nullable | string_constant, model) |
 *  Namespace(nullable | namespace_constant, model) |
 *  ScriptObject(traits, (nullable, final) | scriptobject_constant, model)
 *
 ***************************************************************/

/**
 * This enum describes the key roots of the type lattice.  Every Type
 * has a kind, which is one of these enums, and additional fields that
 * further refine the set of values in that type.
 */
enum TypeKind {
  kTypeTop,       // variable ranging over all types (top)
  kTypeTopData,      // variable ranging over all data types
  kTypeBottom,    // empty value set, subtype of all

  kTypeEffect,    // effect type (outside world state)
  kTypeState,     // (frame) state type
  kTypeTraits,    // traits objects
  kTypeOrdinal,   // ordinals (slot positions)
  kTypeName,      // (multi)names
  kTypeMethod,    // method info objects
  kTypeEnv,       // method env objects

  kTypeAny,       // the AS * type
  kTypeObject,    // the (non-final) AS Object type
  kTypeVoid,      // the type of 'undefined'

  kTypeString,
  kTypeNamespace,
  kTypeBoolean,
  kTypeNumber,

  kTypeScriptObject, // any ScriptObject: user classes, final Object, activation objects
};

/**
 * true iff k is an abstract type kind.
 * An IR graph is lowered, i.e. suitable for CG,
 * interpretation, etc., if none of its types are
 * abstract.
 */
inline bool isAbstractType(TypeKind k) {
  switch (k) {
  case kTypeTop:
  case kTypeTopData:
  case kTypeBottom:
    return true;
  default:
    return false;
  }
}

/**
 * true iff k is not an abstract type kind.
 */
inline bool isConcreteType(TypeKind k) {
  return !isAbstractType(k);
}

/**
 * true iff k is a data (i.e., AS language) type kind,
 * as opposed to a VM or abstract type.
 */
inline bool isDataType(TypeKind k) {
  switch (k) {
  case kTypeAny:
  case kTypeObject:
  case kTypeVoid:
  case kTypeString:
  case kTypeNamespace:
  case kTypeBoolean:
  case kTypeNumber:
  case kTypeScriptObject:
    return true;
  default:
    return false;
  }
}

/**
 * true iff k is a VM type kind.
 */
inline bool isVMType(TypeKind k) {
  switch (k) {
  case kTypeEffect:
  case kTypeState:
  case kTypeTraits:
  case kTypeOrdinal:
  case kTypeName:
  case kTypeMethod:
  case kTypeEnv:
    return true;
  default:
    return false;
  }
}

/**
 * model is the physical representation of a value.
 * TODO explain
 */
enum ModelKind {
  kModelAtom,
  kModelString,
  kModelNamespace,
  kModelScriptObject,
  kModelInt,
  kModelDouble,
  kModelInvalid,  // for non-data types
  kModelMAX
};

/**
 * single-character print descriptor for model
 */
inline char modelDesc(ModelKind model) {
  return "ASNOID-"[model];
}

/**
 * true if model is a machine pointer
 */
inline bool isPointer(ModelKind model) {
  switch (model) {
  case kModelString:
  case kModelNamespace:
  case kModelScriptObject:
    return true;
  default:
    return false;
  }
}

/**
 * Type is an immutable value object that represents an element in the type
 * lattice of the optimizer, including constant values.
 * 
 * Each Type represents a set of runtime values.  Each field in
 * a Type restricts the set in some way.
 * 
 * TODO:
 * final-ness of pointers:  <=T vs =T
 * signatures of function closures
 * class heirarchy of Type's?
 */
class Type {
public:
  const char *name;           // Formatted name of this type.
  TypeKind kind;              // TODO const when refactored
  unsigned is_nullable :1;    // TODO const when refactored
  unsigned is_const :1;       // TODO const when refactored

  Type(const char* name, TypeKind kind, bool is_nullable, bool is_const) :
    name(name),
    kind(kind),
    is_nullable(is_nullable),
    is_const(is_const) {
  }

  virtual ~Type() {}

  /** Need this for compare-by-value in nanojit::Hashtable. */
  virtual bool operator==(const Type& other) const = 0;

  /** Need this for compare-by-value in nanojit::Hashtable */
  bool operator!=(const Type& other) const {
    return !(*this == other);
  }

  virtual size_t hashCode() const = 0;

  virtual bool isSubtypeOf(const Type&) const = 0;

  /** build the union of ourselves and type t - service for Lattice::makeUnion */
  virtual const Type* makeUnion(const Type& t, Lattice& lattice) const = 0;

  virtual bool isConst() const {
    return is_const != 0;
  }

protected:
  /** copy ourselves into Lattice - service for Lattice::makeType */
  virtual const Type* copy(Lattice& lattice) const = 0;

  /** name */
  virtual PrintWriter& printName(PrintWriter& console, AvmCore* core) const = 0;

  const char* buildName(Lattice& lat) const;

  friend class Lattice;
  friend class DataType;  // needed for DataType::makeUnion to call Type::makeUnion...?

  friend TypeKind kind(const Type*);
  friend bool isNullable(const Type*);
  //friend bool isConst(const Type*);
  friend const char* typeName(const Type*);
};

inline TypeKind kind(const Type* t) {
  return t->kind;
}

inline bool isNullable(const Type* t) {
  return t->is_nullable != 0;
}

inline bool isConst(const Type* t) {
  return t->isConst();
}

inline bool isDataType(const Type& t) {
  return isDataType(kind(&t));
}

/**
 * true if this type is a linear type which cannot be stored or copied.
 */
inline bool isLinear(const Type* t) {
  // todo what about tuple? Is it bad if multiple IR nodes use the same safepoint?
  // fixme: STATE should be linear too, but this prevents identity opts
  // which we count on, for now.
  return kind(t) == kTypeEffect;// || t == STATE;
}

/**
 * Return true iff value_set(t1) is a subset of value_set(t2)
 */
bool subtypeof(const Type* t1, const Type* t2);

/**
 * Return true if t1's models are a subset of t2's models.
 */
bool submodelof(const Type* t1, const Type* t2);

//
// global Type constants
//

extern const Type* TOP;     // = &TopType::INSTANCE
extern const Type* BOT;     // = &BottomType::INSTANCE
extern const Type* TOPDATA; // = &TopDataType::INSTANCE
extern const Type* EFFECT;  // = &EffectType::INSTANCE
extern const Type* STATE;   // = &StateType::INSTANCE

extern const Type* TRAITS;  // = &TraitsType::BASE
extern const Type* ORDINAL; // = &OrdinalType::BASE
extern const Type* NAME;    // = &NameType::BASE
extern const Type* METHOD;  // = &MethodType::BASE
extern const Type* ENV;     // = &EnvType::BASE

/** safe downcast from Type to given Type subclass */
template <class TYPE_SUB> const TYPE_SUB& toType(const Type& t) {
  assert(t.kind == TYPE_SUB::KIND);
  return (const TYPE_SUB&)t;
}

/** pointer version of toType<T> */
template <class TYPE_SUB> const TYPE_SUB* toType(const Type* t) {
  assert(t->kind == TYPE_SUB::KIND);
  return (const TYPE_SUB*)t;
}

/**
 * SimpleType - representation for the simplest types.
 * Each SimpleType<K> carries a singleton instance
 * that's 1:1 with TypeKind K.
 */
template<TypeKind TYPE_KIND>
class SimpleType : public Type {
protected:
  SimpleType(const char* name, bool is_nullable) :
    Type(name, TYPE_KIND, is_nullable, false) {
  }

  const Type* makeUnion(const Type& t, Lattice&) const {
    return isSubtypeOf(t) ? &t : t.isSubtypeOf(*this) ? this : TOP;
  }

  const Type* copy(Lattice&) const {
    return this;
  }

  PrintWriter& printName(PrintWriter& console, AvmCore*) const {
    assert(false && "SimpleType::printName");
    return console;
  }

public:
  static const TypeKind KIND = TYPE_KIND;

  bool operator==(const Type& t) const {
    return t.kind == TYPE_KIND;
  }

  bool isSubtypeOf(const Type& t) const {
    return t.kind == TYPE_KIND || t.kind == kTypeTop;
  }

  size_t hashCode() const {
    return TYPE_KIND;
  }
};

/**
 * TopType - union of all types; every value.
 */
class TopType : public SimpleType<kTypeTop> {
  TopType() : SimpleType<kTypeTop>("Top", true) {}

public:
  static const TopType INSTANCE;
};

inline bool isTop(const Type* t) {
  return t->kind == kTypeTop;
}

/**
 * BottomType - empty set of values.
 */
class BottomType : public SimpleType<kTypeBottom> {
  BottomType() : SimpleType<kTypeBottom>("_", false) {}

public:
  static const BottomType INSTANCE;

  // Note override
  bool isSubtypeOf(const Type&) const {
    return true;
  }
};

inline bool isBottom(const Type* t) {
  return t->kind == kTypeBottom;
}

/**
 * TopDataType - union of all DataTypes. DataTypes
 * represent sets of surface language values, as opposed
 * to internal VM data values.
 */
class TopDataType : public SimpleType<kTypeTopData> {
  TopDataType() : SimpleType<kTypeTopData>("TopData", true) {}

public:
  static const TopDataType INSTANCE;
};

inline bool isTopData(const Type* t) {
  return t->kind == kTypeTopData;
}

/**
 * EffectType - the type of "world state" values
 * consumed and produced by effectful instructions.
 */
class EffectType : public SimpleType<kTypeEffect> {
  EffectType() : SimpleType<kTypeEffect>("+", false) {}

public:
  static const EffectType INSTANCE;
};

inline bool isEffect(const Type* t) {
  return t->kind == kTypeEffect;
}

/**
 * StateType - the type of stack frame states.
 */
class StateType : public SimpleType<kTypeState> {
  StateType() : SimpleType<kTypeState>("$", false) {}

public:
  static const StateType INSTANCE;
};

inline bool isState(const Type* t) {
  return t->kind == kTypeState;
}

/**
 * VMType - a subclass T of VMType<T, K, R> denotes a VM type:
 * a set of concrete, VM-internal data values. All instances of
 * T share a TypeKind K, which is used by no other type.
 *
 * A singleton instance T::BASE represents the unconstrained
 * type T. Other instances represent T==v:R, T constrained to
 * a single known value v.
 *
 * - SELF_CLASS: the deriving subclass itself
 * - TYPE_KIND: TypeKind of the represented type
 * - VALUE_REP: type of the C++ value representation
 */
template<class SELF_CLASS, TypeKind TYPE_KIND, typename VALUE_REP>
class VMType : public Type {
protected:
  VALUE_REP const_value;

  VMType(const char* name, bool is_nullable) :
    Type(name, TYPE_KIND, is_nullable, false) {
    const_value = (VALUE_REP)0;
  }

  const Type* copy(Lattice& lattice) const;

  const Type* makeUnion(const Type& t, Lattice&) const {
    return isSubtypeOf(t) ? &t :
      t.isSubtypeOf(*this) ? this :
      t.kind == TYPE_KIND ? &SELF_CLASS::BASE :
      TOP;
  }

public:
  VMType(VALUE_REP const_value) :
    Type(NULL, TYPE_KIND, const_value == (VALUE_REP)0, true),
    const_value(const_value) {
  }

  static const TypeKind KIND = TYPE_KIND;

  bool operator==(const SELF_CLASS& t) const {
    return is_const == t.is_const && const_value == t.const_value;
  }

  bool operator==(const Type& t) const {
    return t.kind == TYPE_KIND && *this == (const SELF_CLASS&)t;
  }

  size_t hashCode() const {
    size_t const_hash = is_const ?
        nanojit::DefaultHash<VALUE_REP>::hash(const_value) : 0;
    return kind + const_hash;
  }

  bool isSubtypeOf(const SELF_CLASS& t) const {
    return !t.is_const || (is_const && const_value == t.const_value);
  }

  bool isSubtypeOf(const Type& t) const {
    return t.kind == kTypeTop ||
      (t.kind == TYPE_KIND && isSubtypeOf((SELF_CLASS&)t));
  }

  VALUE_REP getConst() const {
    assert(is_const);
    return const_value;
  }
};

/**
 * TraitsType - the type of Traits objects, used to carry runtime
 * type and object layout information in the VM.
 */
class TraitsType : public VMType<TraitsType, kTypeTraits, Traits*> {
  TraitsType() :
    VMType<TraitsType, kTypeTraits, Traits*>("<>", true) {
  }

  PrintWriter& printName(PrintWriter& console, AvmCore*) const {
    assert(is_const);
    return console << "<" << const_value << ">";
  }

public:
  static const TraitsType BASE;

  TraitsType(Traits* traits_const) :
    VMType<TraitsType, kTypeTraits, Traits*>(traits_const) {}
};

inline bool isTraits(const Type* t) {
  return t->kind == kTypeTraits;
}

inline Traits* traitsVal(const Type* t) {
  return toType<TraitsType>(t)->getConst();
}

/**
 * OrdinalType - the type of ordinal positions, used for
 * slot indexes.
 */
class OrdinalType : public VMType<OrdinalType, kTypeOrdinal, int> {
  OrdinalType() :
    VMType<OrdinalType, kTypeOrdinal, int>("#", false) {
  }

  PrintWriter& printName(PrintWriter& console, AvmCore*) const {
    assert(is_const);
    return console << "#" << const_value;
  }

public:
  static const OrdinalType BASE;

  OrdinalType(int ordinal_const) :
    VMType<OrdinalType, kTypeOrdinal, int>(ordinal_const) {}
};

inline bool isOrdinal(const Type* t) {
  return t->kind == kTypeOrdinal;
}

inline int ordinalVal(const Type* t) {
  return toType<OrdinalType>(t)->getConst();
}

/**
 * NameType - the type of AS Multiname values.
 */
class NameType : public VMType<NameType, kTypeName, const Multiname*> {
  NameType() :
    VMType<NameType, kTypeName, const Multiname*>("`*", false) {
  }

  PrintWriter& printName(PrintWriter& console, AvmCore*) const {
    assert(is_const);
    return console << "`" << Multiname::FormatNameOnly(const_value);
  }

public:
  static const NameType BASE;

  NameType(const Multiname* multiname_const) :
    VMType<NameType, kTypeName, const Multiname*>(multiname_const) {
    assert(multiname_const != NULL);
  }
};

inline bool isName(const Type* t) {
  return t->kind == kTypeName;
}

inline const Multiname* nameVal(const Type* t) {
  return toType<NameType>(t)->getConst();
}

/**
 * MethodType - the type of MethodInfo structures.
 */
class MethodType : public VMType<MethodType, kTypeMethod, MethodInfo*> {
  MethodType() :
    VMType<MethodType, kTypeMethod, MethodInfo*>("?()", false) {
  }

  PrintWriter& printName(PrintWriter& console, AvmCore*) const {
    assert(is_const);
#ifdef AVMPLUS_VERBOSE
    return console << const_value;   // note: PrintWriter appends '()'
#else
    return console << "<method>";
#endif
  }

public:
  static const MethodType BASE;

  MethodType(MethodInfo* method_const) :
    VMType<MethodType, kTypeMethod, MethodInfo*>(method_const) {
    assert(method_const != NULL);
  }
};

inline bool isMethod(const Type* t) {
  return t->kind == kTypeMethod;
}

inline MethodInfo* methodVal(const Type* t) {
  return toType<MethodType>(t)->getConst();
}

/**
 * EnvType - the type of MethodEnv structures.
 * Note: even though EnvTypes are abstractly VM types,
 * EnvType is not a subclass of VMType. This is just a
 * practical consideration: class VMType has infrastructure
 * for singleton-constrained instances, but it isn't useful
 * to constrain EnvType to a known constant MethodEnv.
 * Instead we want to constrain to the subset of MethodEnvs
 * whose MethodInfo is known.
 */
class EnvType : public Type {
  MethodInfo* method;

  EnvType() : Type("E", kTypeEnv, false, false) {
    method = NULL;
  }

  const Type* copy(Lattice& lattice) const;

  const Type* makeUnion(const Type& t, Lattice&) const {
    return isSubtypeOf(t) ? &t :
      t.isSubtypeOf(*this) ? this :
      t.kind == KIND ? &BASE :
      TOP;
  }

  PrintWriter& printName(PrintWriter& console, AvmCore*) const {
    assert(method != NULL);
#ifdef AVMPLUS_VERBOSE
    return console << "E|" << this->method;
#else
    return console << "E|<method>";
#endif
  }

public:   // TODO private to Lattice
  /**
   * NOTE: method is a constraint on the set of MethodEnvs
   * denoted by this type, but it doesn't guarantee that the
   * set is a singleton, which is the actual meaning of is_const.
   */
  EnvType(MethodInfo* method) :
    Type(NULL, kTypeEnv, false, false),
    method(method) {
    assert(method != NULL);
  }

public:
  static const TypeKind KIND = kTypeEnv;
  static const EnvType BASE;

  bool operator==(const EnvType& et) const {
    return method == et.method;
  }

  bool operator==(const Type& t) const {
    return kind == t.kind && *this == (const EnvType&)t;
  }

  size_t hashCode() const {
    return kind + nanojit::DefaultHash<MethodInfo*>::hash(method);
  }

  bool isSubtypeOf(const EnvType& et) const {
    return et.method == NULL || method == et.method;
  }

  bool isSubtypeOf(const Type& t) const {
    return t.kind == kTypeTop ||
      (t.kind == KIND && isSubtypeOf((const EnvType&)t));
  }

  MethodInfo* getMethod() const {
    return method;
  }
};

inline bool isEnv(const Type* t) {
  return t->kind == kTypeEnv;
}

// TODO rename getEnvMethod after refactoring
inline MethodInfo* getMethod(const Type* t) {
  return toType<EnvType>(t)->getMethod();
}

// ------------------------------------------------------------

/**
 * DataType
 */
class DataType : public Type {
protected:
  DataType(TypeKind kind, ModelKind model, Traits* traits, bool is_nullable,
           bool is_const) :
    Type(NULL, kind, is_nullable, is_const),
    traits(traits),
    model(model) {
  }

  virtual PrintWriter& printConstQual(PrintWriter& console, AvmCore* core) const;

public:
  Traits* const traits; // Lowest common ancestor of all possible values.
  const ModelKind model;

  bool excludesNull() const;

  virtual ModelKind defaultModelKind() const;

  virtual const DataType* changeModel(Lattice &lat, ModelKind m) const = 0;

  virtual const Type* makeNotNull(Lattice &lat) const = 0;

  virtual Atom getAtomConst(AvmCore*) const = 0;
};

inline ModelKind model(const Type* t) {
  return isDataType(*t) ? ((DataType*)t)->model : kModelInvalid;
}

inline Traits* getTraits(const Type* t) {
  assert(isDataType(*t));
  return ((DataType*)t)->traits;
}

/**
 * Convenience wrapper to get the BuiltinType for traits t.
 */
inline BuiltinType builtinType(Traits* t) {
  return Traits::getBuiltinType(t);
}

/**
 * Return the BuiltinType of t->traits.
 */
inline BuiltinType builtinType(const DataType* t) {
  assert(isDataType(*t));
  return builtinType(getTraits(t));
}

/**
 * Return the default representation for the given traits.
 */
ModelKind defaultModelKind(Traits* traits);

// ------------------------------------------------------

/**
 * SimpleDataType
 */
template<class SELF_CLASS, TypeKind TYPE_KIND, ModelKind MODEL_KIND, typename VALUE_REP>
class SimpleDataType : public DataType {

  const Type* copy(Lattice& lat) const;

  const DataType* changeModel(Lattice &lat, ModelKind m) const;

  const Type* makeNotNull(Lattice& lat) const;

protected:
  PrintWriter& printName(PrintWriter& console, AvmCore* core) const;

  bool isNull() const {
    return is_const && is_nullable;
  }

  bool isCompatibleModel(ModelKind model) const {
    return model == NATIVE_MODEL || model == kModelAtom;
  }

  // TODO factor
  bool isTraitsSubtype(const DataType& t) const {
    // Even though t1 implements t2, the current type system
    // can't handle interfaces correctly. Read makeUnion comment.
    // we only return true if both are the exact same traits
    Traits* ttraits = getTraits(&t);
    if ((ttraits != NULL) && ttraits->isInterface()) {
      return traits == ttraits;
    }
    return !ttraits ||
        (traits && traits->subtypeof(ttraits)) ||
        // TODO this should go somewhere else
        (t.is_nullable && builtinType(this) == BUILTIN_null);
  }

  bool isNullableSubtype(const Type& t) const {
    return !is_nullable || t.is_nullable;
  }

  bool isKindSubtype(const DataType& dt) const {
    return dt.kind == TYPE_KIND || dt.kind == kTypeAny || dt.kind == kTypeObject ||
      (isNull() && dt.is_nullable && isPointer(model) && isPointer(dt.model));
  }

  bool isConstSubtype(const DataType& dt) const {
    return !dt.isConst() ||
      (isConst() &&
       kind == dt.kind &&
       getConst() == ((SELF_CLASS&)dt).getConst());
  }

public:
  static const ModelKind NATIVE_MODEL = MODEL_KIND;
  static const TypeKind KIND = TYPE_KIND;

  VALUE_REP const_value;

  SimpleDataType(Traits* traits, ModelKind model, bool is_nullable,
              bool is_const, VALUE_REP const_value) :
    DataType(TYPE_KIND, model, traits, is_nullable, is_const),
    const_value(const_value) {
    // require that non-const types park their const_values
    assert(isConst() || const_value == (VALUE_REP)0);
  }

  const SELF_CLASS* makeUnion(const SELF_CLASS& t, Lattice& lat) const;

  const Type* makeUnion(const Type& t, Lattice& lat) const;

  bool operator==(const SELF_CLASS& st) const {
    return traits == st.traits &&
      is_nullable == st.is_nullable &&
      is_const == st.is_const &&
      const_value == st.const_value &&
      model == st.model;
  }

  bool operator==(const Type& t) const {
    return kind == t.kind && *this == (SELF_CLASS&)t;
  }

  size_t hashCode() const {
    size_t traits_hash = nanojit::DefaultHash<Traits*>::hash(traits);
    size_t const_hash = nanojit::DefaultHash<VALUE_REP>::hash(const_value);
    return traits_hash ^ const_hash ^ (size_t(is_nullable) + size_t(model));
  }

  bool isSubtypeOf(const DataType& dt) const {
    return isKindSubtype(dt) &&
        isNullableSubtype(dt) &&
        isTraitsSubtype(dt) &&
        isConstSubtype(dt);
  }

  bool isSubtypeOf(const Type& t) const {
    return
      t.kind == kTypeTop ||
      t.kind == kTypeTopData ||
      (isDataType(t) && isSubtypeOf((DataType&)t));
  }

  VALUE_REP getConst() const {
    assert(isConst());
    return const_value;
  }
};

/**
 * AnyType
 */
class AnyType : public SimpleDataType<AnyType, kTypeAny, kModelAtom, Atom> {
public:
  AnyType(Traits* traits, bool is_nullable);

  AnyType(ModelKind model, Traits* traits, bool is_nullable, bool is_const,
          Atom const_value) :
    SimpleDataType<AnyType, kTypeAny, kModelAtom, Atom>
      (traits, model, is_nullable, is_const, const_value) {
    assert(!is_const);
    assert(builtinType(traits) == BUILTIN_any);
    assert(model == kModelAtom);
  }

  virtual ~AnyType() {}

  ModelKind defaultModelKind() const {
    return kModelAtom;
  }

  PrintWriter& printName(PrintWriter& console, AvmCore*) const {
    console << "*";
    if (excludesNull())
      console << "~";
    return console;
  }

  Atom getAtomConst(AvmCore*) const {
    assert(false);
    return const_value;
  }
};

/**
 * ObjectType
 */
class ObjectType : public SimpleDataType<ObjectType, kTypeObject, kModelAtom, Atom> {
public:
  ObjectType(Traits* traits, bool is_nullable);

  ObjectType(ModelKind model, Traits* traits, bool is_nullable, bool is_const,
             Atom const_value) :
    SimpleDataType<ObjectType, kTypeObject, kModelAtom, Atom>
      (traits, model, is_nullable, is_const, const_value) {
    assert(!is_const);
    assert(builtinType(traits) == BUILTIN_object);
    assert(model == kModelAtom);
  }

  virtual ~ObjectType() {}

  ModelKind defaultModelKind() const {
    return kModelAtom;
  }

  PrintWriter& printName(PrintWriter& console, AvmCore*) const {
    console << "Object";
    if (excludesNull())
      console << "~";
    return console;
  }

  Atom getAtomConst(AvmCore*) const {
    assert(false);
    return const_value;
  }
};

/**
 * VoidType
 */
class VoidType : public SimpleDataType<VoidType, kTypeVoid, kModelAtom, Atom> {
public:
  VoidType(Traits* traits);

  VoidType(ModelKind model, Traits* traits, bool is_nullable, bool is_const,
           Atom const_value) :
    SimpleDataType<VoidType, kTypeVoid, kModelAtom, Atom>
      (traits, model, is_nullable, is_const, const_value) {
    assert(builtinType(traits) == BUILTIN_void);
    assert(is_const);
    assert(AvmCore::isUndefined(const_value));
    assert(model == kModelAtom);
  }

  virtual ~VoidType() {}

  ModelKind defaultModelKind() const {
    return kModelAtom;
  }

  PrintWriter& printName(PrintWriter& console, AvmCore* core) const {
    console << "void";
    printConstQual(console, core);
    return console;
  }

  Atom getAtomConst(AvmCore*) const {
    return const_value;
  }
};

/**
 * ScriptObjectType
 */
class ScriptObjectType : public SimpleDataType<
  ScriptObjectType, kTypeScriptObject, kModelScriptObject, ScriptObject*> {
public:
  ScriptObjectType(ModelKind model, Traits* traits, bool is_nullable);

  ScriptObjectType(ModelKind model, Traits* traits, ScriptObject* const_value);

  ScriptObjectType(ModelKind model, Traits* traits, bool is_nullable,
                   bool is_const, ScriptObject* const_value);

  virtual ~ScriptObjectType() {}

  ModelKind defaultModelKind() const {
    return kModelScriptObject;
  }

  PrintWriter& printName(PrintWriter& console, AvmCore* core) const;

  Atom getAtomConst(AvmCore*) const {
    return const_value->atom();
  }
};

inline ScriptObject* objectVal(const Type* t) {
  return toType<ScriptObjectType>(*t).getConst();
}

/**
 * NamespaceType
 */
class NamespaceType : public SimpleDataType<
  NamespaceType, kTypeNamespace, kModelNamespace, Namespace*> {
public:
  NamespaceType(Lattice& lat, ModelKind model, bool is_nullable);

  NamespaceType(Lattice& lat, ModelKind model, Namespace* const_value);

  NamespaceType(ModelKind model, Traits* traits, bool is_nullable,
                bool is_const, Namespace* const_value);

  virtual ~NamespaceType() {}

  Atom getAtomConst(AvmCore*) const {
    return const_value->atom();
  }
};

inline Namespace* nsVal(const Type* t) {
  return toType<NamespaceType>(*t).getConst();
}

/**
 * StringType
 */
class StringType : public SimpleDataType<
  StringType, kTypeString, kModelString, String*> {

  bool constEquals(String* val) const {
    return const_value->equals(val);
  }

public:
  StringType(Lattice& lat, ModelKind model, bool is_nullable);

  StringType(Lattice& lat, ModelKind model, String* const_value);

  StringType(ModelKind model, Traits* traits, bool is_nullable,
             bool is_const, String* const_value);

  virtual ~StringType() {}

  PrintWriter& printNameQual(PrintWriter& console, AvmCore*) const {
    assert(is_const);
    return console << "='" << const_value << "'";
  }

  Atom getAtomConst(AvmCore*) const {
    return const_value->atom();
  }
};

inline String* stringVal(const Type* t) {
  return toType<StringType>(*t).getConst();
}

/**
 * BooleanType
 */
class BooleanType : public SimpleDataType<
  BooleanType, kTypeBoolean, kModelInt, bool> {
public:
  BooleanType(Lattice& lat, ModelKind model);

  BooleanType(Lattice& lat, bool const_value, ModelKind model);

  BooleanType(ModelKind model, Traits* traits, bool is_nullable,
              bool is_const, bool const_value);

  virtual ~BooleanType() {}

  Atom getAtomConst(AvmCore*) const {
    return getConst() ? trueAtom : falseAtom;
  }
};

inline bool boolVal(const Type* t) {
  return toType<BooleanType>(*t).getConst();
}

// ----------------------------------------------------

/**
 * NumberConstraint
 */
inline bool isWhole(double d) {
  return !MathUtils::isNegZero(d) && MathUtils::toInt(d) == d;
}

inline bool isInt(double d) {
  return isWhole(d) && int32_t(d) == d;
}

inline bool isUInt(double d) {
  return isWhole(d) && uint32_t(d) == d;
}

struct Int32Range {
  const bool is_int;              // whole number and in [0 : 2^32-1]
  const bool is_uint;             // whole number and in [-2^31 : 2^31-1]

  Int32Range() : is_int(false), is_uint(false) {}
  Int32Range(bool is_int, bool is_uint) : is_int(is_int), is_uint(is_uint) {
    assert(is_int || is_uint);
  }

  bool equals(const Int32Range& r) const {
    return is_int == r.is_int && is_uint == r.is_uint;
  }

  bool subsumes(const Int32Range& r) const {
    return (!is_int || r.is_int) && (!is_uint || r.is_uint);
  }

  size_t hashCode() const {
    return size_t(is_int) + size_t(is_uint);
  }
};

struct NumberConstraint {
  enum Kind {
    kValue,
    kInt32Range,
    kAll
  };

  // NOTE: read this as a tagged union
  const Kind kind;
  const Int32Range range;
  const double value;

  NumberConstraint() : kind(kAll), value(0) {}

  NumberConstraint(double v) : kind(kValue), value(v) {}

  NumberConstraint(bool is_int, bool is_uint) :
    kind(kInt32Range), range(is_int, is_uint), value(0) {}

  NumberConstraint(Int32Range r) :
    kind(kInt32Range), range(r), value(0) {}

  NumberConstraint(const NumberConstraint& c) :
    kind(c.kind), range(c.range), value(c.value) {}

  static bool valuesEqual(double v1, double v2) {
    union { double d; int64_t i64; } x, y;
    x.d = v1; y.d = v2;
    return x.i64 == y.i64;
  }

  bool equals(const NumberConstraint& c) const {
    return kind == c.kind &&
      ((kind == kAll) ||
       (kind == kValue && valuesEqual(value, c.value)) ||
       (kind == kInt32Range && range.equals(c.range)));
  }

  bool subsumes(const NumberConstraint& c) const {
    return makeUnion(c).equals(*this);
  }

  size_t hashCode() const {
    return kind ^ range.hashCode() ^ nanojit::DefaultHash<double>::hash(value);
  }

  static NumberConstraint makeValueUnion(double v1, double v2) {
    return
      valuesEqual(v1, v2) ?
        NumberConstraint(v1) :
      (halfmoon::isInt(v1) && halfmoon::isInt(v2)) ?
        NumberConstraint(true, halfmoon::isUInt(v1) && halfmoon::isUInt(v2)) :
      (halfmoon::isUInt(v1) && halfmoon::isUInt(v2)) ?
        NumberConstraint(false, true) :
      NumberConstraint();
  }

  static NumberConstraint makeRangeUnion(double v, const Int32Range& r) {
    bool is_int = halfmoon::isInt(v) && r.is_int;
    bool is_uint = halfmoon::isUInt(v) && r.is_uint;
    return (is_int || is_uint) ?
      NumberConstraint(is_int, is_uint) :
      NumberConstraint();
  }

  static NumberConstraint makeRangeUnion(const Int32Range& r1, const Int32Range& r2) {
    bool is_int = r1.is_int && r2.is_int;
    bool is_uint = r1.is_uint && r2.is_uint;
    return (is_int || is_uint) ?
      NumberConstraint(is_int, is_uint) :
      NumberConstraint();
  }

  NumberConstraint makeUnion(const NumberConstraint& c) const {
    switch (kind) {
    case kValue: {
      switch (c.kind) {
      case kValue: return makeValueUnion(value, c.value);
      case kInt32Range: return makeRangeUnion(value, c.range);
      default: return c;
      }
      break;
    }
    case kInt32Range: {
      switch (c.kind) {
      case kValue: return makeRangeUnion(c.value, range);
      case kInt32Range: return makeRangeUnion(range, c.range);
      default: return c;
      }
      break;
    }
    default:
      return *this;
    }
  }

  bool isConst() const {
    return kind == kValue;
  }

  bool isInt() const {
    return
      (kind == kValue && halfmoon::isInt(value)) ||
      (kind == kInt32Range && range.is_int);
  }

  bool isUInt() const {
    return
      (kind == kValue && halfmoon::isUInt(value)) ||
      (kind == kInt32Range && range.is_uint);
  }
};

/**
 * `
 */
class NumberType : public DataType {
protected:
  const NumberConstraint constraint;

  PrintWriter& printName(PrintWriter& console, AvmCore* core) const {

    console << "Number";

    if (is_const) {
      printConstQual(console, core);
    } else if (constraint.isInt() && constraint.isUInt()) {
      console << "|int&uint";
    } else if (constraint.isInt()) {
      console << "|int";
    } else if (constraint.isUInt()) {
      console << "|uint";
    }

    if (model != defaultModelKind())
      console << "[" << modelDesc(model) << "]";

    return console;
  }

  const Type* copy(Lattice& lat) const;

  const NumberType* changeModel(Lattice &lat, ModelKind m) const;

  const NumberType* makeNotNull(Lattice&) const { return this; }

  bool isCompatibleModel(ModelKind model) const {
    return model == NATIVE_MODEL || model == kModelAtom ||
      (model == kModelInt && (isInt() || isUInt()));
  }

  // TODO factor
  bool isTraitsSubtype(const DataType& t) const {
    // Even though t1 implements t2, the current type system
    // can't handle interfaces correctly. Read makeUnion comment.
    // we only return true if both are the exact same traits
    Traits* ttraits = getTraits(&t);
    if ((ttraits != NULL) && ttraits->isInterface()) {
      return traits == ttraits;
    }
    return !ttraits ||
        (traits && traits->subtypeof(ttraits));
  }

  // TODO factor
  bool isKindSubtype(const DataType& dt) const {
    return dt.kind == KIND || dt.kind == kTypeAny || dt.kind == kTypeObject;
  }

  bool isConstSubtype(const DataType& dt) const {
    return !dt.isConst() ||
      (isConst() &&
       kind == dt.kind &&
       getConst() == ((NumberType&)dt).getConst());
  }

public:
  static const ModelKind NATIVE_MODEL = kModelDouble;
  static const TypeKind KIND = kTypeNumber;
  static const NumberConstraint TRIV;

  NumberType(Lattice& lat, ModelKind model);

  NumberType(Lattice& lat, ModelKind model, bool is_int, bool is_uint);

  NumberType(Lattice& lat, double const_value, ModelKind model);

  NumberType(ModelKind model, Traits* traits, NumberConstraint constraint);

  virtual ~NumberType() {}

  const NumberType* makeUnion(const NumberType& t, Lattice& lat) const;

  const Type* makeUnion(const Type& t, Lattice& lat) const;

  bool operator==(const NumberType& nt) const {
    return constraint.equals(nt.constraint) && model == nt.model;
  }

  bool operator==(const Type& t) const {
    return kind == t.kind && *this == (NumberType&)t;
  }

  size_t hashCode() const {
    size_t const_hash = constraint.hashCode();
    return const_hash ^ size_t(model);
  }

  bool isSubtypeOf(const NumberType& nt) const {
    return nt.constraint.subsumes(constraint);
  }

  bool isSubtypeOf(const DataType& dt) const {
    return isKindSubtype(dt) &&
        isTraitsSubtype(dt) &&
        isConstSubtype(dt);
  }

  bool isSubtypeOf(const Type& t) const {
    return
      t.kind == kTypeTop ||
      t.kind == kTypeTopData ||
      (t.kind == kTypeNumber ?
          isSubtypeOf((NumberType&)t) :
          isSubtypeOf((DataType&)t));
  }

  double getConst() const {
    assert(isConst());
    return constraint.value;
  }

  bool isConst() const {
    return constraint.isConst();
  }

  bool isInt() const {
    return constraint.isInt();
  }

  bool isUInt() const {
    return constraint.isUInt();
  }

  Atom getAtomConst(AvmCore* core) const {
    assert(isConst());
    return core->doubleToAtom(constraint.value);
  }
};

// ------------------------------------------------------

/**
 * True if t is a data type but not one of the specific kinds.
 * This is not "is subtype of any"
 * TODO now excludes kTypeObject, kTypeVoid... restore?
 */
inline bool isAny(const Type* t) {
  return kind(t) == kTypeAny;
}

inline bool isNumber(const Type* t) {
  return kind(t) == kTypeNumber;
}

inline bool isInt(const Type* t) {
  return isNumber(t) && ((NumberType*)t)->isInt();
}

inline bool isUInt(const Type* t) {
  return isNumber(t) && ((NumberType*)t)->isUInt();
}

inline bool isDouble(const Type* t) {
  return (kind(t) == kTypeNumber) &&
    !isInt(t) && !isUInt(t);
}

inline bool isBoolean(const Type* t) {
  return kind(t) == kTypeBoolean;
}

inline bool isString(const Type* t) {
  return kind(t) == kTypeString;
}

inline bool isNamespace(const Type* t) {
  return kind(t) == kTypeNamespace;
}

inline bool isScriptObject(const Type* t) {
  return kind(t) == kTypeScriptObject;
}

/**
 * Return true for so-called AS3 primitive types int, uint, String,
 * Number, and Boolean.
 */
inline bool isPrimitiveTraits(Traits* t) {
  switch (builtinType(t)) {
  case BUILTIN_int:
  case BUILTIN_uint:
  case BUILTIN_number:
  case BUILTIN_boolean:
  case BUILTIN_string:
  case BUILTIN_null:
  case BUILTIN_void:
    return true;
  default:
    return false;
  }
}

/**
 * Return true for so-called AS3 primitive types int, uint, String,
 * Number, and Boolean.
 */
inline bool isPrimitive(const Type* t) {
  assert(isDataType(*t));
  return isPrimitiveTraits(getTraits(t));
}

inline bool isInterface(const Type* t) {
  return getTraits(t)->isInterface();
}

inline Atom atomVal(const Type* t, AvmCore* core) {
  assert(isDataType(*t) && isConst(t));
  return ((DataType*)t)->getAtomConst(core);
}

inline int32_t intVal(const Type* t) {
  assert(isInt(t) && isConst(t));
  // return int32_t(((DataType*)t)->double_const);
  return int32_t(toType<NumberType>(t)->getConst());
}

inline uint32_t uintVal(const Type* t) {
  assert(isUInt(t) && isConst(t));
  // return uint32_t(((DataType*)t)->double_const);
  return uint32_t(toType<NumberType>(t)->getConst());
}

inline double doubleVal(const Type* t) {
  assert(isNumber(t) && isConst(t));
  // return ((DataType*)t)->double_const;
  return toType<NumberType>(t)->getConst();
}

inline size_t argSizeof(const Type* t) {
  return isNumber(t) ? sizeof(double) : sizeof(Atom);
}

inline bool isNull(const Type* t) {
  return isConst(t) && isScriptObject(t) && !objectVal(t);
}

/** 
 * Return true if b is a slot binding.
 */
inline bool isSlot(Binding b) {
  return AvmCore::isSlotBinding(b) != 0;
}

inline bool isVarSlot(Binding b) {
  return AvmCore::isVarBinding(b) != 0;
}

inline bool isConstSlot(Binding b) {
  return AvmCore::isConstBinding(b) != 0;
}

inline bool hasGetter(Binding b) {
  return AvmCore::hasGetterBinding(b);
}

/**
 * Return true if b is a method binding.
 */
inline bool isMethod(Binding b) {
  return AvmCore::isMethodBinding(b);
}

inline bool isInterface(MethodInfo* m) {
  return m->declaringTraits()->isInterface();
}


/**
 * Return the slot number of b.
 */
inline uint32_t toSlot(Binding b) {
  return AvmCore::bindingToSlotId(b);
}

/**
 * Return the disp_id (index in VTable) of the method binding b.
 */
inline uint32_t toMethod(Binding b) {
  return AvmCore::bindingToMethodId(b);
}

/**
 * Return the disp_id of the getter.
 */
inline uint32_t toGetterIndex(Binding b) {
  return AvmCore::bindingToGetterId(b);
}

inline bool isValidBinding(Binding b) {
  return b != BIND_NONE && b != BIND_AMBIGUOUS;
}

inline Traits* getParamTraits(MethodSignaturep ms, int i) {
  return i <= ms->param_count() ? ms->paramTraits(i) : 0;
}

/**
 * Return true if the given name and index value might be a numeric index.
 * This is based on the "maybeIntegerIndex" logic found in Verifier.cpp
 * and CodegenLIR.cpp, for optimizing a[i] expressions.
 */
bool maybeIndex(const Type* name_type, const Type* index_type);

/**
 * A TypeKey is a wrapper for a Type* allowing the pointer to be used
 * as a key in HashMap.  Equality and hash code are computed on the
 * underlying Type, not the pointer value.
 */
class TypeKey {
public:
  TypeKey(int i) : type_(0) {
    assert(!i); (void) i;
  }

  explicit TypeKey(const Type* type) : type_(type) {
  }

  bool operator==(const TypeKey& other) const {
    return *type_ == *other.type_;
  }

  bool operator!=(const TypeKey& other) const {
    return *type_ != *other.type_;
  }

  static size_t hash(const TypeKey& k) {
    return k.type_->hashCode();
  }

  // Allow creation of dummy instance for compatibility with 'alignof'.
  // MSVC++ does not provide this by default.
  TypeKey() : type_(0) {}

private:
  const Type* type_;
};

/**
 * Nullability enum. Used to index Lattice type pairs
 */
enum NullabilityKind {
  kTypeNullable,
  kTypeNotNull
};

/**
 * Lattice is a container of types and implements operations of the type
 * system used by the optimizer.  Effect types are UN (unreachable),
 * and EFFECT (reachable).  Value types are UN (uncomputed), various types,
 * constants, and ANY.
 * 
 * Every generated type value is saved in a hashtable, and new ones are
 * interned (aka "hash-consed") so they can be compared by pointer.
 * fixme: pointer comparison may not be important, but saving memory and
 * avoiding copying large structs by value probably is important.
 */
class Lattice {
  friend class Type;
  typedef HashMap<TypeKey, const Type*, TypeKey,
                  UseOperEqual<TypeKey> > TypeTable;
public:
  Lattice(AvmCore*, Allocator&);

  AvmCore* core() const { return core_; }

  PrintWriter& console() const { return core()->console; }

  /**
   * isNullable() is true if the given type allows null in its value set.
   * False for primitives and true for String, Namespace, Object, user class
   * and interface types, and activation object types.
   */
  static bool isNullable(Traits* t);

  /**
   * Create the type of the requested parameter of a MethodSignature.
   * Parameter 0 is the receiver type and is not nullable.  Extra parameters
   * are required to be Atom.
   */
  const Type* makeParamType(int p, MethodSignaturep);

  /**
   * Look up the given type in the hashtable, add it if necessary, then
   * return the canonical type.  This function always returns a pointer to
   * an arena allocated Type, never a pointer to the argument value.
   */
  const Type* makeType(const Type& t);

  /**
   * Return the type for the given traits, taking into account whether that
   * traits is nullable or not.  If traits is nullable, the created type will
   * have nullable = true.
   */
  const DataType* makeType(Traits* traits) {
    return makeType(traits, isNullable(traits));
  }

  /**
   * Create a type value for the given traits and not_null hint.
   */
  const DataType* makeType(Traits* traits, bool nullable) {
    return makeType(traits, nullable, defaultModelKind(traits));
  }

  /**
   * Generate a type value with the given traits, nullable, and model.
   */
  const DataType* makeType(Traits* traits, bool nullable, ModelKind);

  /**
   * Create a type from a FrameState Value.
   */
  const Type* makeType(const FrameValue& value);

  /**
   * Create a non-null type value given a type-value that might be null.
   */
  const Type* makeNotNull(const Type* type);

  /**
   * Compute the type when converted to Atom.
   */
  const Type* makeAtom(const Type* type) {
    return changeModel(type, kModelAtom);
  }

  /**
   * Compute the type when unboxed to its default (aka native) representation.
   */
  const Type* makeNative(const Type* t) {
    return changeModel(t, defaultModelKind(getTraits(t)));
  }

  /**
   *
   */
  const DataType* changeModel(const Type* type, ModelKind m) {
    assert(isDataType(*type));
    return ((DataType*)type)->changeModel(*this, m);
  }

  // ---

  /** Create a constant TraitsType for the given Traits */
  const TraitsType* makeTraitsConst(Traits* traits) {
    return (TraitsType*)makeType(TraitsType(traits));
  }

  /** Create a constant OrdinalType for the given ordinal */
  const OrdinalType* makeOrdinalConst(int ordinal) {
    return (OrdinalType*)makeType(OrdinalType(ordinal));
  }

  /** Create a constant NameType for the Multiname given by this (pool, index) */
  const NameType* makeNameConst(PoolObject* pool, int index) {
    return (NameType*)makeType(NameType(pool->precomputedMultiname(index)));
  }

  /** Create a constant MethodType for the given MethodInfo* */
  const MethodType* makeMethodConst(MethodInfo* method) {
    return (MethodType*)makeType(MethodType(method));
  }

  /** Create a constant MethodType for the Method given by this (pool, id) */
  const Type* makeMethodConst(PoolObject* pool, uint32_t method_id) {
    return makeMethodConst(pool->getMethodInfo(method_id));
  }

  /**
   * Create a Env type constrained to the given MethodInfo.
   * Semantically, this is the type of all MethodEnvs that
   * carry this MethodInfo.
   */
  const EnvType* makeEnvType(MethodInfo* method) {
    return (EnvType*)makeType(EnvType(method));
  }

  // ---

  /**
   * Create a constant of the given type and value.  Used for
   * undefined and null.
   */
  const DataType* makeAtomConst(Traits* traits, Atom val);

  /** Create a boolean constant: one of { true, false }. */
  const BooleanType* makeBoolConst(bool val, ModelKind m = kModelInt);

  /** Create a string constant. */
  const StringType* makeStringConst(PoolObject*, uint32_t string_index);

  /** Create a string constant. */
  const StringType* makeStringConst(String* str, ModelKind m = kModelString);

  /** Create a namespace constant. */
  const NamespaceType* makeNamespaceConst(Namespace* val, ModelKind m = kModelNamespace);

  /** Create a scriptobject constant. */
  const ScriptObjectType* makeObjectConst(Traits* traits, ScriptObject* val, ModelKind m = kModelScriptObject);

  /** Create an int32 constant in -2^31..2^31-1 */
  const DataType* makeIntConst(int32_t val, bool boxed = false);

  /** Create a uint32 in 0..2^32-1 */
  const DataType* makeUIntConst(uint32_t val, bool boxed = false);

  /** Create a double constant. */
  const DataType* makeDoubleConst(double val, bool boxed = false);

  const DataType* makeNumberConst(double val, bool boxed = false);

  // ---

  /** Return a type for an outer scope */
  const Type* getOuterScopeType(MethodInfo*, int scope_index);

  const Type* getSlotType(const Type* obj, int slot);
  uint32_t getSlotOffset(const Type* obj, int slot);
  Traits* getSlotTraits(const Type* obj, int slot);

  MethodInfo* getSlotMethod(const Type* obj, int slot);
  MethodInfo* getInitializer(const Type* obj);
  bool isResolved(const Type* t);

  const Type* finddefType(PoolObject*, const Type*);

  Binding toBinding(const Type* obj, const Type* name) const;
  Binding toBinding(Traits* obj, const Type* name) const;

  /**
   * Compute the type that represents the union of the value sets
   * of t1 and t2.  Some invariants, assuming t3 = join(t1,t2):
   *  t3 == join(t2,t1)
   *  subtypeof(t1,t3) = true
   *  subtypeof(t2,t3) = true
   */
  const Type* makeUnion(const Type* t1, const Type* t2);

  /**
   * make a copy of the given type
   * TODO this should be a helper function for Types,
   * held here currently by printType. move after fix
   */
  Type* copyType(const Type& t);

  /**
   * Compute the instance type, if the input is a class type, or return NULL
   * if we can't figure it out.
   */
  const DataType* getInstanceType(const Type* class_type);

private:
  void makeNullableTypes(Traits*, const DataType* types[2]);

public:
  /** TODO maybe put this behind an accessor */
  Allocator& alloc;

private:
  AvmCore* core_;
  TypeTable types; // Intern table for types.

public:
  const DataType* boolean_type;
  const DataType* int_type;       // Number stored as int32_t
  const DataType* uint_type;      // Number stored as uint32_t
  const DataType* double_type;    // Number stored as double
  const DataType* object_type[2]; // [nullable, notnull]
  const DataType* string_type[2];
  const DataType* namespace_type[2];
  const DataType* error_type[2];
  const DataType* array_type[2];
  const DataType* vectorint_type[2];
  const DataType* vectoruint_type[2];
  const DataType* vectordouble_type[2];
  const DataType* vectorobject_type[2];
  const DataType* class_type[2];
  const DataType* function_type[2];
  const DataType* void_type;
  const DataType* null_type;
  const DataType* atom_type[2]; // Traits=0, representation must be SST_atom.
  const DataType* scriptobject_type[2]; // anything with ScriptObject* model.

  const Type* method_type;  // TODO remove

  // TODO just keep builtin
  Traits* int_traits;
  Traits* uint_traits;
  Traits* number_traits;
  Traits* boolean_traits;
  Traits* namespace_traits;
  Traits* string_traits;
  Traits* object_traits;
  Traits* class_traits;
  Traits* void_traits;
};

// more Notes from Avik,Basil,Mathew.
// UN is currently doing double-duty.
//   1. it means a type representing the empty set of values.
//   2. its a placeholder for not-yet-evaluated types during
//      the propagateTypes() algorithm.

// -----------------------------------------------------------------------
//
// TODO doesn't go here
//

/**
 * Iterator to walk through the parameters of a method signature.  The iterator
 * keeps track of the byte displacement of each parameter, according to the
 * JIT calling convention for parameter layout.
 *
 * It is legal to iterate past the end of the declared number of parameters.
 * Extra parameters are always type * and model Atom.
 */
class ParamIter {
public:
  ParamIter(MethodSignaturep ms, uint32_t* ap = 0) :
      ms(ms), i(0), disp_(0), ap(ap) {
  }

  bool empty() const {
    return i > ms->param_count();
  }

  void popFront() {
    disp_ += argSize(i > ms->param_count() ? NULL : ms->paramTraits(i));
    ++i;
  }

  int pos() {
    return i;
  }

  int disp() {
    return disp_;
  }

  ModelKind model() const {
    return defaultModelKind(i > ms->param_count() ? (Traits*)0 : ms->paramTraits(i));
  }

  /** Return the BuiltinType of the front parameter. */
  BuiltinType builtinType() const {
    return i > ms->param_count() ? BUILTIN_any : ms->paramTraitsBT(i);
  }

  // Accessors to actual parameters in memory
  String* &asString() const {
    return *((String**) argPtr());
  }

  Namespace* &asNamespace() const {
    return *((Namespace**) argPtr());
  }

  Atom &asAtom() const {
    return *((Atom*) argPtr());
  }

  ScriptObject* &asObject() const {
    return *((ScriptObject**) argPtr());
  }

  uint32_t &asUint() const {
    return *((uint32_t*) argPtr());
  }

  int32_t &asInt() const {
    return *((int32_t*) argPtr());
  }

  intptr_t &asIntptr() const {
    return *((intptr_t*) argPtr());
  }

  double &asDouble() const {
    return *((double*) argPtr());
  }

private:
  char* argPtr() const {
    return (char*) ap + disp_;
  }

private:
  MethodSignaturep const ms;
  int i, disp_;
  uint32_t* ap;
};

} // namespace avmplus
