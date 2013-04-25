/* -*- Mode: C++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 2 -*- */
/* vi: set ts=2 sw=2 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "hm-main.h"
#ifdef VMCFG_HALFMOON

namespace avmplus {
  // todo declare these globally
  void npe(MethodEnv*);
  void upe(MethodEnv*);
}

//
// hand-written stubs for primitive opcodes
//
namespace halfmoon {
using namespace avmplus;

inline static AvmCore* core(MethodFrame* f) {
  return f->env()->core();
}

inline static MethodEnv* env(MethodFrame* f) {
  return f->env();
}

inline static const DomainEnv* domainEnv(MethodFrame* f) {
  return env(f)->domainEnv();
}

inline static Toplevel* toplevel(MethodFrame* f) {
  return f->env()->toplevel();
}

inline static BoolKind boolKind(bool b) {
  return b ? kBoolTrue : kBoolFalse;
}

ScriptObject* Stubs::do_cast(MethodFrame* f, Traits* t, Atom value) {
  assert(coerceKind(t) == HR_cast);
  coerceobj_atom(env(f), value, t);
  return (ScriptObject*)atomPtr(value);
}

Namespace* Stubs::do_castns(MethodFrame* f, Atom value) {
  coercens_atom(env(f), value);
  return (Namespace*)atomPtr(value);
}

String* Stubs::do_abc_typeof(MethodFrame* f, Atom value) {
  return core(f)->_typeof(value);
}

Atom Stubs::do_string2atom(MethodFrame*, String* s) {
  return s->atom();
}

ScriptObject* Stubs::do_abc_finddef(MethodFrame*, const Multiname* name, MethodEnv* env) {
  return env->finddef(name);
}

MethodEnv* Stubs::do_loadenv(MethodFrame*, int disp_id, ScriptObject* object) {
  return object->vtable->methods[disp_id];
}

MethodEnv* Stubs::do_loadenv_atom(MethodFrame* f, int disp_id, Atom object) {
  return toVTable(toplevel(f), object)->methods[disp_id];
}

MethodEnv* Stubs::do_loadenv_interface(MethodFrame*, MethodInfo* interface_info,
                                       ScriptObject* object) {
  return JitFriend::imtEntry(object->vtable, interface_info);
}

MethodEnv* Stubs::do_loadenv_string(MethodFrame* f, int disp_id, String* object) {
  (void) object;
  return toplevel(f)->stringClass()->ivtable()->methods[disp_id];
}

MethodEnv* Stubs::do_loadenv_number(MethodFrame* f, int disp_id, double object) {
  (void) object;
  return toplevel(f)->numberClass()->ivtable()->methods[disp_id];
}

MethodEnv* Stubs::do_loadenv_namespace(MethodFrame* f, int disp_id, Namespace* object) {
  (void) object;
  return toplevel(f)->namespaceClass()->ivtable()->methods[disp_id];
}

MethodEnv* Stubs::do_loadenv_boolean(MethodFrame* f, int disp_id, BoolKind object) {
  (void) object;
  return toplevel(f)->booleanClass()->ivtable()->methods[disp_id];
}

MethodEnv* Stubs::do_loadinitenv(MethodFrame*, ScriptObject* object) {
  return object->vtable->init;
}

MethodEnv* Stubs::do_loadsuperinitenv(MethodFrame*, MethodEnv* env) {
  return JitFriend::superInitEnv(env);
}

MethodEnv* Stubs::do_loadenv_env(avmplus::MethodFrame*, int method_id, avmplus::MethodEnv* env) {
  return env->abcEnv()->getMethod(method_id);
}

ScriptObject* Stubs::do_newinstance(MethodFrame*, ClassClosure* c) {
  return c->vtable->createInstanceProc(c);
}

Atom Stubs::do_toprimitive(MethodFrame*, Atom x) {
  return AvmCore::primitive(x);
}

double Stubs::do_tonumber(MethodFrame*, Atom x) {
  return AvmCore::number(x);
}

int32_t Stubs::do_toint(MethodFrame*, Atom x) {
  return AvmCore::integer(x);
}

uint32_t Stubs::do_touint(MethodFrame*, Atom x) {
  return AvmCore::toUInt32(x);
}

BoolKind Stubs::do_toboolean(MethodFrame*, Atom x) {
  return boolKind(AvmCore::boolean(x) != 0);
}

BoolKind Stubs::do_d2b(MethodFrame*, double d) {
  return boolKind(MathUtils::doubleToBool(d) == 1);
}

BoolKind Stubs::do_cktimeout(MethodFrame* f, MethodEnv* env) {
  // fixme: should env be explicit because of inlining?
  if (core(f)->interruptCheck(true))
    core(f)->handleInterruptMethodEnv(env);
  return kBoolFalse;
}

Atom Stubs::do_abc_add(MethodFrame* f, Atom x, Atom y) {
  return avmplus::op_add(core(f), x, y);
}

double Stubs::do_addd(MethodFrame*, double x, double y) {
  return x + y;
}

double Stubs::do_divd(MethodFrame*, double x, double y) {
  return x / y;
}

double Stubs::do_modulo(MethodFrame*, double x, double y) {
  return MathUtils::mod(x, y);
}

double Stubs::do_subd(MethodFrame*, double x, double y) {
  return x - y;
}

double Stubs::do_muld(MethodFrame*, double x, double y) {
  return x * y;
}

double Stubs::do_negd(MethodFrame*, double x) {
  return -x;
}

int32_t Stubs::do_addi(MethodFrame*, int32_t x, int32_t y) {
  return x + y;
}

int32_t Stubs::do_subi(MethodFrame*, int32_t x, int32_t y) {
  return x - y;
}

int32_t Stubs::do_muli(MethodFrame*, int32_t x, int32_t y) {
  return x * y;
}

int32_t Stubs::do_ori(MethodFrame*, int32_t x, int32_t y) {
  return x | y;
}

int32_t Stubs::do_andi(MethodFrame*, int32_t x, int32_t y) {
  return x & y;
}

int32_t Stubs::do_xori(MethodFrame*, int32_t x, int32_t y) {
  return x ^ y;
}

int32_t Stubs::do_lshi(MethodFrame*, int32_t x, int32_t y) {
  return x << (y & 31);
}

int32_t Stubs::do_rshi(MethodFrame*, int32_t x, int32_t y) {
  return x >> (y & 31);
}

uint32_t Stubs::do_rshui(MethodFrame*, int32_t x, int32_t y) {
  return uint32_t(x) >> (y & 31);
}

int32_t Stubs::do_noti(MethodFrame*, int32_t x) {
  return ~x;
}

int32_t Stubs::do_negi(MethodFrame*, int32_t x) {
  return -x;
}

/// based on core/Interpreter.cpp initMultiname(), adapted for
/// runtime-index, known namespace.
void initnamex(AvmCore* core, const Multiname* name, Atom index,
               Multiname* tempname) {
  assert(name->isRtname() && !name->isRtns());
  *tempname = *name;
  if (AvmCore::isObject(index)) {
    ScriptObject* i = AvmCore::atomToScriptObject(index);
    if (builtinType(i->traits()) == BUILTIN_qName) {
      QNameObject* qname = (QNameObject*) i;
      qname->getMultiname(*tempname);
      if (name->isAttr())
        tempname->setAttr();
      return;
    }
  }
  tempname->setName(core->intern(index));
}

/// based on core/Interpreter.cpp initMultiname(), adapted for
/// runtime-ns, known name.
void initnamens(MethodEnv* env, const Multiname* name, Atom ns,
                Multiname* tempname) {
  assert(name->isRtns() && !name->isRtname());
  *tempname = *name;
  tempname->setNamespace(env->internRtns(ns));
}

/// based on core/Interpreter.cpp initMultiname(), adapted for
/// runtime-index, runtime-namespace.
void initnamensx(MethodEnv* env, const Multiname* name, Atom ns, Atom index,
                 Multiname* tempname) {
  assert(name->isRtname() && name->isRtns());
  *tempname = *name;
  if (AvmCore::isObject(index)) {
    ScriptObject* i = AvmCore::atomToScriptObject(index);
    if (builtinType(i->traits()) == BUILTIN_qName) {
      QNameObject* qname = (QNameObject*) i;
      qname->getMultiname(*tempname);
      if (name->isAttr())
        tempname->setAttr();
      return; // ignore given ns
    }
  }
  tempname->setName(env->core()->intern(index));
  tempname->setNamespace(env->internRtns(ns));
}

void Stubs::do_setpropertylate_u(MethodFrame* f, uint32_t index, Atom object,
                                 Atom value) {
  env(f)->setpropertylate_u(object, index, value);
}

void Stubs::do_setpropertylate_i(MethodFrame* f, int32_t index, Atom object,
                                 Atom value) {
  env(f)->setpropertylate_i(object, index, value);
}

void Stubs::do_setpropertylate_d(MethodFrame* f, double index, Atom object,
                                 Atom value) {
  env(f)->setpropertylate_d(object, index, value);
}

void Stubs::do_array_set_u(MethodFrame*, uint32_t index, ArrayObject* array,
                           Atom value) {
  // See bug acceptance/regress/bug_654807.
  // Cannot directly call _setUintProperty because the overloaded
  // setUintProperty on SemiSealedArrayObject throws an error
  // when swfversion < 13. That method is only called because setUintProperty
  // is overloaded, while _setUintProperty is not. May also mean we cannot call
  // _get/_set properties on everything below. Please check
  array->setUintProperty(index, value);
}

void Stubs::do_array_set_i(MethodFrame*, int32_t index, ArrayObject* array,
                           Atom value) {
  array->_setIntProperty(index, value);
}

void Stubs::do_array_set_d(MethodFrame*, double index, ArrayObject* array,
                           Atom value) {
  array->_setDoubleProperty(index, value);
}

void Stubs::do_vectorint_set_u(MethodFrame*, uint32_t index,
                               IntVectorObject* vector, int32_t value) {
  vector->_setNativeUintProperty(index, value);
}

void Stubs::do_vectorint_set_i(MethodFrame*, int32_t index,
                               IntVectorObject* vector, int32_t value) {
  vector->_setNativeIntProperty(index, value);
}

void Stubs::do_vectorint_set_d(MethodFrame*, double index,
                               IntVectorObject* vector, int32_t value) {
  vector->_setNativeDoubleProperty(index, value);
}

void Stubs::do_vectoruint_set_u(MethodFrame*, uint32_t index,
                                UIntVectorObject* vector, uint32_t value) {
  vector->_setNativeUintProperty(index, value);
}

void Stubs::do_vectoruint_set_i(MethodFrame*, int32_t index,
                                UIntVectorObject* vector, uint32_t value) {
  vector->_setNativeIntProperty(index, value);
}

void Stubs::do_vectoruint_set_d(MethodFrame*, double index,
                                UIntVectorObject* vector, uint32_t value) {
  vector->_setNativeDoubleProperty(index, value);
}

void Stubs::do_vectordouble_set_u(MethodFrame*, uint32_t index,
                                  DoubleVectorObject* vector, double value) {
  vector->_setNativeUintProperty(index, value);
}

void Stubs::do_vectordouble_set_i(MethodFrame*, int32_t index,
                                  DoubleVectorObject* vector, double value) {
  vector->_setNativeIntProperty(index, value);
}

void Stubs::do_vectordouble_set_d(MethodFrame*, double index,
                                  DoubleVectorObject* vector, double value) {
  vector->_setNativeDoubleProperty(index, value);
}

void Stubs::do_abc_setprop(MethodFrame* f, const Multiname* name, Atom object,
                           Atom value) {
  Toplevel* t = toplevel(f);
  t->setproperty(object, name, value, toVTable(t, object));
}

void Stubs::do_abc_setpropx(MethodFrame* f, const Multiname* name, Atom index,
                            Atom object, Atom value) {
  if (!AvmCore::isDictionaryLookup(index, object)) {
    Multiname tempname;
    initnamex(core(f), name, index, &tempname);
    Toplevel* t = toplevel(f);
    t->setproperty(object, &tempname, value, toVTable(t, object));
  } else {
    // dictionary[index] = value
    AvmCore::atomToScriptObject(object)->setAtomProperty(index, value);
  }
}

void Stubs::do_abc_setpropns(MethodFrame* f, const Multiname* name, Atom ns,
                             Atom object, Atom value) {
  Multiname tempname;
  initnamens(env(f), name, ns, &tempname);
  Toplevel* t = toplevel(f);
  t->setproperty(object, &tempname, value, toVTable(t, object));
}

void Stubs::do_abc_setpropnsx(MethodFrame* f, const Multiname* name, Atom ns,
                              Atom index, Atom object, Atom value) {
  Multiname tempname;
  initnamensx(env(f), name, ns, index, &tempname);
  Toplevel* t = toplevel(f);
  t->setproperty(object, &tempname, value, toVTable(t, object));
}

void Stubs::do_abc_initprop(MethodFrame* f, const Multiname* name, Atom object,
                            Atom value) {
  MethodEnv* e = env(f);
  e->initproperty(object, name, value, toVTable(e, object));
}

void Stubs::do_abc_initpropx(MethodFrame* f, const Multiname* name, Atom index,
                             Atom object, Atom value) {
  Multiname tempname;
  initnamex(core(f), name, index, &tempname);
  MethodEnv* e = env(f);
  e->initproperty(object, &tempname, value, toVTable(e, object));
}

void Stubs::do_abc_initpropns(MethodFrame* f, const Multiname* name, Atom ns,
                              Atom object, Atom value) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamens(e, name, ns, &tempname);
  e->initproperty(object, &tempname, value, toVTable(e, object));
}

void Stubs::do_abc_initpropnsx(MethodFrame* f, const Multiname* name, Atom ns,
                               Atom index, Atom object, Atom value) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamensx(e, name, ns, index, &tempname);
  e->initproperty(object, &tempname, value, toVTable(e, object));
}

BoolKind Stubs::do_abc_equals(MethodFrame* f, Atom x, Atom y) {
  return boolKind(core(f)->equals(x, y) == trueAtom);
}

BoolKind Stubs::do_abc_istype(MethodFrame*, Traits* t, Atom x) {
  return boolKind(AvmCore::istype(x, t));
}

Atom Stubs::do_abc_astype(MethodFrame*, Traits* t, Atom x) {
  return AvmCore::astype(x, t);
}

Atom Stubs::do_abc_astypelate(MethodFrame* f, Atom value, Atom class_value) {
  return AvmCore::astype(value, toplevel(f)->toClassITraits(class_value));
}

Atom Stubs::do_cknull(MethodFrame* f, Atom x) {
  if (AvmCore::isNull(x))
    avmplus::npe(env(f));
  if (x == undefinedAtom)
    avmplus::upe(env(f));
  return x;
}

ScriptObject* Stubs::do_cknullobject(MethodFrame* f, ScriptObject* object) {
  if (!object)
    avmplus::npe(env(f));
  return object;
}

Atom Stubs::do_abc_getprop(MethodFrame* f, const Multiname* name, Atom object) {
  return toplevel(f)->getproperty(object, name, toVTable(env(f), object));
}

Atom Stubs::do_abc_getpropx(MethodFrame* f, const Multiname* name, Atom index,
                            Atom object) {
  assert(!AvmCore::isNullOrUndefined(object));
  if (!AvmCore::isDictionaryLookup(index, object)) {
    Multiname tempname;
    initnamex(core(f), name, index, &tempname);
    Toplevel* t = toplevel(f);
    return t->getproperty(object, &tempname, toVTable(t, object));
  } else {
    // = dictionary[index]
    return AvmCore::atomToScriptObject(object)->getAtomProperty(index);
  }
}

Atom Stubs::do_getpropertylate_u(MethodFrame* f, uint32_t index, Atom object) {
  return env(f)->getpropertylate_u(object, index);
}

Atom Stubs::do_getpropertylate_i(MethodFrame* f, int32_t index, Atom object) {
  return env(f)->getpropertylate_i(object, index);
}

Atom Stubs::do_getpropertylate_d(MethodFrame* f, double index, Atom object) {
  return env(f)->getpropertylate_d(object, index);
}

Atom Stubs::do_array_get_u(MethodFrame*, uint32_t index, ArrayObject* array) {
  return array->_getUintProperty(index);
}

Atom Stubs::do_array_get_i(MethodFrame*, int32_t index, ArrayObject* array) {
  return array->_getIntProperty(index);
}

Atom Stubs::do_array_get_d(MethodFrame*, double index, ArrayObject* array) {
  return array->_getDoubleProperty(index);
}

int32_t Stubs::do_vectorint_get_u(MethodFrame*, uint32_t index,
                               IntVectorObject* vector) {
  return vector->_getNativeUintProperty(index);
}

int32_t Stubs::do_vectorint_get_i(MethodFrame*, int32_t index,
                              IntVectorObject* vector) {
  return vector->_getNativeIntProperty(index);
}

int32_t Stubs::do_vectorint_get_d(MethodFrame*, double index,
                                  IntVectorObject* vector) {
  return vector->_getNativeDoubleProperty(index);
}

uint32_t Stubs::do_vectoruint_get_u(MethodFrame*, uint32_t index,
                                    UIntVectorObject* vector) {
  return vector->_getNativeUintProperty(index);
}

uint32_t Stubs::do_vectoruint_get_i(MethodFrame*, int32_t index,
                                    UIntVectorObject* vector) {
  return vector->_getNativeIntProperty(index);
}

uint32_t Stubs::do_vectoruint_get_d(MethodFrame*, double index,
                                    UIntVectorObject* vector) {
  return vector->_getNativeDoubleProperty(index);
}

double Stubs::do_vectordouble_get_u(MethodFrame*, uint32_t index,
                                    DoubleVectorObject* vector) {
  return vector->_getNativeUintProperty(index);
}

double Stubs::do_vectordouble_get_i(MethodFrame*, int32_t index,
                                    DoubleVectorObject* vector) {
  return vector->_getNativeIntProperty(index);
}

double Stubs::do_vectordouble_get_d(MethodFrame*, double index,
                                    DoubleVectorObject* vector) {
  return vector->_getNativeDoubleProperty(index);
}

BoolKind Stubs::do_lessthan(MethodFrame*, Atom x, Atom y) {
  return boolKind(AvmCore::compare(x, y) == trueAtom);
}

BoolKind Stubs::do_lessequals(MethodFrame*, Atom x, Atom y) {
  return boolKind(AvmCore::compare(y, x) == falseAtom);
}

BoolKind Stubs::do_greaterthan(MethodFrame*, Atom x, Atom y) {
  return boolKind(AvmCore::compare(y, x) == trueAtom);
}

BoolKind Stubs::do_greaterequals(MethodFrame*, Atom x, Atom y) {
  return boolKind(AvmCore::compare(x, y) == falseAtom);
}

BoolKind Stubs::do_not(MethodFrame*, BoolKind x) {
  return boolKind(!x);
}

BoolKind Stubs::do_eqi(MethodFrame*, int32_t x, int32_t y) {
  return boolKind(x == y);
}

BoolKind Stubs::do_lti(MethodFrame*, int32_t x, int32_t y) {
  return boolKind(x < y);
}

BoolKind Stubs::do_lei(MethodFrame*, int32_t x, int32_t y) {
  return boolKind(x <= y);
}

BoolKind Stubs::do_gti(MethodFrame*, int32_t x, int32_t y) {
  return boolKind(x > y);
}

BoolKind Stubs::do_gei(MethodFrame*, int32_t x, int32_t y) {
  return boolKind(x >= y);
}

BoolKind Stubs::do_eqd(MethodFrame*, double x, double y) {
  return boolKind(x == y);
}

BoolKind Stubs::do_ltd(MethodFrame*, double x, double y) {
  return boolKind(x < y);
}

BoolKind Stubs::do_led(MethodFrame*, double x, double y) {
  return boolKind(x <= y);
}

BoolKind Stubs::do_gtd(MethodFrame*, double x, double y) {
  return BoolKind(x > y);
}

BoolKind Stubs::do_ged(MethodFrame*, double x, double y) {
  return BoolKind(x >= y);
}

BoolKind Stubs::do_equi(MethodFrame*, uint32_t x, uint32_t y) {
  return boolKind(x == y);
}

BoolKind Stubs::do_ltui(MethodFrame*, uint32_t x, uint32_t y) {
  return boolKind(x < y);
}

BoolKind Stubs::do_leui(MethodFrame*, uint32_t x, uint32_t y) {
  return boolKind(x <= y);
}

BoolKind Stubs::do_gtui(MethodFrame*, uint32_t x, uint32_t y) {
  return boolKind(x > y);
}

BoolKind Stubs::do_geui(MethodFrame*, uint32_t x, uint32_t y) {
  return boolKind(x >= y);
}

BoolKind Stubs::do_eqb(MethodFrame*, BoolKind x, BoolKind y) {
  return boolKind(x == y);
}

BoolKind Stubs::do_eqp(MethodFrame*, ScriptObject* x, ScriptObject* y) {
  return boolKind(x == y);
}

/** string comparison by value equality */
BoolKind Stubs::do_eqs(MethodFrame*, String* x, String* y) {
  return BoolKind(String::equalsWithNullChecks(x, y));
}

