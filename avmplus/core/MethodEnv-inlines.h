/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus
{

REALLY_INLINE MethodEnv* MethodEnv::create(MMgc::GC* gc, MethodInfo* method, ScopeChain* scope)
{
    return new (gc, MMgc::kExact) MethodEnv(method, scope);
}

REALLY_INLINE AbcEnv* MethodEnv::abcEnv() const
{
    return _scope->abcEnv();
}

#ifndef VMCFG_AOT  // Avoid premature inlining for AOT; it prevents CSE
REALLY_INLINE AvmCore* MethodEnv::core() const
{
    return method->pool()->core;
}
#endif

REALLY_INLINE CodeContext* MethodEnv::codeContext() const
{
    return abcEnv()->codeContext();
}

REALLY_INLINE DomainEnv* MethodEnv::domainEnv() const
{
    return abcEnv()->domainEnv();
}

#ifndef VMCFG_AOT  // Avoid premature inlining for AOT; it prevents CSE
REALLY_INLINE ScopeChain* MethodEnv::scope() const
{
    return _scope;
}
#endif

REALLY_INLINE MethodEnv* MethodEnv::super_init() const
{
    AvmAssert(vtable()->base != NULL);
    return vtable()->base->init;
}

#ifndef VMCFG_AOT  // Avoid premature inlining for AOT; it prevents CSE
REALLY_INLINE Toplevel* MethodEnv::toplevel() const
{
    return vtable()->toplevel();
}
#endif

REALLY_INLINE Stringp MethodEnv::traitsName() const
{
    return vtable()->traits->name();
}

REALLY_INLINE Namespacep MethodEnv::traitsNs() const
{
    return vtable()->traits->ns();
}

REALLY_INLINE void MethodEnv::nullcheck(Atom atom)
{
    // Shark recommends inlining the isNullOrUndefined call
    if (AvmCore::isNullOrUndefined(atom))
        nullcheckfail(atom);
}

REALLY_INLINE MethodEnv::ActivationMethodTablePair::ActivationMethodTablePair(VTable *a, WeakKeyHashtable*wkh)
    : activation(a), methodTable(wkh)
{}

REALLY_INLINE MethodEnv::ActivationMethodTablePair* MethodEnv::getPair() const
{
    return (ActivationMethodTablePair*)(activationOrMCTable&~7);
}

REALLY_INLINE int32_t MethodEnv::getType() const
{
    return activationOrMCTable & 3;
}

REALLY_INLINE void MethodEnv::setActivationOrMCTable(void *ptr, int32_t type)
{
    AvmAssert((uintptr_t(ptr) & 7) == 0);
    WB(core()->GetGC(), this, &activationOrMCTable, (uintptr_t)ptr | type);
}

#ifndef VMCFG_AOT  // Avoid premature inlining for AOT; it prevents CSE
REALLY_INLINE VTable* MethodEnv::vtable() const
{
    return _scope->vtable();
}
#endif

REALLY_INLINE MethodSignaturep MethodEnv::get_ms()
{
    if (!method->isResolved())
        method->resolveSignature(this->toplevel());
    return method->getMethodSignature();
}

// specialized for calling init/get functions with no parameters
REALLY_INLINE Atom MethodEnv::coerceEnter(Atom thisArg)
{
    STACKADJUST(); // align stack for 32-bit Windows and MSVC compiler
    Atom ret = (*method->_invoker)(this, 0, &thisArg);
    STACKRESTORE();
    return ret;
}

/**
* General case for method calls
* @param argc The number of arguments that are contained in the argument list (excluding the "thisAtom" element)
* @param args The argument list that shall be passed to the invoked method (thisAtom, arg1 ... argN)
*
* NOTE: The contents of the given argument list can be modified during invocation of the MethodEnv without further warning.
*       Do not reuse an argument list AS-IS for multiple method calls, unless you make sure to reinitialize the contents of
*       the argument list after each call.
*/
REALLY_INLINE Atom MethodEnv::coerceEnter(int32_t argc, Atom* args)
{
    STACKADJUST(); // align stack for 32-bit Windows and MSVC compiler
    Atom result = (*method->_invoker)(this, argc, args);
    STACKRESTORE();
    AvmAssert(VMPI_memset(args, 0, (argc+1)*sizeof(Atom)) == args); // clobber incoming args in DEBUG
    return result;
}

REALLY_INLINE ScriptEnv::ScriptEnv(MethodInfo* _method, ScopeChain* _scope)
    : MethodEnv(_method, _scope)
{
#ifdef VMCFG_AOT
    int method_id = _method->method_id();;
    AbcEnv *abcEnv = _scope->abcEnv();
    if(method_id != -1 && abcEnv->getMethod(method_id) == NULL)
        abcEnv->setMethod(method_id, this);
#endif
}

REALLY_INLINE FunctionEnv::FunctionEnv(MethodInfo* _method, ScopeChain* _scope)
    : MethodEnv(_method, _scope)
{
#ifdef VMCFG_AOT
    int method_id = _method->method_id();;
    AbcEnv *abcEnv = _scope->abcEnv();
    if(method_id != -1 && abcEnv->getMethod(method_id) == NULL)
        abcEnv->setMethod(method_id, this);
#endif
}

} // namespace avmplus
