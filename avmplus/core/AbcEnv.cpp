/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{
    AbcEnv::AbcEnv(PoolObject* _pool,
           CodeContext * _codeContext)
        : m_pool(_pool)
        , m_domainEnv(_codeContext->domainEnv())
        , m_codeContext(_codeContext)
#if defined(VMCFG_AOT) && defined(VMCFG_BUFFER_GUARD)
        , m_lazyEvalGuard(this)
#endif
        , m_core(_pool->core)
    {
        MMgc::GC* gc = _pool->core->GetGC();
        m_finddef_table = FinddefTable::create(gc, NULL /* no finalizer */,
                                               _pool->cpool_mn_offsets.length());
#ifdef DEBUGGER
        if (_pool->core->debugger())
        {
            m_invocationCounts = U64Array::New(gc, _pool->methodCount());
        }
#endif

#ifdef VMCFG_AOT
        AvmAssert(_pool->aotInfo != NULL);
        AvmAssert(_pool->aotInfo->abcEnv != NULL);
        if(*(_pool->aotInfo->abcEnv) == NULL)
        {
            *(_pool->aotInfo->abcEnv) = this;
            Multiname **multiname = _pool->aotInfo->multinames;
            const int32_t *multinameIndex = _pool->aotInfo->multinameIndices;
            while(*multiname)
            {
                _pool->parseMultiname(**multiname, (int) *multinameIndex);
                multiname++;
                multinameIndex++;
            }
        }
#endif
    }

    AbcEnv::~AbcEnv()
    {
        #ifdef VMCFG_NANOJIT
        // if any AbcEnv goes away, we have to flush the BindingCache entries for all
        // extant pools. Since AvmCore keeps a list of all live pools, we just set a flag
        // in AvmCore that triggers the flush in postsweep(). (Note that we can't rely on our pool
        // being valid here; it might have already been collected!)
        // the avmcore's lifetime is owned by the host and could be deleted so check for that first
        AvmCore *current = MMgc::GC::GetGC(this)->core();
        if(current == m_core)
            m_core->flushBindingCachesNextSweep();
        #endif
    }

}
