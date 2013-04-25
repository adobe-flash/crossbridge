/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_exec__
#define __avmplus_exec__

#include "exec-osr.h"

namespace nanojit {
  class CodeList; // Forward declaration for JITObserver
}

namespace avmplus {

/**
 *  Execution manager pure virtual interface.  An execution manager implementation
 *  is responsible for all aspects of AS3 execution, including invocation,
 *  policy decisions for how to execute, when to verify, and when to translate,
 *  if needed.
 *
 *  Although some configurations may only have one concrete implementation,
 *  we use ordinary polymorphism instead of build-time polymorphism, to keep
 *  things simple.
 */
class ExecMgr
{
public:
    virtual ~ExecMgr() {}

    /** Called when a MethodInfo is allocated. */
    virtual void init(MethodInfo*, const NativeMethodInfo*) = 0;

    /** Called when a MethodEnv is allocated. */
    virtual void init(MethodEnv*) = 0;

    /** Called after MethodInfo::resolveSignatures() completes */
    virtual void notifyMethodResolved(MethodInfo*, MethodSignaturep) = 0;

    /** Called after VTable::resolveSignatures() completes. */
    virtual void notifyVTableResolved(VTable*) = 0;

    /** Called after prepareActionPool completes. */
    virtual void notifyAbcPrepared(Toplevel*, AbcEnv*) = 0;

    /** Invoke a function apply-style, by unpacking arguments from an array */
    virtual Atom apply(MethodEnv*, Atom thisArg, ArrayObject* a) = 0;

    /** Invoke a function call-style, with thisArg passed explicitly */
    virtual Atom call(MethodEnv*, Atom thisArg, int32_t argc, Atom* argv) = 0;

#ifdef VMCFG_HALFMOON
    /** revert compiled method to interpreted state */
    // TODO: this only makes sense for compiled methods
    virtual void deoptimize(MethodEnv* env) = 0;
#endif
    
};

/**
 * Execution Engine run modes.  All are defined, but support depends
 * on build-time and run-time flags.
 */
enum Runmode {
    RM_mixed,       /** Hybrid mode, governed by OSR threshold */
    RM_jit_all,     /** Always JIT */
    RM_interp_all   /** Never JIT */
};

// Signature for method invocation when caller coerces arguments
// and boxes results.  Typically the caller is JIT code.
typedef uintptr_t (*GprMethodProc)(MethodEnv*, int32_t, uint32_t *);
typedef double (*FprMethodProc)(MethodEnv*, int32_t, uint32_t *);
#ifdef VMCFG_FLOAT
typedef float4_t (*VecrMethodProc)(MethodEnv*, int32_t, uint32_t *);
#endif

// Signature for invocation when callee coerces & boxes;
// the caller is calling an unknown function with an unknown signature.
typedef Atom (*AtomMethodProc)(MethodEnv*, int, Atom*);

// Signature for invoking a method early bound via an interface
// type reference.  JIT code passes in the IID of the interface method
// to enable searching for the correct concrete method.
typedef uintptr_t (*GprImtThunkProc)(class ImtThunkEnv*, int argc, uint32_t* args, uintptr_t idd);
typedef double (*FprImtThunkProc)(class ImtThunkEnv*, int argc, uint32_t* args, uintptr_t idd);

// Signature for calling a function closure (OP_call)
typedef Atom (*FunctionProc)(FunctionObject*, int argc, Atom* args);

/**
 * Size of a variable in a JIT stack frame, in bytes.  VARSIZE is large
 * enough to hold double, int, pointers, or Atom on 32-bit or 64-bit cpus.
 * This is an aspect of the JIT implementation, but is defined here because
 * the debugger boxing/unboxing code in MethodInfo needs to know it.
 * Note: for VMCFG_FLOAT we use a function call, to decide the size of a variable
 * per method (if method uses float4, then 16 bytes; otherwise, 8 bytes)
 * Also: instead of actually defining VARSIZE, we define VARSHIFT (either 3 or 4)
 * because shifts are faster than multiplications/divisions, when the second 
 * operand is a variable (and it's trivial to derive 'size' from 'shift', and 
 * we only actually need VARSIZE in asserts - everywhere else we just shift).
 */
#ifdef VMCFG_FLOAT
#define VARSHIFT(ptr) ptr->varShift()
#else
#define VARSHIFT(ptr) 3
#endif

#ifdef VMCFG_GENERIC_FLOAT4
typedef float4_t (*VecrThunkProc)(void* thunk, MethodEnv* env, int32_t argc, uint32_t* argv);

#ifdef DEBUGGER
extern const VecrMethodProc debugEnterVECR_adapter;
#endif
extern const VecrMethodProc verifyEnterVECR_adapter;
extern const VecrThunkProc thunkEnterVECR_adapter;
#endif
    
/**
 * Compute number of bytes needed for the unboxed representation
 * of this argument value when passed on the stack.
 */
int32_t argSize(Traits*);

class MethodRecognizer;

/**
 * Associates debugfile/debugline information with locations in JITted code
 */
struct JITDebugInfo
{
    enum Kind {
        kLine, kFile
    };
    struct Info {
        Info(Kind kind) : kind(kind) {}
        const void* pc; // location in native code.
        Kind kind;      // kLine or kFile
        union {
            int line;
            int file;   // pool index of filename string
        };
        Info* next; // next record.
    };

