/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "BuiltinNatives.h"

namespace avmplus
{
    // ---------------------------
    
    FunctionClass::FunctionClass(VTable* cvtable)
        : ClassClosure(cvtable)
    {
        Toplevel* toplevel = this->toplevel();
        toplevel->_functionClass = this;

        AvmAssert(traits()->getSizeOfInstance() == sizeof(FunctionClass));

        setPrototypePtr(call_createEmptyFunction());
        prototypePtr()->setDelegate(toplevel->objectClass->prototypePtr());

        //
        // now that Object, Class, and Function are initialized, we
        // can set up Object.prototype.  other classes will init normally.
        //

        // init Object prototype
        toplevel->objectClass->initPrototype();
    }

    // Function called as constructor ... not supported from user code
    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom FunctionClass::construct(int argc, Atom* /*argv*/)
    {
        // ISSUE do we need an exception here?
        // cn: if argc is 0, this is harmless and we have to return an anonymous
        // function that itself if its > 0, then we can't support it

        /*
        from ECMA 327 5.1 Runtime Compilation
        An implementation that does not support global eval() or calling Function as a function or constructor
        SHALL throw an EvalError exception whenever global eval() (ES3 section 15.1.2.1), Function(p1,
        p2, ..., pn, body) (ES3 section 15.3.1.1), or new Function(p1, p2, ..., pn, body) (ES3 section 15.3.2.1) is
        called.
        */

        if (argc != 0)
        {
            toplevel()->evalErrorClass()->throwError(kFunctionConstructorError);
        }

        return call_createEmptyFunction()->atom();
    }

    int32_t FunctionObject::get_length()
    {
        AvmAssert(get_callEnv() != NULL);
        return get_callEnv()->method->getMethodSignature()->param_count();
    }

    FunctionObject::FunctionObject(VTable* cvtable, MethodEnv* call)
        : ClassClosure(cvtable, ClassClosure::createScriptObjectProc)
        , m_call_ptr(FunctionObject::callFunction)
        , m_callEnv(call)
    {
        // Bugzilla 753120, 767410: it would be nice to assert
        // m_callEnv is non-null, at least for classes that require it
        // be non-null.  (For now, WeakMethodClosure and
        // HostFunctionObject require we allow null m_callEnv.)

        // Since FunctionObject is (pseudo)final, we shouldn't need to calculate this every time,
        // but let's reality-check here just in case.
        AvmAssert(calcCreateInstanceProc(cvtable) == ClassClosure::createScriptObjectProc);
    }

    REALLY_INLINE Atom FunctionObject::getFunctionReceiver(Atom a) const
    {
        AvmAssert(get_callEnv() != NULL);
        if (AvmCore::isNullOrUndefined(a)) {
            // use callee's global object as this.
            // see E3 15.3.4.4
            a = get_callEnv()->scope()->getScope(0);
        }
        MethodSignaturep ms = get_callEnv()->method->getMethodSignature();
        return toplevel()->coerce(a, ms->paramTraits(0));
    }

    /**
     * Function.prototype.call()
     */
    Atom FunctionObject::AS3_call(Atom thisArg, Atom *argv, int argc)
    {
        thisArg = get_coerced_receiver(thisArg);
        return core()->exec->call(get_callEnv(), thisArg, argc, argv);
    }

    /**
     * Function.prototype.apply()
     */
    Atom FunctionObject::AS3_apply(Atom thisArg, Atom argArray)
    {
        thisArg = get_coerced_receiver(thisArg);

        // when argArray == undefined or null, same as not being there at all
        // see Function/e15_3_4_3_1.as

        if (!AvmCore::isNullOrUndefined(argArray))
        {
            AvmCore* core = this->core();

            // FIXME: why not declare argArray as Array in Function.as?
            if (!AvmCore::istype(argArray, ARRAY_TYPE))
                toplevel()->throwTypeError(kApplyError);

            return core->exec->apply(get_callEnv(), thisArg, (ArrayObject*)AvmCore::atomToScriptObject(argArray));
        }
        else
        {
            AvmAssert(get_callEnv() != NULL);
            return get_callEnv()->coerceEnter(thisArg);
        }
    }

    /* virtual */ Atom FunctionObject::get_coerced_receiver(Atom a) const
    {
        return getFunctionReceiver(a);
    }

    // this = argv[0] (ignored)
    // arg1 = argv[1]
    // argN = argv[argc]
    Atom FunctionObject::construct(int argc, Atom* argv)
    {
        AvmAssert(argv != NULL); // need at least one arg spot passed in

        ScriptObject* obj = newInstance();

        // this is a function
        argv[0] = obj->atom(); // new object is receiver
        AvmAssert(get_callEnv() != NULL);
        Atom result = get_callEnv()->coerceEnter(argc, argv);

        // for E3 13.2.2 compliance, check result and return it if (Type(result) is Object)

        /* ISSUE does this apply to class constructors too?

        answer: no.  from E4: A constructor may invoke a return statement as long as that
        statement does not supply a value; a constructor cannot return a value. The newly
        created object is returned automatically. A constructors return type must be omitted.
        A constructor always returns a new instance. */

        return AvmCore::isNull(result) || AvmCore::isObject(result) ? result : obj->atom();
    }

#if defined(DEBUGGER) || defined(VMCFG_AOT)
    /*virtual*/ MethodEnv* FunctionObject::getCallMethodEnv()
    {
        return get_callEnv();
    }
#endif

    /*virtual*/ Atom FunctionObject::call(int argc, Atom* argv)
    {
        // When called via ScriptObject::call virtual call, use stub.
        return (*m_call_ptr)(this, argc, argv);
    }

    /* static */ Atom FunctionObject::callFunction(FunctionObject* f, int argc, Atom* argv)
    {
        argv[0] = f->getFunctionReceiver(argv[0]);
        AvmAssert(f->get_callEnv() != NULL);
        return f->get_callEnv()->coerceEnter(argc, argv);
    }

    /*virtual*/ CodeContext* FunctionObject::getFunctionCodeContext() const
    {
        AvmAssert(get_callEnv() != NULL);
        return get_callEnv()->scope()->abcEnv()->codeContext();
    }

    /*virtual*/ Stringp FunctionObject::implToString() const
    {
        AvmCore* core = this->core();
        AvmAssert(get_callEnv() != NULL);
        Stringp s = core->concatStrings(core->newConstantStringLatin1("[object Function-"), core->intToString(get_callEnv()->method->method_id()));
        return core->concatStrings(s, core->newConstantStringLatin1("]"));
    }
}

namespace avm {
using avmplus::FunctionProc;

HostFunctionObject::HostFunctionObject(VTable* vtable, ScriptObject* delegate)
    : FunctionObject(vtable, NULL /* MethodEnv callee */) {
    setDelegate(delegate);
    m_call_ptr = (FunctionProc) &callHostFunction;
}

// static
Atom HostFunctionObject::callHostFunction(HostFunctionObject* f, int argc, Atom* args) {
    return f->call(argc, args);
}

}

