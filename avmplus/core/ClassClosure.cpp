/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"


namespace avmplus
{

    /*static*/ CreateInstanceProc ClassClosure::calcCreateInstanceProc(VTable* cvtable)
    {
        VTable* ivtable = cvtable->ivtable;
        if (ivtable && ivtable->base)
        {
            ScopeChain* scope = cvtable->init->scope();
            if (scope->getSize())
            {
                Atom baseAtom = scope->getScope(scope->getSize()-1);
                if (!AvmCore::isObject(baseAtom))
                    cvtable->toplevel()->throwVerifyError(kCorruptABCError);

                ScriptObject* base = AvmCore::atomToScriptObject(baseAtom);
                // make sure scope object is base type's class object
                AvmAssert(base->traits()->itraits == cvtable->traits->itraits->base);
                if (base->traits()->itraits->isAbstractBase)
                {
                    // If we get here, it means that we descend from an abstract base class,
                    // but don't have a native createInstanceProc of our own; in that case, we
                    // should just create a plain old ScriptObject. (Note that this can
                    // happen for abstract and abstract-restricted; for the latter, we will do
                    // a second check in checkForRestrictedInheritance() and may reject it anyway.)
                    goto create_normal;
                }
                // ...otherwise, we're done.
                ClassClosure* base_cc = base->toClassClosure();
                AvmAssert(base_cc != NULL);
                CreateInstanceProc proc = base_cc->m_createInstanceProc;
                // If the proc is SemiSealedArrayObject, revert back to normal Array,
                // and let checkForRestrictedInheritance() choose the proper proc:
                // we might be a dynamic subclass of a non-dynamic subclass of Array.
                if (proc == SemiSealedArrayObject::createInstanceProc)
                    proc = ArrayClass::createInstanceProc;

                // Bugzilla 688486: don't use unsubclassed-specialized
                // instance creator to create subclassed instances.
                if (proc == ArrayClass::createUnsubclassedInstanceProc)
                    proc = ArrayClass::createInstanceProc;

                return proc;
            }
        }

create_normal:
        return ClassClosure::createScriptObjectProc;
    }

    /*static*/ CreateInstanceProc FASTCALL ClassClosure::checkForRestrictedInheritance(VTable* ivtable, CreateInstanceProc p)
    {
        if (ivtable)
        {
            Traits* itraits = ivtable->traits;
            if (p == ArrayClass::createInstanceProc && !itraits->needsHashtable())
            {
                // If we are a sealed subclass of Array, we want to check BugCompatibility
                // to see if instances of this class should behave as "semisealed" instead.
                if (!itraits->core->currentBugCompatibility()->bugzilla654807)
                {
                    return SemiSealedArrayObject::createInstanceProc;
                }
            }

            // Bugzilla 688486: if this is unsubclassed array, let it be simple.
            if (itraits == itraits->core->traits.array_itraits)
            {
                return ArrayClass::createUnsubclassedInstanceProc;
            }

            Traits* base = itraits->base;
            if (base != NULL && base->isRestrictedInheritance && base->pool != itraits->pool)
            {
                return ClassClosure::cantInstantiateCreateInstanceProc;
            }
        }
        return p;
    }

    ClassClosure::ClassClosure(VTable* cvtable)
        : ScriptObject(cvtable, NULL)
        , m_createInstanceProc(checkForRestrictedInheritance(cvtable->ivtable, cvtable->ivtable->createInstanceProc))
        // NB: prototype is null right now, but we expect our subclass to
        // initialize it in their ctor (or, at a minimum, before it attempts
        // to create any instances).
    {
        AvmAssert(traits()->getSizeOfInstance() >= sizeof(ClassClosure));

        // All callers of this ctor must have a non-null ivtable.
        AvmAssert(cvtable->ivtable != NULL);
        cvtable->ivtable->createInstanceProc = ClassClosure::reinitNullPrototypeCreateInstanceProc;
        AvmAssert(m_createInstanceProc != reinitNullPrototypeCreateInstanceProc);
        
        // don't assert here any more: MethodClosure descends
        //AvmAssert(cvtable->traits->itraits != NULL);
        //AvmAssert(ivtable() != NULL);
    }

