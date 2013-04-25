/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

#ifdef VMCFG_AOT

#include "AOTCompiler.h"

#ifdef DEBUGGER
#include "avmplusDebugger.h"
#endif

using namespace avmplus;

typedef Traits** LLVMTraitsId;
typedef Namespace** LLVMNamespaceId;

// force instantiation in case the headers haven't already
template class MMgc::WriteBarrierRC<avmplus::String*>;
template class MMgc::WriteBarrierRC<avmplus::Namespace*>;
template class MMgc::WriteBarrierRC<avmplus::ScriptObject*>;    

extern "C" {
    const AOTInfo aotInfos[] = {};
    const size_t nAOTInfos = 0;
    const AbcEnv* aotAbcEnvs[] = {};
    const uint32_t nAotAbcEnvs = 0;
    const AOTInfo builtin_aotInfo = EMPTY_AOTInfo;
    const AOTInfo shell_toplevel_aotInfo = EMPTY_AOTInfo;
    const AOTInfo avmglue_aotInfo = EMPTY_AOTInfo;
    const uint32_t abcNVectorInstances = 0;
    __attribute__ ((visibility("default"))) LLVMVectorInstance abcVectorInstances[] = {};
    const uint32_t g_aotSamplerEnabled = 0;
    LLVMUnusedParam const g_unusedParam = 0;
}

extern "C" __attribute__ ((visibility("default"))) const struct
{
    const char* n_Atom;
    LLVMAtom* m_Atom;

    const char* n_Boolean;
    LLVMBool* m_Boolean;

    const char* n_int;
    int32_t* m_int;

    const char* n_uint;
    uint32_t* m_uint;

    const char* n_ushort;
    uint16_t* m_ushort;
    
    const char* n_MethodEnv;
    MethodEnv* m_MethodEnv;

    const char* n_AbcEnv;
    AbcEnv* m_AbcEnv;

    const char* n_Multiname;
    Multiname* m_Multiname;

    const char* n_Namespace;
    Namespace* m_Namespace;

    const char* n_NamespaceId;
    LLVMNamespaceId* m_NamespaceId;

    const char* n_Number;
    double* m_Number;

    const char* n_ScriptObject;
    ScriptObject* m_ScriptObject;

    const char* n_ArrayObject;
    ArrayObject* m_ArrayObject;

    const char* n_QNameObject;
    QNameObject* m_QNameObject;

    const char* n_String;
    String* m_String;

    const char* n_CompiledHandler;
    AvmThunkNativeFunctionHandler m_CompiledHandler;

    const char* n_ExceptionFrame;
    ExceptionFrame* m_ExceptionFrame;

    const char* n_CallStackNode;
    CallStackNode* m_CallStackNode;

    const char* n_Traits;
    Traits* m_traits;

    const char* n_TraitsId;
    LLVMTraitsId* m_traitsId;

    const char* n_aotInfo;
    AOTInfo* m_aotInfo;

    const char* n_LLVMUnusedParam;
    LLVMUnusedParam* m_UnusedParam;

    const char* n_MultinameIndex;
    LLVMMultinameIndex* m_MultinameIndex;

    const char* n_MultinameIndexMaybeInt;
    LLVMMultinameIndexMaybeInt* m_MultinameIndexMaybeInt;

    const char* n_LLVMMultinamePtrMaybeInt;
    LLVMMultinamePtrMaybeInt* m_LLVMMultinamePtrMaybeInt;

    const char* n_MethodDebugInfo;
    MethodDebugInfo* m_MethodDebugInfo;

    const char* n_char;
    char* m_char;

    const char *n_AOTMethodFrame;
    AOTMethodFrame *m_AOTMethodFrame;

    const char *n_AOTLazyEvalInfo;
    AOTLazyEvalInfo *m_AOTLazyEvalInfo;

    const char* n_AtomWB;
    AtomWB* m_AtomWB;

    const char* n_StringWB;
    MMgc::WriteBarrierRC<String*>* m_StringWB;

    const char* n_NamespaceWB;
    MMgc::WriteBarrierRC<Namespace*>* m_NamespaceWB;

    const char* n_ScriptObjectWB;
    MMgc::WriteBarrierRC<ScriptObject*>* m_ScriptObjectWB;

    const char* n_AOTActivationInfo;
    AOTActivationInfo* m_AOTActivationInfo;

    const char* n_DoubleVectorObject;
    DoubleVectorObject* m_DoubleVectorObject;

    const char* n_IntVectorObject;
    IntVectorObject* m_IntVectorObject;

    const char* n_UIntVectorObject;
    UIntVectorObject* m_UIntVectorObject;

    const char* n_ObjectVectorObject;
    ObjectVectorObject* m_ObjectVectorObject;

    const char* n_InterfaceBindingFunction;
    InterfaceBindingFunction m_InterfaceBindingFunction;

}  abcTypes =
{
    "Atom", 0,
    "Boolean", 0,
    "int", 0,
    "uint", 0,
    "ushort", 0,
    "MethodEnv", 0,
    "AbcEnv", 0,
    "Multiname", 0,
    "Namespace", 0,
    "NamespaceId", 0,
    "Number", 0,
    "ScriptObject", 0,
    "ArrayObject", 0,
    "QNameObject", 0,
    "String", 0,
    "CompiledHandler", 0,
    "ExceptionFrame", 0,
    "CallStackNode", 0,
    "Traits", 0,
    "TraitsId", 0,
    "AOTInfo", 0,
    "UnsedParam", 0,
    "MultinameIndex", 0,
    "MultinameIndexMaybeInt", 0,
    "MultinamePtrMaybeInt", 0,
    "MethodDebugInfo", 0,
    "char", 0,
    "AOTMethodFrame", 0,
    "AOTLazyEvalInfo", 0,
    "AtomWB", 0,
    "StringWB", 0,
    "NamespaceWB", 0,
    "ScriptObjectWB", 0,
    "AOTActivationInfo", 0,
    "DoubleVectorObject", 0,
    "IntVectorObject", 0,
    "UIntVectorObject", 0,
    "ObjectVectorObject", 0,
    "InterfaceBindingFunction", 0,
};

