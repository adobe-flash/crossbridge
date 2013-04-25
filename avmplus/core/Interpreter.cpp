/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"
#include "Interpreter.h"

#ifdef VMCFG_NANOJIT
#   include "exec-osr.h"
#endif

#ifdef AVMPLUS_MAC
#ifndef __GNUC__
// inline_max_total_size() defaults to 10000.
// This module includes so many inline functions that we
// exceed this limit and we start getting compile warnings,
// so bump up the limit for this file.
#pragma inline_max_total_size(26576)
#endif
#endif

// The macro VMCFG_WORDCODE is true if the representation of ABC code is as an array of words and
// not an array of bytes.

namespace avmplus
{
    // an auto-ptr for managing MethodFrame calls.
    class EnterMethodEnv
    {
    public:
        inline explicit EnterMethodEnv(AvmCore* core, MethodEnv* env, MethodFrame& frame) : m_core(core), m_frame(frame)
        {
            m_frame.enter(m_core, env);
        }

        inline ~EnterMethodEnv()
        {
            m_frame.exit(m_core);
        }

        void setCurrent()
        {
            m_core->currentMethodFrame = &m_frame;
        }

    private:
        AvmCore* m_core;
        MethodFrame& m_frame;
    };

#ifdef _DEBUG
    REALLY_INLINE Atom CHECK_INT_ATOM(Atom a)
    {
        AvmAssert(atomKind(a) == kIntptrType && atomIsValidIntptrValue(atomGetIntptr(a)));
        return a;
    }
    REALLY_INLINE Atom MAKE_INTEGER(intptr_t i)
    {
        AvmAssert(atomIsValidIntptrValue(i));
        return (intptr_t(i) << 3) | kIntptrType;
    }
#else
    #define CHECK_INT_ATOM(a) (a)
    #define MAKE_INTEGER(v)   ((intptr_t(v) << 3) | kIntptrType)
#endif


#define IS_INTEGER(v)        (((v) & 7) == kIntptrType)
#define IS_DOUBLE(v)         (((v) & 7) == kDoubleType)
#define IS_BOOLEAN(v)        (((v) & 7) == kBooleanType)
#define IS_STRING(v)         (((v) & 7) == kStringType)
#ifdef VMCFG_FLOAT
#define IS_FLOAT(v)          ((((v) & 7) == kSpecialBibopType) && ((v)!=AtomConstants::undefinedAtom) && bibopKind(v)==kBibopFloatType)
#define IS_FLOAT4(v)         ((((v) & 7) == kSpecialBibopType) && ((v)!=AtomConstants::undefinedAtom) && bibopKind(v)==kBibopFloat4Type)
#endif // VMCFG_FLOAT
// note that the argument to SIGN_EXTEND is expected to be upshifted 3 bits (not a "raw" intptr),
// but it doesn't expect or require the tag bits to be set properly.
#ifdef AVMPLUS_64BIT
// since 64-bit int atoms expect exactly 54 bits of precision, we want to shift bit 54+3 up into the sign bit and back down
#  define SIGN_EXTEND(v)       ((intptr_t(v) << (atomSignExtendShift-AtomConstants::kAtomTypeSize)) >> (atomSignExtendShift-AtomConstants::kAtomTypeSize))
#else
#  define SIGN_EXTEND(v)       (intptr_t(v))
#endif

// CLAMP_32 is equivalent to running an int atom thru AvmCore::integer (ie, truncate to int32_t using the right rules),
// but, like SIGN_EXTEND, it expects the argument to be upshifted 3 bit.
#ifdef AVMPLUS_64BIT
#  define CLAMP_32(v)       ((intptr_t(v) << (32-AtomConstants::kAtomTypeSize)) >> (32-AtomConstants::kAtomTypeSize))
#else
#  define CLAMP_32(v)       (intptr_t(v))
#endif

#define INT32_VALUE(v)       int32_t(atomGetIntptr(v))
#define UINT32_VALUE(v)      uint32_t(atomGetIntptr(v))
#define DOUBLE_VALUE(v)      (*(double*)((v) ^ kDoubleType))
#ifdef VMCFG_FLOAT
#define FLOAT_VALUE(v)       (*(float*)((v) ^ kSpecialBibopType))
#endif
#define IS_BOTH_KIND(a,b,k) (((( (a) ^ (k) ) | ( (b) ^ (k) )) & 7) == 0) // less control flow but more registers -- which is better?
#define IS_BOTH_INTEGER(a,b) IS_BOTH_KIND(a,b,kIntptrType) 
#define IS_BOTH_DOUBLE(a,b)  IS_BOTH_KIND(a,b,kDoubleType)  
#ifdef VMCFG_FLOAT
#define IS_BOTH_BIBOP(a,b,k) IS_BOTH_KIND(a,b,kSpecialBibopType) && \
                             ((a)!=AtomConstants::undefinedAtom) && ((b)!=AtomConstants::undefinedAtom) \
                             && bibopKind(a)==k && bibopKind(b)==k
#define IS_BOTH_FLOAT(a,b)   IS_BOTH_BIBOP(a,b,kBibopFloatType)
#define IS_BOTH_FLOAT4(a,b)  IS_BOTH_BIBOP(a,b,kBibopFloat4Type)
#endif // VMCFG_FLOAT

#ifdef VMCFG_WORDCODE
#  define WORD_CODE_ONLY(x)  x
#  define ABC_CODE_ONLY(x)
#else
#  define WORD_CODE_ONLY(x)
#  define ABC_CODE_ONLY(x)   x
#endif

#ifdef VMCFG_WORDCODE_PEEPHOLE
#  define PEEPHOLE_ONLY(x)  x
#else
#  define PEEPHOLE_ONLY(x)
#endif

#ifdef DEBUGGER
#  define DEBUGGER_ONLY(x)  x
#  define NONDEBUGGER_ONLY(x)
#else
#  define DEBUGGER_ONLY(x)
#  define NONDEBUGGER_ONLY(x) x
#endif
#ifdef DEBUG
#  define DEBUG_ONLY(x)  x
#else
#  define DEBUG_ONLY(x)
#endif

#ifdef VMCFG_WORDCODE
    typedef uintptr_t bytecode_t;
#else
    typedef uint8_t bytecode_t;
#endif

#ifndef VMCFG_WORDCODE
    inline intptr_t readS24(const uint8_t* pc) {
        return AvmCore::readS24(pc);
    }

    inline uintptr_t readU30(const uint8_t*& pc) {
        return AvmCore::readU32(pc);
    }
#endif

// Direct threading in the interpreter.
//
// If you have gcc, direct threading should work out of the box and
// should provide a nice speedup with many platforms and compiler versions.
//
// If you are using Microsoft Visual C/C++ then you may turn on direct
// threading, and you must select one of the two threading implementations
// below.  MSVC_X86_ASM_THREADING should "just work" but is likely
// to be slower than switch dispatch, unless you're using a compiler
// version with little or no optimization.  MSVC_X86_REWRITE_THREADING
// usually improves the performance over switch dispatch, but requires
// a fair amount of manual work if core/Interpreter.cpp has been modified
// since core/FastInterpreter.asm was generated.  The specific work
// needed to regenerate core/FastInterpreter.cpp is described in a comment
// at the head of utils/x86rewrite.as, which you will need to run.

#ifdef VMCFG_DIRECT_THREADED
#  ifndef VMCFG_WORDCODE
#    error "Need word code enabled for this"
#  endif
#  if defined __GNUC__
#    define GNUC_THREADING
#    define DIRECT_DISPATCH
#  elif defined AVMPLUS_WIN32
     // Pick one of the two following options
//#    define MSVC_X86_ASM_THREADING
#    define MSVC_X86_REWRITE_THREADING
#    ifdef MSVC_X86_ASM_THREADING
#      define DIRECT_DISPATCH
#    endif
#    ifdef MSVC_X86_REWRITE_THREADING
#      define SWITCH_DISPATCH
#    endif
#  else
#    error "Threaded code not supported for this platform/compiler"
#  endif
#else
#  define SWITCH_DISPATCH
#endif // compiler/platform vipers' nest

    static Atom* initMultiname(MethodEnv* env, Multiname &name, Atom* sp);
    static Atom* initMultinameNoXMLList(MethodEnv* env, Multiname &name, Atom* sp);
    static Traits* getTraits(const Multiname* name, PoolObject* pool, Toplevel* toplevel, AvmCore* core);
#ifdef AVMPLUS_VERBOSE
    // display contents of current stack frame only
    static void showState(MethodInfo* info, const bytecode_t *code_start, const bytecode_t *pc,
                          Atom* framep, Atom *spp, int scopeDepth, Atom *scopebasep, int max_scope);
#endif

    /**
     * on a backwards branch, check if the interrupt flag is enabled.
     * used by interpreter only.  A copy of this is inline-generated
     * by CodegenLIR at loop headers.
     */
    REALLY_INLINE void branchCheck(AvmCore* core, MethodEnv *env, bool interruptable)
    {
    #ifdef DEBUGGER
        core->sampleCheck();
    #endif
        if (core->interruptCheck(interruptable))
            core->handleInterruptMethodEnv(env);
    }

#ifdef _MSC_VER
#  ifdef MSVC_X86_ASM_THREADING
#    pragma warning(disable:4740)  // "inline assembler suppresses global optimization"
#  endif
#endif // _MSC_VER

#ifdef VMCFG_DIRECT_THREADED

    void** interpGetOpcodeLabels() {
        // *** NOTE ON THREAD SAFETY ***
        //
        // If we ever enable direct threading for a platform where the thread jump targets
        // cannot be constants in a vector but must be written into a vector the first time
        // interpGetOpcodeLabels() is called, then the following call must be within a
        // critical section (or some startup code must make an initial call to make sure
        // the vector is initialized in a thread safe way).  At this time, Visual C++
        // requires this, but we're not using direct threading with Visual C++, so there
        // is no critical section here.
        //
        // The startup code to make the initial call can simply call this function and
        // assign the result to a dummy static global variable; that trick is not portable
        // but it can be used for many platforms (and is used elsewhere).

    #if defined MSVC_X86_ASM_THREADING || defined MSVC_X86_REWRITE_THREADING
        #error "interpGetOpcodeLabels needs a critical section or eager initialization for this compiler / platform combination"
    #endif

        return (void**)interpBoxed(NULL, 0, NULL);
    }

#endif // VMCFG_DIRECT_THREADED

#ifdef VMCFG_NANOJIT
#   define OSR(offset) \
        if (OSR::countEdge(env, info, ms) &&                        \
            OSR::execute(env, framep, ms, pc + (offset), &a1l)) {   \
            methodFrame.setCurrent();                               \
            a1 = a1l;                                               \
            goto return_value_from_interpreter;                     \
        }
#else
#   define OSR(i1)
#endif

