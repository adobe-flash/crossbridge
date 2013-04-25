/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */



#include "avmplus.h"
#include "BuiltinNatives.h"

using namespace MMgc;

namespace avmplus
{
    // ---------------

#ifndef VMCFG_AOT
    NativeInitializer::NativeInitializer(AvmCore* _core,
                                            char const* const* _versioned_uris,
                                            const uint8_t* _abcData,
                                            uint32_t _abcDataLen,
                                            uint32_t _methodCount,
                                            uint32_t _classCount) :
        core(_core),
        versioned_uris(_versioned_uris),
        abcData(_abcData),
        abcDataLen(_abcDataLen),
        methods((MethodType*)core->GetGC()->Calloc(_methodCount, sizeof(MethodType), GC::kZero)),
        classes((ClassType*)core->GetGC()->Calloc(_classCount, sizeof(ClassType), GC::kZero)),
        methodCount(_methodCount),
        classCount(_classCount)
    {
        if (versioned_uris)
            core->addVersionedURIs(versioned_uris);
    }
#else
    NativeInitializer::NativeInitializer(AvmCore* _core,
                                            char const* const* _versioned_uris,
                                            const AOTInfo *_aotInfo,
                                            uint32_t _methodCount,
                                            uint32_t _classCount) :
        core(_core),
        versioned_uris(_versioned_uris),
        abcData(_aotInfo->abcBytes),
        abcDataLen(_aotInfo->nABCBytes),
        methods((MethodType*)core->GetGC()->Calloc((_methodCount>0 ? _methodCount : 1), sizeof(MethodType), GC::kZero)),
        classes((ClassType*)core->GetGC()->Calloc((_classCount>0 ? _classCount : 1), sizeof(ClassType), GC::kZero)),
        methodCount(_methodCount),
        classCount(_classCount)
        , aotInfo(_aotInfo)
        , compiledMethods(_aotInfo->abcMethods)
        , compiledMethodCount(_aotInfo->nABCMethods)
    {
        if (versioned_uris)
            core->addVersionedURIs(versioned_uris);
    }
#endif


    void NativeInitializer::fillInMethods(const NativeMethodInfo* _methodEntry)
    {
        while (_methodEntry->method_id != -1)
        {
            // if we overwrite a native method mapping, something is hosed
            AvmAssert(methods[_methodEntry->method_id] == NULL);
            methods[_methodEntry->method_id] = _methodEntry;
            _methodEntry++;
        }
    }

    void NativeInitializer::fillInClasses(const NativeClassInfo* _classEntry)
    {
        while (_classEntry->class_id != -1)
        {
            // if we overwrite a native class mapping, something is hosed
            AvmAssert(classes[_classEntry->class_id]  == NULL);
            classes[_classEntry->class_id] = _classEntry;
            _classEntry++;
        }
    }

    PoolObject* NativeInitializer::parseBuiltinABC(Domain* domain)
    {
        AvmAssert(domain != NULL);

        ScriptBuffer code = ScriptBuffer(new (core->GetGC()) ReadOnlyScriptBufferImpl(abcData, abcDataLen));

        return core->parseActionBlock(code, /*start*/0, /*toplevel*/NULL, domain, this, kApiVersion_VM_INTERNAL/*active api*/);
    }

    NativeInitializer::~NativeInitializer()
    {
        // might as well explicitly free now
        core->GetGC()->Free(methods);
        core->GetGC()->Free(classes);
    }

#ifdef VMCFG_AOT
    bool NativeInitializer::getCompiledInfo(NativeMethodInfo *info, AvmThunkNativeHandler* handlerOut, Multiname &returnTypeName, uint32_t i) const
    {
        info->thunker = (GprMethodProc)0;
        // NativeMethodInfo.handler is a union of
        // pointer to function and pointer to member function.
        // Set them both so the entire union is initialized.
        // See bugzilla#647660
        info->handler.method = NULL;
        info->handler.function = NULL;

        if (i < compiledMethodCount && compiledMethods[i])
        {
            bool isNumberRetType = false;
            if (NUMBER_TYPE) {
                Multiname numberTypeName(NUMBER_TYPE->ns(), NUMBER_TYPE->name());
                isNumberRetType = returnTypeName.matches(&numberTypeName);
            }
            info->thunker = isNumberRetType ? (GprMethodProc)aotThunkerN : (GprMethodProc)aotThunker;
            handlerOut->function = compiledMethods[i];
            return true;
        }
        else
        {
            handlerOut->function = (AvmThunkNativeFunctionHandler)0;
            return false;
        }
    }
#endif