namespace avmplus
{
    // Stub out the interpreter
    uintptr_t interpGPR(MethodEnv* method, int /*argc*/, uint32_t */*ap*/)
    {
        method->toplevel()->throwVerifyError(kCorruptABCError);
        return 0;
    }

    double interpFPR(MethodEnv* method, int /*argc*/, uint32_t */*ap*/)
    {
        method->toplevel()->throwVerifyError(kCorruptABCError);
        return 0;
    }

    Atom interpBoxed(MethodEnv* method, int /*argc*/, Atom* /*ap*/)
    {
        method->toplevel()->throwVerifyError(kCorruptABCError);
        return 0;
    }
}

#ifdef VMCFG_BUFFER_GUARD
AOTLazyEvalGuard::AOTLazyEvalGuard(const avmplus::AbcEnv *abcEnv)
: LoadRedirGuard(abcEnv->pool()->aotInfo->nLazyEvals)
, m_abcEnv(abcEnv)
{
    PoolObject *pool = m_abcEnv->pool();
    if(pool->aotRoot == NULL) {
        const AOTInfo *info = pool->aotInfo;
        AvmAssert(info != NULL);
        pool->aotRoot = new MMgc::GCRoot(pool->core->gc, m_abcEnv->pool()->aotInfo->lazyEvalVals, m_abcEnv->pool()->aotInfo->nLazyEvals * sizeof(uintptr_t));
        for(uint32_t n = 0; n < info->nLazyEvals; n++)
            info->lazyEvalValPtrs[n] = getNextRedirPtr();
    } else {
        // Currently we require a one-one mapping of AbcEnv to
        // PoolObject. We should only reach this codepath when
        // constructing "empty" AbcEnvs used by media loading
        // operations. We set the abcEnv to NULL just to make it
        // clear that this guard object should never be used.
        m_abcEnv = NULL;
    }
}

uintptr_t *AOTLazyEvalGuard::redirectLoad(uintptr_t *faultAddr)
{
    AvmAssert(m_abcEnv != NULL);
    const AOTInfo *info = m_abcEnv->pool()->aotInfo;
    int lazyEvalNum = faultAddr - getFirstRedirPtr();
    uintptr_t *redir = info->lazyEvalVals + lazyEvalNum;
    // next time, get the right value
    if(info->lazyEvalValPtrs[lazyEvalNum] != redir)
    {
        // printf("fixing %p to %p...\n", faultAddr, redir);
        const AOTLazyEvalInfo &evalInfo = info->lazyEvalInfos[lazyEvalNum];
        avmplus::MethodEnv *env = m_abcEnv->getMethod(evalInfo.methodID);
        // fprintf(stderr, "methodId: %d leNum: %d (%p) %p\n", evalInfo.methodID, lazyEvalNum, faultAddr, env);
        AvmAssert(env != 0);
        *redir = evalInfo.func(env, evalInfo.funcData);
        // set this after computation in case of recursion
        info->lazyEvalValPtrs[lazyEvalNum] = redir;
        // printf("fixed %p to %p...\n", faultAddr, redir);
    }
    return redir;
}
#endif

extern "C" void llvm_unwind() {
    // This implementation gets replaced by one generated
    // by the LLVM Emitter. It exists simply to make it
    // possible to complete a full compile and link of
    // avmshell with no compiled code.

    printf("Error: this should never be called.\n");
    AvmAssert(false);
}

//
// Provide "noinline" definitions of these common values.
// The idea here is to delay inlining until AFTER the stubs are inlined.
// Then LLVM can recognize the opportunity to CSE them up to the top of the function
// and then reuse the values rather than fetching them from scratch each time.
// A final pass marks them inline and inlines them.
//
__attribute__((noinline)) AvmCore* MethodEnv::core() const
{
    return method->pool()->core;
}

__attribute__((noinline)) ScopeChain* MethodEnv::scope() const
{
    return _scope;
}

__attribute__((noinline)) Toplevel* MethodEnv::toplevel() const
{
    return vtable()->toplevel();
}

__attribute__((noinline)) VTable* MethodEnv::vtable() const
{
    return _scope->vtable();
}

__attribute__((noinline)) PoolObject* MethodInfo::pool() const
{
    return _pool;
}

__attribute__((noinline)) MMgc::GC* VTable::gc() const
{
    return traits->core->GetGC();
}

__attribute__((noinline)) AvmCore* VTable::core() const
{
    return traits->core;
}

__attribute__((noinline)) Toplevel* VTable::toplevel() const
{
    return _toplevel;
}

__attribute__((noinline)) VTable* ScopeChain::vtable() const
{
    return _vtable;
}

__attribute__((noinline)) MMgc::GC *MMgc::GCRoot::GetGC() const
{
    return gc;
}

__attribute__((noinline)) PoolObject* aotGetPool(const MethodEnv* env)
{
    return env->method->pool();
}

__attribute__((noinline)) MMgc::GC* aotGetGC(const MethodEnv* env)
{
    return env->core()->GetGC();
}

__attribute__((noinline)) String* aotGetString(const MethodEnv* env, int32_t index)
{
    return aotGetPool(env)->getString(index);
}

#endif //VMCFG_AOT
