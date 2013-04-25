/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus {

using namespace nanojit;

REALLY_INLINE LIns* CodegenLIR::loadIns(LOpcode op, int32_t disp, LIns *base, AccSet accSet, LoadQual loadQual)
{
    return lirout->insLoad(op, base, disp, accSet, loadQual);
}

REALLY_INLINE LIns* CodegenLIR::storeIns(LOpcode op, LIns* val, int32_t disp, LIns *base, AccSet accSet)
{
    return lirout->insStore(op, val, base, disp, accSet);
}

REALLY_INLINE LIns* CodegenLIR::Ins(LOpcode op)
{
    return lirout->ins0(op);
}

REALLY_INLINE LIns* CodegenLIR::Ins(LOpcode op, LIns *a)
{
    return lirout->ins1(op, a);
}

REALLY_INLINE LIns* CodegenLIR::i2dIns(LIns* v)
{
    return lirout->ins1(LIR_i2d, v);
}

REALLY_INLINE LIns* CodegenLIR::ui2dIns(LIns* v)
{
    return lirout->ins1(LIR_ui2d, v);
}

REALLY_INLINE bool InvokerCompiler::copyArgs()
{
    return args_out->isop(LIR_allocp);
}

// Return true if JIT failure can be predicted from method signature prior to JIT invocation.
// This supports a fast-fail mechanism to avoid blowup of non-linear JIT algorithms on atypical
// methods, e.g., very long or with many variables.

REALLY_INLINE bool CodegenLIR::jitWillFail(const MethodSignaturep ms)
{
    // Assembly cannot succeed if frame is too large for NanoJIT.
    // Large frame sizes can lead to pathological VarTracker behavior -- see bug 601794.
    return ms->frame_size() * 2 > NJ_MAX_STACK_ENTRY;
}

template <class C>
REALLY_INLINE C* CacheBuilder<C>::findCacheSlot(const Multiname* name)
{
    for (Seq<C*> *p = caches.get(); p != NULL; p = p->tail)
        if (p->head->name == name)
            return p->head;
    return NULL;
}

// The cache structure is expected to be small in the normal case, so use a
// linear list.  For some programs, notably classical JS programs, it may however
// be larger, and we may need a more sophisticated structure.
template <class C>
REALLY_INLINE C* CacheBuilder<C>::allocateCacheSlot(const Multiname* name)
{
    C* c = findCacheSlot(name);
    if (!c) {
        c = new (codeMgr.allocator) C(name, codeMgr.bindingCaches);
        codeMgr.bindingCaches = c;
        caches.add(c);
    }
    return c;
}

REALLY_INLINE bool CodegenLIR::haveSSE2() const
{
#if defined AVMPLUS_IA32
#  ifdef VMCFG_SSE2
    return core->config.njconfig.i386_sse2; // sse2 based on cpuid & config
#  else // !VMCFG_SSE2
    return true; // x86-32 with SSE2 guaranteed
#endif
#elif defined AVMPLUS_AMD64
    return true;  // x86-64 has SSE2
#else
    return false; // non-x86 family
#endif
}

} // namespace
