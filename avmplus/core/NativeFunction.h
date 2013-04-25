/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef __avmplus_NativeFunction__
#define __avmplus_NativeFunction__

#ifdef VMCFG_AOT
#include "CdeclThunk.h"
struct AOTInfo;
#endif

namespace avmplus
{
#ifdef _DEBUG
    #define AvmThunk_DEBUG_ONLY(...)    __VA_ARGS__
#else
    #define AvmThunk_DEBUG_ONLY(...)
#endif /* DEBUG */

    // Historically, bools are passed in as int32_t, as some calling conventions
    // would only use a single byte and leave the remainder of the argument word
    // as trash; this was used to ensure it was well-defined. (It's not clear to
    // me if this is still an issue, but easier to maintain the convention than
    // change now.)
    typedef int32_t bool32;

#ifdef VMCFG_AOT
    typedef void (AvmPlusScriptableObject::*AvmThunkNativeMethodHandler)();
    typedef void (*AvmThunkNativeFunctionHandler)(AvmPlusScriptableObject* obj);
#endif

    const uintptr_t kUnboxMask = ~uintptr_t(7);
#ifdef _DEBUG
    extern void FASTCALL check_unbox(MethodEnv* env, bool u);

    REALLY_INLINE uintptr_t _AvmThunkUnbox_AvmReceiver(MethodEnv* env, uintptr_t r)
    {
        check_unbox(env, false);
        AvmAssert((r & ~kUnboxMask) == 0);
        return r;
    }
    #define AvmThunkUnbox_AvmReceiver(t,r)      ((t)(_AvmThunkUnbox_AvmReceiver(env, uintptr_t(r))))

    REALLY_INLINE uintptr_t _AvmThunkUnbox_AvmAtomReceiver(MethodEnv* env, uintptr_t r)
    {
        check_unbox(env, true);
        return r & kUnboxMask;
    }
    #define AvmThunkUnbox_AvmAtomReceiver(t,r)      ((t)(_AvmThunkUnbox_AvmAtomReceiver(env, uintptr_t(r))))
#else
    #define AvmThunkUnbox_AvmReceiver(t,r)      ((t)(uintptr_t(r)))
    #define AvmThunkUnbox_AvmAtomReceiver(t,r)  ((t)(uintptr_t(r) & kUnboxMask))
#endif

    #define AvmThunkUnbox_OBJECT(t,r)         ((t)(r))
    #define AvmThunkUnbox_BOOLEAN(t,r)        ((r) != 0)
    #define AvmThunkUnbox_INT(t,r)            int32_t(r)
    #define AvmThunkUnbox_UINT(t,r)           uint32_t(r)
    #define AvmThunkUnbox_NAMESPACE(t,r)      ((t)(r))
    #define AvmThunkUnbox_ATOM(t,r)           ((t)(r))
    #define AvmThunkUnbox_STRING(t,r)         ((t)(r))
    #define AvmThunkUnbox_VOID(t,r)           (error ??? illegal)
    #define AvmThunkUnbox_DOUBLE(t,r)         AvmThunkUnbox_double_impl(&(r))
    #define AvmThunkUnbox_FLOAT(t,r)         (*(const t*)(&(r)))
    #define AvmThunkUnbox_FLOAT4(t,r)        (*(const t*)(&(r)))

    #define AvmThunkArgSize_OBJECT          1
    #define AvmThunkArgSize_BOOLEAN         1
    #define AvmThunkArgSize_INT             1
    #define AvmThunkArgSize_UINT            1
    #define AvmThunkArgSize_NAMESPACE       1
    #define AvmThunkArgSize_ATOM            1
    #define AvmThunkArgSize_STRING          1
    #define AvmThunkArgSize_VOID            (error ??? illegal)
    #define AvmThunkArgSize_FLOAT           1
#ifdef AVMPLUS_64BIT
    #define AvmThunkArgSize_DOUBLE          1
    #define AvmThunkArgSize_FLOAT4          2
#else
    #define AvmThunkArgSize_DOUBLE          2
    #define AvmThunkArgSize_FLOAT4          4
#endif