    Atom interpBoxed(register MethodEnv* env, register int _argc, register Atom* _atomv)
    {
#ifdef VMCFG_DIRECT_THREADED

        // If env is NULL return the jump table.  Optionally initialize it here on those
        // platforms where compile-time initialization is not possible or practical.

        if (env == NULL) {
#  if defined GNUC_THREADING
#      define III(idx, lbl) &&lbl,
#      define XXX(idx) &&L_illegal_op,
            static void* opcode_labels[] = {
#  elif defined MSVC_X86_ASM_THREADING || defined MSVC_X86_REWRITE_THREADING
        static void* opcode_labels[WOP_LAST+1];
        if (opcode_labels[0] == 0) {
#    define XXX(idx) III(idx, L_illegal_op)
#    ifdef MSVC_X86_ASM_THREADING
#        define III(idx, lbl) __asm { \
               __asm mov eax, offset opcode_labels \
               __asm mov ebx, offset lbl \
               __asm mov [eax+4*idx], ebx \
             }
#    else
          extern bool LLLLABEL(int);
#         define III(a,b) extern void LLLLABEL ## _ ## a ## _ ## b(); LLLLABEL ## _ ## a ## _ ## b();
#    endif
#  endif // threading discipline
#  define IIM(a,b) III(a,b)
#  if defined VMCFG_WORDCODE_PEEPHOLE
#      define IIP(a,b) III(a,b)
#  else
#      define IIP(a,b) XXX(a)
#  endif
#  if defined DEBUGGER || !defined VMCFG_WORDCODE
#      define IID(a,b) III(a,b)
#  else
#      define IID(a,b) XXX(a)
#  endif
             XXX(0x00)
             XXX(0x01) /* OP_bkpt */
             III(0x02, L_nop)
             III(0x03, L_throw)
             III(0x04, L_getsuper)
             III(0x05, L_setsuper)
             III(0x06, L_dxns)
             III(0x07, L_dxnslate)
             III(0x08, L_kill)
             XXX(0x09) /* OP_label */
#ifdef VMCFG_FLOAT
             III(0x0A, L_lf32x4)
             III(0x0B, L_sf32x4)
#else
             XXX(0x0A)
             XXX(0x0B)
#endif
             III(0x0C, L_ifnlt)
             III(0x0D, L_ifnle)
             III(0x0E, L_ifngt)
             III(0x0F, L_ifnge)
             III(0x10, L_jump)
             III(0x11, L_iftrue)
             III(0x12, L_iffalse)
             III(0x13, L_ifeq)
             III(0x14, L_ifne)
             III(0x15, L_iflt)
             III(0x16, L_ifle)
             III(0x17, L_ifgt)
             III(0x18, L_ifge)
             III(0x19, L_ifstricteq)
             III(0x1A, L_ifstrictne)
             III(0x1B, L_lookupswitch)
             III(0x1C, L_pushwith)
             III(0x1D, L_popscope)
             III(0x1E, L_nextname)
             III(0x1F, L_hasnext)
             III(0x20, L_pushnull)
             III(0x21, L_pushundefined)
#ifdef VMCFG_FLOAT
             III(0x22, L_pushfloat)
#else
             XXX(0x22)
#endif             
             III(0x23, L_nextvalue)
             XXX(0x24) /* OP_pushbyte */
             XXX(0x25) /* OP_pushshort */
             III(0x26, L_pushtrue)
             III(0x27, L_pushfalse)
             III(0x28, L_pushnan)
             III(0x29, L_pop)
             III(0x2A, L_dup)
             III(0x2B, L_swap)
             III(0x2C, L_pushstring)
             XXX(0x2D) /* OP_pushint */
             XXX(0x2E) /* OP_pushuint */
             III(0x2F, L_pushdouble)
             III(0x30, L_pushscope)
             III(0x31, L_pushnamespace)
             III(0x32, L_hasnext2)
             XXX(0x33)
             XXX(0x34)
             IIM(0x35, L_li8)
             IIM(0x36, L_li16)
             IIM(0x37, L_li32)
             IIM(0x38, L_lf32)
             IIM(0x39, L_lf64)
             IIM(0x3A, L_si8)
             IIM(0x3B, L_si16)
             IIM(0x3C, L_si32)
             IIM(0x3D, L_sf32)
             IIM(0x3E, L_sf64)
             XXX(0x3F)
             III(0x40, L_newfunction)
             III(0x41, L_call)
             III(0x42, L_construct)
             III(0x43, L_callmethod)
             III(0x44, L_callstatic)
             III(0x45, L_callsuper)
             III(0x46, L_callproperty)
             III(0x47, L_returnvoid)
             III(0x48, L_returnvalue)
             III(0x49, L_constructsuper)
             III(0x4A, L_constructprop)
             XXX(0x4B) /* OP_callsuperid */
             III(0x4C, L_callproplex)
             XXX(0x4D) /* OP_callinterface */
             III(0x4E, L_callsupervoid)
             III(0x4F, L_callpropvoid)
             IIM(0x50, L_sxi1)
             IIM(0x51, L_sxi8)
             IIM(0x52, L_sxi16)
             III(0x53, L_applytype)
#ifdef VMCFG_FLOAT
             III(0x54, L_pushfloat4)
#else
             XXX(0x54)
#endif             
             III(0x55, L_newobject)
             III(0x56, L_newarray)
             III(0x57, L_newactivation)
             III(0x58, L_newclass)
             III(0x59, L_getdescendants)
             III(0x5A, L_newcatch)
             XXX(0x5B)
             XXX(0x5C)
             III(0x5D, L_findpropstrict)
             III(0x5E, L_findproperty)
             III(0x5F, L_finddef)
             III(0x60, L_getlex)
             III(0x61, L_setproperty)
             III(0x62, L_getlocal)
             III(0x63, L_setlocal)
             III(0x64, L_getglobalscope)
             III(0x65, L_getscopeobject)
             III(0x66, L_getproperty)
             III(0x67, L_getouterscope)
             III(0x68, L_initproperty)
             XXX(0x69)
             III(0x6A, L_deleteproperty)
             XXX(0x6B)
             III(0x6C, L_getslot)
             III(0x6D, L_setslot)
             III(0x6E, L_getglobalslot)
             III(0x6F, L_setglobalslot)
             III(0x70, L_convert_s)
             III(0x71, L_esc_xelem)
             III(0x72, L_esc_xattr)
             III(0x73, L_convert_i)
             III(0x74, L_convert_u)
             III(0x75, L_convert_d)
             III(0x76, L_convert_b)
             III(0x77, L_convert_o)
             III(0x78, L_checkfilter)
#ifdef VMCFG_FLOAT
             III(0x79, L_convert_f)
             III(0x7A, L_unplus)
             III(0x7B, L_convert_f4)
#else
             XXX(0x79)
             XXX(0x7A)
             XXX(0x7B)
#endif             
             XXX(0x7C)
             XXX(0x7D)
             XXX(0x7E)
             XXX(0x7F)
             III(0x80, L_coerce)
             III(0x81, L_convert_b)   // coerce_b -> convert_b, they are the same
             XXX(0x82) /* OP_coerce_a */
             III(0x83, L_convert_i)   // coerce_i -> convert_i, they are the same
             III(0x84, L_convert_d)   // coerce_d -> convert_d, they are the same
             III(0x85, L_coerce_s)
             III(0x86, L_astype)
             III(0x87, L_astypelate)
             III(0x88, L_convert_u)   // coerce_u -> convert_u, they are the same
             III(0x89, L_coerce_o)
             XXX(0x8A)
             XXX(0x8B)
             XXX(0x8C)
             XXX(0x8D)
             XXX(0x8E)
             XXX(0x8F)
             III(0x90, L_negate)
             III(0x91, L_increment)
             III(0x92, L_inclocal)
             III(0x93, L_decrement)
             III(0x94, L_declocal)
             III(0x95, L_typeof)
             III(0x96, L_not)
             III(0x97, L_bitnot)
             XXX(0x98)
             XXX(0x99)
             XXX(0x9A)
             XXX(0x9B)
             XXX(0x9C)
             XXX(0x9D)
             XXX(0x9E)
             XXX(0x9F)
             III(0xA0, L_add)
             III(0xA1, L_subtract)
             III(0xA2, L_multiply)
             III(0xA3, L_divide)
             III(0xA4, L_modulo)
             III(0xA5, L_lshift)
             III(0xA6, L_rshift)
             III(0xA7, L_urshift)
             III(0xA8, L_bitand)
             III(0xA9, L_bitor)
             III(0xAA, L_bitxor)
             III(0xAB, L_equals)
             III(0xAC, L_strictequals)
             III(0xAD, L_lessthan)
             III(0xAE, L_lessequals)
             III(0xAF, L_greaterthan)
             III(0xB0, L_greaterequals)
             III(0xB1, L_instanceof)
             III(0xB2, L_istype)
             III(0xB3, L_istypelate)
             III(0xB4, L_in)
             XXX(0xB5)
             XXX(0xB6)
             XXX(0xB7)
             XXX(0xB8)
             XXX(0xB9)
             XXX(0xBA)
             XXX(0xBB)
             XXX(0xBC)
             XXX(0xBD)
             XXX(0xBE)
             XXX(0xBF)
             III(0xC0, L_increment_i)
             III(0xC1, L_decrement_i)
             III(0xC2, L_inclocal_i)
             III(0xC3, L_declocal_i)
             III(0xC4, L_negate_i)
             III(0xC5, L_add_i)
             III(0xC6, L_subtract_i)
             III(0xC7, L_multiply_i)
             XXX(0xC8)
             XXX(0xC9)
             XXX(0xCA)
             XXX(0xCB)
             XXX(0xCC)
             XXX(0xCD)
             XXX(0xCE)
             XXX(0xCF)
             III(0xD0, L_getlocal0)
             III(0xD1, L_getlocal1)
             III(0xD2, L_getlocal2)
             III(0xD3, L_getlocal3)
             III(0xD4, L_setlocal0)
             III(0xD5, L_setlocal1)
             III(0xD6, L_setlocal2)
             III(0xD7, L_setlocal3)
             XXX(0xD8)
             XXX(0xD9)
             XXX(0xDA)
             XXX(0xDB)
             XXX(0xDC)
             XXX(0xDD)
             XXX(0xDE)
             XXX(0xDF)
             XXX(0xE0)
             XXX(0xE1)
             XXX(0xE2)
             XXX(0xE3)
             XXX(0xE4)
             XXX(0xE5)
             XXX(0xE6)
             XXX(0xE7)
             XXX(0xE8)
             XXX(0xE9)
             XXX(0xEA)
             XXX(0xEB)
             XXX(0xEC)
             XXX(0xED)
             XXX(0xEE)
             IID(0xEF, L_debug)
             IID(0xF0, L_debugline)
             IID(0xF1, L_debugfile)
             XXX(0xF2) /* OP_bkptline */
             XXX(0xF3) /* OP_timestamp */
             XXX(0xF4)
             XXX(0xF5)
             XXX(0xF6)
             XXX(0xF7)
             XXX(0xF8)
             XXX(0xF9)
             XXX(0xFA)
             XXX(0xFB)
             XXX(0xFC)
             XXX(0xFD)
             XXX(0xFE)
             XXX(0xFF)  /* OP_ext */
             XXX(0x100)
             III(0x101, L_pushbits)
             III(0x102, L_push_doublebits)
             IIP(0x103, L_get2locals)
             IIP(0x104, L_get3locals)
             IIP(0x105, L_get4locals)
             IIP(0x106, L_get5locals)
             IIP(0x107, L_storelocal)
             IIP(0x108, L_add_ll)
             IIP(0x109, L_add_set_lll)
             IIP(0x10A, L_subtract_ll)
             IIP(0x10B, L_multiply_ll)
             IIP(0x10C, L_divide_ll)
             IIP(0x10D, L_modulo_ll)
             IIP(0x10E, L_bitand_ll)
             IIP(0x10F, L_bitor_ll)
             IIP(0x110, L_bitxor_ll)
             IIP(0x111, L_add_lb)
             IIP(0x112, L_subtract_lb)
             IIP(0x113, L_multiply_lb)
             IIP(0x114, L_divide_lb)
             IIP(0x115, L_bitand_lb)
             IIP(0x116, L_bitor_lb)
             IIP(0x117, L_bitxor_lb)
             IIP(0x118, L_iflt_ll)
             IIP(0x119, L_ifnlt_ll)
             IIP(0x11A, L_ifle_ll)
             IIP(0x11B, L_ifnle_ll)
             IIP(0x11C, L_ifgt_ll)
             IIP(0x11D, L_ifngt_ll)
             IIP(0x11E, L_ifge_ll)
             IIP(0x11F, L_ifnge_ll)
             IIP(0x120, L_ifeq_ll)
             IIP(0x121, L_ifne_ll)
             IIP(0x122, L_ifstricteq_ll)
             IIP(0x123, L_ifstrictne_ll)
             IIP(0x124, L_iflt_lb)
             IIP(0x125, L_ifnlt_lb)
             IIP(0x126, L_ifle_lb)
             IIP(0x127, L_ifnle_lb)
             IIP(0x128, L_ifgt_lb)
             IIP(0x129, L_ifngt_lb)
             IIP(0x12A, L_ifge_lb)
             IIP(0x12B, L_ifnge_lb)
             IIP(0x12C, L_ifeq_lb)
             IIP(0x12D, L_ifne_lb)
             IIP(0x12E, L_ifstricteq_lb)
             IIP(0x12F, L_ifstrictne_lb)
             IIP(0x130, L_swap_pop)
             III(0x131, L_findpropglobal)
             III(0x132, L_findpropglobalstrict)
#if defined DEBUGGER && defined VMCFG_WORDCODE
             IID(0x133, L_debugenter)
             IID(0x134, L_debugexit)
#else
             XXX(0x133)
             XXX(0x134)
#endif
             III(0x135, L_lix8)
             III(0x136, L_lix16)
#ifdef VMCFG_FLOAT
             III(0x137, L_float4)
#else
             XXX(0x137)
#endif
#  if defined GNUC_THREADING
            };
            AvmAssert(opcode_labels[0x18] == &&L_ifge);
            AvmAssert(opcode_labels[0x97] == &&L_bitnot);
            AvmAssert(opcode_labels[257] == &&L_pushbits);
#    ifdef VMCFG_WORDCODE_PEEPHOLE
            AvmAssert(opcode_labels[48 + 256] == &&L_swap_pop);
#    endif
#  elif defined MSVC_X86_ASM_THREADING || defined MSVC_X86_REWRITE_THREADING
            } // conditional run-time initialization of jump table
#  endif // threading discipline
        return (Atom)opcode_labels;
        } // env == 0?

#endif  // !VMCFG_DIRECT_THREADED

        // These are local variables that are allocated to alloca'd memory;
        // if alloca() is the real alloca() then that makes no difference, but
        // when alloca() is redirected to the heap it makes stack frames smaller,
        // which matters on systems with short stacks.
        //
        // Storage that is conditionally initialized or rarely used may be moved
        // into this structure, to balance the benefit (smaller stack frames)
        // with the costs (more expensive access, slightly larger code).
        //
        // Be careful: if a member has a destructor then an auto_ptr like mechanism is
        // required to invoke the destructor.  The ExceptionFrame gets such a mechanism
        // when the TRY_UNLESS_HEAPMEM macro is used to initialize it.

        struct InterpreterAuxiliaryFrame
        {
            MethodFrame methodFrame;
            ExceptionFrame ef;
            Multiname multiname2;
        };

#ifdef DEBUGGER
        struct InterpreterAuxiliaryFrameWithCSN : public InterpreterAuxiliaryFrame
        {
            inline InterpreterAuxiliaryFrameWithCSN() : cs(CallStackNode::kEmpty) {}

            CallStackNode  cs;
        };
#endif

        // OPTIMIZEME - opportunity to compute some information only when needed.
        //
        // Some of these, notably cpool_double and envDomain, are cached because
        // programs that use them tend to be quite a bit slower if they're not cached.  (In the
        // ABC interpreter we should also cache cpool_int and cpool_uint.)  But
        // the caching slows down function-heavy programs that don't use them.  It may
        // be fruitful to have a bit in the info that tells the interpreter whether to
        // set these up or not.

        register AvmCore* const core = env->core();
        register Toplevel* const toplevel = env->toplevel();
        register MethodInfo* const info = env->method;
        register PoolObject* const pool = info->pool();

#ifdef DEBUGGER
        const size_t kAuxFrameSize = core->debugger() ? sizeof(InterpreterAuxiliaryFrameWithCSN) : sizeof(InterpreterAuxiliaryFrame);
#else
        const size_t kAuxFrameSize = sizeof(InterpreterAuxiliaryFrame);
#endif

#ifdef AVMPLUS_VERBOSE
        if (pool->isVerbose(VB_interp, info))
            core->console << "interp " << info << '\n';
#endif

        // always do a stack check; this not only checks true stack overflows
        // but also the stack limit is used to gain control for host interrupts.
        // so this stack check doubles as an interrupt check.
        // see also: AvmCore::handleStackOverflow().
        core->stackCheck(env);

        register GCList<GCDouble> const & cpool_double = pool->cpool_double;
        register const bool interruptable = !info->isNonInterruptible();
        register const DomainEnv* envDomain = env->domainEnv();
        // I do *not* like making pc 'volatile'; a smart compiler may handle it well
        // and only spill to memory across a call, but a dumb compiler may not ever
#ifdef VMCFG_STACK_METRICS
        core->recordStackPointer();
#endif
        // keep the value in a register at all.
        MethodSignaturep volatile ms = env->method->getMethodSignature();
#if !defined VMCFG_WORDCODE || defined AVMPLUS_VERBOSE
    #ifdef VMCFG_WORDCODE
        register const bytecode_t* volatile codeStart = info->word_code_start();
    #else
        register const bytecode_t* volatile codeStart = ms->abc_code_start();
    #endif
#endif
#ifdef VMCFG_WORDCODE
        register const bytecode_t* /* NOT VOLATILE */ pc = info->word_code_start();
#else
        register const bytecode_t* /* NOT VOLATILE */ pc = ms->abc_code_start();
#endif
        intptr_t volatile expc=0;
        MMgc::GC::AllocaAutoPtr _framep;
#ifdef AVMPLUS_64BIT
        // Allocation is guaranteed on an 8-byte boundary, but we need 16 for _setjmpex.
        // So allocate 8 bytes extra, then round up to a 16-byte boundary.
        register Atom* const framep =
               (Atom*)avmStackAlloc(core, _framep,
                                    sizeof(Atom)*(ms->frame_size())
                                    + 8
                                    + kAuxFrameSize);
        register InterpreterAuxiliaryFrame* const aux_memory = (InterpreterAuxiliaryFrame*)(((uintptr_t)(framep + ms->frame_size()) + 15) & ~15);
#else
        register Atom* const framep =
                       (Atom*)avmStackAlloc(core, _framep,
                                            sizeof(Atom)*(ms->frame_size())
                                            + kAuxFrameSize);
        union {
            Atom* fa;
            InterpreterAuxiliaryFrame* fi;
        };
        fa = framep + ms->frame_size();
        register InterpreterAuxiliaryFrame* const aux_memory = (InterpreterAuxiliaryFrame*)fi;
#endif

        // It's essential that the MethodFrame is cleaned up upon normal exit,
        // to keep core->currentMethodFrame in order.  A throw past the frame
        // will not perform cleanup.  A manual call just before returning
        // is *not* adequate, as we need to be able to call
        // aux_memory->methodFrame->exit() *after* our TRY/CATCH code has
        // completed (otherwise, it may attempt to "restore" currentStack
        // to a bogus value).  Using a real dtor here ensures we are called
        // after any endTry().
        EnterMethodEnv methodFrame(core, env, aux_memory->methodFrame);

        register Atom* const scopeBase = framep + ms->local_count();
        register Atom* volatile withBase = NULL;
        NONDEBUGGER_ONLY( register ) int volatile scopeDepth = 0;
        register ScopeChain* const scope = env->scope();

        // Compute base of operand stack.
        register Atom* /* NOT VOLATILE */ sp = scopeBase + ms->max_scope() - 1;

#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=663989
        // We used to assume that scope 0 (the global scope) must be a ScriptObject*,
        // but this is not the case: it can legitimately be a String* or a primitive.
        // (Note that no scope can be null or undefined, however.)
        Atom /* NOT VOLATILE */ globalScopeAtom = (scope->getSize() > 0) ?
            scope->getScope(0) :
            nullObjectAtom;
#else
        // Note: scopes can be anything but null or undefined... but scope 0
        // (the global scope) will always be a ScriptObject*.  Code that uses
        // lots of global variables accesses this frequently, so it's worth
        // caching.
        ScriptObject* /* NOT VOLATILE */ globalScope = (scope->getSize() > 0)
            ? AvmCore::atomToScriptObject(scope->getScope(0))
            : NULL;
#endif

        // OPTIMIZEME - opportunities for streamlining the function entry code.
        //
        // * With unbox/box optimization introduced and alloca removed so
        //   that the parameter are as on the heap, we could overlap the
        //   outgoing parameter area with the incoming locals.  This avoids
        //   copying, and will be a win if we don't have to work hard to
        //   figure out when it's applicable.
        //
        // * A minor point is that rest / arguments could possibly be
        //   created by a special opcode so that those flags don't have to be
        //   checked here.  Unlikely to be a time sink.
        //
        // Edwin has suggested that the interp() function should simply take
        // a boxed argument array always and that the call code should always
        // pass a boxed argument array.  We'd end up with two entry points,
        // one for boxed entry and one for unboxed entry.  For interpreted
        // code the boxed entry would jump straight into this function, and
        // the unboxed entry would go through reboxing.  For compiled code it
        // would be the other way around, probably.

        {
            // Copy instance and args to local frame
            const int param_count = ms->param_count();
            for (int i=0, n = _argc < param_count ? _argc : param_count; i <= n; i++)
                framep[i] = _atomv[i];

            // Store original value of argc for createRest and createArguments.
            // argc may be changed by the optional parameter check below.
            int arguments_argc = _argc;

            // Set optional param values.  these not aliased to arguments[] since arguments[]
            // only present with traditional prototype functions (no optional args)
            if (info->hasOptional())
            {
                if (_argc < param_count)
                {
                    // initialize default values
                    for (int i=_argc+1, o=_argc + ms->optional_count() - param_count, n=param_count; i <= n; i++, o++)
                        framep[i] = ms->getDefaultValue(o);
                    _argc = param_count;
                }
            }

            // Set remaining locals to undefined.  Don't have to init scope or stack because
            // our conservative GC scan knows how to ignore garbage.
            for (Atom *p = framep + 1 + param_count; p < scopeBase; p++)
                *p = undefinedAtom;

            // Capture arguments or rest array.
            if (info->needRest())
            {
                framep[param_count+1] = env->createRest(_atomv, arguments_argc)->atom();
            }
            else if (info->needArguments())
            {
                // create arguments using atomv[1..argc].
                // Even tho E3 says create an Object, E4 says create an Array so thats what we will do.
                framep[param_count+1] = env->createArguments(_atomv, arguments_argc)->atom();
            }

#ifdef DEBUGGER
            // in debugger builds, ensure that non-active scope entries are nulled out
            // (really only need to do so if there's a debugger active, but not really worth checking for)
            for (int i = 0; i < ms->max_scope(); ++i)
                scopeBase[i] = nullObjectAtom;
#endif
        }

#ifdef DEBUGGER
        CallStackNode* volatile callStackNode = NULL;
#ifndef VMCFG_WORDCODE
        if (core->debugger())
        {
            callStackNode = new ((char*)aux_memory + offsetof(InterpreterAuxiliaryFrameWithCSN, cs)) CallStackNode(env, (FramePtr)framep, /*frameTraits*/0, &expc);
            env->debugEnterInner();
        }
#endif
#endif

// NEXT dispatches the next instruction.
//
// U30ARG picks up a variable-length unsigned integer argument from the instruction
// stream and advances the PC.
//
// U8ARG picks up a fixed-length unsigned byte argument from the instruction stream
// and advances the PC.
//
// S24ARG picks up a fixed-length signed integer argument from the instruction stream
// and advances the PC.
//
// SAVE_EXPC and variants saves the address of the current opcode in the local 'expc'.
// Used in the case of exceptions.

#ifdef AVMPLUS_VERBOSE
#  define VERBOSE  if (pool->isVerbose(VB_interp, info)) showState(info, codeStart, pc-1, framep, sp, scopeDepth, scopeBase, ms->max_scope())
#else
#  define VERBOSE
#endif

#ifdef VMCFG_WORDCODE

#  if defined VMCFG_DIRECT_THREADED
#    if defined GNUC_THREADING
#      define INSTR(op)       L_##op:  VERBOSE;
#      define NEXT            goto *(*pc++)
#    elif defined MSVC_X86_REWRITE_THREADING
#      define INSTR(op)       case WOP_##op: L_ ## op: VERBOSE;
#      define NEXT            continue
#    elif defined MSVC_X86_ASM_THREADING
#      define INSTR(op)       L_ ## op: VERBOSE;
#      define NEXT __asm { \
                __asm mov ebx, pc \
                __asm mov eax, [ebx] \
                __asm add ebx, 4 \
                __asm mov pc, ebx \
                __asm jmp eax \
           }
