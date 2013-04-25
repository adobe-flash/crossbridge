/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

namespace avmplus
{

REALLY_INLINE AbcEnv* AbcEnv::create(MMgc::GC* gc, PoolObject* builtinPool, CodeContext* builtinCodeContext)
{
    return new (gc, MMgc::kExact, AbcEnv::calcExtra(builtinPool)) AbcEnv(builtinPool, builtinCodeContext);
}
    
REALLY_INLINE AvmCore* AbcEnv::core() const
{
    return m_core;
}

REALLY_INLINE PoolObject* AbcEnv::pool() const
{
    return m_pool;
}

REALLY_INLINE DomainEnv* AbcEnv::domainEnv() const
{
    return m_domainEnv;
}

REALLY_INLINE CodeContext* AbcEnv::codeContext() const
{
    return m_codeContext;
}

REALLY_INLINE FinddefTable* AbcEnv::finddefTable() const
{
    return m_finddef_table;
}

REALLY_INLINE MethodEnv* AbcEnv::getMethod(uint32_t i) const
{
    return m_methods[i];
}

REALLY_INLINE void AbcEnv::setMethod(uint32_t i, MethodEnv* env)
{
    WB(m_pool->core->GetGC(), this, &m_methods[i], env);
}

#ifdef DEBUGGER
REALLY_INLINE uint64_t& AbcEnv::invocationCount(uint32_t i)
{
    AvmAssert(m_invocationCounts != NULL);
    AvmAssert(i < m_pool->methodCount());
    return m_invocationCounts->AsArray()[i];
}
#endif

REALLY_INLINE size_t AbcEnv::calcExtra(PoolObject* pool)
{
    const uint32_t c = pool->methodCount();
    return (c <= 1) ? 0 : (sizeof(MethodEnv*)*(c-1));
}

} // namespace avmplus
