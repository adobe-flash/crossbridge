/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

#ifdef VMCFG_NANOJIT

#include "LirHelper.h"

#ifdef VMCFG_SHARK
#include <dlfcn.h> // for dlopen and dlsym
#endif

namespace avmplus
{

bool neverReturns(const CallInfo* call)
{
    return call == FUNCTIONID(throwAtom) ||
        call == FUNCTIONID(npe) ||
        call == FUNCTIONID(upe) ||
        call == FUNCTIONID(mop_rangeCheckFailed);
}

CodeMgr* initCodeMgr(PoolObject *pool)
{
    if (!pool->codeMgr) {
        CodeMgr *mgr = mmfx_new( CodeMgr(&(pool->core->config.njconfig)) );
        pool->codeMgr = mgr;
#ifdef NJ_VERBOSE
        mgr->log.core = pool->core;
        mgr->log.lcbits = pool->verbose_vb;
#endif
    }
    return pool->codeMgr;
}

LirHelper::LirHelper(PoolObject* pool) :
    pool(pool),
    core(pool->core),
    alloc1(mmfx_new(Allocator())),
    lir_alloc(mmfx_new(Allocator())),
    jit_observer(((BaseExecMgr*)core->exec)->jit_observer),
#ifdef NANOJIT_IA32
    use_cmov(pool->core->config.njconfig.i386_use_cmov)
#else
    use_cmov(true)
#endif
{ }

LirHelper::~LirHelper()
{
    cleanup();
}

void LirHelper::cleanup()
{
    mmfx_delete( alloc1 );
    alloc1 = NULL;
    mmfx_delete( lir_alloc );
    lir_alloc = NULL;
}

// call
LIns* LirHelper::callIns(const CallInfo *ci, uint32_t argc, ...)
{
    va_list ap;
    va_start(ap, argc);
    LIns* ins = vcallIns(ci, argc, ap);
    va_end(ap);
    return ins;
}

LIns* LirHelper::vcallIns(const CallInfo *ci, uint32_t argc, va_list ap)
{
    AvmAssert(argc <= MAXARGS);
    AvmAssert(argc == ci->count_args());
    LIns* argIns[MAXARGS];
    for (uint32_t i=0; i < argc; i++)
        argIns[argc-i-1] = va_arg(ap, LIns*);
    LIns* ins = lirout->insCall(ci, argIns);

    // for non-returning functions ensure that we signify this fact
    // by terminating control-flow with a ret.
    if (neverReturns(ci))
        lirout->ins1(LIR_retp, InsConstPtr(0));
    NanoAssert(!ins->isInReg());
    return ins;
}

LIns* LirHelper::nativeToAtom(LIns* native, Traits* t)
{
    switch (bt(t)) {
#ifdef VMCFG_FLOAT
    case BUILTIN_float: 
        // always allocFloat; we don't currently have the option to represent a float
        // as part of the atom, like doubles/numbers can 
            return callIns(FUNCTIONID(floatToAtom), 2, coreAddr, native);

    case BUILTIN_float4: 
        {
            LIns* local = lirout->insAlloc(sizeof(float4_t));
            stf4(native, local, 0, ACCSET_OTHER);
            return callIns(FUNCTIONID(float4ToAtom), 2, coreAddr, lea(0, local));
        }
#endif

    case BUILTIN_number:
        SSE2_ONLY(if(core->config.njconfig.i386_sse2) {
            return callIns(FUNCTIONID(doubleToAtom_sse2), 2, coreAddr, native);
        })

        return callIns(FUNCTIONID(doubleToAtom), 2, coreAddr, native);

    case BUILTIN_any:
    case BUILTIN_object:
    case BUILTIN_void:
        return native;  // value already represented as Atom

    case BUILTIN_int:
        if (native->isImmI()) {
            int32_t val = native->immI();
            if (atomIsValidIntptrValue(val))
                return InsConstAtom(atomFromIntptrValue(val));
        }
        return callIns(FUNCTIONID(intToAtom), 2, coreAddr, native);

    case BUILTIN_uint:
        if (native->isImmI()) {
            uint32_t val = native->immI();
            if (atomIsValidIntptrValue_u(val))
                return InsConstAtom(atomFromIntptrValue_u(val));
        }
        return callIns(FUNCTIONID(uintToAtom), 2, coreAddr, native);

    case BUILTIN_boolean:
        return ui2p(addi(lshi(native, 3), kBooleanType));

    case BUILTIN_string:
        return addp(native, kStringType);

    case BUILTIN_namespace:
        return addp(native, kNamespaceType);

    default:
        return addp(native, kObjectType);
    }
}

LIns* LirHelper::atomToNative(BuiltinType bt, LIns* atom)
{
    switch (bt)
    {
    case BUILTIN_any:
    case BUILTIN_object:
    case BUILTIN_void:
        return atom;

#ifdef VMCFG_FLOAT
    case BUILTIN_float4:
        if (atom->isImmP()) {
            float4_t f4val;
            AvmCore::float4(&f4val, (Atom)atom->immP());
            return lirout->insImmF4(f4val);
        } else {
            return ldf4(atom, -AtomConstants::kSpecialBibopType, ACCSET_OTHER);
        }

    case BUILTIN_float:
        if (atom->isImmP())
            return lirout->insImmF(AvmCore::atomToFloat((Atom)atom->immP()));
        else
            return ldf(atom, -AtomConstants::kSpecialBibopType, ACCSET_OTHER);
#endif
    case BUILTIN_number:
        if (atom->isImmP())
            return lirout->insImmD(AvmCore::number_d((Atom)atom->immP()));
        else
            return callIns(FUNCTIONID(number_d), 1, atom);

    case BUILTIN_int:
        if (atom->isImmP())
            return InsConst(AvmCore::integer_i((Atom)atom->immP()));
        else
            return callIns(FUNCTIONID(integer_i), 1, atom);

    case BUILTIN_uint:
        if (atom->isImmP())
            return InsConst(AvmCore::integer_u((Atom)atom->immP()));
        else
            return callIns(FUNCTIONID(integer_u), 1, atom);

    case BUILTIN_boolean:
        if (atom->isImmI())
            return InsConst((int32_t)atomGetBoolean((Atom)atom->immP()));
        else
            return p2i(rshup(atom, 3));

    default:
        // pointer type
        if (atom->isImmP())
            return InsConstPtr(atomPtr((Atom)atom->immP()));
        else
            return andp(atom, ~7);
    }

#ifdef __GNUC__
    return 0;// satisfy GCC, although we should never get here
#endif
}

void LirHelper::emitStart(Allocator& alloc, LirBuffer *lirbuf, LirWriter* &lirout) {
    (void)alloc; (void)lirbuf;
    debug_only(
        // catch problems before they hit the writer pipeline
        lirout = validate1 = new (alloc) ValidateWriter(lirout, lirbuf->printer, "emitStart");
    )
    lirout->ins0(LIR_start);

    // create params for saved regs -- processor specific
    for (int i=0; i < NumSavedRegs; i++) {
        LIns *p = lirout->insParam(i, 1); (void) p;
        verbose_only(if (lirbuf->printer)
            lirbuf->printer->lirNameMap->addName(p,
                regNames[REGNUM(RegAlloc::savedRegs[i])]);)
    }
}

void LirHelper::liveAlloc(LIns* alloc)
{
    if (alloc->isop(LIR_allocp))
        livep(alloc);
}

LIns* LirHelper::stForTraits(Traits *t, LIns* val, LIns* p, int32_t d, AccSet accSet)
{
    switch (bt(t)) {
        case BUILTIN_number:
            return std(val, p, d, accSet);
#ifdef VMCFG_FLOAT
        case BUILTIN_float: 
            return stf(val, p, d, accSet);
        case BUILTIN_float4: 
            return stf4(val, p, d, accSet);
#endif
        case BUILTIN_int:
        case BUILTIN_uint:
        case BUILTIN_boolean:
            return sti(val, p, d, accSet);
        default:
            return stp(val, p, d, accSet);
    }
}

LIns* LirHelper::ldForTraits(Traits *t, LIns* p, int32_t d, AccSet accSet)
{
    switch (bt(t)) {
    case BUILTIN_number:
        return ldd(p, d, accSet);
#ifdef VMCFG_FLOAT
    case BUILTIN_float: 
        return ldf(p, d, accSet);
    case BUILTIN_float4: 
        return ldf4(p, d, accSet);
#endif
    case BUILTIN_int:
    case BUILTIN_uint:
    case BUILTIN_boolean:
        return ldi(p, d, accSet);
    default:
        return ldp(p, d, accSet);
    }
}

// check valid pointer and unbox it (returns ScriptObject*)
LIns* LirHelper::downcast_obj(LIns* atom, LIns* env, Traits* t)
{
    callIns(FUNCTIONID(coerceobj_atom), 3, env, atom, InsConstPtr(t));
    return andp(atom, ~7);
}

int32_t LirHelper::argSize(MethodSignaturep ms, int i)
{
    return avmplus::argSize(ms->paramTraits(i));
}

#ifdef NJ_VERBOSE
void AvmLogControl::printf( const char* format, ... )
{
    AvmAssert(core!=NULL);

    va_list vargs;
    va_start(vargs, format);

    char str[1024];
    VMPI_vsnprintf(str, sizeof(str), format, vargs);
    va_end(vargs);

    core->console << str;
}
#endif

#ifdef VMCFG_FLOAT
LIns* LirHelper::BasicLIREmitter::operator()(enum BuiltinType bt, LIns* oper1,LIns* oper2) const
{
    switch(bt){
    case BUILTIN_number: return _lh->binaryIns(_dblOp, oper1, oper2); 
    case BUILTIN_any:    if(_call) return _lh->callIns(_call, 3, _lh->coreAddr, oper1, oper2); // else, fall through - a call is unexpected
    default: return TODO("Unexpected builtin type InstructionEmiter, binary op");
    }
}

LIns* LirHelper::BasicLIREmitter::operator()(enum BuiltinType bt,LIns* oper) const 
{ 
    switch(bt){
    case BUILTIN_number: return _lh->lirout->ins1(_dblOp, oper); 
    case BUILTIN_any:    if(_call) return _lh->callIns(_call, 2, _lh->coreAddr, oper); // else, fall through - a call is unexpected
    default: return TODO("Unexpected builtin type InstructionEmiter, unary op");
    }
} 

LIns* LirHelper::ModuloLIREmitter::operator()(enum BuiltinType bt, LIns* op1,LIns* op2) const
{
    switch(bt){
    case BUILTIN_number: return _lh->callIns(FUNCTIONID(mod), 2, op1, op2 );
    case BUILTIN_any:    return _lh->callIns(FUNCTIONID(op_modulo), 3, _lh->coreAddr, op1, op2);
    default: return TODO("Unexpected builtin type in ModuloLIREmitter"); 
    }
}

LIns* LirHelper::IncrementLIREmitter::operator()(enum BuiltinType bt,LIns* oper) const 
{ 
    switch(bt){
    case BUILTIN_number: return _lh->binaryIns(LIR_addd, oper, _lh->lirout->insImmD(_shouldDecrement?-1:1));
    case BUILTIN_any:  
        {
            /* Note: we add with 1.0f/-1.0f, to make sure that  the results stays float if operand is float */
            const CallInfo* addfunc = _lh->pool->hasFloatSupport() ? FUNCTIONID(op_add) : FUNCTIONID(op_add_nofloat);
            return _lh->callIns( addfunc, 3, _lh->coreAddr, oper, _lh->InsConstAtom(_shouldDecrement? _lh->core->kFltMinusOne:_lh->core->kFltOne ));
        }
    default: return TODO("Unexpected builtin type in IncrementLIREmitter");
    }
}

#endif

} // end namespace avmplus

