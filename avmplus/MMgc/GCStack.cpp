/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MMgc.h"

namespace MMgc
{
#ifdef MMGC_MARKSTACK_ALLOWANCE
    GCMarkStack::GCMarkStack(int32_t allowance)
#else
    GCMarkStack::GCMarkStack()
#endif
        : m_base(NULL)
        , m_top(NULL)
        , m_limit(NULL)
        , m_topSegment(NULL)
        , m_hiddenCount(0)
        , m_hiddenSegments(0)
        , m_extraSegment(NULL)
        , m_deadItem(0)
#ifdef MMGC_MARKSTACK_ALLOWANCE
        , m_allowance(allowance > 0 ? allowance : 2147483647)
#endif
#ifdef MMGC_MARKSTACK_DEPTH
        , m_maxDepth(0)
#endif
    {
        PushSegment(true);
        GCAssert(Invariants());
    }

    GCMarkStack::~GCMarkStack()
    {
        while (m_topSegment != NULL)
            PopSegment();
        if (m_extraSegment)
            FreeStackSegment(m_extraSegment);
    }

    void GCMarkStack::SetDeadItem(void* item)
    {
        GCAssert(item != NULL);
        m_deadItem = uintptr_t(item);
    }

    bool GCMarkStack::Push_LargeExactObjectTail(const void* p, size_t cursor)
    {
        uintptr_t* top = allocSpace(3);
        if (top == NULL)
            return false;
        top[0] = (kLargeExactObjectTail << 2) | kFirstWord;
        top[-1] = uintptr_t(p) | kMiddleWord;
        top[-2] = (uintptr_t(cursor) << 2) | kLastWord;
        return true;
    }

    void GCMarkStack::Pop_LargeExactObjectTail(const void* &p, size_t &cursor)
    {
        GCAssert(PeekTypetag() == kLargeExactObjectTail);

        uintptr_t* top = m_top-1;
        p = (void*)(top[-1] & ~3);
        cursor = size_t(top[-2] & ~3) >> 2;
        freeSpace(3);
    }

    bool GCMarkStack::Push_StackMemory(const void* p, uint32_t size, const void* baseptr)
    {
        uintptr_t* top = allocSpace(4);
        if (top == NULL)
            return false;
        top[0] = (kStackMemory << 2) | kFirstWord;
        top[-1] = uintptr_t(p) | kMiddleWord;
        top[-2] = uintptr_t(size) | kMiddleWord;
        top[-3] = uintptr_t(baseptr) | kLastWord;
        return true;
    }

    void GCMarkStack::Pop_StackMemory(const void* &p, uint32_t &size, const void* &baseptr)
    {
        GCAssert(PeekTypetag() == kStackMemory);

        uintptr_t* top = m_top-1;
        p = (void*)(top[-1] & ~3);
        size = uint32_t(top[-2] & ~3);
        baseptr = (void*)(top[-3] & ~3);
        freeSpace(4);
    }

    bool GCMarkStack::Push_LargeObjectChunk(const void* p, uint32_t size, const void* baseptr)
    {
        uintptr_t* top = allocSpace(4);
        if (top == NULL)
            return false;
        top[0] = (kLargeObjectChunk << 2) | kFirstWord;
        top[-1] = uintptr_t(p) | kMiddleWord;
        top[-2] = uintptr_t(size) | kMiddleWord;
        top[-3] = uintptr_t(baseptr) | kLastWord;
        return true;
    }

    void GCMarkStack::Pop_LargeObjectChunk(const void* &p, uint32_t &size, const void* &baseptr)
    {
        GCAssert(PeekTypetag() == kLargeObjectChunk);

        uintptr_t* top = m_top-1;
        p = (void*)(top[-1] & ~3);
        size = uint32_t(top[-2] & ~3);
        baseptr = (void*)(top[-3] & ~3);
        freeSpace(4);
    }

