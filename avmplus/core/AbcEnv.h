/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_AbcEnv__
#define __avmplus_AbcEnv__

#ifdef VMCFG_AOT
#include "AOTCompiler.h"
#endif

namespace avmplus
{
    // runtime info associated with a pool
    class GC_CPP_EXACT(AbcEnv, MMgc::GCFinalizedObject)
    {
        friend class DomainMgr;

        #ifdef VMCFG_NANOJIT
        friend class CodegenLIR;
        friend class MopsRangeCheckFilter;
        #endif

        AbcEnv(PoolObject* _pool, CodeContext * _codeContext);
        
    public:
        struct FinddefEntry {
            ScriptObject* object; // written only by explicit WB in finddef_miss().
            REALLY_INLINE void gcTrace(MMgc::GC* gc) { gc->TraceLocation(&object); }
        };
        typedef ExactStructContainer<FinddefEntry> FinddefTable;

        static AbcEnv* create(MMgc::GC* gc, PoolObject* builtinPool, CodeContext* builtinCodeContext);
        
        ~AbcEnv();

        AvmCore* core() const;
        PoolObject* pool() const;
        DomainEnv* domainEnv() const;
        CodeContext* codeContext() const;
        FinddefTable* finddefTable() const;

        MethodEnv* getMethod(uint32_t i) const;
        void setMethod(uint32_t i, MethodEnv* env);

#ifdef DEBUGGER
        uint64_t& invocationCount(uint32_t i);
#endif

        static size_t calcExtra(PoolObject* pool);

    // ------------------------ DATA SECTION BEGIN
    GC_DATA_BEGIN(AbcEnv)

    private:
        PoolObject* const           GC_POINTER( m_pool );
        DomainEnv* const            GC_POINTER( m_domainEnv );           // Same as m_codeContext->domainEnv(); replicated here solely for efficiency in jitted code
        CodeContext* const          GC_POINTER( m_codeContext );
#ifdef DEBUGGER
        GCMember<U64Array>          m_invocationCounts;    // Actual size will hold pool->methodCount methods, only allocated if debugger exists.
#endif
        AvmCore* const              m_core;
    public:
        GCMember<FinddefTable>      GC_POINTER(m_finddef_table);
    private:
#if defined(VMCFG_AOT) && defined(VMCFG_BUFFER_GUARD)
        AOTLazyEvalGuard            m_lazyEvalGuard;
#endif
        MethodEnv*                  GC_POINTERS(m_methods[1], "m_pool ? m_pool->methodCount() : 0");  // actual size will hold pool->methodCount methods.
                                                                        // Observe m_pool can be null during construction if allocation triggers GC.

    GC_DATA_END(AbcEnv)
    // ------------------------ DATA SECTION END
    };

    typedef AbcEnv::FinddefTable FinddefTable;
}

#endif // __avmplus_AbcEnv__