    JITDebugInfo() : info(0), last(0) {}

    void add(Info* r) {
        r->next = 0;
        if (last)
            last->next = r;
        else
            info = r;
        last = r;
    }

    Info* info;
    Info* last;
};

/** JITObserver is an interface that is notified for each JITted method */
class JITObserver
{
public:
    virtual ~JITObserver() {}
    /**
     * Notify the observer that the given method was just compiled.
     * codeInfo and debugInfo contain code and debug symbol data about the
     * method.  After notifyMethodJITted is called, the codeInfo and debugInfo
     * buffers may be recycled.  The observer must copy out any required info.
     */
    virtual void notifyMethodJITed(MethodInfo* method,
                                   const nanojit::CodeList* code,
                                   JITDebugInfo *debugInfo) = 0;

    virtual void notifyInvokerJITed(MethodInfo* method,
                                    const nanojit::CodeList* code) = 0;
};

/**
 * BaseExecMgr implements for all policies, and encapsulates
 * jit+abc, abc-only, and wordcode-only mechanisms.  This could be improved
 * by factoring into multiple implementations.
 *
 * Extends GCFinalizedObject because instances contain GC object references
 * and have a destructor that needs to run.
 */
class BaseExecMgr: public MMgc::GCFinalizedObject
    , /* implements */ public ExecMgr
{
public:
    BaseExecMgr(AvmCore*);
    virtual ~BaseExecMgr();

    // ExecMgr methods:
    void init(MethodInfo*, const NativeMethodInfo*);
    void init(MethodEnv*);
    void notifyMethodResolved(MethodInfo*, MethodSignaturep);
    void notifyVTableResolved(VTable*);
    void notifyAbcPrepared(Toplevel*, AbcEnv*);
    Atom apply(MethodEnv*, Atom thisArg, ArrayObject* a);
    Atom call(MethodEnv*, Atom thisArg, int32_t argc, Atom* argv);
#ifdef VMCFG_HALFMOON
    void deoptimize(MethodEnv* env);
#endif

private:
    // Helpers to simply return the current implementation:
    static BaseExecMgr* exec(VTable*);
    static BaseExecMgr* exec(MethodEnv*);

    /** True if method's _isInterpImpl flag is set. */
    static bool isInterpreted(MethodEnv*);

    // Trampolines that verify on first call:
    static uintptr_t verifyEnterGPR(MethodEnv*, int32_t argc, uint32_t* args);
    static double verifyEnterFPR(MethodEnv*, int32_t argc, uint32_t* args);
#ifdef VMCFG_FLOAT
public:
    static float4_t verifyEnterVECR(MethodEnv*, int32_t argc, uint32_t* args);
    static float4_t debugEnterExitWrapperV(MethodEnv* env, int32_t argc, uint32_t* argv);
    static float4_t interpVECR(MethodEnv* method, int argc, uint32_t *ap);
    static float4_t initInterpVECR(MethodEnv*, int, uint32_t*);
private:    
#endif
    static Atom verifyInvoke(MethodEnv*, int32_t argc, Atom* args);
    static void verifyOnCall(MethodEnv*); // helper called by verify trampolines

    // Trampolines to call debugEnter/Exit around native methods:
    static uintptr_t debugEnterExitWrapper32(MethodEnv* env, int32_t argc, uint32_t* argv);
    static double debugEnterExitWrapperN(MethodEnv* env, int32_t argc, uint32_t* argv);

    // Trampoline to set MethodEnv->impl to MethodInfo->impl on first call.
    static uintptr_t delegateInvoke(MethodEnv* env, int32_t argc, uint32_t* ap);

    // Interpreter invocation when called by JIT code.  C++ and Interpreter
    // calls to the interpreter go through one of the invoke_interp variants.
    static uintptr_t interpGPR(MethodEnv* method, int argc, uint32_t *ap);
    static double interpFPR(MethodEnv* method, int argc, uint32_t *ap);

    /** General purpose interpreter invocation. */
    static Atom invokeInterp(MethodEnv* env, int32_t argc, Atom* argv);

    /**
     * Invoke the interpreter for a method that does not need to coerce
     * any arguments, either because there are none, or they're all type *.
     **/
    static Atom invokeInterpNoCoerce(MethodEnv* env, int32_t argc, Atom* argv);

    // Stubs used for invoking interpreted constructor methods; these
    // initialize default values of the new object before invoking the
    // interpreter.  See initObj() in exec.cpp.
    static uintptr_t initInterpGPR(MethodEnv*, int, uint32_t*);
    static double initInterpFPR(MethodEnv*, int, uint32_t*);
    static Atom initInvokeInterp(MethodEnv*, int, Atom*);
    static Atom initInvokeInterpNoCoerce(MethodEnv*, int, Atom*);

    /** Set an object's fields to default values. Called by init stubs. */
    static void initObj(MethodEnv* env, ScriptObject* obj);

    /**
     * Generic interpretive invoker for JIT and native methods that
     * iterates over argument types and coerces each one.
     */
    static Atom invokeGeneric(MethodEnv* env, int32_t argc, Atom* argv);

    /** Invoke a native or jit-compiled method and then box the return value. */
    static Atom endCoerce(MethodEnv*, int32_t argc, uint32_t *ap,
                          MethodSignaturep ms);

    /** Check argc and compute the space required for rest args. */
    static size_t startCoerce(MethodEnv*, int32_t argc, MethodSignaturep ms);

    /** Unbox and coerce arguments for ordinary invocation. */
    static void unboxCoerceArgs(MethodEnv*, int32_t argc, Atom* in,
                                uint32_t *ap, MethodSignaturep ms);

    /** Unbox and coerce arguments for invocation via Function.apply(). */
    static void unboxCoerceArgs(MethodEnv*, Atom thisArg, ArrayObject *a,
                                uint32_t *argv, MethodSignaturep ms);

    /** Unbox and coerce arguments for invocation via Function.call(). */
    static void unboxCoerceArgs(MethodEnv*, Atom thisArg, int32_t argc,
                                Atom* in, uint32_t *argv, MethodSignaturep ms);

    /** Coerce and unbox a single argument. */
    static Atom* FASTCALL coerceUnbox1(MethodEnv*, Atom atom, Traits* t, Atom* args);

    /** Just unbox a single argument that is known to be the correct type already. */
    static Atom* FASTCALL unbox1(Atom atom, Traits* t, Atom* args);

    /**
     * Set trampolines and flags for the interpreter, possibly including an
     * initializer trampoline, and possibly counting invocations for OSR.
     */
    void setInterp(MethodInfo*, MethodSignaturep, bool isOsr);

    /** Set trampolines and flags for a native method. */
    void setNative(MethodInfo*, GprMethodProc p);

    /**
     * Verify any kind of method, by sniffing what kind it is and dispatching
     * to the appropriate case.  Called on or before the first invocation of the
     * target method.  Each subcase is responsible for setting up CodeWriters and
     * then ultimately running the verifier by calling verifyCommon().
     */
    void verifyMethod(MethodInfo*, Toplevel*, AbcEnv*);

    /** "Verify" a native method by installing trampolines and flags. */
    void verifyNative(MethodInfo*, MethodSignaturep);

    /** Verify a method and install interpreter trampolines. */
    void verifyInterp(MethodInfo*, MethodSignaturep, Toplevel*, AbcEnv*);

    /**
     * Actually run the verifier with the given CodeWriter chain, and
     * clean up if it throws a VerifyError.
     */
    void verifyCommon(MethodInfo*, MethodSignaturep, Toplevel*, AbcEnv*, CodeWriter*);

    //
    // Support for verifyall mode mainly consists of hooking into the
    // code loading mechanism and verifying early instead of on the first
    // call.  This includes installing trampolines and doing jit-compiltion,
    // if applicable.  verifyall mode supports a "verifyonly" sub-mode whereby
    // no code is actually executed.
    //
    // Verifying early is done in a semi breadth-first order by maintaining
    // two queues:  verifyFunctionQueue, containing MethodInfo's that are
    // resolved and ready to verify, and verifyTraitsQueue, containing types
    // that contain read-to-verify functions
    //
    // Traits are queued when their corresponding OP_newclass or OP_newactivation
    // is encountered while verifying another method.  Functions are added when
    // we encounter OP_newfunction, or when the declaring traits reaches the head
    // of the queue.
    //
    // Methods have their _isVerifyPending flag set when queued, then changed
    // to _isVerified once verified.
    //
    // In verifyall mode we never install the verify-on-first-call trampolines.
    // In verifyonly mode, all execution is stubbed out by a single stub that
    // just returns undefined.
    //

    friend class VerifyallWriter;
    void enqFunction(MethodInfo* f);
    void enqTraits(Traits* t);
    void verifyEarly(Toplevel* toplevel, AbcEnv* abc_env);
    bool isVerified(const MethodInfo*) const;
    bool isVerifyPending(const MethodInfo*) const;
    void setVerified(MethodInfo*) const;
    void setVerifyPending(MethodInfo*) const;

    //
    // Support for JIT Compilation:
    //
    void setupJit(AvmCore*);

    /** Return true if we should eagerly JIT.  False means use interpreter. */
    bool shouldJitFirst(const AbcEnv*, const MethodInfo*, MethodSignaturep) const;

    /** True if the JIT is enabled */
    bool isJitEnabled() const;

    /** Run the verifier with the JIT attached. */
    void verifyJit(MethodInfo*, MethodSignaturep, Toplevel*, AbcEnv*,
                   OSR *osr_state);

    /** Install JIT code pointers and set MethodInfo::_isJitImpl. */
    void setJit(MethodInfo*, GprMethodProc p);

    /**
     * Invoker called on the first invocation then calls invoke_generic,
     * installs jitInvokerNow yielding a 1-call delay before we try to
     * compile the invoker itself.
     */
    static Atom jitInvokerNext(MethodEnv*, int argc, Atom* args);

    /* Compile now then invoke the compiled invoker. */
    static Atom jitInvokerNow(MethodEnv*, int argc, Atom* args);

    // Support for interface method tables (IMTs).  These enable fast
    // dispatching of an interface method when invoked via an interface-
    // typed reference, when we know the method signature but not the
    // vtable index.  Only applies to call sites in JIT code.
    //
    // Each VTable contains a fixed sized array called the interface
    // method table, each entry points to a concrete MethodEnv, resolveImt(),
    // or dispatchImt().  Initially all entries point to resolveImt().
    //
    // Resolution: All interface methods that the concrete type implements
    // are hashed into the IMT by a unique interface method id (IID).  If an IMT slot
    // has no collisions, then the concrete MethodEnv* for the method implementing
    // that interface method is installed.  Otherwise we install dispatchImt().
    //
    // Dispatching: JIT'd call sites pass the IID of the method they early bound
    // to.  If the called slot contains just a concrete MethodEnv, the IID parameter
    // is ignored.  Otherwise we search a table for the correct concrete
    // method and then invoke it.
    //
    // IIDs are simply the pointer to the interface method's MethodInfo.
    // The IMT itself is declared in class ImtHolder, below.  It is private
    // to the execution mechanism but must be allocated in each VTable instance.
    //

    /**
     * Analyze the implemented types then build the ImtEntry table and
     * install dispatchImt() or a concrete MethodEnv*.
     */
    static class ImtThunkEnv* resolveImtSlot(class ImtThunkEnv*, uintptr_t iid);

    // Helpers for resolveImtSlot():
    void resolveImtSlotFromBase(VTable*, uint32_t slot);
    bool resolveImtSlotSelf(VTable*, uint32_t slot);
    void resolveImtSlotFull(VTable*, uint32_t slot);
    static class ImtEntry* buildImtEntries(VTable* vtable, uint32_t slot, uint32_t& count);

    /** Trampoline to resolve this IMT slot then invoke the proper handler. */
    static uintptr_t resolveImt(class ImtThunkEnv* ite, int argc, uint32_t* ap, uintptr_t iid);

    /** Trampoline which searches for the method with a matching IID. */
    static uintptr_t dispatchImt(class ImtThunkEnv* ite, int argc, uint32_t* ap, uintptr_t iid);

#ifdef VMCFG_COMPILEPOLICY
    /**
     * The policy rule objects utilize MethodRecognizers
     * to determine the policy on per method basis.
     */
    typedef UnmanagedPointerList<MethodRecognizer*> PolicyRuleSet;

    /**
     * This RuleSet object houses two sets of rules, one for jit'ing
     * and one for interp.  No attempt is made to determine if the
     * rules overlap and/or conflict in any manner.
     */
    class JitInterpRuleSet : public MMgc::GCFinalizedObject
    {
    public:
        PolicyRuleSet jit;
        PolicyRuleSet interp;

        JitInterpRuleSet(MMgc::GC* gc);
        ~JitInterpRuleSet();
    };

    bool ruleMatch(PolicyRuleSet* rules, const MethodInfo* m) const;
    bool prepPolicyRules();
#endif

#ifdef VMCFG_HALFMOON
public:
    static void resetMethodInvokers(MethodEnv*);
    static void setRecompileWithProfileData(MethodEnv* env);
    static void setRecompileHotMethod(MethodEnv* env);
    static Atom jitInvokerProfiler(MethodEnv* env, int argc, Atom* args);
    static Atom jitInvokerWithProfileData(MethodEnv* env, int argc, Atom* args);
    static Atom executeMethod(MethodEnv*, int argc, Atom* args);
    static void freeJitCompiledCode(MethodInfo*);
    bool verifyOptimizeJit(MethodInfo* m, MethodSignaturep ms,
                           Toplevel*, AbcEnv*, OSR*);
    void verifyProfilingJit(MethodInfo*, MethodSignaturep, Toplevel*, AbcEnv*);
#endif

private:
    AvmCore* core;
    const struct Config& config;
#ifdef VMCFG_COMPILEPOLICY
    JitInterpRuleSet* _ruleSet;
#endif
#ifdef VMCFG_VERIFYALL
    GCList<MethodInfo> verifyFunctionQueue;
    GCList<Traits> verifyTraitsQueue;
#endif
#ifdef VMCFG_NANOJIT
    friend class OSR;
    friend class CodegenLIR;
    friend class halfmoon::JitFriend;
    friend class LirHelper;
    OSR *current_osr;
    JITObserver *jit_observer; // Current JITObserver or NULL if not profiling.
#endif
};

/**
 * CodeWriter instance to hook into opcodes OP_newfunction, OP_newclass, and
 * OP_newactivation so we can populate verifyFunctionQueue and
 * verifyTraitsQueue in verifyall mode.
 */
class VerifyallWriter : public NullWriter
{
    PoolObject* pool;
    BaseExecMgr* exec;
public:
    VerifyallWriter(MethodInfo*, BaseExecMgr*, CodeWriter*);
    void write(const FrameState*, const uint8_t *pc, AbcOpcode, Traits*);
    void writeOp1(const FrameState*, const uint8_t *pc, AbcOpcode, uint32_t opd1, Traits*);
};

/**
 * Base class for MethodInfo which contains invocation pointers.  These
 * pointers are private to the ExecMgr instance and hence declared here.
 */
class GC_CPP_EXACT(MethodInfoProcHolder, MMgc::GCTraceableObject)
{
    friend class ImtThunkEnv;
    friend class InvokerCompiler;
    friend class BaseExecMgr;
    friend class MethodEnv;
    friend class OSR;

protected:
    MethodInfoProcHolder();

