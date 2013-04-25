/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"
#include "MethodInfo.h"

#ifdef VMCFG_CDECL

#include "CdeclThunk.h"

namespace avmplus
{
    // stores arbitrary Avm values without allocation
    class AvmValue
    {
    private:
        enum { _tagBits = 3 };
        double _d __attribute__((aligned(16)));
        Atom _a;

        double* dblPtr()
        {
            return &_d;
        }

        Atom dblAtom()
        {
            return kDoubleType | (Atom)dblPtr();
        }

    public:
        int kind()
        {
            return atomKind(_a);
        }

        Atom get(Toplevel* toplevel, Traits* t)
        {
            AvmCore* core = toplevel->core();

            BuiltinType bt = Traits::getBuiltinType(t);

            if ((bt == BUILTIN_any || bt == BUILTIN_object) && _a == dblAtom())
                _a = core->doubleToAtom(*dblPtr());

            // should not be in here if we want a void return type.
            AvmAssert(bt != BUILTIN_void);

            switch(bt)
            {
            case BUILTIN_any:
                return _a;
            case BUILTIN_int:
                return (Atom)AvmCore::integer(_a);
            case BUILTIN_uint:
                return (Atom)AvmCore::toUInt32(_a);
            case BUILTIN_boolean:
                return (Atom)AvmCore::boolean(_a);
            case BUILTIN_string:
                return (Atom)core->coerce_s(_a);
            case BUILTIN_namespace:
                if (atomKind(_a) == kNamespaceType)
                    return (Atom)core->atomToNamespace(_a);
                AvmAssert(AvmCore::isNullOrUndefined(_a));
                return nullNsAtom;
            case BUILTIN_object:
                return AvmCore::isNullOrUndefined(_a) ? nullObjectAtom : _a;
            default:
                AvmAssert(t != NUMBER_TYPE); // use getDouble
                return (Atom)AvmCore::atomToScriptObject(toplevel->coerce(_a, t));
            }
        }

        double getDouble()
        {
            return AvmCore::number(_a);
        }

        void set(Atom a, Traits* t)
        {
            if (!t) {
                _a = a;
            } else {
                switch(Traits::getBuiltinType(t))
                {
                case BUILTIN_void:
                    AvmAssert(a == AtomConstants::undefinedAtom);
                    _a = AtomConstants::undefinedAtom;
                    break;
                case BUILTIN_int:
                    setInt((intptr_t)a);
                    break;
                case BUILTIN_uint:
                    setUint((uintptr_t)a);
                    break;
                case BUILTIN_boolean:
                    _a = a ? trueAtom : falseAtom;
                    break;
                case BUILTIN_string:
                    _a = a ? ((Stringp)a)->atom() : nullStringAtom;
                    break;
                case BUILTIN_namespace:
                    _a = a ? ((Namespace*)a)->atom() : nullNsAtom;
                    break;
                case BUILTIN_object:
                    if (atomKind(a) == kUnusedAtomTag)
                        _a = a ? ((ScriptObject*)a)->atom() : nullObjectAtom;
                    else
                        _a = a;
                    break;
                default:
                    AvmAssert(Traits::getBuiltinType(t) != BUILTIN_number);
                    _a = a ? ((ScriptObject*)a)->atom() : nullObjectAtom;
                }
            }
        }

        void setInt(intptr_t i)
        {
            intptr_t iwt = i << _tagBits;
            if ((iwt >> _tagBits) == i) {
                _a = (iwt | kIntptrType);
            } else {
                *dblPtr() = i;
                _a = dblAtom();
            }
        }

        void setUint(uintptr_t u)
        {
            if (u & (~(static_cast<uintptr_t>(-1) >> (_tagBits + 1)))) {
                *dblPtr() = u;
                _a = dblAtom();
            } else {
                _a = (u << 3) | kIntptrType;
            }
        }

        void setDouble(double d)
        {
            *dblPtr() = d;
            _a = dblAtom();
        }
    };

    enum // we try to stick to 4 bits here instead of the 5 in BUILTIN_xxx
    {
        kVOID = 0,
        kOBJECT,
        kCLASS,
        kFUNCTION,
        kARRAY,
        kSTRING,
        kNUMBER,
        kINT,
        kUINT,
        kBOOLEAN,
        kANY,
        kNAMESPACE,
        kVECTORINT,
        kVECTORUINT,
        kVECTORDOUBLE,
        kVECTOROBJ,
        
        
        
        kLASTENUMELEMENT/// KEEP THIS ONE THE LAST, SO THAT THE ASSERTION WORKS!
    };
    /* using MMGC_STATIC_ASSERT for lack of a better one */
    MMGC_STATIC_ASSERT(kLASTENUMELEMENT <=16); 

    static Traits* argTraitsFromType(const AvmCore* core, int32_t n)
    {
        switch(n)
        {
        case kOBJECT: return OBJECT_TYPE;
        case kCLASS: return CLASS_TYPE;
        case kFUNCTION: return FUNCTION_TYPE;
        case kARRAY: return ARRAY_TYPE;
        case kSTRING: return STRING_TYPE;
        case kNUMBER: return NUMBER_TYPE;
        case kINT: return INT_TYPE;
        case kUINT: return UINT_TYPE;
        case kBOOLEAN: return BOOLEAN_TYPE;
        case kVOID: return VOID_TYPE;
        case kANY: return NULL;
        case kNAMESPACE: return NAMESPACE_TYPE;
        case kVECTORINT: return VECTORINT_TYPE;
        case kVECTORUINT: return VECTORUINT_TYPE;
        case kVECTORDOUBLE: return VECTORDOUBLE_TYPE;
        case kVECTOROBJ: return VECTOROBJ_TYPE;
        }
        AvmAssert(false); // shouldn't happen...
        return NULL;
    }

    // iterates over callee types for a method's signature
    class MethodSigArgDescIter
    {
    protected:
        int32_t m_n;
        const MethodInfo* m_methInfo;
        const MethodSignature* m_methSig;

    public:
        MethodSigArgDescIter(MethodInfo* methInfo) : m_n(0), m_methInfo(methInfo), m_methSig(methInfo->getMethodSignature())
        {
        }

        Traits* nextType()
        {
            if (m_n <= m_methSig->param_count())
                return m_methSig->paramTraits(m_n++);
            AvmCore* core = m_methInfo->pool()->core;
            return VOID_TYPE;
        }

        bool hasRest()
        {
            return m_methInfo->needRest() ? true : false;
        }