    REALLY_INLINE double AvmThunkUnbox_double_impl(const Atom* b)
    {
    #if defined(AVMPLUS_64BIT)
        MMGC_STATIC_ASSERT(sizeof(Atom) == sizeof(double));
        return *(const double*)b;
    #elif defined(VMCFG_UNALIGNED_FP_ACCESS)
        MMGC_STATIC_ASSERT(sizeof(Atom)*2 == sizeof(double));
        return *(const double*)b;
    #else
        MMGC_STATIC_ASSERT(sizeof(Atom)*2 == sizeof(double));
        double_overlay u;
        u.bits32[0] = b[0];
        u.bits32[1] = b[1];
        return u.value;
    #endif
    }

    // trick, since values are compile-time known we usually don't need to call intToAtom, can statically transform them
    // good for ints and ints currently
    #define AvmThunkCanBeSmallIntAtom(v)    (!((v) & 0xF0000000))
    #define AvmThunkSmallIntAtom(v)         ((((Atom)(v))<<3) | kIntptrType)

    // note, this isn't complete -- only the ones currently needed are defined.
    // expand as necessary. macros to take advantage of the fact that most
    // args are compile-time constants.
    #define AvmThunkCoerce_INT_DOUBLE(v)        double(v)
    #define AvmThunkCoerce_INT_UINT(v)          uint32_t(v)
    #define AvmThunkCoerce_INT_ATOM(v)          (AvmThunkCanBeSmallIntAtom(v) ? AvmThunkSmallIntAtom(v) : env->core()->intAtom(v))

    #define AvmThunkCoerce_UINT_DOUBLE(v)       double(v)
    #define AvmThunkCoerce_UINT_INT(v)          int32_t(v)
    #define AvmThunkCoerce_UINT_ATOM(v)         (AvmThunkCanBeSmallIntAtom(v) ? AvmThunkSmallIntAtom(v) : env->core()->intAtom(v))

    #define AvmThunkCoerce_BOOLEAN_ATOM(v)      ((v) ? trueAtom : falseAtom)

    #define AvmThunkCoerce_ATOM_DOUBLE(v)       (MathUtils::kNaN)
    #define AvmThunkCoerce_ATOM_STRING(v)       (NULL)
    #define AvmThunkCoerce_ATOM_OBJECT(v)       (NULL)

    #define AvmThunkCoerce_STRING_ATOM(v)       ((v) ? (v)->atom() : nullStringAtom)

    #define AvmThunkGetConstantString(v)        (env->method->pool()->getString(v))

#ifdef VMCFG_AOT
    union AvmThunkNativeHandler
    {
        AvmThunkNativeMethodHandler method;
        AvmThunkNativeFunctionHandler function;
    };
#endif

    struct NativeMethodInfo
    {
    public:
#ifdef VMCFG_AOT
        AvmThunkNativeHandler handler;
#endif
        GprMethodProc thunker;
#if defined(VMCFG_TELEMETRY_SAMPLER) && !defined(VMCFG_AOT)
		GprMethodProc samplerThunker;
#endif
        int32_t method_id;
    };

    struct NativeClassInfo
    {
    public:
        CreateClassClosureProc createClassClosure;
        int32_t class_id;
        uint16_t sizeofClass;
        uint16_t offsetofSlotsClass;
        uint16_t sizeofInstance;
        uint16_t offsetofSlotsInstance;
        bool hasCustomConstruct;
        bool isRestrictedInheritance;
        bool isAbstractBase;
    };


    // ---------------

    class NativeInitializer
    {
    public:
        NativeInitializer(AvmCore* core,
            char const* const* versioned_uris,
            #ifdef VMCFG_AOT
                const AOTInfo *aotInfo,
            #else
                const uint8_t* abcData,
                uint32_t abcDataLen,
            #endif
            uint32_t methodCount,
            uint32_t classCount);

        ~NativeInitializer();

        PoolObject* parseBuiltinABC(Domain* domain);

        const NativeMethodInfo* getNativeInfo(uint32_t i) const { return get_method(i); }

        #ifdef VMCFG_AOT
            bool getCompiledInfo(NativeMethodInfo *info, AvmThunkNativeHandler* handlerOut, Multiname &returnTypeName, uint32_t i) const;
            bool hasBuiltins() const { return methodCount || classCount; }
            const AOTInfo* get_aotInfo() const { return aotInfo; }
        #endif