    void ClassManifestBase::fillInClass(uint32_t class_id, ClassClosure* c)
    {
        AvmAssert(class_id < _count);
        // This is subtle but important: this call exists solely as a hack for the Vector<>
        // classes, which don't look up by name properly. Int/UInt/DoubleVectorClass
        // are all initialized once, as you'd expected, but ObjectVectorClass is initialized
        // multiple times: once for Vector<*>, then again for each specialization. We
        // only want the first one stored (not subsequent specializations), so only
        // store if nothing is there yet.
        if (_classes[class_id] == NULL)
            WBRC(c->gc(), this, &_classes[class_id], c);
    }
    
    ClassClosure* FASTCALL ClassManifestBase::lazyInitClass(uint32_t class_id)
    {
        AvmAssert(class_id < _count);
        ClassClosure** cc = &_classes[class_id];
        if (*cc == NULL)
        {
            PoolObject* pool = _env->abcEnv()->pool();
            Traits* traits = pool->getClassTraits(class_id)->itraits;
            Multiname qname(traits->ns(), traits->name());
            ScriptObject* container = _env->finddef(&qname);
            Atom classAtom = _env->toplevel()->getproperty(container->atom(), &qname, container->vtable);
            WBRC(pool->core->GetGC(), this, cc, (ClassClosure*)AvmCore::atomToScriptObject(classAtom));
        }
#ifdef _DEBUG
        // make sure the handful of special-cased entries in Toplevel match what we have cached.
        Toplevel* t = _env->toplevel();
        if (this == t->_builtinClasses)
        {
            switch(class_id)
            {
            case avmplus::NativeID::abcclass_Object:    AvmAssert(*cc == t->objectClass);     break;
            case avmplus::NativeID::abcclass_Class:     AvmAssert(*cc == t->_classClass);     break;
            case avmplus::NativeID::abcclass_Function:  AvmAssert(*cc == t->_functionClass);  break;
            case avmplus::NativeID::abcclass_Boolean:   AvmAssert(*cc == t->_booleanClass);   break;
            case avmplus::NativeID::abcclass_Namespace: AvmAssert(*cc == t->_namespaceClass); break;
            case avmplus::NativeID::abcclass_Number:    AvmAssert(*cc == t->_numberClass);    break;
#ifdef VMCFG_FLOAT
            case avmplus::NativeID::abcclass_float:     AvmAssert(*cc == t->_floatClass);     break;
            case avmplus::NativeID::abcclass_float4:    AvmAssert(*cc == t->_float4Class);    break;
#endif // VMCFG_FLOAT
            case avmplus::NativeID::abcclass_int:       AvmAssert(*cc == t->_intClass);       break;
            case avmplus::NativeID::abcclass_uint:      AvmAssert(*cc == t->_uintClass);      break;
            case avmplus::NativeID::abcclass_String:    AvmAssert(*cc == t->_stringClass);    break;
            }
        }
#endif
        // This function should *never* return NULL; if we get in this situation, it's
        // almost certainly due to an unexpected error during VM bootstrapping.
        AvmAssert(*cc != NULL);

		// We should check for NULL in production builds as well.  Class initialization
		// is a bit fragile, due to the bootstrapping issues of invoking builtin AS3 code
	    // deep down in the guts of our AS3 execution engine itself.  The check gives us
		// a measure of protection from unexpected errors, avoiding the inevitable segfault
		// which may be an exploitable security vulnerability. We are not prepared to recover
		// from such errors, so we summarily abort execution.
		if (*cc == NULL) MMgc::GCHeap::GetGCHeap()->Abort();  // OOM abort

        return *cc;
    }

    bool ClassManifestBase::gcTrace(MMgc::GC* gc, size_t /*ignored*/)
    {
        gc->TraceLocation<ScriptEnv>(&_env);
        gc->TraceLocations<ClassClosure>(&_classes[0], _count);
        return false;
    }
}