//
// The following methods implement Service Provider API's defined in
// nanojit, which must be implemented by the nanojit embedder.
//
namespace nanojit
{
    int StackFilter::getTop(LIns* /*br*/) {
        AvmAssert(false);
        return 0;
    }

    #ifdef NJ_VERBOSE
    void LInsPrinter::formatGuard(InsBuf*, LIns*) {
        AvmAssert(false);
    }
    void LInsPrinter::formatGuardXov(InsBuf*, LIns*) {
        AvmAssert(false);
    }

    const char* LInsPrinter::accNames[] = {
        "v",    // (1 << 0) == ACCSET_VARS
        "t",    // (1 << 1) == ACCSET_TAGS
        "o",    // (1 << 2) == ACCSET_OTHER
                  "?", "?", "?", "?", "?", "?", "?", "?",   //  3..10 (unused)
        "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",   // 11..20 (unused)
        "?", "?", "?", "?", "?", "?", "?", "?", "?", "?",   // 21..30 (unused)
        "?"                                                 //     31 (unused)
    };
    #endif

    void* Allocator::allocChunk(size_t size, bool /* fallible */) {
        return mmfx_alloc(size);
    }

    void Allocator::freeChunk(void* p) {
        mmfx_free(p);
    }

    void Allocator::postReset() {
    }

#ifdef VMCFG_SHARK

#if defined VMCFG_64BIT
    static const char* JIT_SO_PATH = "/tmp/jit64.so";
#elif defined VMCFG_32BIT
    static const char* JIT_SO_PATH = "/tmp/jit32.so";
#else
#   error "unsupported system"
#endif

