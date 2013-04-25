/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus {

//
// Inline implementations of functions defined in instr.h
//

#define AssertNotNull(a) AvmAssert(!AvmCore::isNullOrUndefined(a))

REALLY_INLINE
bool isObjectPtr(Atom a)
{
    // copy of AvmCore::isObject, but without the null check since we know we have nonnull
    AssertNotNull(a);
    return atomKind(a) == kObjectType;
}

REALLY_INLINE
bool isStringPtr(Atom a)
{
    // copy of AvmCore::isString, but without the null check since we know we have nonnull
    AssertNotNull(a);
    return atomKind(a) == kStringType;
}

REALLY_INLINE
Atom call_method_binding(Atom base, VTable* vtable, Binding b, int argc, Atom* atomv)
{
    // force receiver == base.  if caller used OP_callproplex then receiver was null.
    // todo: split out case for callproplex.
    atomv[0] = base;
    MethodEnv* method = vtable->methods[AvmCore::bindingToMethodId(b)];
    return method->coerceEnter(argc, atomv);
}

template <class E> REALLY_INLINE
Atom call_slot_binding(E env, Atom base, Binding b, int argc, Atom* atomv)
{
    ScriptObject* method = AvmCore::atomToScriptObject(base)->getSlotObject(AvmCore::bindingToSlotId(b));
    return op_call(env, method, argc, atomv);
}

REALLY_INLINE
Atom call_get_binding(Atom base, VTable* vtable, Binding b)
{
    int m = AvmCore::bindingToGetterId(b);
    MethodEnv *f = vtable->methods[m];
    return f->coerceEnter(base);
}

REALLY_INLINE
Atom call_obj_dynamic(Atom base, const Multiname* name, int argc, Atom* atomv)
{
    return AvmCore::atomToScriptObject(base)->callProperty(name, argc, atomv);
}

template <class E> REALLY_INLINE
Atom call_prim_dynamic(E env, Atom val, const Multiname* name, int argc, Atom* args)
{
#ifdef VMCFG_FLOAT
    if (AvmCore::isFloat4(val))
    {
        // See FIXME in Toplevel::getproperty for why this is "correct".
        uint32_t index;
        if (AvmCore::getIndexFromAtom(name->getName()->atom(), &index))
        {
            if (index <= 3)
            {
                float4_t f4 = AvmCore::atomToFloat4(val);
                float* pf4 = reinterpret_cast<float*>(&f4);
                val = env->core()->floatToAtom(pf4[index]);
            }
            return op_call(env, val, argc, args);
        }
    }
#endif
    // primitive types are not dynamic, so we can go directly
    // to their __proto__ object
    ScriptObject* proto = env->toplevel()->toPrototype(val);
    Atom func = proto->getMultinameProperty(name);
    return op_call(env, func, argc, args);
}

/**
 * OP_call.
 *
 * arg0 = argv[0]
 * arg1 = argv[1]
 * argN = argv[argc]
 */
template <class E> REALLY_INLINE
Atom op_call(E env, Atom func, int argc, Atom* atomv)
{
    if (AvmCore::isObject(func))
        return AvmCore::atomToScriptObject(func)->call(argc, atomv);
    return op_call_error(env);
}

/** specialized op_call when you have a ScriptObject* already for the function */
template <class E> REALLY_INLINE
Atom op_call(E env, ScriptObject* func, int argc, Atom* atomv)
{
    if (func)
        return func->call(argc, atomv);
    return op_call_error(env);
}

/**
 * OP_construct.  Note that arguments are in the opposite order from AVM.
 *
 * this = argv[0] // ignored
 * arg1 = argv[1]
 * argN = argv[argc]
 */
template <class E> REALLY_INLINE
Atom op_construct(E env, Atom ctor, int argc, Atom* atomv)
{
    if (AvmCore::isObject(ctor))
        return AvmCore::atomToScriptObject(ctor)->construct(argc, atomv);

    env->toplevel()->throwTypeError(kConstructOfNonFunctionError);
    return unreachableAtom;
}

template <class E> REALLY_INLINE
Binding getBinding(E env, VTable* vtable, const Multiname* ref)
{
    return getBinding(env, vtable->traits, ref);
}

template <class E> REALLY_INLINE
ArrayObject* newarray(E caller_env, int argc, Atom* ap) {
    Toplevel* toplevel = caller_env->toplevel();
    ArrayClass* arrayClass = toplevel->arrayClass();
    return arrayClass->newarray(ap, argc);
}

template <class E> REALLY_INLINE
Atom constructprop(E env, const Multiname* multiname, int argc, Atom* atomv)
{
    Toplevel* toplevel = env->toplevel();
    return constructprop(toplevel, multiname, argc, atomv, toVTable(toplevel, atomv[0]));
}

template <class E> REALLY_INLINE
Atom coerce(E env, Atom atom, Traits* expected)
{
    // do a couple of quick checks to see if we can bail early, since it's often the case
    // that the type is already what we expect (and we can determine that quickly
    // by checks against the Traits BuiltinType)
    if (!expected)
        return atom;

    if (AvmCore::atomDoesNotNeedCoerce(atom, BuiltinType(expected->builtinType)))
        return atom;

    return coerceImpl(env->toplevel(), atom, expected);
}

} // namespace avmplus