        bool needArguments()
        {
            return m_methInfo->needArguments() ? true : false;
        }

        bool needOptionalArgs()
        {
            return true;
        }

        bool isVarArg()
        {
            return false;
        }
    };

    typedef struct _APType* APType; // type doesn't matter.. just don't clash w/ va_list

    class APArgDescIter : public MethodSigArgDescIter
    {
    protected:
        int32_t m_argc;

    public:
        APArgDescIter(int argc, MethodInfo* mi) : MethodSigArgDescIter(mi), m_argc(argc)
        {
#if CDECL_VERBOSE
            if (argc >= 0 && !m_methSig->argcOk(argc))
            {
                AvmCore* core = m_methInfo->pool()->core;

                core->console << "argc bad: " << m_methInfo->format(core) << " : " << argc << "\n";
            }
#endif
            AvmAssert(argc < 0 || m_methSig->argcOk(argc));
        }

        Traits* nextType()
        {
            AvmCore* core = m_methInfo->pool()->core;
#if CDECL_VERBOSE
            core->console << "APArgDescIter::nextType() m_n: " << m_n << "\n";
#endif
            if (m_argc < 0 || m_n <= m_argc)
            {
                if (m_n <= m_methSig->param_count())
                    return MethodSigArgDescIter::nextType();
                else
                {
                    m_n++;
                    return NULL;
                }
            }
            return VOID_TYPE;
        }

        bool hasRest()
        {
            return false;
        }

        bool needArguments()
        {
            return false;
        }

        bool needOptionalArgs()
        {
            return false;
        }

        bool isVarArg() // can just keep pushing atoms
        {
            return true;
        }
    };

    class AtomvArgDescIter
    {
    protected:
        AvmCore* m_core;
        int32_t m_argc;

    public: // TODO is this counting right?
        AtomvArgDescIter(AvmCore* core, int argc = -2) : m_core(core), m_argc(argc) {}

        Traits* nextType()
        {
            if (m_argc == -2)
                return NULL; // all atoms all the time!
            else if (m_argc >= 0)
            {
                m_argc--;
                return NULL;
            }
            AvmCore* core = m_core;
            return VOID_TYPE;
        }

        bool hasRest()
        {
            return false;
        }

        bool needArguments()
        {
            return false;
        }

        bool needOptionalArgs()
        {
            return false;
        }

        bool isVarArg()
        {
            return true;
        }
    };

    // read 4 bit types out of a uintptr_t bitwise
    // 32 bits holds 7 argument descriptions (plus ret type)
    // 64 bits can hold 15 (plus ret type)
    class ImmArgDescIter
    {
    protected:
        uintptr_t m_argDesc;
        AvmCore* m_core;

    public:
        ImmArgDescIter(uintptr_t argDesc, AvmCore* core) : m_argDesc(argDesc), m_core(core)
        {
        }

        // iterates over the types in the argument descriptor
        // VOID signals end of arguments; it is invalid to call nextType again after
        // end of arguments!
        Traits* nextType()
        {
            return argTraitsFromType(m_core, nextTypeKind());
        }

        unsigned nextTypeKind()
        {
            unsigned type = (unsigned)(m_argDesc >> (sizeof(m_argDesc) * 8 - 4));
            m_argDesc <<= 4;
            return type;
        }

        bool hasRest()
        {
            return false;
        }

        bool needArguments()
        {
            return false;
        }

        bool needOptionalArgs()
        {
            return true;
        }

        bool isVarArg()
        {
            return false;
        }
    };

    // read types out of an unsigned char*
    class PtrArgDescIter
    {
    protected:
        // pointer to next byte of arg description
        unsigned char* m_p;
        // currently loaded bits of arg description
        unsigned char m_bitBuf;
        // number of valid bits in m_bitBuf
        unsigned char m_bits;
        AvmCore* m_core;

    public:
        PtrArgDescIter(void* argDesc, AvmCore* core) : m_p((unsigned char* )argDesc), m_bitBuf(0), m_bits(0), m_core(core)
        {
        }

        // iterates over the types in the argument descriptor
        // VOID signals end of arguments; it is invalid to call nextType again after
        // end of arguments!
        Traits* nextType()
        {
            return argTraitsFromType(m_core, nextTypeKind());
        }

        unsigned nextTypeKind()
        {
            if (m_bits == 0)
            {
                m_bitBuf = *m_p++;
                m_bits = 8;
            }

            unsigned type = m_bitBuf >> 4;

            m_bitBuf <<= 4;
            m_bits -= 4;
            return type;
        }

        bool hasRest()
        {
            return false;
        }

        bool needArguments()
        {
            return false;
        }

        bool needOptionalArgs()
        {
            return true;
        }

        bool isVarArg()
        {
            return false;
        }
    };

    // lay out an argument description
    // on x86-32, an argument description is exactly a stack layout
    // on ARM, the first 4 words are r0-r3 and the rest is exactly a stack layout
    // on x86-64, it will no doubt be more complicated...
    class ArgDescLayout
    {
    protected:
        void* m_dst;
#ifdef AVMPLUS_ARM
        void* m_minDst;
#endif

    public:
        // NULL dst is legal for measuring a call layout
        ArgDescLayout(void* dst) : m_dst(dst)
#ifdef AVMPLUS_ARM
        , m_minDst((void*)((uintptr_t)dst + 16)) // must make room for a1-a4 always
#endif
        {
#ifdef AVMPLUS_ARM
            AvmAssert(!(7 & (uintptr_t)dst));
#else
            AvmAssert(!(15 & (uintptr_t)dst));
#endif
        }

        void* argEnd()
        {
#ifdef AVMPLUS_ARM
            return (m_dst > m_minDst) ? m_dst : m_minDst;
#else
            return m_dst;
#endif
        }

        // TODO really just platform int? that's what AtomMethodProc style uses
        int32_t* int32Arg()
        {
            int32_t* result = (int32_t*)m_dst;
            m_dst = (void*)(4 + (uintptr_t)m_dst);
            return result;
        }

        double* doubleArg()
        {
            // TODO: doubles on ARM may need to be aligned, but because we have only
            // two arg sizes (4 and 8 bytes) it won't affect us right now
            double* result = (double* )m_dst;
            m_dst = (void*)(8 + (uintptr_t)m_dst);
            return result;
        }

        void** ptrArg()
        {
            void** result = (void**)m_dst;
            m_dst = (void*)(4 + (uintptr_t)m_dst);
            return result;
        }
    };