    ClassClosure::ClassClosure(VTable* cvtable, CreateInstanceProc createInstanceProc)
        : ScriptObject(cvtable, NULL)
        , m_createInstanceProc(checkForRestrictedInheritance(cvtable->ivtable, createInstanceProc))
        // NB: prototype is null right now, but we expect our subclass to
        // initialize it in their ctor (or, at a minimum, before it attempts
        // to create any instances).
    {
        AvmAssert(traits()->getSizeOfInstance() >= sizeof(ClassClosure));

        // FunctionObject can legally have a null ivtable, and uses this ctor, so check.
        VTable* const ivtable = cvtable->ivtable;
        if (ivtable != NULL)
        {
            ivtable->createInstanceProc = ClassClosure::reinitNullPrototypeCreateInstanceProc;
        }
        AvmAssert(m_createInstanceProc != reinitNullPrototypeCreateInstanceProc);

        // don't assert here any more: MethodClosure descends
        //AvmAssert(cvtable->traits->itraits != NULL);
        //AvmAssert(ivtable() != NULL);
    }

    /*static*/ ClassClosure* FASTCALL ClassClosure::createClassClosure(VTable* cvtable)
    {
        ClassClosure* cc = new (cvtable->gc(), MMgc::kExact, cvtable->getExtraSize()) ClassClosure(cvtable, calcCreateInstanceProc(cvtable));
        AvmAssert(cc->prototypePtr() == NULL);
        cc->createVanillaPrototype();
        return cc;
    }
    
    void ClassClosure::createVanillaPrototype()
    {
        m_prototype = toplevel()->objectClass->construct();
        AvmAssert(m_prototype != NULL);
    }

    Atom ClassClosure::get_prototype()
    {
        // Illegal to apply this getter to anything but instances of Class
        // (verifier should ensure this)
        return m_prototype ? m_prototype->atom() : undefinedAtom;
    }

    void ClassClosure::set_prototype(Atom value)
    {
        // ISSUE can value be null/undefined?

        if (AvmCore::isNullOrUndefined(value))
        {
            setPrototypePtr(NULL);
        }
        else
        {
            if (!AvmCore::isObject(value))
            {
                toplevel()->throwTypeError(kPrototypeTypeError);
            }

            // allow any prototype object.  if the object has methods or slots, so be it
            setPrototypePtr(AvmCore::atomToScriptObject(value));
        }
    }

    void FASTCALL ClassClosure::setPrototypePtr(ScriptObject* p)
    {
        m_prototype = p;
        if (p == NULL)
        {
            VTable* const ivtable = this->vtable->ivtable;
            if (ivtable != NULL)
            {
                ivtable->createInstanceProc = ClassClosure::reinitNullPrototypeCreateInstanceProc;
            }
        }
    }

    GCRef<ScriptObject> ClassClosure::constructObject()
    {
        Atom emptyArg = nullObjectAtom;
        Atom const outAtom = this->construct(0, &emptyArg);
        // All of the autogenerated constructObject() stubs are guaranteed to never return NULL,
        // so let's honor that contract here too: in the unlikely event a subclass returns NULL
        // or a non-object Atom, we'll throw. (Generally, this shouldn't ever happen, so we'll
        // also assert.)
        AvmAssert(AvmCore::isObject(outAtom));
        if (!AvmCore::isObject(outAtom))
            throwCantInstantiateError();
        return GCRef<ScriptObject>(AvmCore::atomToScriptObject(outAtom));
    }