    // TODO: lock access to these statics.
    static void *sHandle = NULL;
    static char *sStart = NULL;
    static char *sEnd = NULL;
    static char *sCur = NULL;
    size_t overage = 0;

    void* CodeAlloc::allocCodeChunk(size_t nbytes)
    {
        if (!sHandle) {
            sHandle = dlopen(JIT_SO_PATH, RTLD_NOW);
            if (sHandle) {
                // Get the bounds of the code area by looking for known symbols,
                // then make that area writable.
                sStart = (char*) dlsym(sHandle, "_jitStart");
                sEnd = (char*) dlsym(sHandle, "_jitEnd");
                if (!mprotect(sStart, sEnd - sStart,
                              PROT_READ | PROT_WRITE | PROT_EXEC))
                    sCur = sStart;
            } else {
                const char *err = dlerror();
                fprintf(stderr, "dlopen error: %s\n", err);
                // will fall through to AVMPI_allocateCodeMemory below.
            }
        }

        if (sCur) {
            char *next = sCur + nbytes;
            if (next < sEnd) {
                char *alloc = sCur;
                sCur = next;
                return alloc;
            }
        }

        // Out of space in jit.so, fall back to AVMPI to get code mem.
        overage += nbytes;
        fprintf(stderr, "out of jit.so memory, allocated %luKB from system\n",
               overage / 1024);
        char* mem = (char*) AVMPI_allocateCodeMemory(nbytes);
        mprotect((maddr_ptr) mem, (unsigned int) nbytes,
                 PROT_EXEC | PROT_READ | PROT_WRITE);

        return mem;
    }