double Stubs::do_i2d(MethodFrame*, int32_t x) {
  return x;
}

double Stubs::do_u2d(MethodFrame*, uint32_t x) {
  return x;
}

int32_t Stubs::do_d2i(MethodFrame*, double d) {
  return (int32_t) d;
}

uint32_t Stubs::do_d2u(MethodFrame*, double d) {
  return (uint32_t) d;
}

uint32_t Stubs::do_i2u(MethodFrame*, int32_t x) {
  return x;
}

int32_t Stubs::do_u2i(MethodFrame*, uint32_t x) {
  return x;
}

int32_t Stubs::do_doubletoint32(MethodFrame*, double x) {
  return AvmCore::doubleToInt32(x);
}

Atom Stubs::do_double2atom(MethodFrame* f, double x) {
  return core(f)->doubleToAtom(x);
}

Atom Stubs::do_int2atom(MethodFrame* f, int32_t x) {
  return core(f)->intToAtom(x);
}

Atom Stubs::do_uint2atom(MethodFrame* f, uint32_t x) {
  return core(f)->uintToAtom(x);
}

Atom Stubs::do_scriptobject2atom(MethodFrame*, ScriptObject* object) {
  return object->atom();
}

Atom Stubs::do_bool2atom(MethodFrame*, BoolKind x) {
  return x ? trueAtom : falseAtom;
}

