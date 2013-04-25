/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __ZCT_INLINES__
#define __ZCT_INLINES__

/**
 * In-line functions for ZCT.cpp and ZCT.h, factored out to break otherwise
 * forward references to class definitions.
 */

namespace MMgc
{
    REALLY_INLINE void ZCT::Add(RCObject *obj REFCOUNT_PROFILING_ARG(bool initial))
    {
        REFCOUNT_PROFILING_ONLY( gc->policy.signalZCTAdd(initial, 0); )

        // Note:
        //  - If gc->collecting is true then top == limit and we'll
        //    take the slow path; the slow path takes care of extra
        //    checking during sweeping
        //  - 'limit' is set such that 'topIndex' will not grow beyond
        //    the limit of the ZCT_INDEX field of obj, so no checking
        //    for that is required

        if (top < limit) {
            *top++ = obj;
#ifdef DEBUG
            obj->setZCTIndexAndMaybeUnpin(topIndex++, KeepPinned());
#else
            obj->setZCTIndexAndMaybeUnpin(topIndex++, uint32_t(reaping));
#endif
        }
        else
            AddSlow(obj);
    }

    REALLY_INLINE uint32_t ZCT::KeepPinned()
    {
        uint32_t keepPinned = reaping;
#ifdef DEBUG
        // The validation MarkSweep happens right before we Reap
        // and reaping isn't set but we don't want to unpin things
        // because the validation MarkSweep does the pinning and
        // Reap itself does no pinning so if we cleared the flag
        // we could reap an item on the stack.
        if(gc->performingDRCValidationTrace)
            keepPinned = 1;
#endif      
        return keepPinned;
    }

    REALLY_INLINE void ZCT::Remove(RCObject *obj REFCOUNT_PROFILING_ARG(bool final))
    {
        REFCOUNT_PROFILING_ONLY( gc->policy.signalZCTRemove(final); )

        // Note:
        //  - We just clear out the entry, compaction takes care of NULL entries later.
        //    The volume of removals is expected to be small (see statistics earlier),
        //    so excessive memory use due to no freelist should not be a problem.

        uint32_t index = obj->getZCTIndex();
        GCAssert(Get(index) == obj);
        Put(index, NULL);
        obj->ClearZCTFlag();
    }

    REALLY_INLINE bool ZCT::IsReaping()
    {
        return reaping;
    }

    REALLY_INLINE uint32_t ZCT::BlockNumber(uint32_t idx)
    {
        return idx/CAPACITY(RCObject**);
    }

    REALLY_INLINE uint32_t ZCT::EntryNumber(uint32_t idx)
    {
        return idx & (CAPACITY(RCObject*)-1);
    }

    REALLY_INLINE RCObject** ZCT::PointerTo(uint32_t idx)
    {
        GCAssert((slowState && idx < slowTopIndex) || (!slowState && idx < topIndex));  // Fascistic compiler settings compel us to add the inner parens here
        return blocktable[BlockNumber(idx)] + EntryNumber(idx);
    }

    REALLY_INLINE RCObject* ZCT::Get(uint32_t idx)
    {
        return *PointerTo(idx);
    }

    REALLY_INLINE void ZCT::Put(uint32_t idx, RCObject* value)
    {
        *PointerTo(idx) = value;
    }
}

#endif // __ZCT_INLINES__