    bool GCMarkStack::Push_LargeRootChunk(const void* p, uint32_t size, const void* baseptr)
    {
        uintptr_t* top = allocSpace(4);
        if (top == NULL)
            return false;
        top[0] = (kLargeRootChunk << 2) | kFirstWord;
        top[-1] = uintptr_t(p) | kMiddleWord;
        top[-2] = uintptr_t(size) | kMiddleWord;
        top[-3] = uintptr_t(baseptr) | kLastWord;
        return true;
    }
    
    void GCMarkStack::Pop_LargeRootChunk(const void* &p, uint32_t &size, const void* &baseptr)
    {
        GCAssert(PeekTypetag() == kLargeRootChunk);
        
        uintptr_t* top = m_top-1;
        p = (void*)(top[-1] & ~3);
        size = uint32_t(top[-2] & ~3);
        baseptr = (void*)(top[-3] & ~3);
        freeSpace(4);
    }
    
    bool GCMarkStack::Push_LargeObjectProtector(const void* p)
    {
        uintptr_t* top = allocSpace(2);
        if (top == NULL)
            return false;
        top[0] = (kLargeObjectProtector << 2) | kFirstWord;
        top[-1] = uintptr_t(p) | kLastWord;
        return true;
    }

    void GCMarkStack::Pop_LargeObjectProtector(const void* &p)
    {
        GCAssert(PeekTypetag() == kLargeObjectProtector);

        uintptr_t* top = m_top-1;
        p = (void*)(top[-1] & ~3);
        freeSpace(2);
    }

    bool GCMarkStack::Push_RootProtector(const void* p)
    {
        uintptr_t* top = allocSpace(2);
        if (top == NULL)
            return false;
        top[0] = (kRootProtector << 2) | kFirstWord;
        top[-1] = uintptr_t(p) | kLastWord;
        return true;
    }

    void GCMarkStack::Pop_RootProtector(const void* &p)
    {
        GCAssert(PeekTypetag() == kRootProtector);

        uintptr_t* top = m_top-1;
        p = (void*)(top[-1] & ~3);
        freeSpace(2);
    }

    void GCMarkStack::ClearRootProtectorAndChunkAbove(uintptr_t index, const void* rootptr)
    {
#ifdef DEBUG
        uintptr_t* top = (uintptr_t*)index;
        GCAssert(top[0] == ((kRootProtector << 2) | kFirstWord));
        GCAssert((void*)(top[-1] & ~3) == rootptr);
#endif

        ClearItemAt(index);

        uintptr_t* itemptr = (uintptr_t*)index;
        while ((itemptr = GetNextItemAbove(itemptr)) != NULL) {
            if (*itemptr == ((kLargeRootChunk << 2) | kFirstWord)) {
                const void* baseptr = (void*)(itemptr[-3] & ~3);
                if (baseptr == rootptr) {
                    ClearItemAt((uintptr_t)itemptr);
                    return;
                }
            }
        }
    }

    void GCMarkStack::Clear()
    {
        // Clear out the elements
        while (m_topSegment->m_prev != NULL)
            PopSegment();
        m_top = m_base;

        // Discard the cached segment
        if (m_extraSegment != NULL) {
            FreeStackSegment(m_extraSegment);
            m_extraSegment = NULL;
        }
        GCAssert(Invariants());
    }

    inline uintptr_t* GCMarkStack::items(GCMarkStack::StackSegment* seg)
    {
        GCAssert(sizeof(StackSegment) % sizeof(uintptr_t) == 0);
        return (uintptr_t*)seg + sizeof(StackSegment)/sizeof(uintptr_t);
    }

    inline uintptr_t* GCMarkStack::limit(GCMarkStack::StackSegment* seg)
    {
        static const size_t kMarkStackItems = (GCHeap::kBlockSize - sizeof(StackSegment)) / sizeof(uintptr_t);
#ifdef DEBUG
        return items(seg) + kMarkStackItems - 2;    // Two sentinel words at the end
#else
        return items(seg) + kMarkStackItems;
#endif
    }