    void CodeAlloc::freeCodeChunk(void* addr, size_t nbytes)
    {
        if (addr < sStart || addr >= sEnd) {
            // Did not come from DL region.
            AVMPI_freeCodeMemory(addr, nbytes);
        }
    }

    void CodeAlloc::markCodeChunkExec(void* /*addr*/, size_t /*nbytes*/) {
    }

    void CodeAlloc::markCodeChunkWrite(void* /*addr*/, size_t /*nbytes*/) {
    }

#else // !VMCFG_SHARK

    void* CodeAlloc::allocCodeChunk(size_t nbytes) {
        return AVMPI_allocateCodeMemory(nbytes);
    }

    void CodeAlloc::freeCodeChunk(void* addr, size_t nbytes) {
        AVMPI_freeCodeMemory(addr, nbytes);
    }

    void CodeAlloc::markCodeChunkExec(void* addr, size_t nbytes) {
        //printf("protect   %d %p\n", (int)nbytes, addr);
        AVMPI_makeCodeMemoryExecutable(addr, nbytes, true); // RX
    }

    void CodeAlloc::markCodeChunkWrite(void* addr, size_t nbytes) {
        //printf("unprotect %d %p\n", (int)nbytes, addr);
        AVMPI_makeCodeMemoryExecutable(addr, nbytes, false); // RW
    }
#endif

#ifdef DEBUG
    // Note this method should only be called during debug builds.
    bool CodeAlloc::checkChunkMark(void* addr, size_t nbytes, bool isExec) {
        bool b = true;

        // iterate over each page checking permission bits
        size_t psize = VMPI_getVMPageSize();
        uintptr_t last = alignTo((uintptr_t)addr + nbytes-1, psize);
        uintptr_t start = (uintptr_t)addr;
        for( uintptr_t n=start; b && n<=last; n += psize) {
#ifdef AVMPLUS_WIN32
            /* windows */
            MEMORY_BASIC_INFORMATION buf;
            VMPI_memset(&buf, 0, sizeof(MEMORY_BASIC_INFORMATION));
            SIZE_T sz = VirtualQuery((LPCVOID)n, &buf, sizeof(buf));
            NanoAssert(sz > 0);
            b = isExec ? buf.Protect == PAGE_EXECUTE_READ
                       : buf.Protect == PAGE_READWRITE;
            NanoAssert(b);
#elif defined(__MACH30__) && !defined(VMCFG_SHARK)
            /* mach / osx */
            vm_address_t vmaddr = (vm_address_t)n;
            vm_size_t vmsize = psize;
            vm_region_basic_info_data_64_t inf;
            mach_msg_type_number_t infoCnt = sizeof(vm_region_basic_info_data_64_t);
            mach_port_t port;
            VMPI_memset(&inf, 0, infoCnt);
            kern_return_t err = vm_region_64(mach_task_self(), &vmaddr, &vmsize, VM_REGION_BASIC_INFO_64, (vm_region_info_t)&inf, &infoCnt, &port);
            NanoAssert(err == KERN_SUCCESS);
            b = isExec ? inf.protection == (VM_PROT_READ | VM_PROT_EXECUTE)
                       : inf.protection == (VM_PROT_READ | VM_PROT_WRITE);
            NanoAssert(b);
#else
            (void)psize;
            (void)last;
            (void)start;
            (void)n;
            (void)isExec;
#endif
        }
        return b;
    }

