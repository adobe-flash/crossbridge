/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus {

//
// ABC Instruction implementations and helper functions.
//
// Many are parameterized on E (environment) where E can be Toplevel*
// or MethodEnv*, because often it pays to defer loading those pointers
// until it's needed, which often is only in the error case.
//
// the JIT uses MethodEnv instantiations.  The interpreter and Toplevel
// classes currently use Toplevel* instantiations.
//

/**
 * find the binding for a property given a full multiname reference.  The lookup
 * must produce a single binding, or it's an error.  Note that the name could be
 * bound to the same binding in multiple namespaces.
 */
template <class E>
Binding getBinding(E env, Traits* traits, const Multiname* ref);

/**
 * specialized getBinding() when you only have vtable.  hides the
 * traits lookup from vtable
 */
template <class E>
Binding getBinding(E env, VTable* vtable, const Multiname* ref);

/**
 * Throw a callOfNonFunction error.  This is factored out of the
 * op_call functions below to reduce their need to spill callee-saved
 * registers on the fast-path.
 */
template <class E> NO_INLINE
Atom op_call_error(E env);

/**
 * implements OP_call, including error handling for non-callable values
 */
template <class E>
Atom op_call(E env, Atom func, int argc, Atom* atomv);

/**
 * specialized OP_call implementation when you have a ScriptObject*
 * already.  Includes null pointer handling
 */
template <class E>
Atom op_call(E env, ScriptObject* func, int argc, Atom* atomv);

/**
 * implements OP_construct, including error handling for non-constructor
 * values of "ctor"
 */
template <class E>
Atom op_construct(E env, Atom ctor, int argc, Atom* atomv);

/**
 * implements OP_callproperty.  Caller is responsible for obtaining vtable
 * and binding, usually by calling toVTable() and getBinding().  Null
 * pointer checking must be done by caller.
 */
template <class E>
Atom callprop_b(E env, Atom base, const Multiname* name, int argc, Atom* atomv, VTable* vtable, Binding b);

/**
 * get the __proto__ object for this value.  When the value is an object,
 * we look it up directly.  For primitives (String, Namespace, int,
 * Boolean, uint, and Number, we get the prototype from the primitive's
 * class object.  Includes null/undefined error handling.
 */
template <class E>
ScriptObject* toPrototype(Atom obj);

/**
 * get the vtable for the given value, or throw null/undefined exception.
 * when the value is a primitive we get the vtable from the environment.
 */
template <class E>
VTable* toVTable(E env, Atom atom);

/**
 * OP_applytype implementation.
 * used to create concrete class objects for parameterizations of Vector.
 */
template <class E>
Atom op_applytype(E env, Atom factory, int argc, Atom* args);

/**
 * implementaion of OP_newarray for creating array literals
 */
template <class E>
ArrayObject* newarray(E caller_env, int argc, Atom* ap);

/**
 * implementation of OP_astype when RHS type is not
 * known at JIT time
 */
template <class E>
Atom astype_late(E caller_env, Atom value, Atom type);

/**
 * implementation of OP_instanceof
 */
template <class E>
Atom instanceof(E caller_env, Atom val, Atom ctor);

/**
 * implementation of OP_in
 */
template <class E>
Atom op_in(E caller_env, Atom name, Atom obj);

/**
 * inline-cache enabled finddef.  if the cache for this slot is valid, return
 * the result from calling finddef on this (env,multiname) pair previously.
 */
ScriptObject* FASTCALL finddef_cache(ScriptObject** obj_ptr, MethodFrame* frame);

/**
 * Implementation of OP_constructprop
 */
template <class E>
Atom constructprop(E env, const Multiname* multiname, int argc, Atom* atomv);
Atom constructprop(Toplevel* toplevel, const Multiname* multiname, int argc, Atom* atomv, VTable* vtable);

/**
 * This is the implicit coercion operator.  It is kind of like a
 * Java downcast, but because of how E4 works, there are some cases
 * when it returns a different value than what you pass in.
 *
 * It will happily return null if you pass in null for
 * any reference type.  And, it will throw an exception if the
 * value is not in the type's value set.  It does not do type
 * conversion.
 *
 * @param  atom      The atom containing the value to coerce.
 * @param  itraits   The itraits of the type to coerce to.
 * @return The coerced atom.
 * @throws Exception if the value is not in the type's value
 *                   set.
 */
Atom coerceImpl(const Toplevel*, Atom atom, Traits* expected);
template <class E>
Atom coerce(E env, Atom atom, Traits* expected);

/**
 * coerce specialized for ScriptObject* based types;
 * coerceobj() either throws an error or returns with no side effects
 * after returning, caller can safely downcast obj.  obj can still be null.
 */
void coerceobj_obj(MethodEnv* env, ScriptObject* obj, Traits*);

/**
 * Coerce a value to namespace.  No need to check traits, we can do it just
 * with atom tag checking.
 */
void coercens_atom(MethodEnv* env, Atom atom);

/**
 * generic coerceobj accepting any value and coercing to one of the ScriptObject*
 * types.  If this method returns, caller can safely use (ScriptObject*)atomPtr(atom).
 * (note: result can still be null).
 */
void coerceobj_atom(MethodEnv* env, Atom atom, Traits*);

/**
 * generic implementation of OP_add/subtract/modulo/multiply/divide
 */
Atom op_add(AvmCore*, Atom lhs, Atom rhs);
#ifdef VMCFG_FLOAT
Atom op_add_nofloat(AvmCore*, Atom lhs, Atom rhs);
Atom op_multiply(AvmCore*, Atom lhs, Atom rhs);
Atom op_subtract(AvmCore*, Atom lhs, Atom rhs);
Atom op_modulo(AvmCore*, Atom lhs, Atom rhs);
Atom op_divide(AvmCore*, Atom lhs, Atom rhs);
Atom op_negate(AvmCore*, Atom oper );
#endif // VMCFG_FLOAT

/**
 * Arithmetic fastpath helper functions
 */
#ifdef VMCFG_FASTPATH_ADD
Atom op_add_a_aa(AvmCore* core, Atom lhs, Atom rhs);
Atom op_add_a_ai(AvmCore* core, Atom lhs, int32_t rhs);
Atom op_add_a_ia(AvmCore* core, int32_t lhs, Atom rhs);
Atom op_add_a_ad(AvmCore* core, Atom lhs, double rhs);
Atom op_add_a_da(AvmCore* core, double lhs, Atom rhs);
#ifdef VMCFG_FLOAT
Atom op_add_a_aa_nofloat(AvmCore* core, Atom lhs, Atom rhs);
Atom op_add_a_ai_nofloat(AvmCore* core, Atom lhs, int32_t rhs);
Atom op_add_a_ia_nofloat(AvmCore* core, int32_t lhs, Atom rhs);
Atom op_add_a_ad_nofloat(AvmCore* core, Atom lhs, double rhs);
Atom op_add_a_da_nofloat(AvmCore* core, double lhs, Atom rhs);
#endif
#endif

void FASTCALL mop_rangeCheckFailed(MethodEnv* env);
// load-and-sign-extend
int32_t FASTCALL mop_lix8(const void* addr);
int32_t FASTCALL mop_lix16(const void* addr);
// load-and-zero-extend
int32_t FASTCALL mop_liz8(const void* addr);
int32_t FASTCALL mop_liz16(const void* addr);
int32_t FASTCALL mop_li32(const void* addr);
double FASTCALL mop_lf32(const void* addr);
double FASTCALL mop_lf64(const void* addr);
void FASTCALL mop_si8(void* addr, int32_t value);
void FASTCALL mop_si16(void* addr, int32_t value);
void FASTCALL mop_si32(void* addr, int32_t value);
void mop_sf32(void* addr, double value);
void mop_sf64(void* addr, double value);
#ifdef VMCFG_FLOAT
void FASTCALL mop_lf32x4(float4_t* result, const void* addr);
void mop_sf32x4(void* addr, const float4_t& value);
#endif

} // namespace avmplus
