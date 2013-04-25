/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "avmplus.h"

namespace avmplus
{
    using namespace MMgc;

    QCache::QCache(uint32_t _max, MMgc::GC* _gc) :
        m_gc(_gc),
        m_head(NULL),
        m_count(0),
        m_max(0)
    {
        MathUtils::RandomFastInit(&m_rand);
        resize(_max);
    }

    QCache::~QCache()
    {
        flush();
    }

    void QCache::flush()
    {
        QCachedItem* gen = first();
        while (gen)
        {
            QCachedItem* n = next(gen);
            gen->next = NULL;
            gen = n;
        }
        m_head = NULL;
        m_count = 0;
    }

    void QCache::resize(uint32_t _max)
    {
        flush();
        // 0 -> unlimited
        m_max = !_max ? 0xffffffff : _max;
    }

    QCachedItem* QCache::add(QCachedItem* gen)
    {
        AvmAssert(gen && !next(gen));

        QCachedItem* evicted = NULL;

        // if no space, evict oldest
        if (m_count >= m_max)
        {
            AvmAssert(m_count == m_max);
            uint32_t which = MathUtils::Random(count(), &m_rand);
            QCachedItem* evicted_prev = NULL;
            for (QCachedItem* td = first(); td; td = next(td))
            {
                if (!which--)
                    break;
                evicted_prev = td;
            }
            if (evicted_prev)
            {
                evicted = evicted_prev->next;
                AvmAssert(evicted != NULL && evicted != gen);
                WB(m_gc, evicted_prev, &evicted_prev->next, evicted->next);
            }
            else
            {
                evicted = m_head;
                AvmAssert(evicted != NULL && evicted != gen);
                WB(m_gc, this, &m_head, evicted->next);
            }
            evicted->next = NULL;   // never need a WB to write NULL
            --m_count;
            #ifdef QCACHE_DEBUG
            validate();
            #endif
        }

        // add at head
        WB(m_gc, gen, &gen->next, m_head);
        WB(m_gc, this, &m_head, gen);
        ++m_count;

        #ifdef QCACHE_DEBUG
        validate();
        #endif

        AvmAssert(evicted ? evicted->next == NULL : true);
        return evicted;
    }

    #ifdef QCACHE_DEBUG
    void QCache::validate() const
    {
        if (!m_head)
        {
            AvmAssert(m_count == 0);
            return;
        }
        uint32_t c = 0;
        QCachedItem* prev = NULL;
        for (QCachedItem* td = first(); td; td = next(td))
        {
            AvmAssert(prev ? prev->next == td : true);
            ++c;
            prev = td;
            if (c > m_max)
            {
                // oops, probably a cycle, busted code
                AvmAssert(0);
            }
        }
        AvmAssert(c == m_count);
    }
    #endif
}