    inline GCMarkStack::StackSegment::StackSegment()
    {
        m_savedTop = NULL;
        m_prev = NULL;
#ifdef DEBUG
        sentinel1 = 0;            // catch overwrites
        sentinel2 = 0;            //   as soon as they happen
        limit(this)[0] = 0;       // also
        limit(this)[1] = 0;       //   at the end
#endif
    }

    bool GCMarkStack::PopulateExtraSegment(bool mustSucceed)
    {
        if (m_extraSegment == NULL) {
            void *memory = AllocStackSegment(mustSucceed);
            if (memory == NULL)
                return false;
            m_extraSegment = new (memory) StackSegment();
        }
        return true;
    }

    bool GCMarkStack::PushSegment(bool mustSucceed)
    {
        GCAssert(sizeof(StackSegment) <= GCHeap::kBlockSize);
        if (!PopulateExtraSegment(mustSucceed))
            return false;
        if (m_topSegment != NULL) {
            m_hiddenSegments++;
            m_hiddenCount += uint32_t(m_top - m_base);
            m_topSegment->m_savedTop = m_top;
        }
        StackSegment* seg = m_extraSegment;
        m_extraSegment = NULL;
        seg->m_prev = m_topSegment;
        m_topSegment = seg;
        m_base = items(m_topSegment);
        m_limit = limit(m_topSegment);
        m_top = m_base;
        return true;
    }

    void GCMarkStack::PopSegment_UnlessLast()
    {
        if (m_topSegment->m_prev != NULL)
            PopSegment();
    }

    void GCMarkStack::PopSegment()
    {
        StackSegment* seg = m_topSegment;
        m_topSegment = seg->m_prev;

        if (m_topSegment != NULL) {
            m_base = items(m_topSegment);
            m_limit = limit(m_topSegment);
            m_top = m_topSegment->m_savedTop;
            m_hiddenCount -= uint32_t(m_top - m_base);
            m_hiddenSegments--;
            m_topSegment->m_savedTop = NULL;
        }
        else {
            m_base = m_top = m_limit = NULL;
            GCAssert(m_hiddenCount == 0);
            GCAssert(m_hiddenSegments == 0);
        }

        if (m_extraSegment == NULL) {
            seg->m_prev = NULL;
            m_extraSegment = seg;
        }
        else
            FreeStackSegment(seg);
    }

    GCMarkStack::StackSegment* GCMarkStack::FindLastSegment(StackSegment* first)
    {
        while (first->m_prev != NULL)
            first = first->m_prev;
        return first;
    }

#ifdef MMGC_MARKSTACK_ALLOWANCE
    bool GCMarkStack::MakeSpaceForSegments(int32_t nseg)
    {
        // If we have no space, and aren't going to pop a segment that's empty
        // before we return, and there's not an extra segment to give up, then
        // we fail.
        if (m_allowance >= nseg)
            return true;
        if (m_allowance+1 == nseg && m_top == m_base)
            return true;
        if (m_allowance+1 == nseg && m_extraSegment != NULL) {
            FreeStackSegment(m_extraSegment);
            m_extraSegment = NULL;
            return true;
        }
        return false;
    }
#endif

    void GCMarkStack::TransferOneInactiveSegmentFrom(GCMarkStack& other)
    {
        GCAssert(other.InactiveSegments() > 0);
        
#ifdef MMGC_MARKSTACK_ALLOWANCE
        if (!MakeSpaceForSegments(1))
            return;
#endif

        // Pick off the one below the top always.

        StackSegment* subject = other.m_topSegment->m_prev;
        uint32_t subject_hc = uint32_t(subject->m_savedTop - items(subject));

        other.m_topSegment->m_prev = subject->m_prev;
        other.m_hiddenCount -= subject_hc;
        other.m_hiddenSegments--;
#ifdef MMGC_MARKSTACK_ALLOWANCE
        other.m_allowance++;
#endif

        FindLastSegment(m_topSegment)->m_prev = subject;
        subject->m_prev = NULL;

        m_hiddenCount += subject_hc;
        m_hiddenSegments++;
#ifdef MMGC_MARKSTACK_ALLOWANCE
        // The allowance may temporarily go negative until we pop a segment below.
        m_allowance--;
#endif

        // If a segment was inserted into an empty stack then pop the now empty top segment.
        if (m_top == m_base)
            PopSegment();

        GCAssert(Invariants());
        GCAssert(other.Invariants());
    }