Atom Stubs::do_ns2atom(MethodFrame*, Namespace* ns) {
  return ns->atom();
}

ScriptObject* Stubs::do_atom2scriptobject(MethodFrame*, Atom x) {
  assert(AvmCore::isObject(x) || AvmCore::isNull(x));
  return (ScriptObject*)atomPtr(x);
}

String* Stubs::do_atom2string(MethodFrame*, Atom x) {
  assert(AvmCore::isString(x) || AvmCore::isNull(x));
  return (String*)atomPtr(x);
}

BoolKind Stubs::do_atom2bool(MethodFrame*, Atom x) {
  assert(AvmCore::isBoolean(x));
  return BoolKind(x == trueAtom);
}

Namespace* Stubs::do_atom2ns(MethodFrame*, Atom x) {
  assert(AvmCore::isNamespace(x) || AvmCore::isNull(x));
  return (Namespace*)atomPtr(x);
}

int Stubs::do_atom2int(MethodFrame*, Atom x) {
  return AvmCore::integer_i(x);
}

uint32_t Stubs::do_atom2uint(MethodFrame*, Atom x) {
  return AvmCore::integer_u(x);
}

double Stubs::do_atom2double(MethodFrame*, Atom x) {
  return AvmCore::number_d(x);
}

Atom Stubs::do_construct(MethodFrame* f, Atom ctor, int argc, Atom* args) {
  return avmplus::op_construct(env(f), ctor, argc - 1, args);
}

