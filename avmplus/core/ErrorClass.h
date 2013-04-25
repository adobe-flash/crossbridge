/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_ErrorClass__
#define __avmplus_ErrorClass__


namespace avmplus
{
    /**
     * class Error
     */
    class GC_AS3_EXACT(ErrorClass, ClassClosure)
    {
    protected:
        ErrorClass(VTable* cvtable);
        
    public:
        REALLY_INLINE static ErrorClass* create(MMgc::GC* gc, VTable* cvtable)
        {
            return new (gc, MMgc::kExact, cvtable->getExtraSize()) ErrorClass(cvtable);
        }

        Atom call(int argc, Atom* argv)
        {
            return construct(argc, argv);
        }

        /** @name static methods */
        /*@{*/
        Stringp getErrorMessage(int errorID) const;
#ifdef DEBUGGER
        Atom getStackTrace(Atom thisAtom,
                           Atom *argv,
                           int argc);
#endif /* DEBUGGER */

    // ------------------------ DATA SECTION BEGIN
        GC_NO_DATA(ErrorClass)

    private:
        DECLARE_SLOTS_ErrorClass;
    // ------------------------ DATA SECTION END
    };

    /**
     * The ErrorObject class is the C++ implementation of the
     * Error class in the ECMA-262 Specification.
     *
     * The main difference between an ErrorObject and a regular
     * ScriptObject is the presence of the stack trace, which
     * can be retrieved with the getStackTrace method.
     */
    class GC_AS3_EXACT(ErrorObject,ScriptObject)
    {
        friend class ErrorClass;
    protected:
        ErrorObject(VTable *vtable, ScriptObject *delegate);

    public:
#ifdef DRC_TRIVIAL_DESTRUCTOR
        ~ErrorObject() {
#ifdef DEBUGGER
            stackTrace = NULL;
#endif
        }
#endif

        Stringp getStackTrace() const;
        StackTrace* getStackTraceObject() const { return stackTrace; }
        
    // ------------------------ DATA SECTION BEGIN
        GC_DATA_BEGIN(ErrorObject)

    private:
        StackTrace* GC_POINTER(stackTrace);

        GC_DATA_END(ErrorObject)

        DECLARE_SLOTS_ErrorObject;
    // ------------------------ DATA SECTION END
    };

    /**
     * NativeErrorClass
     *
     * This class exists to override the [[Call]] entry point
     * to the class closure, so that it constructs an error object
     * instead of attempting a coercion.
     */
    class GC_CPP_EXACT(NativeErrorClass, ClassClosure)
    {
    protected:
        NativeErrorClass(VTable* cvtable);

    public:
        Atom call(int argc, Atom* argv)
        {
            return construct(argc, argv);
        }

        GC_NO_DATA(NativeErrorClass)
    };

    // ArgumentError
    
    class GC_AS3_EXACT(ArgumentErrorClass, NativeErrorClass)
    {
    protected:
        ArgumentErrorClass(VTable* cvtable) : NativeErrorClass(cvtable) {}
        
    public:
        GC_NO_DATA(ArgumentErrorClass);
        DECLARE_SLOTS_ArgumentErrorClass;
    };
    
    class GC_AS3_EXACT(ArgumentErrorObject, ErrorObject)
    {
        friend class ArgumentErrorClass;
    protected:
        ArgumentErrorObject(VTable *vtable, ScriptObject *delegate) : ErrorObject(vtable, delegate) {}
        
    private:
        GC_NO_DATA(ArgumentErrorObject);
        DECLARE_SLOTS_ArgumentErrorObject;
    };

    // DefinitionError
    
    class GC_AS3_EXACT(DefinitionErrorClass, NativeErrorClass)
    {
    protected:
        DefinitionErrorClass(VTable* cvtable) : NativeErrorClass(cvtable) {}

    public:
        GC_NO_DATA(DefinitionErrorClass);
        DECLARE_SLOTS_DefinitionErrorClass;
    };

    class GC_AS3_EXACT(DefinitionErrorObject, ErrorObject)
    {
        friend class DefinitionErrorClass;
    protected:
        DefinitionErrorObject(VTable *vtable, ScriptObject *delegate) : ErrorObject(vtable, delegate) {}
        
    private:
        GC_NO_DATA(DefinitionErrorObject);
        DECLARE_SLOTS_DefinitionErrorObject;
    };

    // EvalError
    
    class GC_AS3_EXACT(EvalErrorClass, NativeErrorClass)
    {
    protected:
        EvalErrorClass(VTable* cvtable) : NativeErrorClass(cvtable) {}
        
    public:
        GC_NO_DATA(EvalErrorClass);
        DECLARE_SLOTS_EvalErrorClass;
    };
    
    class GC_AS3_EXACT(EvalErrorObject, ErrorObject)
    {
        friend class EvalErrorClass;
    protected:
        EvalErrorObject(VTable *vtable, ScriptObject *delegate) : ErrorObject(vtable, delegate) {}
        
    private:
        GC_NO_DATA(EvalErrorObject);
        DECLARE_SLOTS_EvalErrorObject;
    };
    
    // RangeError
    
    class GC_AS3_EXACT(RangeErrorClass, NativeErrorClass)
    {
    protected:
        RangeErrorClass(VTable* cvtable) : NativeErrorClass(cvtable) {}
        
    public:
        GC_NO_DATA(RangeErrorClass);
        DECLARE_SLOTS_RangeErrorClass;
    };
    
    class GC_AS3_EXACT(RangeErrorObject, ErrorObject)
    {
        friend class RangeErrorClass;
    protected:
        RangeErrorObject(VTable *vtable, ScriptObject *delegate) : ErrorObject(vtable, delegate) {}
        