    void ValidateWriter::checkAccSet(LOpcode op, LIns* base, int32_t disp, AccSet accSet)
    {
        (void)disp;
        LIns* vars = checkAccSetExtras ? (LIns*)checkAccSetExtras[0] : 0;
        LIns* tags = checkAccSetExtras ? (LIns*)checkAccSetExtras[1] : 0;

        // not enough to check base == xxx , since cse or lirbuffer may split (ld/st,b,d) into (ld/st,(addp,b,d),0)
        bool isTags = (base == tags) ||
                      ( (base->opcode() == LIR_addp) && (base->oprnd1() == tags) );
        bool isVars = (base == vars) ||
                      ( (base->opcode() == LIR_addp) && (base->oprnd1() == vars) );
        bool isUnknown = !isTags && !isVars;

        bool ok;

        NanoAssert(accSet != ACCSET_NONE);
        switch (accSet) {
        case avmplus::ACCSET_VARS:   ok = isVars;        break;
        case avmplus::ACCSET_TAGS:   ok = isTags;        break;
        case avmplus::ACCSET_OTHER:  ok = isUnknown;     break;
        default:
            // This assertion will fail if any single-region AccSets aren't covered
            // by the switch -- only multi-region AccSets should be handled here.
            AvmAssert(compressAccSet(accSet).val == MINI_ACCSET_MULTIPLE.val);
            ok = true;
            break;
        }

        if (!ok) {
            InsBuf b1, b2;
            printer->formatIns(&b1, base);
            VMPI_snprintf(b2.buf, b2.len, "but the base pointer (%s) doesn't match", b1.buf);
            errorAccSet(lirNames[op], accSet, b2.buf);
         }
    }
#endif

}

#endif // VMCFG_NANOJIT
