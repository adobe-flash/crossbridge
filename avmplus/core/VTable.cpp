/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
//#define DOPROF
//#include "vprof.h"

namespace avmplus
{
    VTable::VTable(Traits* traits, VTable* _base, Toplevel* toplevel)
        : _toplevel(toplevel)
        , init(NULL)
        , base(_base)
        , ivtable(NULL)
        , traits(traits)
        , createInstanceProc(_base ? _base->createInstanceProc : ClassClosure::impossibleCreateInstanceProc)
        , linked(false)
#ifdef VMCFG_AOT
        , interfaceBindingFunction(traits->m_interfaceBindingFunction)
#endif
    {
        AvmAssert(toplevel != NULL);
        AvmAssert(traits != NULL);
    }

    void VTable::resolveSignatures(ScopeChain* scope)
    {
        AvmAssert(scope != NULL);

        if( this->linked )
            return;
        // don't mark as resolved until the end of the function:
        // if traits->resolveSignatures() throws, we end up with the VTable as
        // "resolved" but the Traits not, which makes us crash in unpredictable ways.
        if (!traits->isResolved())
        {
            traits->resolveSignatures(toplevel());
            traits->setDeclaringScopes(scope->scopeTraits());
        }

#if defined(DEBUG) || defined(_DEBUG)
        // have to use local variables for CodeWarrior
        Traits* traitsBase = traits->base;
        Traits* baseTraits = base ? base->traits : 0;
        // make sure the traits of the base vtable matches the base traits
        AvmAssert((base == NULL && traits->base == NULL) || (base != NULL && traitsBase == baseTraits));
#endif // DEBUG

        AvmCore* core = traits->core;
        MMgc::GC* gc = core->GetGC();

        if (traits->init && !this->init)
        {
            this->init = makeMethodEnv(traits->init, scope);
        }

        // populate method table
        const TraitsBindingsp td = traits->getTraitsBindings();
        const TraitsBindingsp btd = td->base;
        for (uint32_t i = 0, n = td->methodCount; i < n; i++)
        {
            MethodInfo* method = td->getMethod(i);

            if (btd && i < btd->methodCount && method == btd->getMethod(i))
            {
                // inherited method
                // this->methods[i] = base->methods[i];
                WB(gc, this, &methods[i], base->methods[i]);
                continue;
            }

            // new definition
            if (method != NULL)
            {
                //this->methods[i] = new (gc) MethodEnv(method, this);
                WB(gc, this, &methods[i], makeMethodEnv(method, scope));
                continue;
            }
            #ifdef AVMPLUS_VERBOSE
            if (traits->pool->isVerbose(VB_traits))
            {
                // why would the compiler assign sparse disp_id's?
                traits->core->console << "WARNING: empty disp_id " << i << " on " << traits << "\n";
            }
            #endif
        }

        // this is done here b/c this property of the traits isn't set until the
        // Dictionary's ClassClosure is called
        if (base && base->traits->isDictionary())
            traits->set_isDictionary();

        traits->core->exec->notifyVTableResolved(this);

        linked = true;
    }

    MethodEnv* VTable::makeMethodEnv(MethodInfo* func, ScopeChain* scope)
    {
        AvmCore* core = this->core();
        AbcEnv* abcEnv = scope->abcEnv();
        MethodEnv* methodEnv = MethodEnv::create(core->GetGC(), func, scope);
        // register this env in the callstatic method table
        int method_id = func->method_id();
        if (method_id != -1)
        {
            AvmAssert(abcEnv->pool() == func->pool());
            if (abcEnv->getMethod(method_id) == NULL)
            {
                abcEnv->setMethod(method_id, methodEnv);
            }
            #ifdef AVMPLUS_VERBOSE
            else if (func->pool()->isVerbose(VB_traits))
            {
                core->console << "WARNING: tried to re-register global MethodEnv for " << func << "\n";
            }
            #endif
        }
        return methodEnv;
    }

#ifdef DEBUGGER
    uint64_t VTable::bytesUsed() const
    {
        uint64_t bytesUsed = sizeof(VTable);

        if(ivtable != NULL)
            bytesUsed += ivtable->bytesUsed();

        const TraitsBindingsp td = traits->getTraitsBindings();
        const uint32_t n = td->methodCount;
        const uint32_t baseMethodCount = base ? td->base->methodCount : 0;
        bytesUsed += td->methodCount*sizeof(MethodInfo*);

        for (uint32_t i=0; i < n; i++)
        {
            MethodInfo* method = td->getMethod(i);

            if (i < baseMethodCount && td->base && method == td->base->getMethod(i))
            {
                continue;
            }
            else if(method != NULL)
            {
                bytesUsed += method->bytesUsed();
            }
        }
        return bytesUsed;
    }
#endif