    private:
        GC_NO_DATA(RangeErrorObject);
        DECLARE_SLOTS_RangeErrorObject;
    };
    
    // ReferenceError
    
    class GC_AS3_EXACT(ReferenceErrorClass, NativeErrorClass)
    {
    protected:
        ReferenceErrorClass(VTable* cvtable) : NativeErrorClass(cvtable) {}
        
    public:
        GC_NO_DATA(ReferenceErrorClass);
        DECLARE_SLOTS_ReferenceErrorClass;
    };
    
    class GC_AS3_EXACT(ReferenceErrorObject, ErrorObject)
    {
        friend class ReferenceErrorClass;
    protected:
        ReferenceErrorObject(VTable *vtable, ScriptObject *delegate) : ErrorObject(vtable, delegate) {}
        
    private:
        GC_NO_DATA(ReferenceErrorObject);
        DECLARE_SLOTS_ReferenceErrorObject;
    };
    
    // SecurityError
    
    class GC_AS3_EXACT(SecurityErrorClass, NativeErrorClass)
    {
    protected:
        SecurityErrorClass(VTable* cvtable) : NativeErrorClass(cvtable) {}
        
    public:
        GC_NO_DATA(SecurityErrorClass);
        DECLARE_SLOTS_SecurityErrorClass;
    };
    
    class GC_AS3_EXACT(SecurityErrorObject, ErrorObject)
    {
        friend class SecurityErrorClass;
    protected:
        SecurityErrorObject(VTable *vtable, ScriptObject *delegate) : ErrorObject(vtable, delegate) {}
        
    private:
        GC_NO_DATA(SecurityErrorObject);
        DECLARE_SLOTS_SecurityErrorObject;
    };
    
    // SyntaxError
    
    class GC_AS3_EXACT(SyntaxErrorClass, NativeErrorClass)
    {
    protected:
        SyntaxErrorClass(VTable* cvtable) : NativeErrorClass(cvtable) {}
        
    public:
        GC_NO_DATA(SyntaxErrorClass);
        DECLARE_SLOTS_SyntaxErrorClass;
    };
    
    class GC_AS3_EXACT(SyntaxErrorObject, ErrorObject)
    {
        friend class SyntaxErrorClass;
    protected:
        SyntaxErrorObject(VTable *vtable, ScriptObject *delegate) : ErrorObject(vtable, delegate) {}
        
    private:
        GC_NO_DATA(SyntaxErrorObject);
        DECLARE_SLOTS_SyntaxErrorObject;
    };
    
    // TypeError
    
    class GC_AS3_EXACT(TypeErrorClass, NativeErrorClass)
    {
    protected:
        TypeErrorClass(VTable* cvtable) : NativeErrorClass(cvtable) {}
        
    public:
        GC_NO_DATA(TypeErrorClass);
        DECLARE_SLOTS_TypeErrorClass;
    };
    
    class GC_AS3_EXACT(TypeErrorObject, ErrorObject)
    {
        friend class TypeErrorClass;
    protected:
        TypeErrorObject(VTable *vtable, ScriptObject *delegate) : ErrorObject(vtable, delegate) {}
        
    private:
        GC_NO_DATA(TypeErrorObject);
        DECLARE_SLOTS_TypeErrorObject;
    };
    
    // UninitializedError
    
    class GC_AS3_EXACT(UninitializedErrorClass, NativeErrorClass)
    {
    protected:
        UninitializedErrorClass(VTable* cvtable) : NativeErrorClass(cvtable) {}
        
    public:
        GC_NO_DATA(UninitializedErrorClass);
        DECLARE_SLOTS_UninitializedErrorClass;
    };
    
    class GC_AS3_EXACT(UninitializedErrorObject, ErrorObject)
    {
        friend class UninitializedErrorClass;
    protected:
        UninitializedErrorObject(VTable *vtable, ScriptObject *delegate) : ErrorObject(vtable, delegate) {}
        
    private:
        GC_NO_DATA(UninitializedErrorObject);
        DECLARE_SLOTS_UninitializedErrorObject;
    };
    
    // URIError
    
    class GC_AS3_EXACT(URIErrorClass, NativeErrorClass)
    {
    protected:
        URIErrorClass(VTable* cvtable) : NativeErrorClass(cvtable) {}
        
    public:
        GC_NO_DATA(URIErrorClass);
        DECLARE_SLOTS_URIErrorClass;
    };
    
    class GC_AS3_EXACT(URIErrorObject, ErrorObject)
    {
        friend class URIErrorClass;
    protected:
        URIErrorObject(VTable *vtable, ScriptObject *delegate) : ErrorObject(vtable, delegate) {}
        
    private:
        GC_NO_DATA(URIErrorObject);
        DECLARE_SLOTS_URIErrorObject;
    };

    // VerifyError
    
    class GC_AS3_EXACT(VerifyErrorClass, NativeErrorClass)
    {
    protected:
        VerifyErrorClass(VTable* cvtable) : NativeErrorClass(cvtable) {}
        
    public:
        GC_NO_DATA(VerifyErrorClass);
        DECLARE_SLOTS_VerifyErrorClass;
    };
    
    class GC_AS3_EXACT(VerifyErrorObject, ErrorObject)
    {
        friend class VerifyErrorClass;
    protected:
        VerifyErrorObject(VTable *vtable, ScriptObject *delegate) : ErrorObject(vtable, delegate) {}
        
    private:
        GC_NO_DATA(VerifyErrorObject);
        DECLARE_SLOTS_VerifyErrorObject;
    };
}

#endif /* __avmplus_ErrorClass__ */
