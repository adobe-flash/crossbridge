/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCStack_inlines__
#define __GCStack_inlines__

namespace MMgc
{
    REALLY_INLINE uintptr_t GCMarkStack::Top()
    {
        GCAssert(!IsEmpty());
        return uintptr_t(m_top-1);
    }
    
    REALLY_INLINE bool GCMarkStack::Push_GCObject(const void* p)
    {
        uintptr_t* top = allocSpace(1);
        if (top == NULL)
            return false;
        top[0] = uintptr_t(p);
        return true;
    }
    
    REALLY_INLINE const void* GCMarkStack::Pop_GCObject()
    {
        GCAssert(!IsEmpty());
        uintptr_t w = m_top[-1];
        GCAssert(!(w & 1));
        if ((w & 3) == 0) {
            freeSpace(1);
            return (void*)w;
        }
        return NULL;
    }
    
    REALLY_INLINE GCMarkStack::TypeTag GCMarkStack::PeekTypetag()
    {
        GCAssert(!IsEmpty());
        uintptr_t w = m_top[-1];
        GCAssert(!(w & 1));
        if ((w & 3) == 0)
            return kGCObject;
        return GCMarkStack::TypeTag(w >> 2);
    }

    REALLY_INLINE bool GCMarkStack::IsEmpty()
    {
        // See Invariants(): m_top == m_base only when there is no older segment
        // and the current segment is empty.
        return m_top == m_base;
    }

    REALLY_INLINE uint32_t GCMarkStack::Count()
    {
        return uint32_t(m_top - m_base) + m_hiddenCount;
    }

    REALLY_INLINE uint32_t GCMarkStack::InactiveSegments()
    {
        return m_hiddenSegments;
    }

#ifdef MMGC_MARKSTACK_DEPTH
    REALLY_INLINE uint32_t GCMarkStack::MaxCount()
    {
        return m_maxDepth;
    }
#endif

    REALLY_INLINE uintptr_t* GCMarkStack::allocSpace(size_t nwords)
    {
        GCAssert(Invariants());
        if (m_top+nwords > m_limit)
            if (!PushSegment()) {
                GCAssert(Invariants());
                return NULL;
            }
        m_top += nwords;
#ifdef MMGC_MARKSTACK_DEPTH
        uint32_t depth = Count();
        if (depth > m_maxDepth)
            m_maxDepth = depth;
#endif
        GCAssert(Invariants());
        return m_top-1;
    }

    REALLY_INLINE void GCMarkStack::freeSpace(size_t nwords)
    {
        GCAssert(Invariants());
        GCAssert(m_top-nwords >= m_base);
        m_top -= nwords;
#ifdef _DEBUG
        VMPI_memset(m_top, 0, nwords*sizeof(uintptr_t));
#endif
        if (m_top == m_base) 
            PopSegment_UnlessLast();
        GCAssert(Invariants());
    }

}

#endif /* __GCStack_inlines__ */