    VTable* VTable::newParameterizedVTable(Traits* param_traits, Stringp fullname)
    {
        Toplevel* toplevel = this->toplevel();
        AvmCore* core = toplevel->core();
        Namespacep traitsNs = this->traits->ns();

        GCRef<builtinClassManifest> builtinClasses = toplevel->builtinClasses();
        GCRef<ObjectVectorClass> vectorobj_cls = builtinClasses->get_Vector_objectClass();
        GCRef<ScopeChain> vectorobj_cscope = vectorobj_cls->vtable->init->scope();
        GCRef<ScopeChain> vectorobj_iscope = vectorobj_cls->ivtable()->init->scope();
        VTable* objVecVTable = vectorobj_cls->vtable;
        AbcEnv* objVecAbcEnv = vectorobj_cscope->abcEnv();
        Toplevel* objVecToplevel = objVecVTable->toplevel();
        VTable* objVecIVTable = objVecVTable->ivtable;

        // these cases should all be filtered out by the caller;
        // we only want to handle Vector<SomeObject> here
        AvmAssert(param_traits != NULL &&
                    param_traits != toplevel->intClass()->vtable->traits->itraits &&
                    param_traits != toplevel->uintClass()->vtable->traits->itraits &&
                    param_traits != toplevel->numberClass()->vtable->traits->itraits);

        PoolObject* traitsPool = this->traits->pool;

        // See comments in VectorClass.h: the lookup of the ctraits under the class name is
        // redundant, because the ctraits are never registered.  That's mostly OK: the call to
        // newParameterizedVTable is guarded by its caller, getTypedVectorClass, and is never
        // invoked more than once on any param_traits for the same factory.

        Stringp classname = core->internString(fullname->appendLatin1("$"));
        Traits* ctraits = core->domainMgr()->findTraitsInPoolByNameAndNS(traitsPool, classname, traitsNs);
        Traits* itraits;
        if (!ctraits)
        {
            // important: base the new ctraits on objVecVTable->traits, *not* this->traits;
            // we want the result to be based off ObjectVectorClass, not VectorClass
            // (otherwise sizeofInstance would be too small and we'd be crashy)
            ctraits = objVecVTable->traits->newParameterizedCTraits(classname, traitsNs);
            ctraits->verifyBindings(toplevel);

            itraits = traitsPool->resolveParameterizedType(toplevel, this->ivtable->traits, param_traits);
            ctraits->itraits = itraits;
        }
        else
        {
            // Never happens
            itraits = ctraits->itraits;
        }

        AvmAssert(itraits != NULL);

        VTable* class_ivtable = builtinClasses->get_ClassClass()->ivtable();

        VTable* cvtab = core->newVTable(ctraits, class_ivtable, objVecToplevel);
        ScopeChain* cvtab_cscope = vectorobj_cscope->cloneWithNewVTable(core->GetGC(), cvtab, objVecAbcEnv);

        VTable* ivtab = core->newVTable(itraits, objVecIVTable, objVecToplevel);
        ScopeChain* ivtab_iscope = vectorobj_iscope->cloneWithNewVTable(core->GetGC(), ivtab, objVecAbcEnv);
        cvtab->ivtable = ivtab;
        ivtab->init = objVecIVTable->init;

        cvtab->resolveSignatures(cvtab_cscope);
        ivtab->resolveSignatures(ivtab_iscope);

        return cvtab;
    }

#ifdef AVMPLUS_VERBOSE
    PrintWriter& VTable::print(PrintWriter& prw) const
    {
        return prw << traits;
    }
#endif
}