            void fillInMethods(const NativeMethodInfo* methodEntry);
            void fillInClasses(const NativeClassInfo* classEntry);
            #ifdef VMCFG_AOT
                REALLY_INLINE const NativeClassInfo* get_class(uint32_t i) const
                {
                    return i < classCount ? classes[i] : 0;
                }
            #else
                REALLY_INLINE const NativeClassInfo* get_class(uint32_t i) const { AvmAssert(i < classCount); return classes[i]; }
            #endif

    private:
            typedef const NativeMethodInfo* MethodType;
            typedef const NativeClassInfo* ClassType;
            REALLY_INLINE const NativeMethodInfo* get_method(uint32_t i) const { AvmAssert(i < methodCount); return methods[i]; }

    private:
        AvmCore* const                          core;
        char const* const*                      versioned_uris;
        const uint8_t* const                    abcData;
        uint32_t const                          abcDataLen;
        MethodType* const                       methods;
        ClassType* const                        classes;
        const uint32_t                          methodCount;
        const uint32_t                          classCount;
        #ifdef VMCFG_AOT
        const AOTInfo*                          aotInfo;
        const AvmThunkNativeFunctionHandler*    compiledMethods;
        const uint32_t                          compiledMethodCount;
        #endif
    };

#ifdef VMCFG_AOT
    #define _NATIVE_METHOD_CAST_PTR(CLS, PTR) \
        reinterpret_cast<AvmThunkNativeMethodHandler>((void(CLS::*)())(PTR))
 
    #define AVMTHUNK_DECLARE_NATIVE_INITIALIZER(NAME) \
        extern PoolObject* initBuiltinABC_##NAME(AvmCore* core, Domain* domain); \
        extern const NativeClassInfo NAME##_classEntries[]; \
        extern const NativeMethodInfo NAME##_methodEntries[];
#else
    #define AVMTHUNK_DECLARE_NATIVE_INITIALIZER(NAME) \
        extern PoolObject* initBuiltinABC_##NAME(AvmCore* core, Domain* domain);
#endif

    #define AVMTHUNK_BEGIN_NATIVE_TABLES(NAME)
    #define AVMTHUNK_END_NATIVE_TABLES()