Atom Stubs::do_abc_callprop(MethodFrame* f, const Multiname* name, int argc,
                            Atom* args) {
  Toplevel* t = toplevel(f);
  return t->callproperty(args[0], name, argc - 1, args, toVTable(t, args[0]));
}

Atom Stubs::do_abc_callpropx(MethodFrame* f, const Multiname* name, Atom index,
                             int argc, Atom* args) {
  Multiname tempname;
  initnamex(core(f), name, index, &tempname);
  Toplevel* t = toplevel(f);
  return t->callproperty(args[0], &tempname, argc - 1, args,
                         toVTable(t, args[0]));
}

Atom Stubs::do_abc_callpropns(MethodFrame* f, const Multiname* name, Atom ns,
                              int argc, Atom* args) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamens(e, name, ns, &tempname);
  return toplevel(f)->callproperty(args[0], &tempname, argc - 1, args,
                                   toVTable(e, args[0]));
}

Atom Stubs::do_abc_callpropnsx(MethodFrame* f, const Multiname* name, Atom ns,
                               Atom index, int argc, Atom* args) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamensx(e, name, ns, index, &tempname);
  return toplevel(f)->callproperty(args[0], &tempname, argc - 1, args,
                                   toVTable(e, args[0]));
}

ClassClosure* Stubs::do_newclass(MethodFrame* f, Traits* ctraits,
                                 ClassClosure* base, int /*scope_count */,
                                 Atom* scopes) {
  return env(f)->newclass(ctraits, base, env(f)->scope(), scopes);
}

