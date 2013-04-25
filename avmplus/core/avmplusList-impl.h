/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_List_impl__
#define __avmplus_List_impl__

namespace avmplus
{
    template<class T, class ListHelper>
    ListImpl<T,ListHelper>::ListImpl(MMgc::GC* gc,
                        uint32_t capacity,
                        const T* args) : m_data(NULL)
    {
        uint32_t const allocCap = (capacity < kListMinCapacity) ? kListMinCapacity : capacity;
        typename ListHelper::LISTDATA* newData = allocData(gc, allocCap);
        ListHelper::wbData(this, &m_data, newData);

        if (args != NULL)
        {
            for (uint32_t i = 0; i < capacity; i++)
                ListHelper::storeInEmpty(m_data, i, args[i]);
            set_length_guarded(capacity);
        }
    }

    template<class T, class ListHelper>
    ListImpl<T,ListHelper>::~ListImpl()
    {
        destroy();
    }

    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::set_length_guarded(uint32_t newlength)
    {
        if (newlength > kListMaxLength)
            MMgc::GCHeap::SignalObjectTooLarge();
        m_data->len = newlength;
    }
    
    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::set_length_guarded(typename ListHelper::LISTDATA* data, uint32_t newlength)
    {
        if (newlength > kListMaxLength)
            MMgc::GCHeap::SignalObjectTooLarge();
        data->len = newlength;
    }
    
    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::freeData(MMgc::GC* gc)
    {
        typename ListHelper::LISTDATA* data = m_data;
        m_data = NULL;
        ListHelper::LISTDATA::free(gc, data);
    }

    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::destroy()
    {
        // Normally, m_data can't be null, but a call to destroy() can
        // cause this to happen. This is necessary for certain shutdown
        // conditions in Flash/AIR.
        if (m_data != NULL)
        {
            if (m_data->len > 0)
                ListHelper::clearRange(m_data, 0, m_data->len);
            freeData(m_data->gc()); // note that this leaves m_data set to NULL.
        }
    }

    template<class T, class ListHelper>
    REALLY_INLINE bool ListImpl<T,ListHelper>::isDestroyed() const
    {
        return m_data == NULL;
    }

    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::set_length(uint32_t len)
    {
        if (m_data->len != len)
        {
            ensureCapacity(len);
            uint32_t const oldlen = m_data->len;
            uint32_t start, count;
            if (len < oldlen)
            {
                start = len;
                count = oldlen - len;
            }
            else
            {
                start = oldlen;
                count = len - oldlen;
            }
            ListHelper::clearRange(m_data, start, count);
            set_length_guarded(len);
        }
    }

    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::set_capacity(uint32_t cap)
    {
        AvmAssert(m_data != NULL);
        if (cap < kListMinCapacity)
            cap = kListMinCapacity;
        if (capacity() != cap)
        {
            MMgc::GC* const gc = m_data->gc();
            AvmAssert(cap > kListMinCapacity);
            uint32_t len = m_data->len;
            // keep existing length if possible (but don't let it exceed cap)
            if (len > cap)
                len = cap;
            typename ListHelper::LISTDATA* newData = allocData(gc, cap);
            if (len > 0)
                VMPI_memcpy(newData->entries, m_data->entries, len * sizeof(typename ListHelper::STORAGE));
            set_length_guarded(newData, len);
            // don't call ListHelper::clearRange here; we want the refCounts to be transferred
            freeData(gc);
            ListHelper::wbData(this, &m_data, newData);
        }
    }

    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::add(T value)
    {
        // Sanity check.
        AvmAssert(m_data->len <= capacity());

        // OPTIMIZEME, this method may be worth inlining
        ensureCapacityExtra(m_data->len, 1);
        ListHelper::storeInEmpty(m_data, m_data->len, value);
        set_length_guarded(m_data->len+1);
    }

    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::add(const ListImpl<T,ListHelper>& that)
    {
        uint32_t const n = that.length();
        // If m_data->len + n overflows uint32_t then ensureCapacityExtra 
        // will not return.
        ensureCapacityExtra(m_data->len, n);
        for (uint32_t i = 0; i < n; ++i)
            ListHelper::storeInEmpty(m_data, m_data->len + i, that.get(i));
        // No overflow here for reasons stated above.
        set_length_guarded(m_data->len + n);
    }

    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::insert(uint32_t index, T value, uint32_t count)
    {
        // Sanity check
        AvmAssert(m_data->len <= capacity());

        // If m_data->len + count overflows uint32_t then ensureCapacityExtra 
        // will not return.
        ensureCapacityExtra(m_data->len, count);
        if (index < m_data->len)
            ListHelper::moveRange(m_data, index, index + count, m_data->len - index);
        else
            index = m_data->len;
        for (uint32_t i = 0; i < count; ++i)
            ListHelper::store(m_data, index + i, value);
        // No overflow here for reasons stated above.
        set_length_guarded(m_data->len + count);
    }

    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::clear()
    {
        AvmAssert(m_data != NULL);
        if (m_data->len > 0)
        {
            ListHelper::clearRange(m_data, 0, m_data->len);
            m_data->len = 0;    // Clears length, no check needed
        }
        if (capacity() > kListMinCapacity)
        {
            MMgc::GC* const gc = m_data->gc();
            typename ListHelper::LISTDATA* newData = allocData(gc, kListMinCapacity);
            newData->len = 0;   // Clears length, no length needed
            freeData(gc);
            ListHelper::wbData(this, &m_data, newData);
        }

        AvmAssert(m_data->len == 0);
    }

#ifdef VMCFG_FLOAT
    template<>
    inline int32_t ListImpl<float4_t,DataListHelper<float4_t, 16> >::indexOf(float4_t value) const
    {
        for (uint32_t i = 0, n = m_data->len; i < n; i++)
            if (f4_eq_i(DataListHelper<float4_t, 16>::load(m_data, i), value))
                return int32_t(i);
        return -1;
    }
#endif