    // Move all the segments from the other stack into the bottom of
    // this stack.

    bool GCMarkStack::TransferEverythingFrom(GCMarkStack& other)
    {
        if (other.IsEmpty())
            return true;

        uint32_t incoming_segments = other.m_hiddenSegments + 1;

#ifdef MMGC_MARKSTACK_ALLOWANCE
        if (!MakeSpaceForSegments(incoming_segments))
            return false;
#endif

        // Make sure we have a segment in the other stack with which to reestablish invariants.
#ifdef MMGC_MARKSTACK_ALLOWANCE
        // Optimistically assume we'll succeed so that there will be an allowance for the other
        // stack to allocate in.
        other.m_allowance += incoming_segments;
        if (!other.PopulateExtraSegment(false)) {
            other.m_allowance -= incoming_segments; // Won't be moving anything after all
            return false;
        }
#else
        if (!other.PopulateExtraSegment(false))
            return false;
#endif

        // Seal the other stack and obtain its data
        other.m_topSegment->m_savedTop = other.m_top;
        StackSegment* subject = other.m_topSegment;
        uint32_t subject_hc = other.m_hiddenCount + uint32_t(other.m_topSegment->m_savedTop - items(other.m_topSegment));
        uint32_t subject_hs = incoming_segments;

        // Reestablish the null state in the other stack
        GCAssert(other.m_extraSegment != NULL);
        other.m_topSegment = other.m_extraSegment;
        other.m_extraSegment = NULL;
        other.m_hiddenCount = 0;
        other.m_hiddenSegments = 0;
        other.m_base = items(other.m_topSegment);
        other.m_limit = limit(other.m_topSegment);
        other.m_top = other.m_base;

        // Link the segments into our stack and update our state
        FindLastSegment(m_topSegment)->m_prev = subject;
        m_hiddenCount += subject_hc;
        m_hiddenSegments += subject_hs;
#ifdef MMGC_MARKSTACK_ALLOWANCE
        // The allowance may temporarily go negative until we pop a segment below.
        m_allowance -= incoming_segments;
#endif
        
        // If the segments were inserted into an empty stack then pop the now empty top segment.
        if (m_top == m_base)
            PopSegment();

        GCAssert(Invariants());
        GCAssert(other.Invariants());

        return true;
    }

    REALLY_INLINE const void* GCMarkStack::Peek_GCObject()
    {
        GCAssert(!IsEmpty());
        uintptr_t w = m_top[-1];
        GCAssert(!(w & 1));
        if ((w & 3) == 0) {
            return (void*)w;
        }
        return NULL;
    }

    bool GCMarkStack::TransferSomethingFrom(GCMarkStack& other)
    {
        if (other.IsEmpty())
            return true;

        const void* peek = other.Peek_GCObject();
        GCAssert(peek != NULL);
        bool pushed = this->Push_GCObject(peek);
        if (pushed) {
            const void* popped = other.Pop_GCObject();
            (void)popped;
            GCAssert(peek == popped);
        }

        GCAssert(Invariants());
        GCAssert(other.Invariants());
        return pushed;
    }

    uintptr_t* GCMarkStack::GetNextItemAbove(uintptr_t* item)
    {
        if (item == m_top-1)
            return NULL;

        m_topSegment->m_savedTop = m_top;
        StackSegment* seg = m_topSegment;
        StackSegment* prev = NULL;
        while (seg != NULL) {
            if(item >= items(seg) && item < seg->m_savedTop) {
                // If the current item is the top item in the segment then the item
                // above is in the segment above, otherwise it's in this segment
                uintptr_t* p = (item == seg->m_savedTop-1) ? items(prev) : item+1;

                // Scan forward in the item to find the top word.
                while ((*p & 1) != 0)
                    p++;
                return p;
            }
            prev = seg;
            seg = seg->m_prev;
        }
        return NULL;
    }