    GC_DATA_BEGIN(MethodInfoProcHolder)

private:
    union {
        GprMethodProc  _implGPR;
        FprMethodProc  _implFPR;
        FLOAT_ONLY(VecrMethodProc _implVECR;)
    };
    /** pointer to invoker used when callee must coerce args. */
    AtomMethodProc _invoker;

    GC_DATA_END(MethodInfoProcHolder)
};

/**
 * Base class for MethodEnv and ImtThunkEnv, containing copies of the
 * trampoline from MethodInfo.  In JIT configurations this saves one
 * load along the call fast path.  Calls from C++ or the Interpreter
 * always go through MethodInfo._invoker.
 */
class GC_CPP_EXACT(MethodEnvProcHolder, MMgc::GCTraceableObject)
{
    friend class CodegenLIR;
    friend class BaseExecMgr;
    friend class OSR;
    friend class halfmoon::JitFriend;

protected:
    MethodEnvProcHolder();

    GC_NO_DATA(MethodEnvProcHolder)

private:
    union {
        GprMethodProc   _implGPR;
        FprMethodProc   _implFPR;
        FLOAT_ONLY(VecrMethodProc  _implVECR;)
        GprImtThunkProc _implImtGPR;
    };

#ifdef VMCFG_AOT
public:
    inline GprMethodProc implGPR() const { return _implGPR; }
    inline FprMethodProc implFPR() const { return _implFPR; }
#endif

};

/**
 * Holder for interface method dispatch table within class VTable,
 * declared here because it is private to the ExecMgr implementation.
 */
class ImtHolder : public MMgc::GCInlineObject
{
    friend class BaseExecMgr;
    friend class CodegenLIR;
    friend class halfmoon::JitFriend;

