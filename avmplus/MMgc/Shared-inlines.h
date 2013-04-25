/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __Shared_inlines__
#define __Shared_inlines__

// Inline methods shared across classes


namespace MMgc
{
    // FL* are freelist helpers, put in one place to ease Valgrind support
    REALLY_INLINE void **FLSeed(void **item, void *next)
    {
        VALGRIND_MAKE_MEM_DEFINED(item, sizeof(void*));
        item[0] = next;
        VALGRIND_MAKE_MEM_UNDEFINED(item, sizeof(void*));
        return (void**)next;
    }

    REALLY_INLINE void FLPush(void* &head, const void *next)
    {
        *(void**)next = head;
        head = (void*)next;
    }

    REALLY_INLINE void *FLPop(void* &head)
    {
        void *p = head;
        VALGRIND_MAKE_MEM_DEFINED(p, sizeof(void*));
        head = *(void**)p;
        VALGRIND_MAKE_MEM_UNDEFINED(p, sizeof(void*));
        return p;
    }

    REALLY_INLINE void *FLPopAndZero(void* &head)
    { 
        void *p = FLPop(head);
        VALGRIND_MAKE_MEM_DEFINED(p, sizeof(void*));
        *(void**)p = NULL;
        VALGRIND_MAKE_MEM_UNDEFINED(p, sizeof(void*));
        return p;
    }

    REALLY_INLINE void *FLNext(void *item)
    {
        void **p = (void**)item;
        VALGRIND_MAKE_MEM_DEFINED(p, sizeof(void*));
        void *next = p[0];
        VALGRIND_MAKE_MEM_UNDEFINED(p, sizeof(void*));
        return next;
    }

    template<typename T, int growthIncrement>
    bool BasicList<T,growthIncrement>::Add(T item)
    {
        if (holes && iteratorCount == 0)
            Compact();
        if (count == capacity)
        {
            uint32_t tryCapacity = capacity + growthIncrement;
            T* newItems = (T*)VMPI_alloc(GCHeap::CheckForCallocSizeOverflow(tryCapacity, sizeof(T)));

            if (newItems == NULL)
                return false;
            memset(newItems, 0, sizeof(T)*tryCapacity);

            capacity = tryCapacity;
            if (items)
                VMPI_memcpy(newItems, items, count * sizeof(T));
            VMPI_free(items);
            items = newItems;
        }

		uint32_t targetOffset = 0;
        if (holes)
        {
            for (uint32_t j = 0; j < capacity && !targetOffset; j++)
            {
            	// find the first hole to insert item:
                if (items[j] == NULL)
                {
					targetOffset = j;
                }
            }
        }
		else
		{
			targetOffset = count;
		}

        items[targetOffset] = item;
        count++;
        return true;
    }

}

#endif /* __Shared_inlines__ */