ClassClosure* Stubs::do_newfunction(MethodFrame* f, MethodInfo* info,
                                      int /*scope_count*/, Atom* scopes) {
  return env(f)->newfunction(info, scopes);
}

void Stubs::do_throw(MethodFrame* f, Atom e) {
  return core(f)->throwAtom(e);
}

Atom Stubs::do_getouterscope(MethodFrame*, int k, MethodEnv* env) {
  return env->scope()->getScope(k);
}

/// DEOPT
/// FIXME: This stub should not be required, though it's not clear how to
/// convince templates.py of this.  Note that no stubs are generated for
/// HM_deopt_safepoint and HM_deopt_finishcall.
void Stubs::do_deopt_finish(MethodFrame*) {}

// debugline: (Effect, Int) -> Effect
void Stubs::do_debugline(MethodFrame*, int32_t line) {
  (void)line;
}
  
// debugfile: (Effect, String) -> Effect
void Stubs::do_debugfile(MethodFrame*, String*) {}

inline Atom abc_find(MethodEnv* env, const Multiname* name, Atom* scopes,
                     int scope_count, bool strict, Atom* withbase) {
  // fixme: handle withbase
  return env->findproperty(env->scope(), scopes, scope_count, name, strict, withbase);
}

inline Atom abc_findx(MethodEnv* env, const Multiname* name, Atom index,
                      Atom* scopes, int scope_count, bool strict, Atom* withbase) {
  // fixme: handle withbase
  Multiname tempname;
  initnamex(env->core(), name, index, &tempname);
  return env->findproperty(env->scope(), scopes, scope_count, &tempname,
                           strict, withbase);
}

inline Atom abc_findns(MethodEnv* env, const Multiname* name, Atom ns,
                       Atom* scopes, int scope_count, bool strict, Atom* withbase) {
  // fixme: handle withbase
  Multiname tempname;
  initnamens(env, name, ns, &tempname);
  return env->findproperty(env->scope(), scopes, scope_count, &tempname,
                           strict, withbase);
}

inline Atom abc_findnsx(MethodEnv* env, const Multiname* name, Atom ns,
                       Atom index, Atom* scopes, int scope_count, bool strict, Atom* withbase) {
  // fixme: handle withbase
  Multiname tempname;
  initnamensx(env, name, ns, index, &tempname);
  return env->findproperty(env->scope(), scopes, scope_count, &tempname,
                           strict, withbase);
}

Atom Stubs::do_abc_findproperty(MethodFrame*, const Multiname* name,
                                MethodEnv* env, int32_t withbase, int scope_count, Atom* scopes) {
  assert(withbase >= -1 && withbase < scope_count);
  Atom* withbasep = (withbase == -1) ? NULL : scopes + withbase;
  return abc_find(env, name, scopes, scope_count, false, withbasep);
}

Atom Stubs::do_abc_findpropstrict(MethodFrame*, const Multiname* name,
                                MethodEnv* env, int32_t withbase, int scope_count, Atom* scopes) {
  assert(withbase >= -1 && withbase < scope_count);
  Atom* withbasep = (withbase == -1) ? NULL : scopes + withbase;
  return abc_find(env, name, scopes, scope_count, true, withbasep);
}

Atom Stubs::do_abc_findpropertyx(MethodFrame*, const Multiname* name,
                                 MethodEnv* env, int32_t withbase, Atom index, int scope_count,
                                 Atom* scopes) {
  assert(withbase >= -1 && withbase < scope_count);
  Atom* withbasep = (withbase == -1) ? NULL : scopes + withbase;
  return abc_findx(env, name, index, scopes, scope_count, false, withbasep);
}

Atom Stubs::do_abc_findpropstrictx(MethodFrame*, const Multiname* name,
                                   MethodEnv* env, int32_t withbase, Atom index, int scope_count,
                                   Atom* scopes) {
  assert(withbase >= -1 && withbase < scope_count);
  Atom* withbasep = (withbase == -1) ? NULL : scopes + withbase;
  return abc_findx(env, name, index, scopes, scope_count, true, withbasep);
}

Atom Stubs::do_abc_findpropertyns(MethodFrame*, const Multiname* name,
                                  MethodEnv* env, int32_t withbase, Atom ns, int scope_count,
                                  Atom* scopes) {
  assert(withbase >= -1 && withbase < scope_count);
  Atom* withbasep = (withbase == -1) ? NULL : scopes + withbase;
  return abc_findns(env, name, ns, scopes, scope_count, false, withbasep);
}

Atom Stubs::do_abc_findpropstrictns(MethodFrame*, const Multiname* name,
                                    MethodEnv* env, int32_t withbase, Atom ns, int scope_count,
                                    Atom* scopes) {
  assert(withbase >= -1 && withbase < scope_count);
  Atom* withbasep = (withbase == -1) ? NULL : scopes + withbase;
  return abc_findns(env, name, ns, scopes, scope_count, true, withbasep);
}