    template<class T, class ListHelper>
    int32_t ListImpl<T,ListHelper>::indexOf(T value) const
    {
        for (uint32_t i = 0, n = m_data->len; i < n; i++)
            if (ListHelper::load(m_data, i) == value)
                return int32_t(i);
        return -1;
    }

#ifdef VMCFG_FLOAT
    template<>
    inline int32_t ListImpl<float4_t,DataListHelper<float4_t, 16> >::lastIndexOf(float4_t value) const
    {
        for (uint32_t i = m_data->len; i > 0; i--)
            if (f4_eq_i(DataListHelper<float4_t, 16>::load(m_data, i-1), value))
                return int32_t(i);
        return -1;
    }
#endif

    template<class T, class ListHelper>
    int32_t ListImpl<T,ListHelper>::lastIndexOf(T value) const
    {
        for (uint32_t i = m_data->len; i > 0; i--)
            if (ListHelper::load(m_data, i-1) == value)
                return int32_t(i);
        return -1;
    }

    template<class T, class ListHelper>
    T ListImpl<T,ListHelper>::removeAt(uint32_t index)
    {
        AvmAssert(!isEmpty());
        uint32_t const len_minus_1 = m_data->len - 1;
        AvmAssert(index <= len_minus_1);
        T const old = ListHelper::load(m_data, index);
        ListHelper::clearRange(m_data, index, 1);
        if (index < len_minus_1)
        {
            ListHelper::moveRange(m_data, index+1, index, len_minus_1 - index);
        }
        m_data->len = len_minus_1;    // Reduces length, no check needed.
        return old;
    }

    // Benchmarking shows it's worth specializing this,
    // rather than using removeAt(len-1), as it's used
    // in Array.pop, which can be fairly hot.
    template<class T, class ListHelper>
    T ListImpl<T,ListHelper>::removeLast()
    {
        AvmAssert(!isEmpty());
        uint32_t const index = m_data->len - 1;
        T const old = ListHelper::load(m_data, index);
        ListHelper::clearRange(m_data, index, 1);
        m_data->len = index;    // Reduces length, no check needed.
        return old;
    }