#    endif // threading discipline
#  else // VMCFG_DIRECT_THREADED
#    define INSTR(op)       case WOP_##op: VERBOSE;
#    define NEXT            continue
#  endif

#  define U30ARG            (*pc++)
#  define U8ARG             (*pc++)
#  define S24ARG            (intptr_t)(*pc++)
#  ifdef DEBUGGER
     // expc is visible outside this function, so make sure it's correct.
     // It's probably possible to adjust it on demand outside the function too,
     // because code that accesses it will have access to "info" and can
     // perform the adjustment.
#    define SAVE_EXPC              expc = pc-1-info->word_code_start()
#    define SAVE_EXPC_TARGET(off)  expc = pc + (off) - info->word_code_start()
#  else
     // Adjusted on demand in the CATCH clause.  Reduces size of interpreter function
     // by 2.5KB of object code (x86 / gcc4.0 / -O3).
#    define SAVE_EXPC              expc = (intptr_t)pc
#    define SAVE_EXPC_TARGET(off)  expc = (intptr_t)(pc + (off) + 1)
#  endif

#else // !VMCFG_WORDCODE

#  define INSTR(op) case OP_##op: VERBOSE;

#  define NEXT              continue
#  define U30ARG            (tmp_pc=pc, tmp_u30 = uint32_t(readU30(tmp_pc)), pc = tmp_pc, tmp_u30)
#  define U8ARG             (*pc++)
#  define S24ARG            (pc+=3, readS24(pc-3))
#  define SAVE_EXPC              expc = pc-1-codeStart
#  define SAVE_EXPC_TARGET(off)  expc = pc + (off) - codeStart

#endif // VMCFG_WORDCODE

        // The following variables are here for the following purposes:
        //
        //  - parameter passing between shared bytecode bodies
        //
        //  - reducing the size of the stack frame on compilers that allocate
        //    individual frame slots to individual block-scoped variables.
        //    Visual C++ is particularly bad.
        //
        // Please do not initialize these variables.
        //
        // Please do not remove the 'register' qualifier.  The variables are so
        // qualified so that their addresses can't be taken.
        //
        // Please do not try to put any of these in a union together.

        register Atom a1, a2, a3;
        register Atom* a2p;
        register intptr_t i1, i2;
        register uintptr_t u1, u2;
        register uintptr_t u1t, u2t, u3t;   // private to the generic arithmetic macros
        register double d1, d2;
        register bool b1;
        register uint8_t ub2;
        register ScriptObject *o1;
        register const Multiname* multiname;
        register MethodEnv* f;
        register Traits* t1;
        uint16_t uh2l;              // not register - its address /can/ be taken
        int32_t i32l;               // ditto
        float f2l;                  // ditto
        double d2l;                 // ditto
#ifdef VMCFG_FLOAT
        float4_t f4l;               // ditto
#endif
        Atom a1l;                   // ditto
#ifndef VMCFG_WORDCODE
        register uint32_t tmp_u30;
        const bytecode_t* tmp_pc;
#endif

    MainLoop:
#ifdef VMCFG_WORDCODE
        TRY_UNLESS_HEAPMEM((char*)aux_memory + offsetof(InterpreterAuxiliaryFrame, ef), core, !info->word_code_exceptions(), kCatchAction_SearchForActionScriptExceptionHandler) {
#else
        TRY_UNLESS_HEAPMEM((char*)aux_memory + offsetof(InterpreterAuxiliaryFrame, ef), core, !info->abc_exceptions(), kCatchAction_SearchForActionScriptExceptionHandler) {
#endif

#ifdef DIRECT_DISPATCH
        NEXT;
#else
        for (;;)
        {
#  ifdef SUPERWORD_PROFILING
            WordcodeTranslator::swprofPC(pc);
#  endif
#  ifdef VMCFG_WORDCODE
            AvmAssert((*pc & 65535) == ((*pc >> 16) & 65535));
            switch ((*pc++) & 65535)
#  else
            switch (*pc++)
#  endif
            {
#endif // dispatch

            INSTR(returnvoid) {
                a1 = undefinedAtom;
                goto return_value_from_interpreter;
            }

            INSTR(returnvalue) {
                a1 = *sp;
            return_value_from_interpreter:
#if defined DEBUGGER && !defined VMCFG_WORDCODE
                if (callStackNode)
                {
                    env->debugExit(callStackNode);
                    callStackNode->reset();
                }
#endif
                SAVE_EXPC;
                a1 = toplevel->coerce(a1, ms->returnTraits());
#ifdef AVMPLUS_VERBOSE
                if (pool->isVerbose(VB_interp, info))
                    core->console << "exit " << info << '\n';
#endif
                return a1;
            }

            INSTR(nop) {
                NEXT;
            }

#ifndef VMCFG_WORDCODE
            INSTR(label) {
                NEXT;
            }
#endif

#ifndef VMCFG_WORDCODE
            INSTR(timestamp) {
                NEXT;
            }
#endif

#ifndef VMCFG_WORDCODE
            INSTR(bkpt) {
                NEXT;
            }
#endif

#ifndef VMCFG_WORDCODE
            INSTR(bkptline) {
                U30ARG;
                NEXT;
            }
#endif

#ifndef VMCFG_WORDCODE
            INSTR(coerce_a) { // no-op since interpreter only uses atoms
                NEXT;
            }
#endif

#if defined DEBUGGER || !defined VMCFG_WORDCODE
            INSTR(debugline) {
                SAVE_EXPC;
                u1 = U30ARG;
                DEBUGGER_ONLY( if (core->debugger()) core->debugger()->debugLine((int32_t)u1); )
                NEXT;
            }
#endif

#if defined DEBUGGER || !defined VMCFG_WORDCODE
            INSTR(debug) {
                WORD_CODE_ONLY( pc += 4 );
                ABC_CODE_ONLY( pc += AvmCore::calculateInstructionWidth(pc-1) - 1 );
                NEXT;
            }
#endif

#if defined DEBUGGER || !defined VMCFG_WORDCODE
            INSTR(debugfile) {
                SAVE_EXPC;
                u1 = U30ARG;
                DEBUGGER_ONLY( if (core->debugger()) core->debugger()->debugFile(pool->getString((int32_t)u1)); )
                NEXT;
            }
#endif

#if defined DEBUGGER && defined VMCFG_WORDCODE
            INSTR(debugenter) {
                AvmAssert(core->debugger() != NULL);
                AvmAssert(callStackNode == NULL);
                callStackNode = new ((char*)aux_memory + offsetof(InterpreterAuxiliaryFrameWithCSN, cs)) CallStackNode(env, (FramePtr)framep, /*frameTraits*/0, &expc);
                env->debugEnterInner();
                NEXT;
            }
#endif

#if defined DEBUGGER && defined VMCFG_WORDCODE
            INSTR(debugexit) {
                AvmAssert(core->debugger() != NULL);
                AvmAssert(callStackNode != NULL);
                env->debugExit(callStackNode);
                callStackNode->reset();
                NEXT;
            }
#endif

            INSTR(jump) {
                i1 = S24ARG;
                if (i1 < 0) {
                    SAVE_EXPC_TARGET(i1);
                    branchCheck(core, env, interruptable);
                    OSR(i1);
                }
                pc += i1;
                NEXT;
            }

            INSTR(pushnull) {
                *(++sp) = nullObjectAtom;
                NEXT;
            }

            INSTR(pushundefined) {
                *(++sp) = undefinedAtom;
                NEXT;
            }

            INSTR(pushstring) {
                *(++sp) = pool->getString((uint32_t)U30ARG)->atom();
                NEXT;
            }

#ifndef VMCFG_WORDCODE
            INSTR(pushint) {
                *(++sp) = core->intToAtom(pool->cpool_int[(uint32_t)U30ARG]);
                NEXT;
            }
#endif

#ifndef VMCFG_WORDCODE
            INSTR(pushuint) {
                *(++sp) = core->uintToAtom(pool->cpool_uint[(uint32_t)U30ARG]);
                NEXT;
            }
#endif

            INSTR(pushdouble) {
                *(++sp) = kDoubleType|(uintptr_t)cpool_double[(uint32_t)U30ARG];
                NEXT;
            }

#ifdef VMCFG_FLOAT
            INSTR(pushfloat) {
                AvmAssertMsg( pool->hasFloatSupport(), "float instruction(pushfloat), but float support disabled");
                *(++sp) = kSpecialBibopType|(uintptr_t)pool->cpool_float[(uint32_t)U30ARG];
                NEXT;
            }
                    
            INSTR(pushfloat4) {
                AvmAssertMsg( pool->hasFloatSupport(), "float instruction(pushfloat4), but float support disabled");
                *(++sp) = kSpecialBibopType|(uintptr_t)pool->cpool_float4[(uint32_t)U30ARG];
                NEXT;
            }
#endif
            INSTR(pushnamespace) {
                *(++sp) = pool->cpool_ns[(uint32_t)U30ARG]->atom();
                NEXT;
            }

            INSTR(getlocal) {
                u1 = U30ARG;
                *(++sp) = framep[u1];
                NEXT;
            }

            INSTR(getlocal0) {
                *(++sp) = framep[0];
                NEXT;
            }

            INSTR(getlocal1) {
                *(++sp) = framep[1];
                NEXT;
            }

            INSTR(getlocal2) {
                *(++sp) = framep[2];
                NEXT;
            }

            INSTR(getlocal3) {
                *(++sp) = framep[3];
                NEXT;
            }

            INSTR(pushtrue) {
                *(++sp) = trueAtom;
                NEXT;
            }

            INSTR(pushfalse) {
                *(++sp) = falseAtom;
                NEXT;
            }

            INSTR(pushnan) {
                *(++sp) = core->kNaN;
                NEXT;
            }

            INSTR(pop) {
                sp--;
                NEXT;
            }

            INSTR(dup) {
                sp++;
                sp[0] = sp[-1];
                NEXT;
            }

            INSTR(swap) {
                a1 = sp[0];
                sp[0] = sp[-1];
                sp[-1] = a1;
                NEXT;
            }

            INSTR(convert_s) {
                SAVE_EXPC;
                sp[0] = core->string(sp[0])->atom();
                NEXT;
            }

            INSTR(esc_xelem) { // ToXMLString will call EscapeElementValue
                SAVE_EXPC;
                sp[0] = core->ToXMLString(sp[0])->atom();
                NEXT;
            }

            INSTR(esc_xattr) {
                SAVE_EXPC;
                sp[0] = core->EscapeAttributeValue(sp[0])->atom();
                NEXT;
            }

            INSTR(convert_d) {
            ABC_CODE_ONLY( convert_d_impl: )
                if (!IS_DOUBLE(sp[0])) {
                    SAVE_EXPC;
                    sp[0] = core->numberAtom(sp[0]);
                }
                NEXT;
            }

#ifdef VMCFG_FLOAT
            INSTR(convert_f) {
                // Note the instruction may be generated from the verifier.
                if (!IS_FLOAT(sp[0])) {
                    SAVE_EXPC;
                    sp[0] = core->floatAtom(sp[0]);
                }
                NEXT;
            }

            INSTR(convert_f4) {
                // Note the instruction may be generated from the verifier.
                if (!IS_FLOAT4(sp[0])) {
                    SAVE_EXPC;
                    sp[0] = core->float4Atom(sp[0]);
                }
                NEXT;
            }                    
       
            INSTR(unplus) {
                // Note the instruction may be generated from the verifier.
                if (!IS_FLOAT(sp[0]) && !IS_FLOAT4(sp[0])) {
                    SAVE_EXPC;
                    sp[0] = core->numericAtom(sp[0]);
                } 
                NEXT;
            }
#endif
#ifndef VMCFG_WORDCODE  /* Jump table forwards to convert_d */
            INSTR(coerce_d) {
                goto convert_d_impl;
            }
#endif

            INSTR(convert_b) {
            ABC_CODE_ONLY( convert_b_impl: )
                a1 = sp[0]; // boolean value
                if (IS_BOOLEAN(a1))
                    ;
                else if (IS_INTEGER(a1))
                    sp[0] = a1 == zeroIntAtom ? falseAtom : trueAtom;
                else
                    sp[0] = AvmCore::booleanAtom(a1);
                NEXT;
            }

#ifndef VMCFG_WORDCODE  /* Jump table forwards to convert_b */
            INSTR(coerce_b) {
                goto convert_b_impl;
            }
#endif

            INSTR(convert_o) {
                if (AvmCore::isNullOrUndefined(sp[0])) {
                    SAVE_EXPC;
                    env->nullcheck(sp[0]);
                }
                NEXT;
            }

            INSTR(negate) {
                a1 = sp[0];
                if (IS_INTEGER(a1) && a1 != zeroIntAtom) {
                    i1 = -atomGetIntptr(a1); // *not* INT32_VALUE
                    if (atomIsValidIntptrValue(i1)) {
                        sp[0] = MAKE_INTEGER(i1);
                        NEXT;
                    }
                }
                SAVE_EXPC;
#ifdef VMCFG_FLOAT
                if(pool->hasFloatSupport() ){
                    SAVE_EXPC;
                    sp[0] = op_negate(core, a1);
                    NEXT;
                }
#endif
                sp[0] = core->doubleToAtom(-AvmCore::number(a1));
                NEXT;
            }

            INSTR(negate_i) {
                // OPTIMIZEME - negate_i
                SAVE_EXPC;
                sp[0] = core->intToAtom(-AvmCore::integer(sp[0]));
                NEXT;
            }

            INSTR(kill) {
                u1 = U30ARG;
                framep[u1] = undefinedAtom;
                NEXT;
            }

            INSTR(typeof) {
                *sp = core->_typeof(*sp)->atom();
                NEXT;
            }

            INSTR(not) {
                a1 = sp[0];     // boolean value
                if (IS_BOOLEAN(a1))
                    ;
                else if (IS_INTEGER(a1))
                    a1 = a1 == zeroIntAtom ? falseAtom : trueAtom;
                else
                    a1 = AvmCore::booleanAtom(a1);
                sp[0] = a1 ^ (trueAtom ^ falseAtom);
                NEXT;
            }

            INSTR(bitnot) {
                a1 = sp[0];
                if (IS_INTEGER(a1)) {
                    sp[0] = MAKE_INTEGER(~int32_t(atomGetIntptr(a1)));
                    NEXT;
                }
                SAVE_EXPC;
                *sp = core->intToAtom(~AvmCore::integer(a1));
                NEXT;
            }

            INSTR(setlocal) {
                u1 = U30ARG;
                framep[u1] = *(sp--);
                NEXT;
            }

            INSTR(setlocal0) {
                framep[0] = *(sp--);
                NEXT;
            }

            INSTR(setlocal1) {
                framep[1] = *(sp--);
                NEXT;
            }

            INSTR(setlocal2) {
                framep[2] = *(sp--);
                NEXT;
            }

            INSTR(setlocal3) {
                framep[3] = *(sp--);
                NEXT;
            }

#define ADD_TWO_VALUES_AND_NEXT(a1, a2, dest) \
    if (IS_BOTH_INTEGER(a1, a2)) { \
        u1t = a1 ^ kIntptrType; \
        u2t = a2 ^ kIntptrType; \
        u3t = SIGN_EXTEND(u1t + u2t); \
        if ((intptr_t)(u1t ^ u2t) < 0 || (intptr_t)(u1t ^ u3t) >= 0) { \
            dest = CHECK_INT_ATOM(u3t | kIntptrType); \
            NEXT; \
        } \
    } \
    else if (IS_BOTH_DOUBLE(a1, a2)) { \
        dest = core->doubleToAtom(DOUBLE_VALUE(a1) + DOUBLE_VALUE(a2)); \
        NEXT; \
    } \
FLOAT_ONLY(\
    else if (pool->hasFloatSupport() ) { \
       if ( IS_BOTH_FLOAT(a1, a2) ) {\
            dest = core->floatToAtom(FLOAT_VALUE(a1) + FLOAT_VALUE(a2)); \
            NEXT; \
       }\
       if(IS_BOTH_FLOAT4(a1,a2)){\
            float4_decl_v(a1);\
            float4_decl_v(a2);\
            \
            dest = core->float4ToAtom( f4_add(a1v,a2v) ); \
            NEXT;\
       }\
    } else { /* pool doesn't have float support */ \
        SAVE_EXPC; \
        dest = op_add_nofloat(core, a1, a2);\
        NEXT;\
    }\
)\
    SAVE_EXPC; \
    dest = op_add(core, a1, a2);\
    NEXT

            INSTR(increment) {
                SAVE_EXPC;
                a2p = sp;
            increment_impl:
                a1 = *a2p;
                if (IS_INTEGER(a1)) {
                    i1 = atomGetIntptr(a1);
                    if (i1 < atomMaxIntValue)
                        *a2p = CHECK_INT_ATOM((intptr_t)a1 + (1 << 3));
                    else
                        *a2p = core->doubleToAtom((double)i1 + 1.0);
                    NEXT;
                }
                if (IS_DOUBLE(a1)) {
                    *a2p = core->doubleToAtom(DOUBLE_VALUE(a1) + 1.0);
                    NEXT;
                }
#ifdef VMCFG_FLOAT
                if (pool->hasFloatSupport()) {
                    a1 = core->numericAtom(a1);
                    if (IS_FLOAT(a1)) {
                        *a2p = core->floatToAtom(FLOAT_VALUE(a1) + 1.0f);
                        NEXT;
                    }
                    if (IS_FLOAT4(a1)) {
                        float4_t one = {1, 1, 1, 1};
                        *a2p = core->float4ToAtom(f4_add(AvmCore::atomToFloat4(a1), one));
                        NEXT;
                    }
                } else
#endif // VMCFG_FLOAT
                {
                    a1 = core->numberAtom(a1);
                }
                *a2p = core->increment_number_d(a1, 1);
                NEXT;
            }

            INSTR(inclocal) {
                SAVE_EXPC;
                a2p = framep+U30ARG;
                goto increment_impl;
            }

            INSTR(increment_i) {
                SAVE_EXPC;
                a2p = sp;
            increment_i_impl:
                a1 = *a2p;
                if (IS_INTEGER(a1)) {
                    #ifdef AVMPLUS_64BIT
                        // Result always fits 64-bit kIntptrType atom.
                        *a2p = CHECK_INT_ATOM(CLAMP_32((intptr_t)a1 + (1 << 3)));
                    #else
                        i1 = atomGetIntptr(a1);
                        if (i1 < atomMaxIntValue)
                            *a2p = CHECK_INT_ATOM((intptr_t)a1 + (1 << 3));
                        else
                            *a2p = core->intToAtom((int32_t)i1 + 1);
                    #endif
                    NEXT;
                }
                if (IS_DOUBLE(a1)) {
                    *a2p = core->intToAtom((int32_t)core->atomToDouble(a1) + 1);
                    NEXT;
                }
                *a2p = core->increment_i(a1, 1);
                NEXT;
            }

            INSTR(inclocal_i) {
                SAVE_EXPC;
                a2p = framep+U30ARG;
                goto increment_i_impl;
            }

            INSTR(decrement) {
                SAVE_EXPC;
                a2p = sp;
            decrement_impl:
                a1 = *a2p;
                if (IS_INTEGER(a1)) {
                    i1 = atomGetIntptr(a1);
                    if (i1 > atomMinIntValue)
                        *a2p = CHECK_INT_ATOM((intptr_t)a1 - (1 << 3));
                    else
                        *a2p = core->doubleToAtom((double)i1 - 1.0);
                    NEXT;
                }
                if (IS_DOUBLE(a1)) {
                    *a2p = core->doubleToAtom(DOUBLE_VALUE(a1) - 1.0);
                    NEXT;
                }
#ifdef VMCFG_FLOAT
                if (pool->hasFloatSupport()) {
                    a1 = core->numericAtom(a1);
                    if (IS_FLOAT(a1)) {
                        *a2p = core->floatToAtom(FLOAT_VALUE(a1) - 1.0f);
                        NEXT;
                    }
                    if (IS_FLOAT4(a1)) {
                        float4_t one = {1, 1, 1, 1};
                        *a2p = core->float4ToAtom(f4_sub(AvmCore::atomToFloat4(a1), one));
                        NEXT;
                    }
                } else
#endif // VMCFG_FLOAT
                {
                    a1 = core->numberAtom(a1);
                }
                *a2p = core->increment_number_d(a1, -1);
                NEXT;
            }

            INSTR(declocal) {
                SAVE_EXPC;
                a2p = framep+U30ARG;
                goto decrement_impl;
            }

            INSTR(decrement_i) {
                SAVE_EXPC;
                a2p = sp;
            decrement_i_impl:
                a1 = *a2p;
                if (IS_INTEGER(a1)) {
                    #ifdef AVMPLUS_64BIT
                        // Result always fits 64-bit kIntptrType atom.
                        *a2p = CHECK_INT_ATOM(CLAMP_32((intptr_t)a1 - (1 << 3)));
                    #else
                        i1 = atomGetIntptr(a1);
                        if (i1 > atomMinIntValue)
                            *a2p = CHECK_INT_ATOM((intptr_t)a1 - (1 << 3));
                        else
                            *a2p = core->intToAtom((int32_t)i1 - 1);
                    #endif
                    NEXT;
                }
                if (IS_DOUBLE(a1)) {
                    *a2p = core->intToAtom((int32_t)core->atomToDouble(a1) - 1);
                    NEXT;
                }
                *a2p = core->increment_i(a1, -1);
                NEXT;
            }

            INSTR(declocal_i) {
                SAVE_EXPC;
                a2p = framep+U30ARG;
                goto decrement_i_impl;
            }

            INSTR(add) {
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
            PEEPHOLE_ONLY( add_two_values_into_tos_impl: )
                ADD_TWO_VALUES_AND_NEXT(a1, a2, sp[0]);
            }

            INSTR(add_i) {
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
                if (IS_BOTH_INTEGER(a1, a2)) {
                    u1t = a1 ^ kIntptrType;
                    u2t = a2 ^ kIntptrType;
                    u3t = CLAMP_32(u1t + u2t);
                    sp[0] = CHECK_INT_ATOM(u3t | kIntptrType);
                    NEXT;
                }
                if (IS_BOTH_DOUBLE(a1, a2)) {
                    i1 = (int32_t)AvmCore::integer_d(DOUBLE_VALUE(a1));
                    i2 = (int32_t)AvmCore::integer_d(DOUBLE_VALUE(a2));
                    goto finish_add_i;
                }
                SAVE_EXPC;
                i1 = AvmCore::integer(a1);
                i2 = AvmCore::integer(a2);
            finish_add_i:
                sp[0] = core->intToAtom((int32_t)(i1 + i2));
                NEXT;
            }

            INSTR(subtract) {
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
#ifdef VMCFG_WORDCODE_PEEPHOLE
            sub_two_values_and_next:
#endif
                if (IS_BOTH_INTEGER(a1, a2)) {
                    u1t = a1 ^ kIntptrType;
                    u2t = a2 ^ kIntptrType;
                    u3t = SIGN_EXTEND(u1t - u2t);
                    if ((intptr_t)(u1t ^ u2t) >= 0 || (intptr_t)(u1t ^ u3t) >= 0) {
                        sp[0] = CHECK_INT_ATOM(u3t | kIntptrType);
                        NEXT;
                    }
                }
                if (IS_BOTH_DOUBLE(a1, a2)) {
                    sp[0] = core->doubleToAtom(DOUBLE_VALUE(a1) - DOUBLE_VALUE(a2));
                    NEXT;
                }
                SAVE_EXPC;
#ifdef VMCFG_FLOAT
                if (pool->hasFloatSupport()){
                    a1 = core->numericAtom(a1);
                    a2 = core->numericAtom(a2);
                    if( IS_BOTH_FLOAT(a1, a2) ) {
                        sp[0] = core->floatToAtom(FLOAT_VALUE(a1) - FLOAT_VALUE(a2));
                        NEXT;
                    }
                    if(AvmCore::isFloat4(a1) || AvmCore::isFloat4(a2)){
                        float4_decl_v(a1);
                        float4_decl_v(a2);
                        sp[0] = core->float4ToAtom( f4_sub(a1v,a2v) ); 
                        NEXT;
                    }
                }
#endif // VMCFG_FLOAT
                d1 = AvmCore::number(a1);
                d2 = AvmCore::number(a2);
                sp[0] = core->doubleToAtom(d1 - d2);
                NEXT;
            }

            INSTR(subtract_i) {
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
                if (IS_BOTH_INTEGER(a1, a2)) {
                    u1t = a1 ^ kIntptrType;
                    u2t = a2 ^ kIntptrType;
                    u3t = CLAMP_32(u1t - u2t);
                    sp[0] = CHECK_INT_ATOM(u3t | kIntptrType);
                    NEXT;
                }
                if (IS_BOTH_DOUBLE(a1, a2)) {
                    i1 = (int32_t)AvmCore::integer_d(DOUBLE_VALUE(a1));
                    i2 = (int32_t)AvmCore::integer_d(DOUBLE_VALUE(a2));
                    goto finish_subtract_i;
                }
                SAVE_EXPC;
                i1 = AvmCore::integer(a1);
                i2 = AvmCore::integer(a2);
            finish_subtract_i:
                sp[0] = core->intToAtom((int32_t)(i1 - i2));
                NEXT;
            }

            INSTR(multiply) {
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
#ifdef VMCFG_WORDCODE_PEEPHOLE
            mul_two_values_and_next:
#endif
                // OPTIMIZEME - multiplication of small integers might be optimized?
                if (IS_BOTH_DOUBLE(a1, a2)) {
                    sp[0] = core->doubleToAtom(DOUBLE_VALUE(a1) * DOUBLE_VALUE(a2)); 
                    NEXT;
                }
                SAVE_EXPC;
#ifdef VMCFG_FLOAT
                if (pool->hasFloatSupport()){
                    a1 = core->numericAtom(a1);
                    a2 = core->numericAtom(a2);
                    if(IS_BOTH_FLOAT(a1, a2)) {
                        sp[0] = core->floatToAtom(FLOAT_VALUE(a1) * FLOAT_VALUE(a2)); 
                        NEXT;
                    }
                    if(AvmCore::isFloat4(a1) || AvmCore::isFloat4(a2)){
                        float4_decl_v(a1);
                        float4_decl_v(a2);
                        sp[0] = core->float4ToAtom( f4_mul(a1v,a2v) ); 
                        NEXT;
                    }
                }
#endif // VMCFG_FLOAT
                d1 = AvmCore::number(a1);
                d2 = AvmCore::number(a2);
                sp[0] = core->doubleToAtom(d1 * d2);
                NEXT;
            }

            INSTR(multiply_i) {
                // OPTIMIZEME - multiplication of small integers might be optimized?
                SAVE_EXPC;
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
                i1 = AvmCore::integer(a1);
                i2 = AvmCore::integer(a2);
                sp[0] = core->intToAtom((int32_t)(i1 * i2));
                NEXT;
            }

            INSTR(divide) {
                // OPTIMIZEME - division of small integers might be optimized?
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
#ifdef VMCFG_WORDCODE_PEEPHOLE
            div_two_values_and_next:
#endif
                if (IS_BOTH_DOUBLE(a1, a2)) {
                    sp[0] = core->doubleToAtom(DOUBLE_VALUE(a1) / DOUBLE_VALUE(a2));
                    NEXT;
                }
                SAVE_EXPC;
#ifdef VMCFG_FLOAT
                if(pool->hasFloatSupport()) {
                    a1 = core->numericAtom(a1);
                    a2 = core->numericAtom(a2);
                    if(IS_BOTH_FLOAT(a1, a2)) {
                        sp[0] = core->floatToAtom(FLOAT_VALUE(a1) / FLOAT_VALUE(a2)); 
                        NEXT;
                    }
                    if(AvmCore::isFloat4(a1) || AvmCore::isFloat4(a2)){
                        float4_decl_v(a1);
                        float4_decl_v(a2);
                        sp[0] = core->float4ToAtom( f4_div(a1v, a2v) ); 
                        NEXT;
                    }
                }
#endif // VMCFG_FLOAT
                d1 = AvmCore::number(a1);
                d2 = AvmCore::number(a2);
                sp[0] = core->doubleToAtom(d1 / d2);
                NEXT;
            }

            INSTR(modulo) {
                // FIXME - dodgy optimization?
                // Can the integer modulo overflow somehow?  Is it portable?
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
#ifdef VMCFG_WORDCODE_PEEPHOLE
            mod_two_values_and_next:
#endif
                if (IS_BOTH_INTEGER(a1, a2) && a2 != zeroIntAtom) {
                    i1 = INT32_VALUE(a1) % INT32_VALUE(a2);
                    if (atomIsValidIntptrValue(i1)) {
                        sp[0] = MAKE_INTEGER(i1);
                        NEXT;
                    }
                }
                else if (IS_BOTH_DOUBLE(a1, a2)) {
                    sp[0] = core->doubleToAtom(MathUtils::mod(DOUBLE_VALUE(a1), DOUBLE_VALUE(a2)));
                    NEXT;
                }

                SAVE_EXPC;
#ifdef VMCFG_FLOAT
                if( pool->hasFloatSupport() ) {
                    a1 = core->numericAtom(a1);
                    a2 = core->numericAtom(a2);
                    if( IS_BOTH_FLOAT(a1, a2) ) {
                        sp[0] = core->floatToAtom( (float) MathUtils::mod(FLOAT_VALUE(a1), FLOAT_VALUE(a2))); 
                        NEXT;
                    }
                    if(AvmCore::isFloat4(a1) || AvmCore::isFloat4(a2)){
                        float4_t x; AvmCore::float4(&x, a1);
                        float4_t y; AvmCore::float4(&y, a2);
                        float rx = (float) MathUtils::mod(f4_x(x), f4_x(y));
                        float ry = (float) MathUtils::mod(f4_y(x), f4_y(y));
                        float rz = (float) MathUtils::mod(f4_z(x), f4_z(y));
                        float rw = (float) MathUtils::mod(f4_w(x), f4_w(y));
                        float4_t res = {rx,ry,rz,rw};
                        sp[0] = core->float4ToAtom( res ); 
                        NEXT;
                    }
                }
#endif // VMCFG_FLOAT
                d1 = AvmCore::number(a1);
                d2 = AvmCore::number(a2);
                sp[0] = core->doubleToAtom(MathUtils::mod(d1, d2));
                NEXT;
            }

            INSTR(lshift) {
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
            //lshift_two_values_and_next:
                if (IS_BOTH_INTEGER(a1,a2)) {
                    i1 = INT32_VALUE(a1) << (INT32_VALUE(a2) & 0x1F);
                    if (atomIsValidIntptrValue(i1)) {
                        sp[0] = SIGN_EXTEND(MAKE_INTEGER(i1));
                        NEXT;
                    }
                }
                SAVE_EXPC;
                i1 = AvmCore::integer(a1);
                u2 = AvmCore::toUInt32(a2);
                sp[0] = core->intToAtom( (int32_t)(i1 << (u2 & 0x1F)) );
                NEXT;
            }

            INSTR(rshift) {
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
            //rshift_two_values_and_next:
                if (IS_BOTH_INTEGER(a1,a2)) {
                    sp[0] = MAKE_INTEGER(INT32_VALUE(a1) >> (INT32_VALUE(a2) & 0x1F));
                    NEXT;
                }
                SAVE_EXPC;
                i1 = AvmCore::integer(a1);
                u2 = AvmCore::toUInt32(a2);
                sp[0] = core->intToAtom( (int32_t)(i1 >> (u2 & 0x1F)) );
                NEXT;
            }

            INSTR(urshift) {
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
            //urshift_two_values_and_next:
                if (IS_BOTH_INTEGER(a1,a2)) {
                    u1 = (UINT32_VALUE(a1) >> (INT32_VALUE(a2) & 0x1F));
                    if (atomIsValidIntptrValue_u(u1)) {
                        sp[0] = MAKE_INTEGER(u1);
                        NEXT;
                    }
                }
                SAVE_EXPC;
                u1 = AvmCore::toUInt32(a1);
                u2 = AvmCore::toUInt32(a2);
                sp[0] = core->uintToAtom( (uint32_t)(u1 >> (u2 & 0x1F)) );
                NEXT;
            }

// The OR with tag is only necessary for xor, which passes kIntptrType.  The
// others pass 0, and we assume the compiler optimizes the OR away.

#define BITOP_TWO_VALUES_AND_NEXT(op, a1, a2, dest, tag) \
    if (IS_BOTH_INTEGER(a1,a2)) { \
        dest = (CLAMP_32((a1) op (a2)) | tag); \
        NEXT; \
    } \
    SAVE_EXPC; \
    i1 = AvmCore::integer(a1); \
    i2 = AvmCore::integer(a2); \
    dest = core->intToAtom((int32_t)(i1 op i2)); \
    NEXT

            INSTR(bitand) {
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
#ifdef VMCFG_WORDCODE_PEEPHOLE
            bitand_two_values_and_next:
#endif
                BITOP_TWO_VALUES_AND_NEXT(&, a1, a2, sp[0], 0);
            }

            INSTR(bitor) {
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
#ifdef VMCFG_WORDCODE_PEEPHOLE
            bitor_two_values_and_next:
#endif
                BITOP_TWO_VALUES_AND_NEXT(|, a1, a2, sp[0], 0);
            }

            INSTR(bitxor) {
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
#ifdef VMCFG_WORDCODE_PEEPHOLE
            bitxor_two_values_and_next:
#endif
                BITOP_TWO_VALUES_AND_NEXT(^, a1, a2, sp[0], kIntptrType);
            }

            INSTR(equals) {
                // OPTIMIZEME - equals on some classes of values?
                SAVE_EXPC;
                sp[-1] = core->equals(sp[-1], sp[0]);
                sp--;
                NEXT;
            }

            INSTR(strictequals) {
                // OPTIMIZEME - strictequals on some classes of values?
                sp[-1] = AvmCore::stricteq(sp[-1], sp[0]);
                sp--;
                NEXT;
            }

            INSTR(lookupswitch) {
#ifdef VMCFG_WORDCODE
                const uintptr_t* base = pc-1;
                uint32_t index = AvmCore::integer_i(*(sp--));
                intptr_t default_offset = S24ARG;
                uintptr_t case_count = U30ARG;
                if (index <= case_count)
                    pc = base + pc[index];
                else
                    pc = base + default_offset;
#else
                const uint8_t* base = pc-1;
                // safe to assume int since verifier checks for int
                uint32_t index = AvmCore::integer_i(*(sp--));
                const uint8_t* switch_pc = pc+3;
                uint32_t case_count = uint32_t(readU30(switch_pc)) + 1;
                pc = base+readS24( index < case_count ? (switch_pc + 3*index) : pc );
#endif
                if (pc <= base) {
                    // if backedge, need an interrupt check
                    SAVE_EXPC; // interrupts should originate from target of backedge
                    branchCheck(core, env, interruptable);
                }
                NEXT;
            }

            INSTR(iftrue) {
                a2 = trueAtom;
            branch_on_boolean:
                a1 = *(sp--);
                if (IS_BOOLEAN(a1))
                    ;
                else if (IS_INTEGER(a1))
                    a1 = a1 == zeroIntAtom ? falseAtom : trueAtom;
                else
                    a1 = AvmCore::booleanAtom(a1);  // does not throw or change the XML namespace
                i1 = S24ARG;
                if (a1 == a2)
                {
                    if (i1 < 0) {
                        SAVE_EXPC_TARGET(i1);
                        branchCheck(core, env, interruptable);
                        OSR(i1);
                    }
                    pc += i1;
                }
                NEXT;
            }

            INSTR(iffalse) {
                a2 = falseAtom;
                goto branch_on_boolean;
            }

// The client of IFCMP_TWO_VALUES must save the EXPC before extracting
// the operands from the instruction stream because it is used in various
// contexts where the instruction sizes and layouts aren't the same.

#define GENERATE_FLAG(numeric_cmp, generic_cmp, a1, a2) \
    if (IS_BOTH_INTEGER(a1, a2)) \
        b1 = a1 numeric_cmp a2; \
    else if (IS_BOTH_DOUBLE(a1, a2)) \
        b1 = DOUBLE_VALUE(a1) numeric_cmp DOUBLE_VALUE(a2); \
FLOAT_ONLY(\
    else if (IS_BOTH_FLOAT(a1, a2) && pool->hasFloatSupport()) \
        b1 = FLOAT_VALUE(a1) numeric_cmp FLOAT_VALUE(a2); \
)\
    else \
        b1 = generic_cmp

#define IFCMP_TWO_VALUES(numeric_cmp, generic_cmp, a1, a2, i1) \
    GENERATE_FLAG(numeric_cmp, generic_cmp, a1, a2); \
    if (b1) \
    { \
        if (i1 < 0) \
        { \
            SAVE_EXPC_TARGET(i1); \
            branchCheck(core, env, interruptable); \
            OSR(i1); \
        } \
        pc += i1; \
    }

#define IFNCMP_TWO_VALUES(numeric_cmp, generic_cmp, a1, a2, i1) \
    GENERATE_FLAG(numeric_cmp, generic_cmp, a1, a2); \
    if (!b1) \
    { \
        if (i1 < 0) \
        { \
            SAVE_EXPC_TARGET(i1); \
            branchCheck(core, env, interruptable); \
            OSR(i1); \
        } \
        pc += i1; \
    }

#define LOAD_OFFSET_AND_FETCH_SS(a1,a2,i1) \
    SAVE_EXPC; \
    a1 = sp[-1]; \
    a2 = sp[0]; \
    sp -= 2; \
    i1 = S24ARG

            INSTR(ifeq) {
                LOAD_OFFSET_AND_FETCH_SS(a1,a2,i1);
            PEEPHOLE_ONLY( compare_eq_and_branch_impl: )
                IFCMP_TWO_VALUES(==, core->equals(a1,a2) == trueAtom, a1, a2, i1);
                NEXT;
            }

            INSTR(ifne) {
                LOAD_OFFSET_AND_FETCH_SS(a1,a2,i1);
            PEEPHOLE_ONLY( compare_ne_and_branch_impl: )
                IFNCMP_TWO_VALUES(==, core->equals(a1,a2) == trueAtom, a1, a2, i1);
                NEXT;
            }

            INSTR(ifstricteq) {
                LOAD_OFFSET_AND_FETCH_SS(a1,a2,i1);
            PEEPHOLE_ONLY( compare_stricteq_and_branch_impl: )
                IFCMP_TWO_VALUES(==, AvmCore::stricteq(a1,a2) == trueAtom, a1, a2, i1);
                NEXT;
            }

            INSTR(ifstrictne) {
                LOAD_OFFSET_AND_FETCH_SS(a1,a2,i1);
            PEEPHOLE_ONLY( compare_strictne_and_branch_impl: )
                IFNCMP_TWO_VALUES(==, AvmCore::stricteq(a1,a2) == trueAtom, a1, a2, i1);
                NEXT;
            }

            INSTR(iflt) {
                LOAD_OFFSET_AND_FETCH_SS(a1,a2,i1);
            PEEPHOLE_ONLY( compare_lt_and_branch_impl: )
                IFCMP_TWO_VALUES(<, AvmCore::compare(a1,a2) == trueAtom, a1, a2, i1);
                NEXT;
            }

            INSTR(ifnlt) {
                LOAD_OFFSET_AND_FETCH_SS(a1,a2,i1);
            PEEPHOLE_ONLY( compare_nlt_and_branch_impl: )
                IFNCMP_TWO_VALUES(<, AvmCore::compare(a1, a2) == trueAtom, a1, a2, i1);
                NEXT;
            }

            INSTR(ifle) {
                LOAD_OFFSET_AND_FETCH_SS(a1,a2,i1);
            PEEPHOLE_ONLY( compare_le_and_branch_impl: )
                IFCMP_TWO_VALUES(<=, AvmCore::compare(a2, a1) == falseAtom,a1,a2,i1);
                NEXT;
            }

            INSTR(ifnle) {
                LOAD_OFFSET_AND_FETCH_SS(a1,a2,i1);
            PEEPHOLE_ONLY( compare_nle_and_branch_impl: )
                IFNCMP_TWO_VALUES(<=, AvmCore::compare(a2, a1) == falseAtom,a1,a2,i1);
                NEXT;
            }

            INSTR(ifgt) {
                LOAD_OFFSET_AND_FETCH_SS(a1,a2,i1);
            PEEPHOLE_ONLY( compare_gt_and_branch_impl: )
                IFCMP_TWO_VALUES(>, AvmCore::compare(a2, a1) == trueAtom,a1,a2,i1);
                NEXT;
            }

            INSTR(ifngt) {
                LOAD_OFFSET_AND_FETCH_SS(a1,a2,i1);
            PEEPHOLE_ONLY( compare_ngt_and_branch_impl: )
                IFNCMP_TWO_VALUES(>, AvmCore::compare(a2, a1) == trueAtom,a1,a2,i1);
                NEXT;
            }

            INSTR(ifge) {
                LOAD_OFFSET_AND_FETCH_SS(a1,a2,i1);
            PEEPHOLE_ONLY( compare_ge_and_branch_impl: )
                IFCMP_TWO_VALUES(>=, AvmCore::compare(a1, a2) == falseAtom,a1,a2,i1);
                NEXT;
            }

            INSTR(ifnge) {
                LOAD_OFFSET_AND_FETCH_SS(a1,a2,i1);
            PEEPHOLE_ONLY( compare_nge_and_branch_impl: )
                IFNCMP_TWO_VALUES(>=, AvmCore::compare(a1, a2) == falseAtom,a1,a2,i1);
                NEXT;
            }

#define CMP2(numeric_cmp, generic_cmp) \
    a1 = sp[-1]; \
    a2 = sp[0]; \
    sp--; \
    if (IS_BOTH_INTEGER(a1, a2))                                \
        b1 = a1 numeric_cmp a2; \
    else if (IS_BOTH_DOUBLE(a1, a2)) \
        b1 = DOUBLE_VALUE(a1) numeric_cmp DOUBLE_VALUE(a2); \
    else { \
        SAVE_EXPC; \
        b1 = generic_cmp; \
    } \
    sp[0] = b1 ? trueAtom : falseAtom;

            INSTR(lessthan) {
                CMP2(<, AvmCore::compare(a1,a2) == trueAtom);
                NEXT;
            }

            INSTR(lessequals) {
                CMP2(<=, AvmCore::compare(a2, a1) == falseAtom);
                NEXT;
            }

            INSTR(greaterthan) {
                CMP2(>, AvmCore::compare(a2, a1) == trueAtom);
                NEXT;
            }

            INSTR(greaterequals) {
                CMP2(>=, AvmCore::compare(a1, a2) == falseAtom);
                NEXT;
            }

            INSTR(newobject) {
                SAVE_EXPC;
                i1 = (intptr_t)U30ARG;
                a1 = env->op_newobject(sp, (int)i1)->atom();
                *(sp -= 2*i1-1) = a1;
                NEXT;
            }

            INSTR(newarray) {
                SAVE_EXPC;
                i1 = (intptr_t)U30ARG;
                a1 = avmplus::newarray(toplevel, (int)i1, sp-i1+1)->atom();
                *(sp -= i1-1) = a1;
                NEXT;
            }

// OPTIMIZEME - isRuntime and isRtns are known at translation time.
// Using type information in the verifier it may be possible to determine
// that isDictionaryLookup is false, too.  Should specialize instructions to
// handle common cases.
//
// But does it matter? getproperty, findproperty are heavyweight operations, and
// opportunities for early binding are resolved during translation.  Still, it may
// make a small difference in connection with avoiding multiname copying (now implemented),
// since most fields of the multiname data structure will not be accessed.

// OPTIMIZEME - inline fast case for toVTable?
// Presumably it will very often be a non-null object; now we have to make a call and
// a test and a switch to get to that.  Should we in-line?
// How will RTTI caching affect that?

#define GET_MULTINAME_PTR(decl, arg)  decl = pool->precomputedMultiname(uint32_t(arg))

            INSTR(getlex) {
                SAVE_EXPC;
                // findpropstrict + getproperty
                // stack in:  -
                // stack out: value
                GET_MULTINAME_PTR(multiname, U30ARG);
                // only non-runtime names are allowed.  but this still includes
                // wildcard and attribute names.
                a1 = env->findproperty(scope, scopeBase, scopeDepth, multiname, true, withBase);
                *(++sp) = toplevel->getproperty(a1, multiname, toplevel->toVTable(a1));
                NEXT;
            }

#if 0
            // Multiname is neither isRuntime or isRtns
            INSTR(getproperty_fast) {
                SAVE_EXPC;
                GET_MULTINAME_PTR(multiname, U30ARG);
                sp[0] = toplevel->getproperty(sp[0], multiname, toplevel->toVTable(sp[0]));
                NEXT;
            }
#endif

            // get a property using a multiname ref
            INSTR(getproperty) {
                SAVE_EXPC;
                GET_MULTINAME_PTR(multiname, U30ARG);
                if (!multiname->isRuntime())
                {
                    *sp = toplevel->getproperty(*sp, multiname, toplevel->toVTable(*sp));
                }
                else if (!multiname->isRtns() && IS_INTEGER(*sp) && atomCanBeUint32(*sp) && AvmCore::isObject(sp[-1]))
                {
                    a2 = *(sp--);   // key
                    *sp = AvmCore::atomToScriptObject(*sp)->getUintProperty(UINT32_VALUE(a2));
                }
                else if(multiname->isRtns() || !AvmCore::isDictionaryLookup(*sp, *(sp-1)))
                {
                    aux_memory->multiname2 = *multiname;
                    sp = initMultiname(env, aux_memory->multiname2, sp);
                    *sp = toplevel->getproperty(*sp, &aux_memory->multiname2, toplevel->toVTable(*sp));
                }
                else
                {
                    a2 = *(sp--);   // key
                    *sp = AvmCore::atomToScriptObject(*sp)->getAtomProperty(a2);
                }
                NEXT;
            }

            // set a property using a multiname ref
            INSTR(setproperty) {
                SAVE_EXPC;
                GET_MULTINAME_PTR(multiname, U30ARG);
                a1 = *(sp--);       // value
                if (!multiname->isRuntime())
                {
                    a3 = *(sp--);   // object
                    toplevel->setproperty(a3, multiname, a1, toplevel->toVTable(a3));
                }
                else if (!multiname->isRtns() && IS_INTEGER(*sp) && atomCanBeUint32(*sp) && AvmCore::isObject(sp[-1]))
                {
                    a2 = *(sp--);       // key
                    a3 = *(sp--);       // object
                    AvmCore::atomToScriptObject(a3)->setUintProperty(UINT32_VALUE(a2), a1);
                }
                else if(multiname->isRtns() || !AvmCore::isDictionaryLookup(*sp, *(sp-1)))
                {
                    aux_memory->multiname2 = *multiname;
                    sp = initMultiname(env, aux_memory->multiname2, sp);
                    a3 = *(sp--);       // object
                    toplevel->setproperty(a3, &aux_memory->multiname2, a1, toplevel->toVTable(a3));
                }
                else
                {
                    a2 = *(sp--);   // key
                    a3 = *(sp--);   // object
                    AvmCore::atomToScriptObject(a3)->setAtomProperty(a2, a1);
                }
                NEXT;
            }

            INSTR(initproperty) {
                SAVE_EXPC;
                GET_MULTINAME_PTR(multiname, U30ARG);
                a1 = *(sp--);       // value
                if (multiname->isRuntime())
                {
                    aux_memory->multiname2 = *multiname;
                    sp = initMultiname(env, aux_memory->multiname2, sp);
                    multiname = &aux_memory->multiname2;
                }
                a2 = *(sp--);   // object
                env->initproperty(a2, multiname, a1, toplevel->toVTable(a2));
                NEXT;
            }

            INSTR(getdescendants) {
                SAVE_EXPC;
                GET_MULTINAME_PTR(multiname, U30ARG);
                if (multiname->isRuntime())
                {
                    aux_memory->multiname2 = *multiname;
                    sp = initMultiname(env, aux_memory->multiname2, sp);
                    multiname = &aux_memory->multiname2;
                }
                sp[0] = env->getdescendants(sp[0], multiname);
                NEXT;
            }

            INSTR(checkfilter) {
                SAVE_EXPC;
                env->checkfilter(sp[0]);
                NEXT;
            }

            INSTR(findpropstrict) {
                b1 = true;
                goto findproperty_impl;
            }

            INSTR(findproperty) {
                b1 = false;
            findproperty_impl:
                SAVE_EXPC;
                GET_MULTINAME_PTR(multiname, U30ARG);
                if (multiname->isRuntime())
                {
                    aux_memory->multiname2 = *multiname;
                    sp = initMultiname(env, aux_memory->multiname2, sp);
                    multiname = &aux_memory->multiname2;
                }
                *(++sp) = env->findproperty(scope, scopeBase, scopeDepth, multiname, b1, withBase);
                NEXT;
            }

            INSTR(finddef) {
                SAVE_EXPC;
                u1 = U30ARG;
                o1 = finddef_cache((ScriptObject**)&(*env->abcEnv()->finddefTable())[(uint32_t)u1],
                                   &aux_memory->methodFrame);
                *(++sp) = o1->atom();
                NEXT;
            }

            INSTR(nextname) {
                SAVE_EXPC;
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
                // verifier checks for int
                sp[0] = env->nextname(a1, AvmCore::integer_i(a2));
                NEXT;
            }

            INSTR(nextvalue) {
                SAVE_EXPC;
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
                // verifier checks for int
                sp[0] = env->nextvalue(a1, AvmCore::integer_i(a2));
                NEXT;
            }

            INSTR(hasnext) {
                SAVE_EXPC;
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
                // verifier checks for int
                sp[0] = core->intToAtom(env->hasnext(a1, AvmCore::integer_i(a2)));
                NEXT;
            }

            INSTR(hasnext2) {
                SAVE_EXPC;
                u1 = U30ARG;                       // objReg
                u2  = U30ARG;                      // indexReg
                a1l = framep[u1];                  // objAtom
                i32l = AvmCore::integer(framep[u2]);  // index
                *(++sp) = env->hasnextproto(a1l, i32l) ? trueAtom : falseAtom;
                framep[u1] = a1l;
                framep[u2] = core->intToAtom(i32l);
                NEXT;
            }

            // sign extends -- results always fit into an atom, no need to call intToAtom
            // since we are downshifting anyway, integrate final upshift-by-3 into downshift
            // rather than using MAKE_INTEGER macro.
            INSTR(sxi1) {
                i1 = AvmCore::integer(sp[0]);
                sp[0] = CHECK_INT_ATOM(Atom(((i1 << (8*sizeof(Atom)-1)) >> ((8*sizeof(Atom)-1)-3)) | kIntptrType));
                NEXT;
            }

            INSTR(sxi8) {
                i1 = AvmCore::integer(sp[0]);
                sp[0] = CHECK_INT_ATOM(Atom(((i1 << (8*(sizeof(Atom)-1))) >> ((8*(sizeof(Atom)-1))-3)) | kIntptrType));
                NEXT;
            }

            INSTR(sxi16) {
                i1 = AvmCore::integer(sp[0]);
                sp[0] = CHECK_INT_ATOM(Atom(((i1 << (8*(sizeof(Atom)-2))) >> ((8*(sizeof(Atom)-2))-3)) | kIntptrType));
                NEXT;
            }

        // note that the mops "addr" (offset from globalMemoryBase) is in fact a signed int, so we have to check
        // for it being < 0 ... but we can get by with a single unsigned compare since all values < 0 will be > size
#define MOPS_RANGE_CHECK(addr, type) \
        if (uint32_t(addr) > (envDomain->globalMemorySize() - sizeof(type))) { avmplus::mop_rangeCheckFailed(env); }

#if defined(VMCFG_UNALIGNED_INT_ACCESS) && defined(VMCFG_LITTLE_ENDIAN)
    #define MOPS_LOAD_INT(addr, type, call, result) \
            MOPS_RANGE_CHECK(addr, type) \
            union { const uint8_t* p8; const type* p; }; \
            p8 = envDomain->globalMemoryBase() + (addr); \
            result = *p;

    #define MOPS_STORE_INT(addr, type, call, value) \
            MOPS_RANGE_CHECK(addr, type) \
            union { uint8_t* p8; type* p; }; \
            p8 = envDomain->globalMemoryBase() + (addr); \
            *p = (type)(value);
#else
    #define MOPS_LOAD_INT(addr, type, call, result) \
            MOPS_RANGE_CHECK(addr, type) \
            result = (type)avmplus::mop_##call(envDomain->globalMemoryBase() + (addr));

    #define MOPS_STORE_INT(addr, type, call, value) \
            MOPS_RANGE_CHECK(addr, type) \
            avmplus::mop_##call(envDomain->globalMemoryBase() + (addr), value);
#endif

#if defined(VMCFG_UNALIGNED_FP_ACCESS) && defined(VMCFG_LITTLE_ENDIAN)
    #define MOPS_LOAD_FP(addr, type, call, result) \
            MOPS_RANGE_CHECK(addr, type) \
            union { const uint8_t* p8; const type* p; }; \
            p8 = envDomain->globalMemoryBase() + (addr); \
            result = *p;

    #define MOPS_STORE_FP(addr, type, call, value) \
            MOPS_RANGE_CHECK(addr, type) \
            union { uint8_t* p8; type* p; }; \
            p8 = envDomain->globalMemoryBase() + (addr); \
            *p = (type)(value);
#else
    #define MOPS_LOAD_FP(addr, type, call, result) \
            MOPS_RANGE_CHECK(addr, type) \
            result = (type)avmplus::mop_##call(envDomain->globalMemoryBase() + (addr));

    #define MOPS_STORE_FP(addr, type, call, value) \
            MOPS_RANGE_CHECK(addr, type) \
            avmplus::mop_##call(envDomain->globalMemoryBase() + (addr), value);
#endif

#if defined(VMCFG_LITTLE_ENDIAN)
    // Note, this depends on one of three conditions being true:
    //  - float4_t is a generic structure type that is not mapped directly
    //    to the SIMD type, and ByteArray storage is 4-byte aligned
    //  - SIMD loads/stores to unaligned addresses is OK
    //  - ByteArray storage is 16-byte aligned
    #define MOPS_LOAD_F4(addr, call, result) \
            addr &= ~15; \
            MOPS_RANGE_CHECK(addr, float4_t); \
            union { const uint8_t* p8; const float4_t* p; }; \
            p8 = envDomain->globalMemoryBase() + (addr); \
            result = *p;

    #define MOPS_STORE_F4(addr, call, value) \
            addr &= ~15; \
            MOPS_RANGE_CHECK(addr, float4_t); \
            union { const uint8_t* p8; float4_t* p; }; \
            p8 = envDomain->globalMemoryBase() + (addr); \
            *p = (value);
#else
    #define MOPS_LOAD_F4(addr, call, result) \
            addr &= ~15; \
            MOPS_RANGE_CHECK(addr, float4_t); \
            avmplus::mop_##call(&result, envDomain->globalMemoryBase() + (addr));

    #define MOPS_STORE_F4(addr, call, value) \
            addr &= ~15; \
            MOPS_RANGE_CHECK(addr, float4_t); \
            avmplus::mop_##call(envDomain->globalMemoryBase() + (addr), value);
#endif

            // loads
#ifdef VMCFG_WORDCODE
            INSTR(lix8) {
                i1 = AvmCore::integer(sp[0]);       // i1 = addr
                MOPS_LOAD_INT(i1, int8_t, lix8, i32l);  // i32l = result
                sp[0] = MAKE_INTEGER(i32l);     // always fits in atom
                NEXT;
            }

            INSTR(lix16) {
                i1 = AvmCore::integer(sp[0]);       // i1 = addr
                MOPS_LOAD_INT(i1, int16_t, lix16, i32l);    // i32l = result
                sp[0] = MAKE_INTEGER(i32l);     // always fits in atom
                NEXT;
            }
#endif

            INSTR(li8) {
                i1 = AvmCore::integer(sp[0]);       // i1 = addr
                MOPS_LOAD_INT(i1, uint8_t, liz8, ub2);  // ub2 = result
                sp[0] = MAKE_INTEGER(ub2);      // always fits in atom
                NEXT;
            }

            INSTR(li16) {
                i1 = AvmCore::integer(sp[0]);       // i1 = addr
                MOPS_LOAD_INT(i1, uint16_t, liz16, uh2l);   // uh2l = result
                sp[0] = MAKE_INTEGER(uh2l);     // always fits in atom
                NEXT;
            }

            INSTR(li32) {
                i1 = AvmCore::integer(sp[0]);       // i1 = addr
                MOPS_LOAD_INT(i1, int32_t, li32, i32l); // i32l = result
                sp[0] = core->intToAtom(i32l);
                NEXT;
            }

            INSTR(lf32) {
                i1 = AvmCore::integer(sp[0]);       // i1 = addr
                MOPS_LOAD_FP(i1, float, lf32, f2l);     // f2l = result
                sp[0] = core->doubleToAtom(f2l);
                NEXT;
            }

            INSTR(lf64) {
                i1 = AvmCore::integer(sp[0]);       // i1 = addr
                MOPS_LOAD_FP(i1, double, lf64, d2l);        // d2l = result
                sp[0] = core->doubleToAtom(d2l);
                NEXT;
            }

#ifdef VMCFG_FLOAT
            INSTR(lf32x4) {
                i1 = AvmCore::integer(sp[0]);       // i1 = addr
                MOPS_LOAD_F4(i1, lf32x4, f4l);        // f4l = result
                sp[0] = core->float4ToAtom(f4l);
                NEXT;
            }
#endif

            // stores
            INSTR(si8) {
                ub2 = (uint8_t)AvmCore::integer(sp[-1]);    // u2 = value
                i1 = AvmCore::integer(sp[0]);               // i1 = addr
                MOPS_STORE_INT(i1, uint8_t, si8, ub2);
                sp -= 2;
                NEXT;
            }

            INSTR(si16) {
                uh2l = (uint16_t)AvmCore::integer(sp[-1]);  // uh2l = value
                i1 = AvmCore::integer(sp[0]);       // i1 = addr
                MOPS_STORE_INT(i1, uint16_t, si16, uh2l);
                sp -= 2;
                NEXT;
            }

            INSTR(si32) {
                i32l = AvmCore::integer(sp[-1]);    // i32l = value
                i1 = AvmCore::integer(sp[0]);       // i1 = addr
                MOPS_STORE_INT(i1, uint32_t, si32, i32l);
                sp -= 2;
                NEXT;
            }

            INSTR(sf32) {
                f2l = (float)AvmCore::number(sp[-1]);       // d2l = value
                i1 = AvmCore::integer(sp[0]);       // i1 = addr
                MOPS_STORE_FP(i1, float, sf32, f2l);
                sp -= 2;
                NEXT;
            }

            INSTR(sf64) {
                d2l = AvmCore::number(sp[-1]);
                i1 = AvmCore::integer(sp[0]);
                MOPS_STORE_FP(i1, double, sf64, d2l);
                sp -= 2;
                NEXT;
            }

#ifdef VMCFG_FLOAT
            INSTR(sf32x4) {
                AvmCore::float4(&f4l, sp[-1]);
                i1 = AvmCore::integer(sp[0]);
                MOPS_STORE_F4(i1, sf32x4, f4l);
                sp -= 2;
                NEXT;
            }
#endif
                   
            // delete property using multiname
            INSTR(deleteproperty) {
                SAVE_EXPC;
                GET_MULTINAME_PTR(multiname, U30ARG);
                if (!multiname->isRuntime())
                {
                    sp[0] = env->delproperty(sp[0], multiname);
                }
                else if (!multiname->isRtns() && IS_INTEGER(*sp) && atomCanBeUint32(*sp) && AvmCore::isObject(sp[-1]))
                {
                    a2 = *(sp--);   // key
                    *sp = AvmCore::atomToScriptObject(*sp)->delUintProperty(UINT32_VALUE(a2)) ? trueAtom : falseAtom;
                }
                else if(multiname->isRtns() || !AvmCore::isDictionaryLookup(*sp, *(sp-1)))
                {
                    aux_memory->multiname2 = *multiname;
                    sp = initMultinameNoXMLList(env, aux_memory->multiname2, sp);
                    sp[0] = env->delproperty(sp[0], &aux_memory->multiname2);
                }
                else
                {
                    a2 = *(sp--);   // key
                    sp[0] = AvmCore::atomToScriptObject(sp[0])->deleteAtomProperty(a2) ? trueAtom : falseAtom;
                }
                NEXT;
            }

            INSTR(setslot) {
                SAVE_EXPC;
                a1 = sp[-1];
                a2 = sp[0];
                sp -= 2;
                env->nullcheck(a1);
                u1 = U30ARG-1;    // slot_id
                AvmCore::atomToScriptObject(a1)->coerceAndSetSlotAtom((uint32_t)u1, a2);
                NEXT;
            }

            INSTR(getslot) {
                SAVE_EXPC;
                env->nullcheck(sp[0]);
                // OPTIMIZEME - cleanup after ABC interpreter defenestration.
                // Perform the -1 adjustment in the bytecode translator, not here every time.
                sp[0] = AvmCore::atomToScriptObject(sp[0])->getSlotAtom((uint32_t)U30ARG-1);
                NEXT;
            }

            INSTR(setglobalslot) {
                SAVE_EXPC;
                // find the global activation scope (object at depth 0 on scope chain)
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=663989
                // Note: the global scope can legally be any non-null. non-undefined Atom; 
                // however, the Verifier requires that the argument for setglobalslot 
                // be early-bindable. This precludes the "*" type, and any type defined
                // in a different ABC block; since primitive (non-ScriptObject) types are
                // builtin, this prevents user code from using this opcode on such a type.
                AvmAssert(AvmCore::isObject(globalScopeAtom));
#else
                AvmAssert(globalScope != NULL);
#endif

                // OPTIMIZEME - cleanup after ABC interpreter defenestration.
                // Perform the -1 adjustment in the bytecode translator, not here every time.
                u1 = U30ARG-1;    // slot_id
                a1 = sp[0];       // value
                sp--;
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=663989
                AvmCore::atomToScriptObject(globalScopeAtom)->coerceAndSetSlotAtom((uint32_t)u1, a1);
#else
                globalScope->coerceAndSetSlotAtom((uint32_t)u1, a1);
#endif
                NEXT;
            }

            INSTR(getglobalslot) {
                SAVE_EXPC;
                // find the global activation scope (object at depth 0 on scope chain)
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=663989
                // Note: the global scope can legally be any non-null. non-undefined Atom; 
                // however, the Verifier requires that the argument for getglobalslot 
                // be early-bindable. This precludes the "*" type, and any type defined
                // in a different ABC block; since primitive (non-ScriptObject) types are
                // builtin, this prevents user code from using this opcode on such a type.
                AvmAssert(AvmCore::isObject(globalScopeAtom));
#else
                AvmAssert(globalScope != NULL);
#endif

                // OPTIMIZMEME - cleanup after ABC interpreter defenestration.
                // Perform the -1 adjustment in the bytecode translator, not here every time.
                sp++;
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=663989
                sp[0] = AvmCore::atomToScriptObject(globalScopeAtom)->getSlotAtom((uint32_t)(U30ARG-1));
#else
                sp[0] = globalScope->getSlotAtom((uint32_t)(U30ARG-1));
#endif
                NEXT;
            }

            // OPTIMIZEME - presumably there are many ways in which the call opcodes may be specialized
            // to avoid the full function prologue?

            INSTR(call) {
                SAVE_EXPC;
                i1 = (intptr_t)U30ARG; // argc
                // stack in: function, receiver, arg1, ... argN
                // stack out: result
                a1 = avmplus::op_call(toplevel, sp[-i1-1]/*function*/, (int32_t)i1, sp-i1);
                *(sp = sp-i1-1) = a1;
                NEXT;
            }

            INSTR(construct) {
                SAVE_EXPC;
                i1 = (intptr_t)U30ARG;  // argc
                // stack in: function, arg1, ..., argN
                // stack out: new instance
                a1 = toplevel->op_construct(sp[-i1]/*function*/, (int32_t)i1, sp-i1);
                *(sp = sp-i1) = a1;
                NEXT;
            }

            INSTR(newfunction) {
                SAVE_EXPC;
                sp++;
                MethodInfo *body = pool->getMethodInfo((uint32_t)U30ARG);
                sp[0] = env->newfunction(body, scopeBase)->atom();
                NEXT;
            }

            INSTR(newclass) {
                SAVE_EXPC;
                u1 = U30ARG;
                Traits* ctraits = pool->getClassTraits((uint32_t)u1);
                o1 = (ScriptObject*)(~7 & toplevel->coerce(sp[0], CLASS_TYPE));
                sp[0] = env->newclass(ctraits, (ClassClosure*)o1, scope, scopeBase)->atom();
                NEXT;
            }

            INSTR(callstatic) {
                SAVE_EXPC;
                // stack in: receiver, arg1..N
                // stack out: result
                u1 = U30ARG;            // method_id
                i2 = (intptr_t)U30ARG;  // argc
                env->nullcheck(sp[-i2]);
                // ISSUE if arg types were checked in verifier, this coerces again.
                f = env->abcEnv()->getMethod((uint32_t)u1);
                // ensure the caller matches receiver
                sp[-i2] = toplevel->coerce(sp[-i2], f->method->getMethodSignature()->paramTraits(0));
                a1 = f->coerceEnter((int32_t)i2, sp-i2);
                *(sp -= i2) = a1;
                NEXT;
            }

            INSTR(callmethod) {
                SAVE_EXPC;
                // stack in: receiver, arg1..N
                // stack out: result
                u1 = U30ARG-1;         // disp_id
                i2 = (intptr_t)U30ARG; // argc
                a2p = sp-i2;           // atomv

                // must be a real class instance for this to be used.  primitives that have
                // methods will only have final bindings and no dispatch table.
                VTable* vtable = toplevel->toVTable(a2p[0]); // includes null check
                AvmAssert(u1 < vtable->traits->getTraitsBindings()->methodCount);
                f = vtable->methods[u1];
                // ISSUE if arg types were checked in verifier, this coerces again.
                a1 = f->coerceEnter((int32_t)i2, a2p);
                *(sp -= i2) = a1;
                NEXT;
            }

            INSTR(callproperty) {
                u1 = WORD_CODE_ONLY(WOP_callproperty) ABC_CODE_ONLY(OP_callproperty);
            callproperty_impl:
                SAVE_EXPC;
                GET_MULTINAME_PTR(multiname, U30ARG);
                i1 = (intptr_t)U30ARG; /* argc */
                a2p = sp - i1; /* atomv */
                sp = a2p;
                if (multiname->isRuntime())
                {
                    aux_memory->multiname2 = *multiname;
                    sp = initMultiname(env, aux_memory->multiname2, sp);
                    multiname = &aux_memory->multiname2;
                }
                a1 = *sp; /* base */
                if (u1 == WORD_CODE_ONLY(WOP_callproplex) ABC_CODE_ONLY(OP_callproplex))
                    a2p[0] = nullObjectAtom;
                *sp = toplevel->callproperty(a1, multiname, (int32_t)i1, a2p, toplevel->toVTable(a1));
                if (u1 == WORD_CODE_ONLY(WOP_callpropvoid) ABC_CODE_ONLY(OP_callpropvoid))
                    sp--;
                NEXT;
            }

            INSTR(callproplex) {
                u1 = WORD_CODE_ONLY(WOP_callproplex) ABC_CODE_ONLY(OP_callproplex);
                goto callproperty_impl;
            }

            INSTR(callpropvoid) {
                u1 = WORD_CODE_ONLY(WOP_callpropvoid) ABC_CODE_ONLY(OP_callpropvoid);
                goto callproperty_impl;
            }

            INSTR(constructprop) {
                SAVE_EXPC;
                GET_MULTINAME_PTR(multiname, U30ARG);
                i1 = (intptr_t)U30ARG;  // argc
                sp -= i1;
                a2p = sp;               // argv
                if (multiname->isRuntime())
                {
                    aux_memory->multiname2 = *multiname;
                    sp = initMultiname(env, aux_memory->multiname2, sp);
                    multiname = &aux_memory->multiname2;
                    a2p[0] = *sp;
                }
                *sp = toplevel->constructprop(multiname, (int32_t)i1, a2p, toplevel->toVTable(a2p[0]));
                NEXT;
            }

            INSTR(applytype) {
                SAVE_EXPC;
                i1 = (intptr_t)U30ARG;  // argc
                // stack in: factory, arg1, ... argN
                // stack out: result
                a1 = op_applytype(env, sp[-i1]/*function*/, (int32_t)i1, sp-i1+1);
                *(sp = sp-i1) = a1;
                NEXT;
            }

            INSTR(callsuper) {
                u1 = WORD_CODE_ONLY(WOP_callsuper) ABC_CODE_ONLY(OP_callsuper);
            callsuper_impl:
                SAVE_EXPC;
                GET_MULTINAME_PTR(multiname, U30ARG);
                i1 = (intptr_t)U30ARG; /* argc */
                sp -= i1;
                a2p = sp; /* atomv */
                if (multiname->isRuntime())
                {
                    aux_memory->multiname2 = *multiname;
                    sp = initMultiname(env, aux_memory->multiname2, sp);
                    multiname = &aux_memory->multiname2;
                    a2p[0] = *sp;
                }
                env->nullcheck(a2p[0]);
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=671134
                // ensure the receiver matches supertype
                a2p[0] = toplevel->coerce(a2p[0], env->method->declaringTraits()->base);
#endif /* MARK_SECURITY_CHANGE */
                *sp = env->callsuper(multiname, (int32_t)i1, a2p);
                if (u1 == WORD_CODE_ONLY(WOP_callsupervoid) ABC_CODE_ONLY(OP_callsupervoid))
                    sp--;
                NEXT;
            }

            INSTR(callsupervoid) {
                u1 = WORD_CODE_ONLY(WOP_callsupervoid) ABC_CODE_ONLY(OP_callsupervoid);
                goto callsuper_impl;
            }

            // An possible optimization is to split instructions that have a runtime multiname
            // into two: one that initializes aux_memory->multiname2, and another that simply picks
            // up that value.  That way there will be less code in the interpreter, and the
            // common case will be faster.

            INSTR(getsuper) {
                a1 = 0;         // not a value - instruction is getsuper
            getsuper_setsuper_impl:
                SAVE_EXPC;
                GET_MULTINAME_PTR(multiname, U30ARG);
                if (multiname->isRuntime())
                {
                    aux_memory->multiname2 = *multiname;
                    sp = initMultiname(env, aux_memory->multiname2, sp);
                    multiname = &aux_memory->multiname2;
                }
                a2 = *(sp--);   // object
                env->nullcheck(a2);
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=671134
                // ensure the caller matches supertype
                a2 = toplevel->coerce(a2, env->method->declaringTraits()->base);
#endif /* MARK_SECURITY_CHANGE */
                if (a1 == 0)
                    *(++sp) = env->getsuper(a2, multiname);
                else
                    env->setsuper(a2, multiname, a1);
                NEXT;
            }

            INSTR(setsuper) {
                a1 = *(sp--);   // value
                goto getsuper_setsuper_impl;
            }

            // obj arg1 arg2
            //           sp
            INSTR(constructsuper) {
                SAVE_EXPC;
                // stack in:  obj arg1..N
                // stack out:
                i1 = (intptr_t)U30ARG;  // argc
                env->nullcheck(sp[-i1]);
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=671134
                // ensure the receiver matches supertype
                sp[-i1] = toplevel->coerce(sp[-i1], env->method->declaringTraits()->base);
#endif /* MARK_SECURITY_CHANGE */
                env->super_init()->coerceEnter((int32_t)i1, sp-i1);
                sp -= i1+1;
                NEXT;
            }

#ifndef VMCFG_WORDCODE
            INSTR(pushshort) {
                // this just pushes an integer since we dont have short atoms
                *(++sp) = MAKE_INTEGER(((int16_t)U30ARG));
                NEXT;
            }
#endif

            INSTR(astype) {
                SAVE_EXPC;
                GET_MULTINAME_PTR(multiname, U30ARG);
                sp[0] = AvmCore::astype(sp[0], getTraits(multiname, pool, toplevel, core));
                NEXT;
            }

            INSTR(astypelate) {
                SAVE_EXPC;
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
                sp[0] = AvmCore::astype(a1, toplevel->toClassITraits(a2));
                NEXT;
            }

            INSTR(coerce) {
                SAVE_EXPC;
                // expects a CONSTANT_Multiname cpool index
                // this is the ES4 implicit coersion
                GET_MULTINAME_PTR(multiname, U30ARG);
                sp[0] = toplevel->coerce(sp[0], getTraits(multiname, pool, toplevel, core));
                NEXT;
            }

            INSTR(coerce_o) {
                if (sp[0] == undefinedAtom)
                    sp[0] = nullObjectAtom;
                NEXT;
            }

            INSTR(coerce_s) {
                a1 = sp[0];
                if (!IS_STRING(a1)) {
                    SAVE_EXPC;
                    sp[0] = AvmCore::isNullOrUndefined(a1) ? nullStringAtom : core->string(a1)->atom();
                }
                NEXT;
            }

            // OPTIMIZEME - early binding for a2 of 'is'?
            // (or outright removal in the translator?)

            INSTR(istype) {
                SAVE_EXPC;
                // expects a CONSTANT_Multiname cpool index
                // used when operator "is" RHS is a compile-time type constant
                GET_MULTINAME_PTR(multiname, U30ARG);
                t1 = getTraits(multiname, pool, toplevel, core);    // itraits
                sp[0] = AvmCore::istypeAtom(sp[0], t1);
                NEXT;
            }

            INSTR(istypelate) {
                SAVE_EXPC;
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
                sp[0] = AvmCore::istypeAtom(a1, toplevel->toClassITraits(a2));
                NEXT;
            }

#ifndef VMCFG_WORDCODE
            INSTR(pushbyte) {
                *(++sp) = MAKE_INTEGER((int8_t)U8ARG);
                NEXT;
            }
#endif

            INSTR(getscopeobject) {
                u1 = U8ARG;  // scope_index
                *(++sp) = scopeBase[(uint32_t)u1];
                NEXT;
            }

            INSTR(getouterscope) {
                u1 = U30ARG;  // scope_index
                *(++sp) = scope->getScope((uint32_t)u1);
                NEXT;
            }

            INSTR(getglobalscope) {
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=663989
                AvmAssert(!AvmCore::isNullOrUndefined(globalScopeAtom));
                *(++sp) = globalScopeAtom;
#else
                AvmAssert(globalScope != NULL);
                *(++sp) = globalScope->atom();
#endif
                NEXT;
            }

            INSTR(pushscope) {
                a1 = sp[0]; // scope
                sp--;
                if (AvmCore::isNullOrUndefined(a1)) {
                    SAVE_EXPC;
                    env->nullcheck(a1);
                }
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=663989
                if (scopeDepth == 0 && AvmCore::isNullOrUndefined(globalScopeAtom))
                {
                    AvmAssert(scope->getSize() == 0);
                    globalScopeAtom = a1;
                }
#else
                if (scopeDepth == 0 && globalScope == NULL)
                {
                    AvmAssert(scope->getSize() == 0);
                    globalScope = AvmCore::atomToScriptObject(a1);
                }
#endif
                scopeBase[scopeDepth++] = a1;
                NEXT;
            }

            INSTR(pushwith) {
                a1 = sp[0]; // scope object
                sp--;
                if (AvmCore::isNullOrUndefined(a1)) {
                    SAVE_EXPC;
                    env->nullcheck(a1);
                }
                if (!withBase)
                    withBase = scopeBase+scopeDepth;
                // is it possible to have pushWith for scope 0? not sure, let's do this just in case
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=663989
                if (scopeDepth == 0 && AvmCore::isNullOrUndefined(globalScopeAtom))
                {
                    AvmAssert(scope->getSize() == 0);
                    globalScopeAtom = a1;
                }
#else
                if (scopeDepth == 0 && globalScope == NULL)
                {
                    AvmAssert(scope->getSize() == 0);
                    globalScope = AvmCore::atomToScriptObject(a1);
                }
#endif
                scopeBase[scopeDepth++] = a1;
                NEXT;
            }

            INSTR(newactivation) {
                SAVE_EXPC;
                *(++sp) = env->newActivation()->atom();
                NEXT;
            }

            INSTR(newcatch) {
                SAVE_EXPC;
                u1 = U30ARG;  // catch_index
#ifdef VMCFG_WORDCODE
                t1 = info->word_code_exceptions()->exceptions[u1].scopeTraits;
#else
                t1 = info->abc_exceptions()->exceptions[u1].scopeTraits;
#endif
                *(++sp) = env->newcatch(t1)->atom();
                NEXT;
            }

            INSTR(popscope) {
                if (!--scopeDepth)
                {
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=663989
                    if (scope->getSize() == 0)
                        globalScopeAtom = nullObjectAtom;
#else
                    if (scope->getSize() == 0)
                        globalScope = NULL;
#endif
                }
            #ifdef DEBUGGER
                // in debugger builds, ensure that non-active scope entries are nulled out
                scopeBase[scopeDepth] = nullObjectAtom;
            #endif
                if (withBase >= scopeBase + scopeDepth)
                    withBase = NULL;
                NEXT;
            }

            INSTR(convert_i) {
            ABC_CODE_ONLY( convert_i_impl: )
                a1 = sp[0];
                if (!IS_INTEGER(a1)) {
                    SAVE_EXPC;
                    sp[0] = core->intAtom(a1);
                }
                else {
                    sp[0] = CLAMP_32(a1);
                }
                NEXT;
            }

#ifndef VMCFG_WORDCODE
            INSTR(coerce_i) {
                goto convert_i_impl;
            }
#endif

            INSTR(convert_u) {
            ABC_CODE_ONLY( convert_u_impl: )
                a1 = sp[0];
                if (!IS_INTEGER(a1) || !atomCanBeUint32(a1)) {
                    SAVE_EXPC;
                    sp[0] = core->uintAtom(a1);
                }
                NEXT;
            }

#ifndef VMCFG_WORDCODE
            INSTR(coerce_u) {
                goto convert_u_impl;
            }
#endif

            INSTR(throw) {
                SAVE_EXPC;
                core->throwAtom(*sp--);
                // unreachable
                NEXT;
            }

            INSTR(instanceof) {
                SAVE_EXPC;
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
                sp[0] = toplevel->instanceof(a1, a2);
                NEXT;
            }

            INSTR(in) {
                SAVE_EXPC;
                a1 = sp[-1];
                a2 = sp[0];
                sp--;
                sp[0] = toplevel->in_operator(a1, a2);
                NEXT;
            }

            INSTR(dxns) {
                AvmAssert(info->setsDxns());
                SAVE_EXPC;
                core->setDxns(&aux_memory->methodFrame, pool->getString((uint32_t)U30ARG));
                NEXT;
            }

            INSTR(dxnslate) {
                AvmAssert(info->setsDxns());
                SAVE_EXPC;
                core->setDxnsLate(&aux_memory->methodFrame, *sp);
                sp--;
                NEXT;
            }

#if defined(VMCFG_WORDCODE) && !defined(VMCFG_DIRECT_THREADED)
            // Fleshes out the dispatch table so that it's 0..255, allows
            // some compilers to generate better code for the switch at the
            // top, which switches on the low 8 bits.  (0 is an illegal
            // opcode; 255 is OP_ext, for which there's a case below.)
            case 0: {
                AvmAssert(false);
            }
#endif

#ifdef VMCFG_WORDCODE
#  ifdef MSVC_X86_REWRITE_THREADING
            default:
                // Keep L_illegal_op and L_push_doublebits alive...
                if ((int)pc > 0x100000)
                    goto L_push_doublebits;
                break;
#  endif

            INSTR(pushbits) {
                *++sp = *pc++;
                NEXT;
            }

            // OPTIMIZEME - push_doublebits should probably not cons up a new atom every time,
            // it would be better to keep it in the constant pool.
            //
            // OPTIMIZEME - on 64-bit systems we don't need two operand words here
            //
            // OPTIMIZEME - we can do better if unaligned int64 loads or unaligned fp loads
            // are allowed on the platform.

            INSTR(push_doublebits) {
                // Native layout in the instruction stream, just copy it over
                double_overlay d;
                d.bits32[0] = (uint32_t)*pc++;
                d.bits32[1] = (uint32_t)*pc++;
                *++sp = core->doubleToAtom(d.value);
                NEXT;
            }

#  ifdef VMCFG_WORDCODE_PEEPHOLE

#    define FETCH_LL(a1, a2) \
        u1 = *pc++;             \
        a1 = framep[u1 & 65535];\
        a2 = framep[u1 >> 16]

#    define LOAD_OFFSET_AND_FETCH_LL(a1, a2, i1) \
        SAVE_EXPC; \
        i1 = (intptr_t)*pc++; \
        FETCH_LL(a1, a2)

#    define FETCH_LB(a1, a2) \
        a1 = framep[*pc++]; \
        a2 = *pc++

#    define LOAD_OFFSET_AND_FETCH_LB(a1, a2, i1) \
        SAVE_EXPC; \
        i1 = (intptr_t)*pc++; \
        FETCH_LB(a1, a2)

            // Superwords not in the instruction set.  These are selected by a table
            // driven peephole optimizer, see comments and code in core/WordcodeTranslator.cpp.

            INSTR(get2locals) {
                u1 = *pc++;
                *(++sp) = framep[u1 & 65535];
                *(++sp) = framep[u1 >> 16];
                NEXT;
            }

            INSTR(get3locals) {
                u1 = *pc++;
                *(++sp) = framep[u1 & 1023];
                u1 >>= 10;
                *(++sp) = framep[u1 & 1023];
                *(++sp) = framep[u1 >> 10];
                NEXT;
            }

            INSTR(get4locals) {
                u1 = *pc++;
                *(++sp) = framep[u1 & 255];
                u1 >>= 8;
                *(++sp) = framep[u1 & 255];
                u1 >>= 8;
                *(++sp) = framep[u1 & 255];
                *(++sp) = framep[u1 >> 8];
                NEXT;
            }

            INSTR(get5locals) {
                u1 = *pc++;
                *(++sp) = framep[u1 & 63];
                u1 >>= 6;
                *(++sp) = framep[u1 & 63];
                u1 >>= 6;
                *(++sp) = framep[u1 & 63];
                u1 >>= 6;
                *(++sp) = framep[u1 & 63];
                *(++sp) = framep[u1 >> 6];
                NEXT;
            }

            INSTR(storelocal) {
                framep[*pc++] = *sp;
                NEXT;
            }

            INSTR(add_ll) {
                FETCH_LL(a1,a2);
                ++sp;
                goto add_two_values_into_tos_impl;
            }

            INSTR(add_set_lll) {
                u1 = *pc++;
                a1=framep[u1 & 1023];
                u1 >>= 10;
                a2=framep[u1 & 1023];
                ADD_TWO_VALUES_AND_NEXT(a1, a2, framep[u1 >> 10]);
            }

            INSTR(subtract_ll) {
                FETCH_LL(a1,a2);
                ++sp;
                goto sub_two_values_and_next;
            }

            INSTR(multiply_ll) {
                FETCH_LL(a1,a2);
                ++sp;
                goto mul_two_values_and_next;
            }

            INSTR(divide_ll) {
                FETCH_LL(a1,a2);
                ++sp;
                goto div_two_values_and_next;
            }

            INSTR(modulo_ll) {
                FETCH_LL(a1,a2);
                ++sp;
                goto mod_two_values_and_next;
            }

            INSTR(bitand_ll) {
                FETCH_LL(a1,a2);
                ++sp;
                goto bitand_two_values_and_next;
            }

            INSTR(bitor_ll) {
                FETCH_LL(a1,a2);
                ++sp;
                goto bitor_two_values_and_next;
            }

            INSTR(bitxor_ll) {
                FETCH_LL(a1,a2);
                ++sp;
                goto bitxor_two_values_and_next;
            }

            // OPTIMIZEME - redundant type check in superword.
            // As long as ext_pushbits is only used for integer data we know that
            // a2 is an int in the cases below, so the macros need not check.

            INSTR(add_lb) {
                FETCH_LB(a1, a2);
                ++sp;
                goto add_two_values_into_tos_impl;
            }

            INSTR(subtract_lb) {
                FETCH_LB(a1, a2);
                ++sp;
                goto sub_two_values_and_next;
            }

            INSTR(multiply_lb) {
                FETCH_LB(a1, a2);
                ++sp;
                goto mul_two_values_and_next;
            }

            INSTR(divide_lb) {
                FETCH_LB(a1, a2);
                ++sp;
                goto div_two_values_and_next;
            }

            INSTR(bitand_lb) {
                FETCH_LB(a1, a2);
                ++sp;
                goto bitand_two_values_and_next;
            }

            INSTR(bitor_lb) {
                FETCH_LB(a1, a2);
                ++sp;
                goto bitor_two_values_and_next;
            }

            INSTR(bitxor_lb) {
                FETCH_LB(a1, a2);
                ++sp;
                goto bitxor_two_values_and_next;
            }

            INSTR(iflt_ll) {
                LOAD_OFFSET_AND_FETCH_LL(a1,a2,i1);
                goto compare_lt_and_branch_impl;
            }

            INSTR(ifnlt_ll) {
                LOAD_OFFSET_AND_FETCH_LL(a1,a2,i1);
                goto compare_nlt_and_branch_impl;
            }

            INSTR(ifle_ll) {
                LOAD_OFFSET_AND_FETCH_LL(a1,a2,i1);
                goto compare_le_and_branch_impl;
            }

            INSTR(ifnle_ll) {
                LOAD_OFFSET_AND_FETCH_LL(a1,a2,i1);
                goto compare_nle_and_branch_impl;
            }

            INSTR(ifgt_ll) {
                LOAD_OFFSET_AND_FETCH_LL(a1,a2,i1);
                goto compare_gt_and_branch_impl;
            }

            INSTR(ifngt_ll) {
                LOAD_OFFSET_AND_FETCH_LL(a1,a2,i1);
                goto compare_ngt_and_branch_impl;
            }

            INSTR(ifge_ll) {
                LOAD_OFFSET_AND_FETCH_LL(a1,a2,i1);
                goto compare_ge_and_branch_impl;
            }

            INSTR(ifnge_ll) {
                LOAD_OFFSET_AND_FETCH_LL(a1,a2,i1);
                goto compare_nge_and_branch_impl;
            }

            INSTR(ifeq_ll) {
                LOAD_OFFSET_AND_FETCH_LL(a1,a2,i1);
                goto compare_eq_and_branch_impl;
            }

            INSTR(ifne_ll) {
                LOAD_OFFSET_AND_FETCH_LL(a1,a2,i1);
                goto compare_ne_and_branch_impl;
            }

            INSTR(ifstricteq_ll) {
                LOAD_OFFSET_AND_FETCH_LL(a1,a2,i1);
                goto compare_stricteq_and_branch_impl;
            }

            INSTR(ifstrictne_ll) {
                LOAD_OFFSET_AND_FETCH_LL(a1,a2,i1);
                goto compare_strictne_and_branch_impl;
            }

            INSTR(iflt_lb) {
                LOAD_OFFSET_AND_FETCH_LB(a1,a2,i1);
                goto compare_lt_and_branch_impl;
            }

            INSTR(ifnlt_lb) {
                LOAD_OFFSET_AND_FETCH_LB(a1,a2,i1);
                goto compare_nlt_and_branch_impl;
            }

            INSTR(ifle_lb) {
                LOAD_OFFSET_AND_FETCH_LB(a1,a2,i1);
                goto compare_le_and_branch_impl;
            }

            INSTR(ifnle_lb) {
                LOAD_OFFSET_AND_FETCH_LB(a1,a2,i1);
                goto compare_nle_and_branch_impl;
            }

            INSTR(ifgt_lb) {
                LOAD_OFFSET_AND_FETCH_LB(a1,a2,i1);
                goto compare_gt_and_branch_impl;
            }

            INSTR(ifngt_lb) {
                LOAD_OFFSET_AND_FETCH_LB(a1,a2,i1);
                goto compare_ngt_and_branch_impl;
            }

            INSTR(ifge_lb) {
                LOAD_OFFSET_AND_FETCH_LB(a1,a2,i1);
                goto compare_ge_and_branch_impl;
            }

            INSTR(ifnge_lb) {
                LOAD_OFFSET_AND_FETCH_LB(a1,a2,i1);
                goto compare_nge_and_branch_impl;
            }

            INSTR(ifeq_lb) {
                LOAD_OFFSET_AND_FETCH_LB(a1,a2,i1);
                goto compare_eq_and_branch_impl;
            }

            INSTR(ifne_lb) {
                LOAD_OFFSET_AND_FETCH_LB(a1,a2,i1);
                goto compare_ne_and_branch_impl;
            }

            INSTR(ifstricteq_lb) {
                LOAD_OFFSET_AND_FETCH_LB(a1,a2,i1);
                goto compare_stricteq_and_branch_impl;
            }

            INSTR(ifstrictne_lb) {
                LOAD_OFFSET_AND_FETCH_LB(a1,a2,i1);
                goto compare_strictne_and_branch_impl;
            }

            INSTR(swap_pop) {
                sp[-1] = sp[0];
                --sp;
                NEXT;
            }

#  endif // VMCFG_WORDCODE_PEEPHOLE

            INSTR(findpropglobal) {
                b1 = false;  // whether to throw or not
                goto findpropglobal_impl;
            }

            INSTR(findpropglobalstrict) {
                b1 = true;
            findpropglobal_impl:
                u1 = *pc++;      // multiname_index
                SAVE_EXPC;
                GET_MULTINAME_PTR(multiname, u1);
                AvmAssert(multiname->isBinding());
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=663989
                AvmAssert(!AvmCore::isNullOrUndefined(globalScopeAtom));
                a1 = env->findglobalproperty(globalScopeAtom, multiname);  // container
#else
                AvmAssert(globalScope != NULL);
                a1 = env->findglobalproperty(globalScope->atom(), multiname);  // container
#endif
                if (AvmCore::isNullOrUndefined(a1))
                {
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=663989
                    if (b1)
                        toplevel->throwReferenceError(kUndefinedVarError, multiname);
                    *(++sp) = globalScopeAtom;
#else
                    if (b1)
                        toplevel->throwReferenceError(kUndefinedVarError, multiname);
                    else
                        *(++sp) = globalScope->atom();
#endif
                }
                else
                {
                    *(++sp) = a1;
                }
                NEXT;
            }
                    
#ifdef VMCFG_FLOAT
            INSTR(float4) {
                // Does not pop its arguments but replaces the top argument by the result,
                // the stack is cleaned up by subsequent code.  This mess is brought upon
                // us by Verifier::emitCallpropertySlot(), primarily.
                float4_t result;
                result.w = AvmCore::singlePrecisionFloat(sp[0]);
                result.z = AvmCore::singlePrecisionFloat(sp[-1]);
                result.y = AvmCore::singlePrecisionFloat(sp[-2]);
                result.x = AvmCore::singlePrecisionFloat(sp[-3]);
                sp[0] = core->float4ToAtom(result);
                NEXT;
            }
#endif
#endif  // VMCFG_WORDCODE
#if defined SWITCH_DISPATCH
            } // switch
            // illegal instruction or accidental break
            goto L_illegal_op;
        } // for
#else
            goto L_illegal_op;
#endif

        }  // End TRY

        CATCH (Exception *exception)
        {
            // find handler; rethrow if no handler.
#if defined VMCFG_WORDCODE && !defined DEBUGGER
            ExceptionHandler *handler = core->findExceptionHandler(info, (uintptr_t*)expc-1-info->word_code_start(), exception);
#else
            ExceptionHandler *handler = core->findExceptionHandler(info, expc, exception);
#endif
            // handler found in current method
#ifdef DEBUGGER
            // This is a little hokey, see https://bugzilla.mozilla.org/show_bug.cgi?id=470954.
            //
            // The debugenter instruction sets up core->callStack, we do this lazily to save
            // time in builds where the debugger is enabled at compile time but not present
            // at run time.
            //
            // The problem is that CATCH restores core->callStack to its old value, saved by TRY.
            // So we force it to the new value here if there is a new value.  Then TRY will save the
            // value again (the new value this time) which we restore redundantly the next time
            // there is an exception, if any.  The debugexit instruction will take care of restoring
            // the actual old value.
            if (callStackNode != NULL)
                core->callStack = callStackNode;
#endif
#ifdef VMCFG_WORDCODE
            pc = info->word_code_start() + handler->target;
#else
            pc = codeStart + handler->target;
#endif
            scopeDepth = 0;
#ifdef DEBUGGER
            // in debugger builds, ensure that non-active scope entries are nulled out
            for (int i = 0, n = ms->max_scope(); i < n; ++i)
                scopeBase[i] = nullObjectAtom;
#endif
#ifndef MARK_SECURITY_CHANGE // https://bugzilla.mozilla.org/show_bug.cgi?id=663989
            globalScopeAtom = (scope->getSize() > 0) ? scope->getScope(0) : nullObjectAtom;
#else
            globalScope = (scope->getSize() > 0) ? AvmCore::atomToScriptObject(scope->getScope(0)) : NULL;
#endif
            sp = scopeBase + ms->max_scope() - 1;
            *(++sp) = exception->atom;
            goto MainLoop;
        }
        END_CATCH
        END_TRY

        // Target of various error cases

        L_illegal_op:
            AvmAssert(!"Illegal operation!");
            goto L_illegal_op;
    } // end interpBoxed()

    // Note, this function is not on the hot path normally, so optimizing it is not urgent.
    //
    // OPTIMIZEME - statically knowable if name isRtname or isRtns; exploit this somehow?
    // OPTIMIZEME - often knowable whether the TOS is an object or something simple; exploit this?

    Atom* initMultiname(MethodEnv* env, Multiname &name, Atom* sp)
    {
        if (name.isRtname())
        {
            Atom index = *(sp--);
            AvmCore* core = env->core();

            // is it a qname?
            if (AvmCore::isObject(index))
            {
                ScriptObject* i = AvmCore::atomToScriptObject(index);
                if (i->traits() == core->traits.qName_itraits)
                {
                    QNameObject* qname = (QNameObject*) i;
                    bool attr = name.isAttr();
                    qname->getMultiname(name);
                    if (attr)
                        name.setAttr(attr);

                    // Discard runtime namespace if present
                    if (name.isRtns())
                        sp--;

                    return sp;
                }
            }

            name.setName(core->intern(index));
        }

        if (name.isRtns())
            name.setNamespace(env->internRtns(*(sp--)));

        return sp;
    }

    Atom* initMultinameNoXMLList(MethodEnv* env, Multiname &name, Atom* sp)
    {
        if (name.isRtname() && AvmCore::isXMLList(sp[0])) {
            // Error according to E4X spec, section 11.3.1
            env->toplevel()->throwTypeError(kDeleteTypeError, env->core()->toErrorString(env->toplevel()->toTraits(sp[0])));
        }
        return initMultiname(env, name, sp);
    }

    Traits* getTraits(const Multiname* name, PoolObject* pool, Toplevel* toplevel, AvmCore* core)
    {
        // See Verifier::checkTypeName for the canonical code

        Traits* t = core->domainMgr()->findTraitsInPoolByMultiname(pool, *name);
        // Verifier::checkTypeName has ensured this is a valid, non-ambiguous binding
        AvmAssert(t != NULL && t != (Traits*)BIND_AMBIGUOUS);
        if (name->isParameterizedType())
        {
            const Multiname* param_name;
            GET_MULTINAME_PTR(param_name, name->getTypeParameter());
            core->stackCheck(toplevel);
            Traits* param_traits = name->getTypeParameter() ? getTraits(param_name, pool, toplevel, core) : NULL;
            t = pool->resolveParameterizedType(toplevel, t, param_traits);
        }
        return t;
    }