    // This is used by construct="native" to special-case construction of some objects.
    Atom ClassClosure::construct_native(CreateInstanceProc ciproc, int argc, Atom* argv)
    {
        VTable* ivtable = this->ivtable();
        AvmAssert(ivtable != NULL);
        AvmAssert(argv != NULL); // need at least one arg spot passed in

        ScriptObject* obj = (*ciproc)(this);
        AvmAssert(obj != NULL); //should never be null
        Atom a = obj->atom();
        argv[0] = a; // new object is receiver
        ivtable->init->coerceEnter(argc, argv);
        // this is a class. always return new instance.
        return a;
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom ClassClosure::construct(int argc, Atom* argv)
    {
        VTable* ivtable = this->ivtable();
        AvmAssert(ivtable != NULL);
        AvmAssert(argv != NULL); // need at least one arg spot passed in

        ScriptObject* obj = newInstance();
        AvmAssert(obj != NULL); //should never be null
        Atom a = obj->atom();
        argv[0] = a; // new object is receiver
        ivtable->init->coerceEnter(argc, argv);
        // this is a class. always return new instance.
        return a;
    }

    // this = argv[0]
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom ClassClosure::call(int argc, Atom* argv)
    {
        Toplevel* toplevel = this->toplevel();
        // explicit coercion of a class object.
        if (argc != 1)
        {
            toplevel->throwArgumentError(kCoerceArgumentCountError, toplevel->core()->toErrorString(argc));
        }
        return toplevel->coerce(argv[1], (Traits*)ivtable()->traits);
    }

#ifdef DEBUGGER
    uint64_t ClassClosure::bytesUsed() const
    {
        uint64_t bytesUsed = ScriptObject::bytesUsed();
        bytesUsed += vtable->bytesUsed();
        return bytesUsed;
    }
#endif

#ifdef AVMPLUS_VERBOSE
    PrintWriter& ClassClosure::print(PrintWriter& prw) const
    {
        return traits()->name()
            ? prw << traits()
            : prw << "CC{}@" << asAtomHex(atom());
    }
#endif

    Stringp ClassClosure::implToString() const
    {
        AvmCore* core = this->core();
        Traits* t = this->traits()->itraits;
        Stringp s = core->concatStrings(core->newConstantStringLatin1("[class "), t->name());
        return core->concatStrings(s, core->newConstantStringLatin1("]"));
    }

    /*static*/
    ScriptObject* FASTCALL ClassClosure::reinitNullPrototypeCreateInstanceProc(ClassClosure* cls)
    {
        if (cls->m_prototype == NULL)
        {
            // ES3 spec, 13.2.2 (we've already ensured prototype is either an Object or null)
            ScriptObject* prototype = AvmCore::atomToScriptObject(cls->toplevel()->objectClass->get_prototype());
            cls->m_prototype = prototype;
        }
        VTable* const ivtable = cls->vtable->ivtable;
        AvmAssert(ivtable != NULL);
        CreateInstanceProc p = cls->m_createInstanceProc;
        AvmAssert(p != NULL);
        AvmAssert(p != impossibleCreateInstanceProc);
        AvmAssert(p != reinitNullPrototypeCreateInstanceProc);
        ivtable->createInstanceProc = p;
        return p(cls);
    }

    ScriptObject* FASTCALL ClassClosure::createScriptObjectProc(ClassClosure* cls)
    {
        return ScriptObject::create(cls->gc(), cls->ivtable(), cls->prototypePtr());
    }

    ScriptObject* FASTCALL ClassClosure::cantInstantiateCreateInstanceProc(ClassClosure* cls)
    {
        cls->throwCantInstantiateError();
        return NULL;
    }

    ScriptObject* FASTCALL ClassClosure::impossibleCreateInstanceProc(ClassClosure* cls)
    {
        // should not be possible to call directly
        AvmAssert(!"Should not be invoked");
        cls->toplevel()->throwTypeError(kCorruptABCError);
        return NULL;
    }

    void ClassClosure::throwError(int errorID, Stringp arg1, Stringp arg2, Stringp arg3)
    {
        core()->throwErrorV(this, errorID, arg1, arg2, arg3);
    }

    bool FASTCALL ClassClosure::isTypeImpl(Atom value)
    {
        return AvmCore::istype(value, ivtable()->traits);
    }
    
    Atom FASTCALL ClassClosure::asTypeImpl(Atom value)
    {
        return AvmCore::astype(value, ivtable()->traits);
    }
    
    Atom FASTCALL ClassClosure::coerceToTypeImpl(Atom value)
    {
        return toplevel()->coerce(value, ivtable()->traits);
    }

    void ClassClosure::initPrototypeConstructor()
    {
        AvmCore* core = this->core();
        ScriptObject* cc_proto = this->prototypePtr();
        AvmAssert(cc_proto != NULL);
        cc_proto->setStringProperty(core->kconstructor, this->atom());
        cc_proto->setStringPropertyIsEnumerable(core->kconstructor, false);
    }

}