    // IMT_SIZE should be a number that is relatively prime to sizeof(MethodInfo)/8
    // since we use the MethodInfo pointer as the interface method id (IID).
    // smaller = dense table, few large conflict tables
    // larger  = sparse table, many small conflict tables

    #ifdef _DEBUG
    static const uint32_t IMT_SIZE = 3; // Good for testing all code paths.
    #else
    static const uint32_t IMT_SIZE = 7; // Marginally better for speed/size tradeoff.
    #endif

    static uintptr_t getIID(MethodInfo*);    // Return a full IID for the given interface method.
    static uint32_t hashIID(uintptr_t iid);  // Hash the IID into an IMT slot number.
    static uint32_t hashIID(MethodInfo*);    // Hash the method's IID into an IMT slot number.
    class ImtThunkEnv* entries[IMT_SIZE];

public:
    void gcTrace(MMgc::GC* gc) {
        gc->TraceLocations((void**)entries, IMT_SIZE);
    }
};

#if defined(_MSC_VER) && defined(AVMPLUS_IA32)
// These macros are used for dynamically aligning our
// stack before calling into our JITed code.  By aligning our
// stack to an 8 byte boundary before calling into our JITed
// code, we will keep all double access aligned and improve performance.
#define STACKADJUST() \
    __asm \
    { \
        __asm push ecx \
        __asm mov ecx, esp \
        __asm and ecx, 0x4 \
        __asm sub esp, ecx \
        __asm push ecx \
    }

#define STACKRESTORE() \
    __asm \
    { \
        __asm pop ecx \
        __asm add esp, ecx \
        __asm pop ecx \
    }
#else
#define STACKADJUST()
#define STACKRESTORE()
#endif

}
#endif // __avmplus_exec__