    // ---------------

#ifdef VMCFG_AOT
    #define AVMTHUNK_BEGIN_NATIVE_METHODS(NAME) \
        const NativeMethodInfo NAME##_methodEntries[] = {
#else
    #define AVMTHUNK_BEGIN_NATIVE_METHODS(NAME) \
        static const NativeMethodInfo NAME##_methodEntries[] = {
#endif

#ifdef VMCFG_AOT
    #define _AVMTHUNK_NATIVE_METHOD(CLS, METHID, IMPL) \
        { { _NATIVE_METHOD_CAST_PTR(CLS, &IMPL) }, (GprMethodProc)avmplus::NativeID::METHID##_thunk, avmplus::NativeID::METHID },
#else
#ifdef VMCFG_TELEMETRY_SAMPLER
	#define _AVMTHUNK_NATIVE_METHOD(CLS, METHID, IMPL) \
		{ (GprMethodProc)avmplus::NativeID::METHID##_thunk, (GprMethodProc)avmplus::NativeID::METHID##_sampler_thunk, avmplus::NativeID::METHID },
#else
    #define _AVMTHUNK_NATIVE_METHOD(CLS, METHID, IMPL) \
        { (GprMethodProc)avmplus::NativeID::METHID##_thunk, avmplus::NativeID::METHID },
#endif
#endif

    #define AVMTHUNK_NATIVE_METHOD(METHID, IMPL) \
        _AVMTHUNK_NATIVE_METHOD(ScriptObject, METHID, IMPL)

    #define AVMTHUNK_NATIVE_METHOD_STRING(METHID, IMPL) \
        _AVMTHUNK_NATIVE_METHOD(avmplus::String, METHID, IMPL)

    #define AVMTHUNK_NATIVE_METHOD_NAMESPACE(METHID, IMPL) \
        _AVMTHUNK_NATIVE_METHOD(avmplus::Namespace, METHID, IMPL)

#ifdef VMCFG_AOT
    // AOT build env is ok with designated inits
    #define AVMTHUNK_NATIVE_FUNCTION(METHID, IMPL) \
        { { function: reinterpret_cast<AvmThunkNativeFunctionHandler>(IMPL) }, (GprMethodProc)avmplus::NativeID::METHID##_thunk, avmplus::NativeID::METHID },
    #define AVMTHUNK_END_NATIVE_METHODS() \
        { { NULL }, NULL, -1 } };
#else
#ifdef VMCFG_TELEMETRY_SAMPLER
	#define AVMTHUNK_NATIVE_FUNCTION(METHID, IMPL) \
		{ (GprMethodProc)avmplus::NativeID::METHID##_thunk, (GprMethodProc)avmplus::NativeID::METHID##_sampler_thunk, avmplus::NativeID::METHID },
	#define AVMTHUNK_END_NATIVE_METHODS() \
		{ NULL, NULL, -1 } };
#else
    #define AVMTHUNK_NATIVE_FUNCTION(METHID, IMPL) \
        { (GprMethodProc)avmplus::NativeID::METHID##_thunk, avmplus::NativeID::METHID },
    #define AVMTHUNK_END_NATIVE_METHODS() \
        { NULL, -1 } };
#endif    
#endif

    // ---------------

    #define AVMTHUNK_BEGIN_NATIVE_CLASSES(NAME) \
        const NativeClassInfo NAME##_classEntries[] = {

    #define AVMTHUNK_NATIVE_CLASS(CLSID, CLS, FQCLS, OFFSETOFSLOTSCLS, INST, OFFSETOFSLOTSINST, CUSTOMCONSTRUCT, RESTRICTEDINHERITANCE, ABSTRACTBASE) \
        { FQCLS::createClassClosure,\
          avmplus::NativeID::CLSID,\
          sizeof(FQCLS),\
          OFFSETOFSLOTSCLS,\
          sizeof(INST),\
          OFFSETOFSLOTSINST,\
          CUSTOMCONSTRUCT,\
          RESTRICTEDINHERITANCE,\
          ABSTRACTBASE\
        },

    #define AVMTHUNK_END_NATIVE_CLASSES() \
        { NULL, -1, 0, 0, 0, 0, false, false, false } };

#ifdef VMCFG_AOT
    #define AVMTHUNK_DEFINE_NATIVE_INITIALIZER(NAME)
#else
    #define AVMTHUNK_DEFINE_NATIVE_INITIALIZER(NAME) \
        PoolObject* initBuiltinABC_##NAME(AvmCore* core, Domain* domain) { \
            NativeInitializer ninit(core, \
                avmplus::NativeID::NAME##_versioned_uris, \
                avmplus::NativeID::NAME##_abc_data, \
                avmplus::NativeID::NAME##_abc_length, \
                avmplus::NativeID::NAME##_abc_method_count, \
                avmplus::NativeID::NAME##_abc_class_count); \
            ninit.fillInClasses(NAME##_classEntries); \
            ninit.fillInMethods(NAME##_methodEntries); \
            return ninit.parseBuiltinABC(domain); \
        }
#endif

    #define AVM_INIT_BUILTIN_ABC_IN_DOMAIN(MAPNAME, CORE, DOMAIN) \
        avmplus::NativeID::initBuiltinABC_##MAPNAME((CORE), (DOMAIN))

    #define AVM_INIT_BUILTIN_ABC(MAPNAME, CORE) \
        avmplus::NativeID::initBuiltinABC_##MAPNAME((CORE), (CORE)->builtinDomain)

    class ClassManifestBase : public MMgc::GCTraceableObject
    {
    public:
        avmplus::ScriptEnv* env() const { return _env; }
    protected:
        uint32_t const _count;
        avmplus::ScriptEnv* const _env;         // const non-RC, so no WB needed
        avmplus::ClassClosure* _classes[1];     // lying, really [_count]. written with explicit WBRCs.
    protected:
        REALLY_INLINE ClassManifestBase(uint32_t count, avmplus::ScriptEnv* e) : _count(count), _env(e) { }
        ClassClosure* FASTCALL lazyInitClass(uint32_t class_id);
        void fillInClass(uint32_t class_id, ClassClosure* c);
        virtual bool gcTrace(MMgc::GC* gc, size_t);
    };
}

#endif /* __avmplus_NativeFunction__ */