Atom Stubs::do_abc_findpropertynsx(MethodFrame*, const Multiname* name,
                                   MethodEnv* env, int32_t withbase, Atom ns,
                                   int scope_count, Atom* scopes) {
  assert(withbase >= -1 && withbase < scope_count);
  Atom* withbasep = (withbase == -1) ? NULL : scopes + withbase;
  Atom index = *scopes;
  scopes++;
  scope_count--;
  return abc_findnsx(env, name, ns, index, scopes, scope_count, false, withbasep);
}

Atom Stubs::do_abc_findpropstrictnsx(MethodFrame*, const Multiname* name,
                                     MethodEnv* env, int32_t withbase, Atom ns,
                                     int scope_count, Atom* scopes) {
  assert(withbase >= -1 && withbase < scope_count);
  Atom* withbasep = (withbase == -1) ? NULL : scopes + withbase;
  Atom index = *scopes;
  scopes++;
  scope_count--;
  return abc_findnsx(env, name, ns, index, scopes, scope_count, true, withbasep);
}

BoolKind Stubs::do_abc_strictequals(MethodFrame*, Atom x, Atom y) {
  return boolKind(AvmCore::stricteq(x, y) == trueAtom);
}

ArrayObject* Stubs::do_newarray(MethodFrame* f, int count, Atom* values) {
  return avmplus::newarray(toplevel(f), count, values);
}

String* Stubs::do_caststring(MethodFrame* f, Atom x) {
  return core(f)->coerce_s(x);
}

String* Stubs::do_abc_convert_s(MethodFrame* f, Atom x) {
  return core(f)->string(x);
}

ScriptObject* Stubs::do_newactivation(MethodFrame*, MethodEnv* env) {
  return env->newActivation();
}

Atom Stubs::do_call(MethodFrame* f, Atom function, int argc, Atom* args) {
  return op_call(env(f), function, argc - 1, args);
}

BoolKind Stubs::do_abc_instanceof(MethodFrame* f, Atom x, Atom t) {
  return boolKind(toplevel(f)->instanceof(x, t) == trueAtom);
}

Atom Stubs::do_abc_callsuper(MethodFrame* f, const Multiname* name, int argc,
                             Atom* args) {
  assert(!name->isRuntime());
  // CallStmt's always counts the "this" arg, but VM calling conventions don't.
  return env(f)->callsuper(name, argc - 1, args);
}

Atom Stubs::do_abc_callsuperx(MethodFrame* f, const Multiname* name, Atom index,
                              int argc, Atom* args) {
  Multiname tempname;
  initnamex(core(f), name, index, &tempname);
  return env(f)->callsuper(&tempname, argc - 1, args);
}

Atom Stubs::do_abc_callsuperns(MethodFrame* f, const Multiname* name, Atom ns,
                               int argc, Atom* args) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamens(e, name, ns, &tempname);
  return e->callsuper(&tempname, argc - 1, args);
}

Atom Stubs::do_abc_callsupernsx(MethodFrame* f, const Multiname* name, Atom ns,
                                Atom index, int argc, Atom* args) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamensx(e, name, ns, index, &tempname);
  return e->callsuper(&tempname, argc - 1, args);
}

BoolKind Stubs::do_abc_istypelate(MethodFrame* f, Atom object, Atom atomType) {
  return BoolKind(AvmCore::istype(object, toplevel(f)->toClassITraits(atomType)));
}

Atom Stubs::do_castobject(MethodFrame*, Atom object) { 
  if (object == undefinedAtom) {
    return nullObjectAtom;
  } else {
    return object;
  }
}

ScriptObject* Stubs::do_newobject(MethodFrame* f, int argc, Atom* argv) { 
  // op_newobject expects argv to be at the end of the object property pairs
  Atom* endOfObjectPropertyPairs = argv + argc - 1;
  int numberOfProperties = argc / 2; // argc for newobject is actually the number of property pairs
  return env(f)->op_newobject(endOfObjectPropertyPairs, numberOfProperties);
}

BoolKind Stubs::do_abc_in(MethodFrame* f, Atom name, Atom object) {
  return boolKind(toplevel(f)->in_operator(name, object) == trueAtom);
}

BoolKind Stubs::do_abc_deleteprop(MethodFrame* f, const Multiname* name,
                                  Atom object) {
  return boolKind(env(f)->delproperty(object, name) == trueAtom);
}

void checkXMLList(MethodFrame* f, Atom index) {
  if (AvmCore::isXMLList(index)) {
    // Error according to E4X spec, section 11.3.1
    toplevel(f)->throwTypeError(kDeleteTypeError,
                                core(f)->toErrorString(toplevel(f)->toTraits(index)));
  }
}

BoolKind Stubs::do_abc_deletepropx(MethodFrame* f, const Multiname* name,
                                   Atom index, Atom object) {
  if (!AvmCore::isDictionaryLookup(index, object)) {
    checkXMLList(f, index);
    Multiname tempname;
    initnamex(core(f), name, index, &tempname);
    return boolKind(env(f)->delproperty(object, &tempname) == trueAtom);
  } else {
    // delete dictionary[index]
    return boolKind(AvmCore::atomToScriptObject(object)->deleteAtomProperty(index));
  }
}

BoolKind Stubs::do_abc_deletepropns(MethodFrame* f, const Multiname* name,
                                    Atom ns, Atom object) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamens(e, name, ns, &tempname);
  return boolKind(e->delproperty(object, &tempname) == trueAtom);
}

BoolKind Stubs::do_abc_deletepropnsx(MethodFrame* f, const Multiname* name,
                                     Atom ns, Atom index, Atom object) {
  checkXMLList(f, index);
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamensx(e, name, ns, index, &tempname);
  return boolKind(e->delproperty(object, &tempname) == trueAtom);
}

Atom Stubs::do_abc_nextname(MethodFrame* f, Atom object, int counter) {
  return env(f)->nextname(object, counter);
}