#ifdef AVMPLUS_VERBOSE
    /**
     * display contents of current stack frame only.
     */
    void showState(MethodInfo* info, const bytecode_t *code_start, const bytecode_t *pc, Atom* framep, Atom *spp, int scopeDepth, Atom *scopebasep, int max_scope)
    {
#ifdef VMCFG_WORDCODE
#  ifdef VMCFG_DIRECT_THREADED
        // 'opcode' is really a code pointer.
        void* address = (void*)*pc;
        WordOpcode opcode = (WordOpcode)0;
        void** jumptable = interpGetOpcodeLabels();
        for ( int i=0 ; i <= WOP_LAST ; i++ ) {
            if (jumptable[i] == address) {
                opcode = (WordOpcode)i;
                break;
            }
        }
        // this cannot happen due to the structure of the loop above
        // (and triggers a warning in some exceptionally-picky compilers)
        //if (opcode < 0 || opcode > WOP_LAST)
        //  opcode = (WordOpcode)0;
# else
        WordOpcode opcode = (WordOpcode) *pc;
#  endif
#else
        AbcOpcode opcode = (AbcOpcode) *pc;
#endif
        PoolObject* pool = info->pool();
        AvmCore* core = pool->core;
        ptrdiff_t off = pc - code_start;
        ptrdiff_t sp = spp - framep;
        ptrdiff_t scopep = scopebasep + scopeDepth - 1 - framep;
        ptrdiff_t scopeBase = scopebasep - framep;
        ptrdiff_t stackBase = scopebasep + max_scope - framep;

// this is handy for debugging 64-specific interpreter bugs.
//#ifdef AVMPLUS_64BIT
//      core->console << "                        stack-raw:";
//      for (ptrdiff_t i=stackBase; i <= sp; i++) {
//          core->console << " 0x" << MathUtils::convertDoubleToStringRadix(core, framep[i]>>32, 16) <<
//                                  MathUtils::convertDoubleToStringRadix(core, framep[i]&0xffffffff, 16);
//      }
//      core->console << '\n';
//#endif

        // locals
        core->console << "                          [";
        for (ptrdiff_t i=0, n=scopeBase; i<n; i++) {
            core->console << asAtom(framep[i]);
            if (i+1<n) core->console << ' ';
        }

        // scope chain
        core->console << "] {";
        for (ptrdiff_t i=scopeBase, n=scopep; i<=n; i++) {
            core->console << asAtom(framep[i]);
            if (i+1<=n) core->console << ' ';
        }

        // stack
        core->console << "} (";
        ptrdiff_t stackStart = stackBase;
        const size_t stackLimit = 20; // don't display more than this, to reduce verbosity
        size_t stackDepth = (stackBase <= sp) ? sp - stackBase : 0;
        if (stackDepth > stackLimit) {
            stackStart = sp - stackLimit;
            core->console << "..." << (stackStart-stackBase) << ": ";
        }
        for (ptrdiff_t i=stackStart, n=sp; i<=n; i++) {
            core->console << asAtom(framep[i]);
            if (i+1<=n) core->console << ' ';
        }

        // opcode
        core->console << ")\n  ";
#ifdef DEBUGGER
        if (core->debugger() && core->callStack && core->callStack->filename())
        {
            core->console << '[' << core->callStack->filename() << ':' << (uint32_t)core->callStack->linenum() << "] ";
        }
#endif
        core->console << (int)off << ':';
        // Using largest-possible-pointer here for code_end; this is obviously not correct,
        // but extracting the proper code_end requires diving into the method's ABC bytecode
        // (and may not be easily accessible at all for wordcode), and more importantly,
        // is only used for range-checking of fuzzed opcodes... all of which should have been caught
        // by Verifier, thus checking should be unnecessary here.
        const bytecode_t* const code_end = (const bytecode_t*)uintptr_t(-1);
#ifdef VMCFG_WORDCODE
        core->formatOpcode(core->console, pc, code_end, (WordOpcode)((int32_t)opcode&0xffff), off, pool);
#else
        core->formatOpcode(core->console, pc, code_end, opcode, off, pool);
#endif
        core->console << '\n';
    }
#endif // AVMPLUS_VERBOSE

#ifdef VMCFG_HALFMOON
    Atom interpBoxedAtLocation(MethodEnv* env, int abc_pc,
                               FramePopulator& populator) {
        AvmAssert(false && "not implemented yet");
        (void)env;
        (void)abc_pc;
        (void)populator;
        return unreachableAtom;
    }

    Atom interpBoxedAtLocation(int abc_pc, Atom* locals, Atom* operand_stack,
                               Atom* scope_stack, int sp, int scope_height,
                               MethodEnv* env) {
        AvmAssert(false && "not implemented yet");
        (void)abc_pc;
        (void)locals;
        (void)operand_stack;
        (void)scope_stack;
        (void)sp;
        (void)scope_height;
        (void)env;
        return unreachableAtom;
    }
#endif // VMCFG_HALFMOON

} // namespace avmplus