    // About overwriting dead items with a distinguished non-NULL GC object pointer:
    //
    // It's not strictly necessary to do it this way.  The two alternatives are to use
    // a distinguished type of stack item (kDeadItem), or using a NULL pointer.  A kDeadItem
    // type could be used at zero cost, dead items would be handled on the slow path.
    // However a kDeadItem would always be at least two words long and could not be used
    // to clear out a GCObject.  Also, a kDeadItem would be variable-length, complicating
    // code that parses stack items.  A NULL GCObject pointer is possible as the current
    // marker architecture would just divert those items onto the slow path at zero cost.
    // However, the invariant that there are no NULL pointers on the stack is worth more
    // to me at present.

    void GCMarkStack::ClearItemAt(uintptr_t index)
    {
        GCAssert(m_deadItem != 0);
        uintptr_t* p = (uintptr_t*)index;
        uintptr_t w = *p;
        p[0] = m_deadItem;
        if ((w & 3) != 0) {
            switch (TypeTag(w >> 2)) {
                default:
                    GCAssert(!"Unknown TypeTag in ClearItemAt");
                    break;
                case kStackMemory:
                case kLargeObjectChunk:
                case kLargeRootChunk:
                    p[-3] = m_deadItem;
                case kLargeExactObjectTail:
                    p[-2] = m_deadItem;
                case kRootProtector:
                case kLargeObjectProtector:
                    p[-1] = m_deadItem;
            }
        }
        GCAssert(Invariants());
    }

    inline void* GCMarkStack::AllocStackSegment(bool mustSucceed)
    {
#ifdef MMGC_MARKSTACK_ALLOWANCE
        if (m_allowance == 0) {
            // This is fine, mustSucceed is only used for the first segment in
            // a stack, and the allowance is per-stack.
            GCAssert(!mustSucceed);
            return NULL;
        }
        --m_allowance;
#endif
        if (mustSucceed)
            return GCHeap::GetGCHeap()->Alloc(1, GCHeap::flags_Alloc);
        else
            return GCHeap::GetGCHeap()->AllocNoOOM(1, GCHeap::flags_Alloc | GCHeap::kCanFail);
    }

    inline void GCMarkStack::FreeStackSegment(void* p)
    {
#ifdef MMGC_MARKSTACK_ALLOWANCE
        ++m_allowance;
#endif
        GCHeap::GetGCHeap()->FreeNoOOM(p);
    }

#ifdef _DEBUG
    bool GCMarkStack::Invariants()
    {
#ifdef MMGC_MARKSTACK_ALLOWANCE
        GCAssert(m_allowance >= 0);
#endif
        GCAssert(m_topSegment->sentinel1 == 0);
        GCAssert(m_topSegment->sentinel2 == 0);
        GCAssert(limit(m_topSegment)[0] == 0);
        GCAssert(limit(m_topSegment)[1] == 0);
        GCAssert(limit(m_topSegment) == m_limit);
        GCAssert(m_top >= m_base);
        GCAssert(m_top <= m_limit);
        GCAssert(m_topSegment->m_prev == NULL || m_top > m_base);
        uint32_t hc = 0;
        uint32_t ns = 0;
        for ( StackSegment* seg=m_topSegment->m_prev ; seg != NULL ; seg = seg->m_prev ) {
            GCAssert(seg->sentinel1 == 0);
            GCAssert(seg->sentinel2 == 0);
            GCAssert(limit(seg)[0] == 0);
            GCAssert(limit(seg)[1] == 0);
            hc += uint32_t(seg->m_savedTop - items(seg));
            ns++;
        }
        GCAssert(ns == InactiveSegments());
        GCAssert(hc == m_hiddenCount);
        GCAssert(Count() == hc + (m_top - m_base));
        return true;
    }
#endif
}