Atom Stubs::do_abc_nextvalue(MethodFrame* f, Atom object, int counter) {
  return env(f)->nextvalue(object, counter);
}

Atom Stubs::do_applytype(MethodFrame* f, int argc, Atom* argv) { 
  Atom factory = argv[0];
  Atom* args = argv + 1; // args has to skip the factory object
  argc -= 1; // abcbuilder adds + 1 for the factory object;
  return op_applytype(env(f), factory, argc, args);
}

inline Atom getBase(Atom* args) {
  Atom base = args[0];
  args[0] = nullObjectAtom;
  return base;
}

Atom Stubs::do_abc_callproplex(MethodFrame* f, const Multiname* name, int argc,
                               Atom* args) {
  Atom base = getBase(args);
  Toplevel* t = toplevel(f);
  return t->callproperty(base, name, argc - 1, args, toVTable(t, base));
}

Atom Stubs::do_abc_callproplexx(MethodFrame* f, const Multiname* name,
                                Atom index, int argc, Atom* args) {
  Multiname tempname;
  initnamex(core(f), name, index, &tempname);
  Atom base = getBase(args);
  Toplevel* t = toplevel(f);
  return t->callproperty(base, name, argc - 1, args, toVTable(t, base));
}

Atom Stubs::do_abc_callproplexns(MethodFrame* f, const Multiname* name,
                                 Atom ns, int argc, Atom* args) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamens(e, name, ns, &tempname);
  Atom base = getBase(args);
  return toplevel(f)->callproperty(base, name, argc - 1, args, toVTable(e, base));
}

Atom Stubs::do_abc_callproplexnsx(MethodFrame* f, const Multiname* name,
                                  Atom ns, Atom index, int argc, Atom* args) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamensx(e, name, ns, index, &tempname);
  Atom base = getBase(args);
  return toplevel(f)->callproperty(base, name, argc - 1, args, toVTable(e, base));
}

String* Stubs::do_concat_strings(MethodFrame* f, String* lhs, String* rhs) {
  return core(f)->concatStrings(lhs, rhs);
}

Atom Stubs::do_abc_getsuper(MethodFrame* f, const Multiname* name,
                            Atom object) {
  return env(f)->getsuper(object, name);
}

Atom Stubs::do_abc_getsuperx(MethodFrame* f, const Multiname* name, Atom index,
                             Atom object) {
  Multiname tempname;
  initnamex(core(f), name, index, &tempname);
  return env(f)->getsuper(object, &tempname);
}

Atom Stubs::do_abc_getsuperns(MethodFrame* f, const Multiname* name, Atom ns,
                              Atom object) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamens(e, name, ns, &tempname);
  return e->getsuper(object, &tempname);
}

Atom Stubs::do_abc_getsupernsx(MethodFrame* f, const Multiname* name, Atom ns,
                               Atom index, Atom object) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamensx(e, name, ns, index, &tempname);
  return e->getsuper(object, &tempname);
}

Atom Stubs::do_abc_constructprop(MethodFrame* f, const Multiname* name,
                                 int argc, Atom* args) {
  return constructprop(env(f), name, argc - 1, args);
}

Atom Stubs::do_abc_constructpropx(MethodFrame* f, const Multiname* name,
                                  Atom index, int argc, Atom* args) {
  Multiname tempname;
  initnamex(core(f), name, index, &tempname);
  return constructprop(env(f), &tempname, argc - 1, args);
}

Atom Stubs::do_abc_constructpropns(MethodFrame* f, const Multiname* name,
                                   Atom ns, int argc, Atom* args) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamens(e, name, ns, &tempname);
  return constructprop(e, &tempname, argc - 1, args);
}

Atom Stubs::do_abc_constructpropnsx(MethodFrame* f, const Multiname* name,
                                    Atom ns, Atom index, int argc, Atom* args) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamensx(e, name, ns, index, &tempname);
  return constructprop(e, &tempname, argc - 1, args);
}

void Stubs::do_constructsuper(MethodFrame*, MethodEnv* e, int argc, Atom* argv) { 
  e->super_init()->coerceEnter(argc - 1, argv);
}

void Stubs::do_abc_dxns(MethodFrame* f, String* uri) {
  core(f)->setDxns(f, uri);
}

void Stubs::do_abc_dxnslate(MethodFrame* f, Atom uri) {
  core(f)->setDxnsLate(f, uri);
}

void Stubs::do_abc_setsuper(MethodFrame* f, const Multiname* name, Atom object,
                            Atom value) {
  env(f)->setsuper(object, name, value);
}

void Stubs::do_abc_setsuperx(MethodFrame* f, const Multiname* name, Atom index,
                             Atom object, Atom value) {
  Multiname tempname;
  initnamex(core(f), name, index, &tempname);
  env(f)->setsuper(object, &tempname, value);
}

void Stubs::do_abc_setsuperns(MethodFrame* f, const Multiname* name, Atom ns,
                              Atom object, Atom value) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamens(e, name, ns, &tempname);
  e->setsuper(object, &tempname, value);
}

void Stubs::do_abc_setsupernsx(MethodFrame* f, const Multiname* name, Atom ns,
                               Atom index, Atom object, Atom value) {
  Multiname tempname;
  MethodEnv* e = env(f);
  initnamensx(e, name, ns, index, &tempname);
  e->setsuper(object, &tempname, value);
}

Atom Stubs::do_coerce(MethodFrame* f, Traits* t, Atom val) { 
  return toplevel(f)->coerce(val, t);
}

Atom Stubs::do_abc_getdescendants(MethodFrame* f, const Multiname* name,
                                  Atom object) {
  return env(f)->getdescendants(object, name);
}

Atom Stubs::do_abc_getdescendantsx(MethodFrame* f, const Multiname* name,
                                   Atom index, Atom object) {
  Multiname tempname;
  initnamex(core(f), name, index, &tempname);
  return env(f)->getdescendants(object, &tempname);
}

Atom Stubs::do_abc_getdescendantsns(MethodFrame* f, const Multiname* name,
                                    Atom ns, Atom object) {
  Multiname tempname;
  initnamens(env(f), name, ns, &tempname);
  return env(f)->getdescendants(object, &tempname);
}