    // calculate the size required for the given set of argument types for a cdecl call
    // including MethodEnv
    template <class ARG_TYPE_ITER> static int32_t argDescSize(ARG_TYPE_ITER calleeTypeIter, AvmCore*)
    {
        ArgDescLayout l(NULL);

        for (;;)
        {
            Traits* t = calleeTypeIter.nextType();

            BuiltinType bt = Traits::getBuiltinType(t);

            if (bt == BUILTIN_void) break;

            switch(Traits::getBuiltinType(t))
            {
            case BUILTIN_int:
            case BUILTIN_uint:
            case BUILTIN_boolean:
                l.int32Arg();
                break;
            case BUILTIN_number:
                l.doubleArg();
                break;
            default:
                l.ptrArg();
                break;
            }
        }
        if (calleeTypeIter.needArguments() || calleeTypeIter.hasRest())
        {
            l.ptrArg(); // argv
            l.int32Arg(); // argc
            l.ptrArg(); // rest
        }
        l.ptrArg(); // MethodEnv
        return (int32_t)(uintptr_t)l.argEnd();
    }

    // calculate the size required for the given set of argument types passed "ap" style
    // just figures out the size of the ap struct... doesn't include MethodEnv
    template <class ARG_TYPE_ITER1, class ARG_TYPE_ITER2> static int32_t apArgDescSize(ARG_TYPE_ITER1 callerTypeIter, ARG_TYPE_ITER2 calleeTypeIter, AvmCore* core)
    {
        int32_t size = 0;

        // count "regular" arguments... stored as unaligned native
        for (;;)
        {
            Traits* t = calleeTypeIter.nextType();

            BuiltinType bt = Traits::getBuiltinType(t);

            // no more "regular" arguments? break out
            if (bt == BUILTIN_void) break;

            // no more caller arguments? done
            if (callerTypeIter.nextType() == VOID_TYPE)
                return size;

            switch(Traits::getBuiltinType(t))
            {
            case BUILTIN_int:
            case BUILTIN_uint:
            case BUILTIN_boolean:
                size += sizeof(int32_t);
                break;
            case BUILTIN_number:
                size += sizeof(double);
                break;
            default:
                size += sizeof(void*);
                break;
            }
        }
        // count the rest as pointers (Atoms)
        while (callerTypeIter.nextType() != VOID_TYPE)
            size += sizeof(void*);
        return size;
    }

#if defined(AVMPLUS_IA32)
    // pseudo-cross-compiler macros for inline assembly...
    // gcc/x86 in particular isn't really production grade
#ifdef _WIN32
    // msvc stuff works well inline...
#define ASM_FUNC_BEGIN(R, N, A) static R __declspec(naked) N A {
#define ASM_FUNC_END(N) }
#define ASM1(X) __asm { X }
#define ASM2(X,Y) __asm { X,Y }
#define ASM_CALL(X) __asm { call X }
#define ASM_REDIR(F) __asm { jmp F }
#else
    // gcc doesn't support naked inline functions despite the clear use for them
    // this stuff is hackery to help development -- gcc targets should have .s files for production
#define ASM_FUNC_BEGIN(R, N, A) typedef R (* N##_type)A; static void*  N##_container () { \
        void* result; \
        __asm__ (" mov $L"#N"_astart, %[result]" : [result] "=r" (result)); \
        goto ret; asmlbl: \
        __asm__ (" .intel_syntax noprefix "); \
        __asm__ ("L"#N"_astart: ");
#define ASM_FUNC_END(N) __asm__ (" .att_syntax noprefix "); ret: if (result == 0) goto asmlbl; return result; } N##_type N = (N##_type)N##_container();
#define ASM1(X) __asm__ ( #X );
#define ASM2(X,Y) __asm__ ( #X","#Y );
#define ASM_CALL(X) __asm__ ("call _"#X"\n");
#define ASM_REDIR(F) __asm__ ( "push [_"#F"]\n ret");
#endif
#elif defined(AVMPLUS_ARM)
    // gcc doesn't support naked inline functions despite the clear use for them
    // this stuff is hackery to help development -- gcc targets should have .s files for production
#define ASM_BP __asm__("stmdb sp!, {a1, a2, a3, a4}\n swi 0x80\n ldmia sp!, {a1, a2, a3, a4}")
#define ASM_FUNC_BEGIN(R, N, A) \
        extern R N A; \
        __asm__(".section __TEXT,__text,regular,pure_instructions"); \
        __asm__(".align 2"); \
        __asm__(".globl _"#N" "); \
        __asm__("_"#N": ");
#define ASM_FUNC_END(N)
#define ASM_REDIR(F) __asm__ ( "b _"#F" ");

#endif

    // prototype for an argument coercer
    // env is MethodEnv coercing for
    // callerArgDesc is an opaque description of the arguments to be found via callerArgDesc
    // callerAp is the variadic args passed to coerce32CdeclImmArgDesc, et al
    // calleeArgDescBuf is the argument buffer into which coerced arguments are writter
    // returns a pointer to a function used for return value conversion or NULL is none is needed
    typedef void* (*ArgCoercer)(void* callee, MethodEnv* env, Traits* retTraits, uintptr_t callerArgDesc, void* callerAp, void* calleeArgDescBuf);

    extern "C" {

        // core of the thunking mechanism...
        // doesn't necessarily return an int32_t... the N/double variant is identical
        // -- callee is the cdecl method to invoke
        // -- calleeArgDescBufSize is the amount of space to allocate for the coerced args (and possibly register alloc info for, say, x86/64)
        // -- argCoercer is a callback that does the actual filling of the coerced argument buf
        // -- env is the MethodEnv we're calling (used to get argument traits -- does not make callee redundant as env may have >1 impl)
        // -- calleeArgDesc is an opaque description of the types of variadic arguments in callerAp
        // -- callerAp represents the arguments to coerce
        ASM_FUNC_BEGIN(Atom, coerce32CdeclShim,
                (void* callee, unsigned calleeArgDescBufSize, ArgCoercer argCoercer, MethodEnv* env, Traits* retTraits, uintptr_t callerArgDesc, void* callerAp))
#ifdef AVMPLUS_ARM
        __asm__("stmdb  sp!, {v1, v2, v3, v7, lr}");
        __asm__("mov v7, sp");
        // a1 = callee
        // a2 = calleeArgDescBufSize
        // a3 = argCoercer
        // a4 = env
        // [v7] = retTraits
        // [v7, #4] = callerArgDesc
        // [v7, #8] = callerAp
        __asm__("sub sp, sp, a2"); // make room for args
        __asm__("and sp, sp, #-8"); // double word align
        __asm__("mov v2, a3"); // save off argCoercer
        __asm__("mov v3, a4"); // save off env

        // a1 stays callee
        __asm__("mov a2, a4"); // pass env
        __asm__("ldr a3, [v7, #20]"); // pass retTraits
        __asm__("ldr a4, [v7, #24]"); // pass callerArgDesc
        __asm__("mov v1, sp"); // pass calleeArgDescBuf
        __asm__("stmdb sp!, {v1}");
        __asm__("ldr v1, [v7, #28]"); // pass callerAp
        __asm__("stmdb sp!, {v1}");
        __asm__("mov v1, a1"); // save off callee
        __asm__("bl _via_v2"); // call coercer!
        __asm__("add sp, sp, #8"); // restore stack
        __asm__("mov v2, a1"); // remember returnCoercer
        __asm__("ldmia  sp!, {a1, a2, a3, a4}"); // move first 4 arg words into registers
        __asm__("bl _via_v1"); // call the implementation!
        __asm__("mov sp, v7"); // restore stack
        __asm__("cmp v2, #0"); // maybe call returnCoercer -- a1 and a2 will be the double or a1 will be the 32 and a2 will be a dummy
        __asm__("ldrne a3, [v7, #20]"); // retTraits
        __asm__("movne a4, v3"); // env
        __asm__("blne _via_v2");
        __asm__("ldmia  sp!, {v1, v2, v3, v7, pc}"); // done!
        __asm__("_via_v1: bx v1");
        __asm__("_via_v2: bx v2");
#else
#ifdef _WIN32
        (void)callee;
        (void)calleeArgDescBufSize;
        (void)argCoercer;
        (void)env;
        (void)retTraits;
        (void)callerArgDesc;
        (void)callerAp;
#endif
        ASM1(       push ebp)
        ASM2(       mov ebp, esp)
        ASM1(       push esi)
        // [ebp] = saved ebp
        // [ebp+4] = return address
        // [ebp+8] = callee
        // [ebp+12] = calleeArgDescBufSize
        // [ebp+16] = argCoercer
        // [ebp+20] = env
        // [ebp+24] = retTraits
        // [ebp+28] = callerArgDesc
        // [ebp+32] = callerAp
        ASM2(       sub esp, [ebp+12] ) //  make room for args
        ASM2(       and esp, 0xfffffff0 ) //  16 byte aligned
        ASM2(       mov eax, esp)
        ASM2(       sub esp, 8 ) // 16 byte aligned for call
        ASM1(       push eax ) //  calleeArgDescBuf
        ASM1(       push [ebp+32] ) //  callerAp
        ASM1(       push [ebp+28] ) //  callerArgDesc
        ASM1(       push [ebp+24] ) //  retTraits
        ASM1(       push [ebp+20] ) //  env
        ASM1(       push [ebp+8] ) //  callee
        ASM1(       call [ebp+16] ) //  map args: argCoercer(callee, env, retTraits, callerArgDesc, callerAp, callerArgDescBuf);
        ASM2(       add esp, 32)
        ASM2(       mov esi, eax ) //  save result mapping func
        ASM1(       call [ebp+8] ) //  call method
        ASM2(       lea esp, [ebp-4]) // restore stack
        ASM2(       cmp esi, 0)
        ASM1(       je coerce32CdeclShim_done)
        ASM2(       sub esp, 8 ) // 16 byte aligned for call
        ASM1(       push [ebp+20] ) //  env
        ASM1(       push [ebp+24] ) //  retTraits
        ASM1(       call esi ) //  map return value: retCoercer(retTraits, env)
        ASM2(       add esp, 16)
        ASM1(coerce32CdeclShim_done:)
        ASM1(       pop esi)
        ASM1(       pop ebp)
        ASM1(       ret)
#endif
        ASM_FUNC_END(coerce32CdeclShim)

        ASM_FUNC_BEGIN(double, coerceNCdeclShim, (void* callee,
                unsigned calleeArgDescBufSize, ArgCoercer argCoercer, MethodEnv* env, Traits* retTraits, uintptr_t callerArgDesc, void* callerAp))
#ifdef _WIN32
                (void)callee;
        (void)calleeArgDescBufSize;
        (void)argCoercer;
        (void)env;
        (void)retTraits;
        (void)callerArgDesc;
        (void)callerAp;
#endif
        ASM_REDIR(coerce32CdeclShim) // exact same impl
        ASM_FUNC_END(coerceNCdeclShim)

        // Number => something
        Atom returnCoercerNImpl(double n, Traits* retTraits, MethodEnv* env)
        {
            AvmValue v;

            v.setDouble(n);
            return v.get(env->toplevel(), retTraits);
        }

        // Number => some 32
        ASM_FUNC_BEGIN(Atom, returnCoercerN, (Traits* retTraits, MethodEnv* env))
#ifdef AVMPLUS_ARM
        __asm__("b _returnCoercerNImpl"); // straight through
#else
        ASM1(       push ebp) // this is necessary to keep pthreads happy!
        ASM2(       mov ebp, esp)
        ASM2(       sub esp, 12)
        ASM1(       push [ebp+12]) // env
        ASM1(       push [ebp+8]) // retTraits
        ASM2(       sub esp, 8)
        ASM1(       fstp qword ptr [esp]) // callee will have left a value on the FP stack
        ASM_CALL(returnCoercerNImpl)
        ASM2(       add esp, 28)
        ASM1(       pop ebp)
        ASM1(       ret)
#endif
        ASM_FUNC_END(returnCoercerN)

#ifndef AVMPLUS_ARM
        ASM_FUNC_BEGIN(Atom, returnCoercerNPop, (Traits* retTraits, MethodEnv* env))
        ASM1(       push ebp) // this is necessary to keep pthreads happy!
        ASM2(       mov ebp, esp)
        ASM1(       fstp st(0)) // callee will have left a value on the FP stack
        ASM1(       pop ebp)
        ASM1(       ret)
        ASM_FUNC_END(returnCoercerNPop)
#endif

        // something => Number
        double returnCoercerN32Impl(Atom a, Traits* /*retTraits*/, MethodEnv* env)
        {
            Traits* calleeRT = env->method->getMethodSignature()->returnTraits();
            AvmValue v;

            v.set(a, calleeRT);

            return v.getDouble();
        }

        // some 32 => Number
        ASM_FUNC_BEGIN(double, returnCoercerN32, (Traits* retTraits, MethodEnv* env))
#ifdef AVMPLUS_ARM
        __asm__("mov a2, a3"); // a2 is a dummy
        __asm__("mov a3, a4");
        __asm__("b _returnCoercerN32Impl");
#else
        ASM1(       push ebp) // this is necessary to keep pthreads happy!
        ASM2(       mov ebp, esp)
        ASM1(       push [ebp+12]) // env
        ASM1(       push [ebp+8]) // retTraits
        ASM1(       push eax)
        ASM_CALL(returnCoercerN32Impl) // will push something on the FP stack
        ASM2(       add esp, 12)
        ASM1(       pop ebp)
        ASM1(       ret)
#endif
        ASM_FUNC_END(returnCoercerN32)

        // something => something
        Atom returnCoercer32Impl(Atom a, Traits* retTraits, MethodEnv* env)
        {
            Traits* calleeRT = env->method->getMethodSignature()->returnTraits();
            AvmValue v;

            v.set(a, calleeRT);
            return v.get(env->toplevel(), retTraits);
        }

        // some 32 => some 32
        ASM_FUNC_BEGIN(Atom, returnCoercer32, (Traits* retTraits, MethodEnv* env))
#ifdef AVMPLUS_ARM
        __asm__("mov a2, a3"); // a2 is a dummy
        __asm__("mov a3, a4");
        __asm__("b _returnCoercer32Impl");
#else
        ASM1(       push ebp) // this is necessary to keep pthreads happy!
        ASM2(       mov ebp, esp)
        ASM1(       push [ebp+12]) // env
        ASM1(       push [ebp+8]) // retTraits
        ASM1(       push eax)
        ASM_CALL(returnCoercer32Impl)
        ASM2(       add esp, 12)
        ASM1(       pop ebp)
        ASM1(       ret)
#endif
        ASM_FUNC_END(returnCoercer32)

    }

    // returns any function required to coerce the callee's return type to the
    // caller's desired return type
    static void* returnCoercer(AvmCore* core, Traits* calleeRT, Traits* callerRT)
    {
        // same or caller will discard? no conversion
        if (callerRT == calleeRT)
            return NULL;
        if (callerRT == VOID_TYPE)
        {
#ifndef AVMPLUS_ARM
            if (calleeRT == NUMBER_TYPE)
                return (void*)returnCoercerNPop;
            else
#endif
                return NULL;
        }
        // both integral types? no conversion
        if ((callerRT == INT_TYPE || callerRT == UINT_TYPE) &&
                (calleeRT == INT_TYPE || calleeRT == UINT_TYPE))
            return NULL;
        // is callee a double returner?
        if (calleeRT == NUMBER_TYPE)
            return (void*)returnCoercerN; // Number => 32
        // how about caller?
        if (callerRT == NUMBER_TYPE)
            return (void*)returnCoercerN32; // 32 => Number
        // everything else
        return (void*)returnCoercer32; // 32 => 32
    }

    static int32_t arg32(va_list& ap)
    {
        return va_arg(ap, int32_t); // x86-64?
    }

    static int32_t arg32(APType& ap)
    {
        int32_t result = *(int32_t *)ap;
        ap = (APType)((uintptr_t)ap + sizeof(int32_t)); // x86-64?
        return result;
    }

    static double argN(va_list& ap)
    {
        return va_arg(ap, double);
    }

    static double argN(APType& ap)
    {
        double result = *(double*)ap;
        ap = (APType)((uintptr_t)ap + sizeof(double));
        return result;
    }

    template <class ARG_ITER> static Atom coerceArgToAny(Toplevel* toplevel, ARG_ITER& ap, Traits* callerT)
    {
        AvmCore* core = toplevel->core();
        AvmValue v;

        if (callerT == NUMBER_TYPE)
            v.setDouble(argN(ap));
        else
            v.set((Atom)arg32(ap), callerT);
        return v.get(toplevel, NULL);
    }

    // coerces a single argument and writes it to an argument desc layout
    template <class ARG_ITER> static void coerceArg(Toplevel* toplevel, ArgDescLayout& l, Traits* calleeT, ARG_ITER& callerAp, Traits* callerT)
    {
        AvmCore* core = toplevel->core();

        if (calleeT == callerT && calleeT != OBJECT_TYPE) // OBJECT_TYPE might be a naked ScriptObject... let AvmValue handle it
        {
            if (calleeT == NUMBER_TYPE)
                *l.doubleArg() = argN(callerAp);
            else
                *l.int32Arg() = arg32(callerAp);
        }
        else if (calleeT == NUMBER_TYPE)
        {
            AvmValue v;
            v.set((Atom)arg32(callerAp), callerT);
            *l.doubleArg() = v.getDouble();
        }
        else if (callerT == NUMBER_TYPE)
        {
            AvmValue v;
            v.setDouble(argN(callerAp));
            *l.int32Arg() = v.get(toplevel, calleeT);
        }
        else
        {
            AvmValue v;
            v.set((Atom)arg32(callerAp), callerT);
            *l.int32Arg() = v.get(toplevel, calleeT);
        }
    }

    static void coerceArgAtom(Toplevel* toplevel, ArgDescLayout& l, Traits* calleeT, Atom a)
    {
        APType ap = (APType)&a;
        coerceArg(toplevel, l, calleeT, ap, NULL);
    }

    // coerces a single argument and writes it into an AtomList
    static void coerceArg(Toplevel* toplevel, AtomList &atoms, Traits* calleeT, va_list& callerAp, Traits* callerT)
    {
      AvmCore* core = toplevel->core();
      
      if (callerT == NUMBER_TYPE)
      {
        AvmValue v;
        v.setDouble(argN(callerAp));
        atoms.add((Atom)v.get(toplevel, calleeT));
      }
      else
      {
        AvmValue v;
        v.set((Atom)arg32(callerAp), callerT);
        atoms.add((Atom)v.get(toplevel, calleeT));
      }
    }

    static void coerceArgAtom(Toplevel*, AtomList &atoms, Traits* /*calleeT*/, Atom a)
    {
      atoms.add(a);
    }

    // coerces a single argument and writes it to an "ap" style arg list
    static void coerceArg(Toplevel* toplevel, APType& ap, Traits* calleeT, va_list& callerAp, Traits* callerT)
    {
        AvmCore* core = toplevel->core();

        if (calleeT == callerT && calleeT != OBJECT_TYPE) // OBJECT_TYPE might be a naked ScriptObject... let AvmValue handle it
        {
            if (calleeT == NUMBER_TYPE)
            {
                *(double* )ap = argN(callerAp);
                ap = (APType)(sizeof(double) + (uintptr_t)ap);
            }
            else
            {
                *(int32_t*)ap = arg32(callerAp);
                ap = (APType)(sizeof(int32_t) + (uintptr_t)ap);
            }
        }
        else if (calleeT == NUMBER_TYPE)
        {
            AvmValue v;
            v.set((Atom)arg32(callerAp), callerT);
            *(double* )ap = v.getDouble();
            ap = (APType)(sizeof(double) + (uintptr_t)ap);
        }
        else if (callerT == NUMBER_TYPE)
        {
            AvmValue v;
            v.setDouble(argN(callerAp));
            *(int32_t*)ap = v.get(toplevel, calleeT);
            ap = (APType)(sizeof(int32_t) + (uintptr_t)ap);
        }
        else
        {
            AvmValue v;
            v.set((Atom)arg32(callerAp), callerT);
            *(int32_t*)ap = v.get(toplevel, calleeT);
            ap = (APType)(sizeof(int32_t) + (uintptr_t)ap);
        }
    }

    static void coerceArgAtomI(Toplevel* toplevel, APType& ap, Traits* calleeT, ...)
    {
        va_list va;

        va_start(va, calleeT);
        coerceArg(toplevel, ap, calleeT, va, NULL);
        va_end(va);
    }

    static void coerceArgAtom(Toplevel* toplevel, APType& ap, Traits* calleeT, Atom a)
    {
        coerceArgAtomI(toplevel, ap, calleeT, a);
    }

    static void handleRest(Toplevel*, ArgDescLayout& l, ArrayObject *rest)
    {
        uint32_t argc = rest->getDenseLength();
        Atom *argv = rest->getDenseCopy();

        *l.ptrArg() = argv; // TODO argv
        *l.int32Arg() = argc; // TODO argc
        *l.ptrArg() = rest; // rest
    }

    static void handleRest(Toplevel*, APType&, ArrayObject*)
    {
        AvmAssert(false); // AP doesn't handle rest in the CC
    }
    
    static void handleRest(Toplevel*, AtomList&, ArrayObject*)
    {
        AvmAssert(false);
    }

    // coerces a set of arguments and writes to a given argument description
    template <class ARG_TYPE_ITER1, class ARG_ITER, class ARG_TYPE_ITER2, class ARG_WRITER>
    static int32_t argCoerceLoop(MethodEnv* env, ARG_TYPE_ITER1 callerTypeIter, ARG_ITER callerAp,
            ARG_TYPE_ITER2 calleeTypeIter, ARG_WRITER &argDescWriter)
    {
        Toplevel* toplevel = env->toplevel();
        MethodInfo* info = env->method;
        AvmCore* core = env->core();
        int32_t argc = 0;

        // map args
        ArrayObject* argsOrRest = NULL;
        ArrayObject* args = NULL; // "arguments"
        const MethodSignature* ms = env->method->getMethodSignature();

        // "arguments" captures all arguments
        if (calleeTypeIter.needArguments())
        {
            ARG_TYPE_ITER1 callerTypeIterTemp = callerTypeIter;
            ARG_ITER callerApTemp = callerAp;
            Traits* callerT = callerTypeIterTemp.nextType();

            AvmAssert(callerT != VOID_TYPE);
            Atom a = coerceArgToAny(toplevel, callerApTemp, callerT);
            args = env->createArguments(&a, 0);
        }

        for (;;)
        {
            Traits* callerT = callerTypeIter.nextType();

#if CDECL_VERBOSE
            if (callerT)
                core->console << " callerT: " << callerT->formatClassName() << "\n";
            else
                core->console << " callerT: *\n";
#endif
            // no more params from caller? break out
            if (callerT == VOID_TYPE)
                break;

            Traits* calleeT = calleeTypeIter.nextType();

#if CDECL_VERBOSE
            if (calleeT)
                core->console << " calleeT: " << calleeT->formatClassName() << "\n";
            else
                core->console << " calleeT: *\n";
#endif

            // no more normal params for callee
            if (calleeT == VOID_TYPE)
            {
                if (!ms->allowExtraArgs())
                {
                    toplevel->argumentErrorClass()->throwError(kWrongArgumentCountError,
                            core->toErrorString(info),
                            core->toErrorString(ms->requiredParamCount()),
                            core->toErrorString(argc));
                }
                else // fill up rest/argument/var args
                {
                    // can we just keep pushing args? (i.e., "ap" style)
                    if (calleeTypeIter.isVarArg())
                    {
#if CDECL_VERBOSE
                        core->console << " argCoerceLoop: passing extra params as vararg\n";
#endif
                        AvmAssert(!argsOrRest); // shouldn't have rest or arguments if vararg
                        do
                        {
                            // just keep writing "atom"s
                            coerceArg(toplevel, argDescWriter, NULL, callerAp, callerT);
                            callerT = callerTypeIter.nextType();
                            argc++;
                        } while (callerT != VOID_TYPE);
                    }
                    else
                    {
                        // are we actually using the args?
                        if (calleeTypeIter.needArguments() || calleeTypeIter.hasRest())
                        {
#if CDECL_VERBOSE
                            core->console << " argCoerceLoop: passing extra params as Array\n";
#endif
                            if (!argsOrRest)
                                argsOrRest = args ? args : toplevel->arrayClass()->newArray(1);
                            do
                            {
                                Atom a = coerceArgToAny(toplevel, callerAp, callerT);
                                argsOrRest->push(&a, 1);
                                callerT = callerTypeIter.nextType();
                            } while (callerT != VOID_TYPE);
                        }
#if CDECL_VERBOSE
                        else
                            core->console << " argCoerceLoop: discarding extra params\n";
#endif
                    }
                    break;
                }
            }

            // copy arg into "arguments"
            if (argsOrRest)
            {
                ARG_ITER ap = callerAp;
                Atom a = coerceArgToAny(toplevel, ap, callerT);
                argsOrRest->push(&a, 1);
            }
            else if (args) // arguments doesn't take "this" so
                argsOrRest = args;
            coerceArg(toplevel, argDescWriter, calleeT, callerAp, callerT);
            argc++;
        }

        if (calleeTypeIter.needOptionalArgs())
        {
            // deal with "optional" args
            int32_t regArgs = ms->param_count() + 1;
            int optNum = argc - regArgs + ms->optional_count();

            if (optNum < 0) // not enough non-optional arguments passed...
                toplevel->argumentErrorClass()->throwError(kWrongArgumentCountError,
                        core->toErrorString(info),
                        core->toErrorString(ms->requiredParamCount()),
                        core->toErrorString(argc));

            while (argc < regArgs) // optional...
            {
                Traits* calleeT = calleeTypeIter.nextType();
                coerceArgAtom(toplevel, argDescWriter, calleeT, ms->getDefaultValue(optNum++));
                argc++;
            }
        }
        // pass ArrayObject through
        if (calleeTypeIter.needArguments() || calleeTypeIter.hasRest())
        {
            if (!argsOrRest)
                argsOrRest = args ? args : toplevel->arrayClass()->newArray(0);
            handleRest(toplevel, argDescWriter, argsOrRest);
        }
        return argc;
    }

    static void passBaseArgs(MethodEnv* env, va_list& callerAp, ArgDescLayout& l)
    {
        (void)callerAp;
        (void)l;
        (void)env;
    }

    static void passBaseArgs(MethodEnv* env, APType& callerAp, ArgDescLayout& l)
    {
        (void)callerAp;
        (void)l;
        (void)env;
    }

    static void passTailArgs(MethodEnv* env, va_list& callerAp, ArgDescLayout& l)
    {
        (void)callerAp;
        (void)l;

        // pass MethodEnv at the end
        *l.ptrArg() = env;
    }

    static void passTailArgs(MethodEnv* env, APType& callerAp, ArgDescLayout& l)
    {
        (void)callerAp;
        (void)l;

        // pass MethodEnv at the end
        *l.ptrArg() = env;
    }

    // coerces a set of variadic arguments to a cdecl arg description
    template <class ARG_TYPE_ITER, class AP_TYPE> static void* argCoercer(void* /*callee*/, MethodEnv* env, Traits* callerRT, ARG_TYPE_ITER callerTypeIter, AP_TYPE callerAp, void* calleeArgDescBuf)
    {
        ArgDescLayout l(calleeArgDescBuf);

        passBaseArgs(env, callerAp, l);

        MethodInfo* info = env->method;
        MethodSigArgDescIter calleeTypeIter(info);
        Traits* calleeRT = info->getMethodSignature()->returnTraits();
        AvmCore* core = info->pool()->core;
        argCoerceLoop(env, callerTypeIter, callerAp, calleeTypeIter, l);
#if CDECL_VERBOSE
        core->console << "argCoercer: " << info->format(core) << " ";
        if (callerRT)
            core->console << callerRT->format(core);
        else
            core->console << "*";
        core->console << " -> ";
        if (calleeRT)
            core->console << calleeRT->format(core);
        else
            core->console << "*";
        core->console << "\n";
#endif

        passTailArgs(env, callerAp, l);
        // return any return type Coercer
        return returnCoercer(core, calleeRT, callerRT);
    }

    // callerArgDesc is the number of atoms
    static void* atomvArgDescCoercer(void* callee, MethodEnv* env, Traits* retTraits, uintptr_t callerArgDesc, void* callerAp, void* calleeArgDescBuf)
    {
        AtomvArgDescIter callerTypeIter(env->core(), (int32_t)callerArgDesc);

        AvmAssert(false); // TESTME -- AtomvArgDescIter off by one error or not
        return argCoercer(callee, env, retTraits, callerTypeIter, (APType)callerAp, calleeArgDescBuf);
    }

    // callerArgDesc is the number of args
    static void* apArgDescCoercer(void* callee, MethodEnv* env, Traits* retTraits, uintptr_t callerArgDesc, void* callerAp, void* calleeArgDescBuf)
    {
        APArgDescIter callerTypeIter((int32_t)callerArgDesc, env->method);

        return argCoercer(callee, env, retTraits, callerTypeIter, (APType)callerAp, calleeArgDescBuf);
    }

    // callerArgDesc is a pointer to a string of nybbles describing arg types
    static void* ptrArgDescCoercer(void* callee, MethodEnv* env, Traits* retTraits, uintptr_t callerArgDesc, void* callerAp, void* calleeArgDescBuf)
    {
        PtrArgDescIter callerTypeIter((void*)callerArgDesc, env->core());

        return argCoercer(callee, env, retTraits, callerTypeIter, *(va_list*)callerAp, calleeArgDescBuf);
    }

    // callerArgDesc is a value containing nybbles describing arg types
    static void* immArgDescCoercer(void* callee, MethodEnv* env, Traits* retTraits, uintptr_t callerArgDesc, void* callerAp, void* calleeArgDescBuf)
    {
        ImmArgDescIter callerTypeIter(callerArgDesc, env->core());

        return argCoercer(callee, env, retTraits, callerTypeIter, *(va_list*)callerAp, calleeArgDescBuf);
    }

    // amount of stack space needed to call the given method cdecl style
    static int32_t argDescSize(MethodInfo* info)
    {
        AvmCore* core = info->pool()->core;
        MethodSigArgDescIter calleeTypeIter(info);

        return argDescSize(calleeTypeIter, core);
    }

    // calls "env" with supplied variadic arguments described by the "immediate" flavor of argument
    // description in argDesc
    // returns an int32_t value
    Atom coerce32CdeclArgDescEnter(Traits* retTraits, uintptr_t argDesc, MethodEnv* env, va_list ap)
    {
        MethodInfo* info = env->method;
        Atom result = coerce32CdeclShim(
                (void*)info->handler_function(), argDescSize(info),
                immArgDescCoercer, env, retTraits, argDesc, &ap);
        return result;
    }

    // calls "env" with supplied variadic arguments described by the "pointer" flavor of argument
    // description in argDesc
    // returns an int32_t value
    Atom coerce32CdeclArgDescEnter(Traits* retTraits, char* argDesc, MethodEnv* env, va_list ap)
    {
        MethodInfo* info = env->method;
        Atom result = coerce32CdeclShim(
                (void*)info->handler_function(), argDescSize(info), ptrArgDescCoercer, env, retTraits, (uintptr_t)argDesc, &ap);
        return result;
    }

    Atom coerce32CdeclArgDescEnter(Traits* retTraits, MethodEnv* env, int argc, Atom* argv)
    {
        MethodInfo* info = env->method;
        Atom result = coerce32CdeclShim(
                (void*)info->handler_function(), argDescSize(info), atomvArgDescCoercer, env, retTraits, (uintptr_t)argc, (void*)argv);
        return result;
    }

    Atom coerce32CdeclArgDescEnter(Traits* retTraits, MethodEnv* env, int argc, uint32_t* ap)
    {
        MethodInfo* info = env->method;
        Atom result = coerce32CdeclShim(
                (void*)info->handler_function(), argDescSize(info), apArgDescCoercer, env, retTraits, (uintptr_t)argc, (void*)ap);
        return result;
    }

    // calls "env" with supplied variadic arguments described by the "immediate" flavor of argument
    // description in argDesc
    // returns a double value
    double coerceNCdeclArgDescEnter(uintptr_t argDesc, MethodEnv* env, va_list ap)
    {
        MethodInfo* info = env->method;
        AvmCore* core = env->core();

        double result = coerceNCdeclShim(
                (void*)info->handler_function(), argDescSize(info),
                immArgDescCoercer, env, NUMBER_TYPE, argDesc, &ap);
        return result;
    }

    // calls "env" with supplied variadic arguments described by the "pointer" flavor of argument
    // description in argDesc
    // returns a double value
    double coerceNCdeclArgDescEnter(char* argDesc, MethodEnv* env, va_list ap)
    {
        MethodInfo* info = env->method;
        AvmCore* core = env->core();

        double result = coerceNCdeclShim(
                (void*)info->handler_function(), argDescSize(info), ptrArgDescCoercer, env, NUMBER_TYPE, (uintptr_t)argDesc, &ap);
        return result;
    }

    double coerceNCdeclArgDescEnter(MethodEnv* env, int argc, Atom* argv)
    {
        MethodInfo* info = env->method;
        AvmCore* core = env->core();

        double result = coerceNCdeclShim(
                (void*)info->handler_function(), argDescSize(info), atomvArgDescCoercer, env, NUMBER_TYPE, (uintptr_t)argc, (void*)argv);
        return result;
    }

    double coerceNCdeclArgDescEnter(MethodEnv* env, int argc, uint32_t* ap)
    {
        MethodInfo* info = env->method;
        AvmCore* core = env->core();
        double result = coerceNCdeclShim(
                (void*)info->handler_function(), argDescSize(info), apArgDescCoercer, env, NUMBER_TYPE, (uintptr_t)argc , (void*)ap);
        return result;
    }

    // calculate size needed for ap style argument block
    int32_t argDescApSize(uintptr_t argDesc, MethodEnv* env)
    {
        APArgDescIter calleeTypeIter(-1, env->method);
        ImmArgDescIter callerTypeIter(argDesc, env->core());
        return apArgDescSize(callerTypeIter, calleeTypeIter, env->core());
    }

    int32_t argDescApSize(char* argDesc, MethodEnv* env)
    {
        APArgDescIter calleeTypeIter(-1, env->method);
        PtrArgDescIter callerTypeIter(argDesc, env->core());
        return apArgDescSize(callerTypeIter, calleeTypeIter, env->core());
    }

    // convert arguments to ap style argument block, returning "argc"
    int32_t argDescArgsToAp(void* calleeArgDescBuf, uintptr_t argDesc, MethodEnv* env, va_list ap)
    {
        APArgDescIter calleeTypeIter(-1, env->method);
        ImmArgDescIter callerTypeIter(argDesc, env->core());
        APType dst = (APType)calleeArgDescBuf;
        return argCoerceLoop(env, callerTypeIter, ap, calleeTypeIter, dst) - 1;
    }

    int32_t argDescArgsToAp(void* calleeArgDescBuf, char* argDesc, MethodEnv* env, va_list ap)
    {
        APArgDescIter calleeTypeIter(-1, env->method);
        PtrArgDescIter callerTypeIter(argDesc, env->core());
        APType dst = (APType)calleeArgDescBuf;
        return argCoerceLoop(env, callerTypeIter, ap, calleeTypeIter, dst) - 1;
    }

    // count arguments... no size calculations
    template <class ARG_TYPE_ITER> static int32_t argDescArgCount(ARG_TYPE_ITER iter)
    {
        int32_t result = 0;

        while (iter.nextTypeKind() != kVOID)
            result++;
        return result;
    }

    // return number of arguments in description
    int32_t argDescArgCount(uintptr_t argDesc)
    {
        ImmArgDescIter iter(argDesc, NULL);
        return argDescArgCount(iter);
    }

    int32_t argDescArgCount(char* argDesc)
    {
        PtrArgDescIter iter(argDesc, NULL);
        return argDescArgCount(iter);
    }

    // convert arguments to Atoms
    void argDescArgsToAtomv(Atom* args, uintptr_t argDesc, MethodEnv* env, va_list ap)
    {
        AvmCore* core = env->core();
        ImmArgDescIter callerTypeIter(argDesc, core);
        AtomvArgDescIter calleeTypeIter(core);
        APType dst = (APType)args;
        argCoerceLoop(env, callerTypeIter, ap, calleeTypeIter, dst);
    }

    void argDescArgsToAtomv(Atom* args, char* argDesc, MethodEnv* env, va_list ap)
    {
        AvmCore* core = env->core();
        PtrArgDescIter callerTypeIter(argDesc, core);
        AtomvArgDescIter calleeTypeIter(core);
        APType dst = (APType)args;
        argCoerceLoop(env, callerTypeIter, ap, calleeTypeIter, dst);
    }
    
    // convert arguments to AtomList
    void argDescArgsToAtomList(AtomList& dst, uintptr_t argDesc, MethodEnv* env, va_list ap)
    {
        AvmCore* core = env->core();
        ImmArgDescIter callerTypeIter(argDesc, core);
        AtomvArgDescIter calleeTypeIter(core);
        argCoerceLoop(env, callerTypeIter, ap, calleeTypeIter, dst);
    }

    void argDescArgsToAtomList(AtomList& dst, char* argDesc, MethodEnv* env, va_list ap)
    {
        AvmCore* core = env->core();
        PtrArgDescIter callerTypeIter(argDesc, core);
        AtomvArgDescIter calleeTypeIter(core);
        argCoerceLoop(env, callerTypeIter, ap, calleeTypeIter, dst);
    }

#ifdef VMCFG_AOT
    uintptr_t aotThunker(MethodEnv* env, int32_t argc, uint32_t* argv)
    {
        Traits* rt = env->method->getMethodSignature()->returnTraits();
        return coerce32CdeclArgDescEnter(rt, env, argc, (uint32_t* )argv);
    }

    double aotThunkerN(MethodEnv* env, int32_t argc, uint32_t* argv)
    {
        return coerceNCdeclArgDescEnter(env, argc, (uint32_t* )argv);
    }
#endif // VMCFG_AOT
}

#endif // VMCFG_CDECL