    template<class T, class ListHelper>
    void FASTCALL ListImpl<T,ListHelper>::ensureCapacityImpl(uint32_t cap)
    {
        AvmAssert(m_data != NULL);
        AvmAssert(capacity() >= kListMinCapacity);
        AvmAssert(cap > capacity());

        // OPTIMIZEME: cap += (cap/4) is the growth algorithm used by the old AtomArray.
        // Is this the best algorithm? Not sure, but it's definitely better
        // in our perf tests than the one used by the old List<> implementation.

        // Overflow check
        if (UINT32_T_MAX - cap < cap/4)
            MMgc::GCHeap::SignalObjectTooLarge();
        cap += (cap/4);

        AvmAssert(cap > kListMinCapacity);
        MMgc::GC* const gc = m_data->gc();
        typename ListHelper::LISTDATA* newData = allocData(gc, cap);

        VMPI_memcpy(newData->entries, m_data->entries, m_data->len * sizeof(typename ListHelper::STORAGE));
        newData->len = m_data->len;   // No check needed, copies a valid datum
        // don't call ListHelper::clearRange here; we want the refCounts to be transferred
        freeData(gc);
        ListHelper::wbData(this, &m_data, newData);
    }

    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::insert(uint32_t index, const T* args, uint32_t argc)
    {
        uint32_t const len = m_data->len;

        // If m_data->len + argc overflows uint32_t then ensureCapacityExtra
        // will not return.
        ensureCapacityExtra(len, argc);
        if (index < len)
            ListHelper::moveRange(m_data, index, index + argc, len - index);
        else
            index = len;

        // allow args == NULL, which leaves the inserted entries empty
        // (VectorObject makes use of this to avoid temporary storage in some cases)
        if (args != NULL)
        {
            for (uint32_t i = 0; i < argc; ++i)
                ListHelper::storeInEmpty(m_data, index + i, args[i]);
        }

        // No overflow check required for reasons stated above.
        set_length_guarded(m_data->len + argc);
    }

    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::splice(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, const T* args)
    {
        uint32_t const len = m_data->len;
    
        // We only need to expand if we are inserting more than we are deleting,
        // so don't bother calling it unless that's the case, as it means we don't
        // have to worry about overflow-checking "insertCount - deleteCount".
        // If len + insertCount - deleteCount overflows then ensureCapacityExtra
        // will not return.
        if (insertCount > deleteCount)
            ensureCapacityExtra(len, insertCount - deleteCount);
        if (insertCount < deleteCount)
        {
            ListHelper::clearRange(m_data, insertPoint + insertCount, deleteCount - insertCount);
            ListHelper::moveRange(m_data, insertPoint + deleteCount, insertPoint + insertCount, len - insertPoint - deleteCount);
        }
        else if (insertCount > deleteCount)
        {
            ListHelper::moveRange(m_data, insertPoint, insertPoint + insertCount - deleteCount, len - insertPoint);
        }
        
        // allow args == NULL, which leaves the inserted entries empty
        // (VectorObject makes use of this to avoid temporary storage in some cases)
        if (args != NULL)
        {
            for (uint32_t i = 0; i < insertCount; ++i)
            {
                ListHelper::store(m_data, insertPoint + i, args[i]);
            }
        }

        // No overflow check needed for reasons stated above.
        set_length_guarded(len + insertCount - deleteCount);
    }

    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::splice(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, const ListImpl<T,ListHelper>& that, uint32_t thatOffset)
    {
        uint32_t const len = m_data->len;
        // We only need to expand if we are inserting more than we are deleting,
        // so don't bother calling it unless that's the case, as it means we don't
        // have to worry about overflow-checking "insertCount - deleteCount".
        // If len + insertCount - deleteCount overflows then ensureCapacityExtra
        // will not return.
        if (insertCount > deleteCount)
            ensureCapacityExtra(len, insertCount - deleteCount);
        if (insertCount < deleteCount)
        {
            ListHelper::clearRange(m_data, insertPoint + insertCount, deleteCount - insertCount);
            ListHelper::moveRange(m_data, insertPoint + deleteCount, insertPoint + insertCount, len - insertPoint - deleteCount);
        }
        else if (insertCount > deleteCount)
        {
            ListHelper::moveRange(m_data, insertPoint, insertPoint + insertCount - deleteCount, len - insertPoint);
        }

        for (uint32_t i = 0; i < insertCount; ++i)
        {
            AvmAssert(i + thatOffset < that.length());
            T const value = ListHelper::load(that.m_data, i + thatOffset);
            ListHelper::store(m_data, insertPoint + i, value);
        }

        // No overflow check needed for reasons stated above.
        set_length_guarded(len + insertCount - deleteCount);
    }

    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::reverse()
    {
        uint32_t const len = m_data->len;
        if (len > 1)
        {
            MMgc::GC* const gc = m_data->gc();
            if (sizeof(m_data->entries[0]) == sizeof(void*) && gc->IsPointerToGCObject(m_data))
            {
                // It's really only necessary to do this for things that
                // might be GC pointers, but it doesn't really hurt to do
                // for other things that just happen to be the same size.
                gc->reversePointersWithinBlock(m_data, offsetof(typename ListHelper::LISTDATA, entries), len);
            }
            else
            {
                for (uint32_t i = 0, n = len/2; i < n; i++)
                {
                    uint32_t const i2 = len - i - 1;
                    T const v  = ListHelper::load(m_data, i);
                    T const v2 = ListHelper::load(m_data, i2);
                    ListHelper::store(m_data, i, v2);
                    ListHelper::store(m_data, i2, v);
                }
            }
        }
    }

#ifdef VMCFG_FLOAT
    template<>
    inline uint32_t ListImpl<float4_t,DataListHelper<float4_t, 16> >::removeNullItems()
    {
        // For float4 lists this has no utility at present, so do nothing.  float4 values
        // are non-nullable and removeNullItems is only used on lists with references
        return 0;
    }
#endif

    template<class T, class ListHelper>
    uint32_t ListImpl<T,ListHelper>::removeNullItems()
    {
        uint32_t kept = 0;
        for (uint32_t i = 0, n = m_data->len; i < n; ++i)
        {
            T const v  = ListHelper::load(m_data, i);
            if (v != (T)0)
            {
                if (kept < i)
                    ListHelper::store(m_data, kept, v);
                ++kept;
            }
        }
        uint32_t const removed = m_data->len - kept;
        if (kept < m_data->len)
        {
            ListHelper::clearRange(m_data, kept, removed);
            m_data->len = kept;     // No check needed since kept < m_data->len
        }
        return removed;
    }

    template<class T, class ListHelper>
    void ListImpl<T,ListHelper>::gcTrace(MMgc::GC* gc)
    {
        ListHelper::gcTrace(gc, &m_data);
    }
}

#endif /* __avmplus_List_impl__ */
