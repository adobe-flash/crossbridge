/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_QCache__
#define __avmplus_QCache__

#ifdef _DEBUG
    #define QCACHE_DEBUG
#endif

#include "MathUtils.h"

/*
    QCache is a simple framework for maintaining a cache of items, with a random
    eviction policy. The cache is limited by count of items, which can be set arbitrarily
    (or allowed to grow to unlimited size).

    The recommended use case is to split a a data structure split into a small, frequently-used
    chunk (owner), and a larger, less-frequently-used chunk (cacheditem) that can be reconstituted
    from compressed data; e.g., the slot-binding tables for Traits, which are kept in memory in
    a highly compressed form (the ABC bytecode), and expanded into convenient hashtable format
    as a cacheditem.

    Important note: For proper use, the only "strong" ref to the cacheditem should be via
    the QCache itself; the owner should keep a reference to the cacheditem via a GCWeakRef, which
    will allow the cacheditem to be collected when it is evicted from the cache. (Naturally, the owner
    must check the GCWeakRef for null before using it.)

    Cached items are stored in a singly-linked list; it's assumed that the cost
    of building a new item to add to the cache is likely to dwarf the cost of walking
    the list to evict an item (this is currently the case for all existing use cases).
*/

namespace avmplus
{
    // QCachedItem is not meant to be instantiated as-is, but to be subclassed.

    class GC_CPP_EXACT(QCachedItem, MMgc::GCTraceableObject)
    {
        friend class QCache;
    protected:
        inline QCachedItem() : next(NULL) { }
        
    private:
        GC_DATA_BEGIN(QCachedItem)
        
        QCachedItem*    GC_POINTER(next);   // written with explicit WB

        GC_DATA_END(QCachedItem)
    };

    class GC_CPP_EXACT(QCache, MMgc::GCFinalizedObject)
    {
    private:
        QCache(uint32_t _max, MMgc::GC* _gc);
    public:
        REALLY_INLINE static QCache* create(uint32_t max, MMgc::GC* gc)
        {
            return new (gc, MMgc::kExact) QCache(max, gc);
        }

        ~QCache();

        inline uint32_t count() const { return m_count; }
        inline uint32_t maxcount() const { return m_max; }  // sadly, "max" is a macro in some environments
        inline QCachedItem* first() const { return m_head; }
        inline QCachedItem* next(QCachedItem* gen) const { return gen->next; }

        void flush();
        void resize(uint32_t _max);

        QCachedItem* add(QCachedItem* gen);

    private:
        #ifdef QCACHE_DEBUG
        void validate() const;
        #endif

        GC_DATA_BEGIN(QCache)

    private:
        MMgc::GC*               m_gc;
        QCachedItem*            GC_POINTER(m_head);
        uint32_t                m_count;
        uint32_t                m_max;
        TRandomFast             m_rand;
        
        GC_DATA_END(QCache)
    };
}

#endif /* __avmplus_QCache__ */