Atom Stubs::do_abc_getdescendantsnsx(MethodFrame* f, const Multiname* name,
                                     Atom ns, Atom index, Atom object) {
  Multiname tempname;
  initnamensx(env(f), name, ns, index, &tempname);
  return env(f)->getdescendants(object, &tempname);
}

/***
 * ns below means namespace
 * x means index, usually an int
 */

Atom Stubs::do_abc_getpropns(MethodFrame* f, const Multiname* name, Atom ns,
                             Atom object) {
  Multiname tempname;
  initnamens(env(f), name, ns, &tempname);
  Toplevel* t = toplevel(f);
  return t->getproperty(object, &tempname, toVTable(t, object));
}

Atom Stubs::do_abc_getpropnsx(MethodFrame* f, const Multiname* name, Atom ns,
                              Atom index, Atom object) {
  Multiname tempname;
  initnamensx(env(f), name, ns, index, &tempname);
  Toplevel* t = toplevel(f);
  return t->getproperty(object, &tempname, toVTable(t, object));
}

int Stubs::do_abc_hasnext(MethodFrame* f, Atom object, int32_t index) {
  return env(f)->hasnext(object, index);
}

inline uint8_t* rangeCheck(MethodFrame* f, int32_t addr, size_t bytes) {
  MethodEnv* e = env(f);
  const DomainEnv* d = e->domainEnv();
  if (uint32_t(addr) > (d->globalMemorySize() - bytes))
    avmplus::mop_rangeCheckFailed(e);
  return d->globalMemoryBase() + addr;
}

// MOPS implementations based on those found in Interpreter.cpp.
#if defined(VMCFG_UNALIGNED_INT_ACCESS) && defined(VMCFG_LITTLE_ENDIAN)

int32_t Stubs::do_li8(MethodFrame* f, int32_t addr) {
  return (int32_t) *((uint8_t*) rangeCheck(f, addr, 1));
}

int32_t Stubs::do_li16(MethodFrame* f, int32_t addr) {
  return (int32_t) *((uint16_t*) rangeCheck(f, addr, 2));
}

int32_t Stubs::do_li32(MethodFrame* f, int32_t addr) {
  return *((int32_t*) rangeCheck(f, addr, 4));
}

void Stubs::do_si8(MethodFrame* f, int32_t value, int32_t addr) {
  *((uint8_t*)rangeCheck(f, addr, 1)) = uint8_t(value);
}

void Stubs::do_si16(MethodFrame* f, int32_t value, int32_t addr) {
  *((uint16_t*)rangeCheck(f, addr, 2)) = uint16_t(value);
}

void Stubs::do_si32(MethodFrame* f, int32_t value, int32_t addr) {
  *((int32_t*)rangeCheck(f, addr, 4)) = value;
}

#else

int32_t Stubs::do_li8(MethodFrame* f, int32_t addr) {
  return avmplus::mop_liz8(rangeCheck(f, addr, 1));
}

int32_t Stubs::do_li16(MethodFrame* f, int32_t addr) {
  return avmplus::mop_liz16(rangeCheck(f, addr, 2));
}

int32_t Stubs::do_li32(MethodFrame* f, int32_t addr) {
  return avmplus::mop_li32(rangeCheck(f, addr, 4));
}

void Stubs::do_si8(MethodFrame* f, int32_t value, int32_t addr) {
  avmplus::mop_si8(rangeCheck(f, addr, 1), value);
}

void Stubs::do_si16(MethodFrame* f, int32_t value, int32_t addr) {
  avmplus::mop_si16(rangeCheck(f, addr, 2), value);
}

void Stubs::do_si32(MethodFrame* f, int32_t value, int32_t addr) {
  avmplus::mop_si32(rangeCheck(f, addr, 4), value);
}

#endif

#if defined(VMCFG_UNALIGNED_FP_ACCESS) && defined(VMCFG_LITTLE_ENDIAN)

double Stubs::do_lf32(MethodFrame* f, int32_t addr) {
  return (double) *((float*) rangeCheck(f, addr, 4));
}

double Stubs::do_lf64(MethodFrame* f, int32_t addr) {
  return *((double*) rangeCheck(f, addr, 8));
}

void Stubs::do_sf32(MethodFrame* f, double value, int32_t addr) {
  *((float*) rangeCheck(f, addr, 4)) = float(value);
}

void Stubs::do_sf64(MethodFrame* f, double value, int32_t addr) {
  *((double*) rangeCheck(f, addr, 8)) = value;
}

#else

double Stubs::do_lf32(MethodFrame* f, int32_t addr) {
  return avmplus::mop_lf32(rangeCheck(f, addr, 4));
}

double Stubs::do_lf64(MethodFrame* f, int32_t addr) {
  return avmplus::mop_lf64(rangeCheck(f, addr, 8));
}

void Stubs::do_sf32(MethodFrame* f, double value, int32_t addr) {
  avmplus::mop_sf32(rangeCheck(f, addr, 4), value);
}

void Stubs::do_sf64(MethodFrame* f, double value, int32_t addr) {
  avmplus::mop_sf64(rangeCheck(f, addr, 8), value);
}

#endif

String* Stubs::do_abc_esc_xelem(MethodFrame* f, Atom value) {
  return core(f)->ToXMLString(value);
}

String* Stubs::do_abc_esc_xattr(MethodFrame* f, Atom value) {
  return core(f)->EscapeAttributeValue(value);
}

Atom Stubs::do_ckfilter(MethodFrame* f, Atom value) {
  env(f)->checkfilter(value);
  return value;
}

Atom Stubs::do_newcatch(MethodFrame* f, Traits* traits) {
  return env(f)->newcatch(traits)->atom();
}

Traits* Stubs::do_slottype(MethodFrame*, ScriptObject*, int) { assert(false && "slottype not implemented"); return 0; }
int Stubs::do_toslot(MethodFrame*, ScriptObject*, const Multiname*) { assert(false && "toslot not implemented"); return 0; }
void Stubs::do_never(MethodFrame*) { assert(false && "never not implemented"); }

} // namespace halfmoon
#endif // #ifdef VMCFG_HALFMOON
