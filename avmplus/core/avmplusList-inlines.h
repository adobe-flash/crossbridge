/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_List_inlines__
#define __avmplus_List_inlines__

#ifndef UINT32_T_MAX
    #define UINT32_T_MAX (uint32_t(0xFFFFFFFFUL))
#endif

namespace avmplus
{
    template<>
    REALLY_INLINE bool TracedListData<Atom>::gcTrace(MMgc::GC* gc, size_t cursor)
    {
        const size_t work_increment = 2000/sizeof(void*);
        const size_t work_count = len;
        if (cursor * work_increment >= work_count)
            return false;

        size_t work = work_increment;
        bool more = true;
        if ((cursor + 1) * work_increment >= work_count)
        {
            work = work_count - (cursor * work_increment);
            more = false;
        }

        gc->TraceAtoms(entries + (cursor*work_increment), work);
        return more;
    }

    template<class T>
    inline bool TracedListData<T>::gcTrace(MMgc::GC* gc, size_t cursor)
    {
        const size_t work_increment = 2000/sizeof(void*);
        const size_t work_count = len;
        if (cursor * work_increment >= work_count)
            return false;
        
        size_t work = work_increment;
        bool more = true;
        if ((cursor + 1) * work_increment >= work_count)
        {
            work = work_count - (cursor * work_increment);
            more = false;
        }

        gc->TraceLocations(entries + (cursor*work_increment), work);
        return more;
    }

    // ----------------------------

    // For information about "align", see the doc block at class DataListHelper.
    
    template<class T, uintptr_t align>
    REALLY_INLINE /*static*/ void DataListHelper<T, align>::wbData(const void* /*container*/, LISTDATA** address, LISTDATA* data)
    {
        *address = data;
    }

    template<class T, uintptr_t align>
    REALLY_INLINE /*static*/ typename DataListHelper<T, align>::TYPE DataListHelper<T, align>::load(LISTDATA* data, uint32_t index)
    {
        typedef typename DataListHelper<T, align>::TYPE VAL;
        AvmAssert(data != NULL);
        if (align == 0)
            return data->entries[index];
        else
            return ((VAL*)((uintptr_t(data->entries) + (align-1)) & ~(align-1)))[index];
    }

    template<class T, uintptr_t align>
    REALLY_INLINE /*static*/ void DataListHelper<T, align>::store(LISTDATA* data, uint32_t index, TYPE value)
    {
        typedef typename DataListHelper<T, align>::TYPE VAL;
        AvmAssert(data != NULL);
        if (align == 0)
            data->entries[index] = value;
        else
            ((VAL*)((uintptr_t(data->entries) + (align-1)) & ~(align-1)))[index] = value;
    }

    template<class T, uintptr_t align>
    REALLY_INLINE /*static*/ void DataListHelper<T, align>::storeInEmpty(LISTDATA* data, uint32_t index, TYPE value)
    {
        typedef typename DataListHelper<T, align>::TYPE VAL;
        // This is commented out because it's not necessary for safety of a DataList,
        // and it can generate an assert in List::insert(), which uses storeInEmpty() for efficiency;
        // since DataListHelper::moveRange doesn't zero out the cleared space we could get this
        // assertion firing. Better to neuter this assertion than add a unnecessary zeroing-out
        // to moveRange() just to pacify it.
        // AvmAssert(data->entries[index] == 0);
        AvmAssert(data != NULL);
        if (align == 0)
            data->entries[index] = value;
        else
            ((VAL*)((uintptr_t(data->entries) + (align-1)) & ~(align-1)))[index] = value;
    }

    template<class T, uintptr_t align>
    REALLY_INLINE /*static*/ void DataListHelper<T, align>::clearRange(LISTDATA* data, uint32_t start, uint32_t count)
    {
        typedef typename DataListHelper<T, align>::TYPE VAL;
        AvmAssert(count > 0);
        VAL* base;
        if (align == 0)
            base = data->entries;
        else
            base = ((VAL*)((uintptr_t(data->entries) + (align-1)) & ~(align-1)));
        VMPI_memset((base + start), 0, count*sizeof(T));
    }
    
    template<class T, uintptr_t align>
    REALLY_INLINE /*static*/ void DataListHelper<T, align>::moveRange(LISTDATA* data, uint32_t srcStart, uint32_t dstStart, uint32_t count)
    {
        typedef typename DataListHelper<T, align>::TYPE VAL;
        VAL* base;
        if (align == 0)
            base = data->entries;
        else
            base = ((VAL*)((uintptr_t(data->entries) + (align-1)) & ~(align-1)));
        VMPI_memmove((base + dstStart), (base + srcStart), count * sizeof(T));
    }

    template<class T, uintptr_t align>
    REALLY_INLINE /*static*/ void DataListHelper<T, align>::gcTrace(MMgc::GC* gc, LISTDATA** loc)
    {
        AvmAssert(!gc->IsPointerToGCPage(*loc));
        (void)gc; (void)loc;
    }

    // ----------------------------

    REALLY_INLINE /*static*/ void GCListHelper::wbData(const void* container, LISTDATA** address, LISTDATA* data)
    {
        MMgc::GC* const gc = data->gc();
        if (gc->IsPointerToGCPage(container))
        {
            WB(gc, gc->FindBeginningFast(container), address, data);
        }
        else
        {
            *address = data;
        }
    }

    REALLY_INLINE /*static*/ GCListHelper::TYPE GCListHelper::load(LISTDATA* data, uint32_t index)
    {
        AvmAssert(data != NULL);
        return data->entries[index];
    }

    REALLY_INLINE /*static*/ void GCListHelper::store(LISTDATA* data, uint32_t index, TYPE value)
    {
        AvmAssert(data != NULL);
        WB(data->gc(), data, &data->entries[index], value);
    }

    REALLY_INLINE /*static*/ void GCListHelper::storeInEmpty(LISTDATA* data, uint32_t index, TYPE value)
    {
        AvmAssert(data != NULL);
        AvmAssert(data->entries[index] == 0);
        WB(data->gc(), data, &data->entries[index], value);
    }

    REALLY_INLINE /*static*/ void GCListHelper::clearRange(LISTDATA* data, uint32_t start, uint32_t count)
    {
        AvmAssert(count > 0);
        VMPI_memset(&data->entries[start], 0, count*sizeof(MMgc::GCObject*));
    }

    REALLY_INLINE /*static*/ void GCListHelper::moveRange(LISTDATA* data, uint32_t srcStart, uint32_t dstStart, uint32_t count)
    {
        AvmAssert(data != NULL);
        data->gc()->movePointersWithinBlock((void**)data,
                                           uint32_t((char*)(&data->entries[dstStart]) - (char*)data),
                                           uint32_t((char*)(&data->entries[srcStart]) - (char*)data),
                                           count,
                                           /*zeroEmptied*/ true);
    }

    REALLY_INLINE /*static*/ void GCListHelper::gcTrace(MMgc::GC* gc, LISTDATA** loc)
    {
        gc->TraceLocation(loc);
    }
    
    // ----------------------------

    REALLY_INLINE /*static*/ void RCListHelper::wbData(const void* container, LISTDATA** address, LISTDATA* data)
    {
        MMgc::GC* const gc = data->gc();
        if (gc->IsPointerToGCPage(container))
        {
            WB(gc, gc->FindBeginningFast(container), address, data);
        }
        else
        {
            *address = data;
        }
    }

    REALLY_INLINE /*static*/ RCListHelper::TYPE RCListHelper::load(LISTDATA* data, uint32_t index)
    {
        AvmAssert(data != NULL);
        return data->entries[index];
    }

    REALLY_INLINE /*static*/ void RCListHelper::store(LISTDATA* data, uint32_t index, TYPE value)
    {
        AvmAssert(data != NULL);
        WBRC(data->gc(), data, &data->entries[index], value);
    }

    REALLY_INLINE /*static*/ void RCListHelper::storeInEmpty(LISTDATA* data, uint32_t index, TYPE value)
    {
        AvmAssert(data != NULL);
        AvmAssert(data->entries[index] == 0);
        // We can't use WriteBarrierRC_ctor here, as it will call GetGC() on the address, which might be
        // wrong if the allocation is large. Instead, replicate the necessary code inline (ugh) in
        // the tradition of AvmCore::atomWriteBarrier_ctor...
        if (value)
        {
            if (data->gc()->BarrierActive())
                data->gc()->InlineWriteBarrierTrap(data);
            value->IncrementRef();
            data->entries[index] = value;
        }
    }

    REALLY_INLINE /*static*/ void RCListHelper::clearRange(LISTDATA* data, uint32_t start, uint32_t count)
    {
        AvmAssert(count > 0);
        for (uint32_t i = start, n = start + count; i < n; i++)
        {
            if (data->entries[i])
            {
                data->entries[i]->DecrementRef();
                data->entries[i] = 0;
            }
        }
    }

    REALLY_INLINE /*static*/ void RCListHelper::moveRange(LISTDATA* data, uint32_t srcStart, uint32_t dstStart, uint32_t count)
    {
        AvmAssert(data != NULL);
        data->gc()->movePointersWithinBlock((void**)data,
                                           uint32_t((char*)(&data->entries[dstStart]) - (char*)data),
                                           uint32_t((char*)(&data->entries[srcStart]) - (char*)data),
                                           count,
                                           /*zeroEmptied*/ true);
    }

    REALLY_INLINE /*static*/ void RCListHelper::gcTrace(MMgc::GC* gc, LISTDATA** loc)
    {
        gc->TraceLocation(loc);
    }

    // ----------------------------

    REALLY_INLINE AtomList::AtomList(MMgc::GC* gc, uint32_t capacity, const Atom* args)
        : ListImpl<Atom,AtomListHelper>(gc, capacity, args)
    {
    }

    REALLY_INLINE void AtomList::setPointer(uint32_t index, Atom value)
    {
        // Yes, this is worth inlining, according to performance testing.  Notably it
        // can be CSE'd when setPointer is inlined into its caller along with another
        // range check.
        if (index >= m_data->len)
        {
            ensureCapacityExtra(index, 1);
            set_length_guarded(index+1);
        }
        AtomListHelper::storePointer(m_data, index, value);
    }

    REALLY_INLINE /*static*/ void AtomListHelper::wbData(const void* container, LISTDATA** address, LISTDATA* data)
    {
        MMgc::GC* const gc = data->gc();
        if (gc->IsPointerToGCPage(container))
        {
            WB(gc, gc->FindBeginningFast(container), address, data);
        }
        else
        {
            *address = data;
        }
    }

    REALLY_INLINE /*static*/ AtomListHelper::TYPE AtomListHelper::load(LISTDATA* data, uint32_t index)
    {
        AvmAssert(data != NULL);
        return data->entries[index];
    }

    REALLY_INLINE /*static*/ void AtomListHelper::store(LISTDATA* data, uint32_t index, TYPE value)
    {
        AvmAssert(data != NULL);
        AvmCore::atomWriteBarrier(data->gc(), data, &data->entries[index], value);
    }

    REALLY_INLINE /*static*/ void AtomListHelper::storePointer(LISTDATA* data, uint32_t index, TYPE value)
    {
        AvmAssert(data != NULL);
#ifdef _DEBUG
        // Never-used slots will be all-bits-zero, used slots will always be tagged properly
        int oldtag = atomKind(data->entries[index]);
        AvmAssert(oldtag == kObjectType || oldtag == kStringType || oldtag == kNamespaceType || data->entries[index] == 0);
        int newtag = atomKind(value);
        AvmAssert(newtag == kObjectType || newtag == kStringType || newtag == kNamespaceType);
#endif
        // This is atomWriteBarrier specialized to the case of known-pointers and inlined.

        MMgc::RCObject* rcptr;
        rcptr = (MMgc::RCObject*)atomPtr(data->entries[index]);
        if (rcptr)
            rcptr->DecrementRef();

        rcptr = (MMgc::RCObject*)atomPtr(value);
        if (rcptr)
            rcptr->IncrementRef();
        
        MMgc::GC* gc = data->gc();
        if (gc->BarrierActive())
            gc->InlineWriteBarrierTrap(data);
        
        data->entries[index] = value;
    }
    
    REALLY_INLINE /*static*/ void AtomListHelper::storeInEmpty(LISTDATA* data, uint32_t index, TYPE value)
    {
        AvmAssert(data != NULL);
        // newly-allocated space clears to 0; clearRange() clears to nullObjectAtom.
        // Both are "empty" as far as AtomList is concerned.
        AvmAssert(data->entries[index] == 0 || data->entries[index] == nullObjectAtom);
        AvmCore::atomWriteBarrier_ctor(data->gc(), data, &data->entries[index], value);
    }

    REALLY_INLINE /*static*/ void AtomListHelper::clearRange(LISTDATA* data, uint32_t start, uint32_t count)
    {
        AvmAssert(count > 0);
        AvmCore::decrementAtomRegion_null(&data->entries[start], count);
    }

    REALLY_INLINE /*static*/ void AtomListHelper::moveRange(LISTDATA* data, uint32_t srcStart, uint32_t dstStart, uint32_t count)
    {
        AvmAssert(data != NULL);
        data->gc()->movePointersWithinBlock((void**)data,
                                           uint32_t((char*)(&data->entries[dstStart]) - (char*)data),
                                           uint32_t((char*)(&data->entries[srcStart]) - (char*)data),
                                           count,
                                           /*zeroEmptied*/ true);
    }

    REALLY_INLINE /*static*/ void AtomListHelper::gcTrace(MMgc::GC* gc, LISTDATA** loc)
    {
        gc->TraceLocation(loc);
    }

    // ----------------------------

    REALLY_INLINE /*static*/ void WeakRefListHelper::wbData(const void* container, LISTDATA** address, LISTDATA* data)
    {
        MMgc::GC* const gc = data->gc();
        if (gc->IsPointerToGCPage(container))
        {
            WB(gc, gc->FindBeginningFast(container), address, data);
        }
        else
        {
            *address = data;
        }
    }

    REALLY_INLINE /*static*/ WeakRefListHelper::TYPE WeakRefListHelper::load(LISTDATA* data, uint32_t index)
    {
        AvmAssert(data != NULL);
        MMgc::GCWeakRef* weak = data->entries[index];
        return weak ? (TYPE)weak->get() : NULL;
    }

    REALLY_INLINE /*static*/ void WeakRefListHelper::store(LISTDATA* data, uint32_t index, TYPE value)
    {
        AvmAssert(data != NULL);
        MMgc::GCWeakRef* weak = value ? value->GetWeakRef() : NULL;
        WB(data->gc(), data, &data->entries[index], weak);
    }

    REALLY_INLINE /*static*/ void WeakRefListHelper::storeInEmpty(LISTDATA* data, uint32_t index, TYPE value)
    {
        AvmAssert(data != NULL);
        AvmAssert(data->entries[index] == 0);
        store(data, index, value);
    }

    REALLY_INLINE /*static*/ void WeakRefListHelper::clearRange(LISTDATA* data, uint32_t start, uint32_t count)
    {
        AvmAssert(count > 0);
        VMPI_memset(&data->entries[start], 0, count*sizeof(STORAGE));
    }

    REALLY_INLINE /*static*/ void WeakRefListHelper::moveRange(LISTDATA* data, uint32_t srcStart, uint32_t dstStart, uint32_t count)
    {
        AvmAssert(data != NULL);
        data->gc()->movePointersWithinBlock((void**)data,
                                           uint32_t((char*)(&data->entries[dstStart]) - (char*)data),
                                           uint32_t((char*)(&data->entries[srcStart]) - (char*)data),
                                           count,
                                           /*zeroEmptied*/ true);
    }

    REALLY_INLINE /*static*/ void WeakRefListHelper::gcTrace(MMgc::GC* gc, LISTDATA** loc)
    {
        gc->TraceLocation(loc);
    }

    // ----------------------------

    template<class T, class ListHelper>
    REALLY_INLINE bool ListImpl<T,ListHelper>::isEmpty() const
    {
        return m_data->len == 0;
    }

    template<class T, class ListHelper>
    REALLY_INLINE uint32_t ListImpl<T,ListHelper>::length() const
    {
        return m_data->len;
    }

    template<class T, class ListHelper>
    REALLY_INLINE uint32_t ListImpl<T,ListHelper>::capacity() const
    {
        return uint32_t((ListHelper::LISTDATA::getSize(m_data) - offsetof(typename ListHelper::LISTDATA, entries)) /
                sizeof(typename ListHelper::STORAGE));
    }

    template<class T, class ListHelper>
    REALLY_INLINE T ListImpl<T,ListHelper>::get(uint32_t index) const
    {
        AvmAssert(index < m_data->len);
        return ListHelper::load(m_data, index);
    }

    template<class T, class ListHelper>
    REALLY_INLINE void ListImpl<T,ListHelper>::replace(uint32_t index, T value)
    {
        AvmAssert(index < m_data->len);
        ListHelper::store(m_data, index, value);
    }

    template<class T, class ListHelper>
    REALLY_INLINE void ListImpl<T,ListHelper>::set(uint32_t index, T value)
    {
        // Yes, this is worth inlining, according to performance testing.
        if (index >= m_data->len)
        {
            ensureCapacityExtra(index, 1);
            set_length_guarded(index+1);
        }
        ListHelper::store(m_data, index, value);
    }

    template<class T, class ListHelper>
    REALLY_INLINE T ListImpl<T,ListHelper>::first() const
    {
        AvmAssert(m_data->len > 0);
        return ListHelper::load(m_data, 0);
    }

    template<class T, class ListHelper>
    REALLY_INLINE T ListImpl<T,ListHelper>::last() const
    {
        AvmAssert(m_data->len > 0);
        return ListHelper::load(m_data, m_data->len-1);
    }

    template<class T, class ListHelper>
    REALLY_INLINE T ListImpl<T,ListHelper>::removeFirst()
    {
        AvmAssert(!isEmpty());
        return removeAt(0);
    }

    template<class T, class ListHelper>
    REALLY_INLINE T ListImpl<T,ListHelper>::operator[](uint32_t index) const
    {
        AvmAssert(index < m_data->len);
        return ListHelper::load(m_data, index);
    }

    // Invariant: if cap + extra overflows uint32_t then this method never returns.
    template<class T, class ListHelper>
    REALLY_INLINE void ListImpl<T,ListHelper>::ensureCapacityExtra(uint32_t cap, uint32_t extra)
    {
        uint32_t const ncap = (cap > UINT32_T_MAX - extra) ?  // if true, cap + extra will overflow a uint32_t...
                              UINT32_T_MAX :                  // ...in that case, choose a size that will definitely fail in ensureCapacityImpl.
                              (cap + extra);
        if (ncap > capacity())
        {
            ensureCapacityImpl(ncap);
        }
    }

    template<class T, class ListHelper>
    REALLY_INLINE void ListImpl<T,ListHelper>::ensureCapacity(uint32_t cap)
    {
        AvmAssert(m_data != NULL);
        if (cap > capacity())
        {
            ensureCapacityImpl(cap);
        }
    }

    template<class T, class ListHelper>
    REALLY_INLINE uint64_t ListImpl<T,ListHelper>::bytesUsed() const
    {
        AvmAssert(m_data != NULL);
        return ListHelper::LISTDATA::getSize(m_data);
    }

    template<class T, class ListHelper>
    REALLY_INLINE /*static*/ typename ListHelper::LISTDATA* ListImpl<T,ListHelper>::allocData(MMgc::GC* gc, uint32_t cap)
    {
        typename ListHelper::LISTDATA* newData = ListHelper::LISTDATA::create(gc, cap);
        newData->len = 0;
        newData->set_gc(gc);
        return newData;
    }
    
    // ----------------------------


    template<class T>
    REALLY_INLINE GCList<T>::GCList(MMgc::GC* gc, uint32_t capacity, const TYPE* args)
        : m_list(gc, capacity, (MMgc::GCObject* const*)args)
    {
        MMGC_STATIC_ASSERT((TypeSniffer<T>::isGCObject::value == true ||
                            TypeSniffer<T>::isGCFinalizedObject::value == true ||
                            TypeSniffer<T>::isGCTraceableObject::value == true) &&
                            TypeSniffer<T>::isRCObject::value == false);
        MMGC_STATIC_ASSERT(TypeSniffer<T>::isNonPointer::value == true);
    }

    template<class T>
    REALLY_INLINE bool GCList<T>::isEmpty() const
    {
        return m_list.isEmpty();
    }

    template<class T>
    REALLY_INLINE uint32_t GCList<T>::length() const
    {
        return m_list.length();
    }

    template<class T>
    REALLY_INLINE uint32_t GCList<T>::capacity() const
    {
        return m_list.capacity();
    }

    template<class T>
    REALLY_INLINE typename GCList<T>::TYPE GCList<T>::get(uint32_t index) const
    {
        return reinterpret_cast<TYPE>(m_list.get(index));
    }

    template<class T>
    REALLY_INLINE typename GCList<T>::TYPE GCList<T>::first() const
    {
        return reinterpret_cast<TYPE>(m_list.first());
    }

    template<class T>
    REALLY_INLINE typename GCList<T>::TYPE GCList<T>::last() const
    {
        return reinterpret_cast<TYPE>(m_list.last());
    }

    template<class T>
    REALLY_INLINE void GCList<T>::replace(uint32_t index, TYPE value)
    {
        m_list.replace(index, reinterpret_cast<MMgc::GCObject*>(value));
    }

    template<class T>
    REALLY_INLINE void GCList<T>::set(uint32_t index, TYPE value)
    {
        m_list.set(index, reinterpret_cast<MMgc::GCObject*>(value));
    }

    template<class T>
    REALLY_INLINE void GCList<T>::add(TYPE value)
    {
        m_list.add(reinterpret_cast<MMgc::GCObject*>(value));
    }

    template<class T>
    REALLY_INLINE void GCList<T>::add(const GCList<T>& that)
    {
        m_list.add(that.m_list);
    }

    template<class T>
    REALLY_INLINE void GCList<T>::insert(uint32_t index, TYPE value, uint32_t count)
    {
        m_list.insert(index, reinterpret_cast<MMgc::GCObject*>(value), count);
    }

    template<class T>
    REALLY_INLINE void GCList<T>::insert(uint32_t index, const TYPE* args, uint32_t argc)
    {
        m_list.insert(index, args, argc);
    }

    template<class T>
    REALLY_INLINE void GCList<T>::splice(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, const TYPE* args)
    {
        m_list.splice(insertPoint, insertCount, deleteCount, args);
    }

    template<class T>
    REALLY_INLINE void GCList<T>::splice(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, const GCList<T>& args, uint32_t argsOffset)
    {
        m_list.splice(insertPoint, insertCount, deleteCount, args, argsOffset);
    }

    template<class T>
    REALLY_INLINE void GCList<T>::reverse()
    {
        m_list.reverse();
    }

    template<class T>
    REALLY_INLINE void GCList<T>::clear()
    {
        m_list.clear();
    }

    template<class T>
    REALLY_INLINE int32_t GCList<T>::indexOf(TYPE value) const
    {
        return m_list.indexOf(reinterpret_cast<MMgc::GCObject*>(value));
    }

    template<class T>
    REALLY_INLINE int32_t GCList<T>::lastIndexOf(TYPE value) const
    {
        return m_list.lastIndexOf(reinterpret_cast<MMgc::GCObject*>(value));
    }

    template<class T>
    REALLY_INLINE typename GCList<T>::TYPE GCList<T>::removeAt(uint32_t index)
    {
        return reinterpret_cast<TYPE>(m_list.removeAt(index));
    }

    template<class T>
    REALLY_INLINE typename GCList<T>::TYPE GCList<T>::removeFirst()
    {
        return reinterpret_cast<TYPE>(m_list.removeFirst());
    }

    template<class T>
    REALLY_INLINE typename GCList<T>::TYPE GCList<T>::removeLast()
    {
        return reinterpret_cast<TYPE>(m_list.removeLast());
    }

    template<class T>
    REALLY_INLINE typename GCList<T>::TYPE GCList<T>::operator[](uint32_t index) const
    {
        return reinterpret_cast<TYPE>(m_list[index]);
    }

    template<class T>
    REALLY_INLINE void GCList<T>::ensureCapacity(uint32_t cap)
    {
        m_list.ensureCapacity(cap);
    }

    template<class T>
    REALLY_INLINE uint64_t GCList<T>::bytesUsed() const
    {
        return m_list.bytesUsed();
    }

    template<class T>
    REALLY_INLINE void GCList<T>::destroy()
    {
        m_list.destroy();
    }

    template<class T>
    REALLY_INLINE bool GCList<T>::isDestroyed() const
    {
        return m_list.isDestroyed();
    }

    template<class T>
    REALLY_INLINE void GCList<T>::gcTrace(MMgc::GC* gc)
    {
        m_list.gcTrace(gc);
    }

    // ----------------------------

    template<class T>
    REALLY_INLINE RCList<T>::RCList(MMgc::GC* gc, uint32_t capacity, const TYPE* args)
        : m_list(gc, capacity, (MMgc::RCObject* const*)args)
    {
        MMGC_STATIC_ASSERT(TypeSniffer<T>::isRCObject::value == true);
        MMGC_STATIC_ASSERT(TypeSniffer<T>::isNonPointer::value == true);
    }

    template<class T>
    REALLY_INLINE bool RCList<T>::isEmpty() const
    {
        return m_list.isEmpty();
    }

    template<class T>
    REALLY_INLINE uint32_t RCList<T>::length() const
    {
        return m_list.length();
    }

    template<class T>
    REALLY_INLINE uint32_t RCList<T>::capacity() const
    {
        return m_list.capacity();
    }

    template<class T>
    REALLY_INLINE typename RCList<T>::TYPE RCList<T>::get(uint32_t index) const
    {
        return reinterpret_cast<TYPE>(m_list.get(index));
    }

    template<class T>
    REALLY_INLINE typename RCList<T>::TYPE RCList<T>::first() const
    {
        return reinterpret_cast<TYPE>(m_list.first());
    }

    template<class T>
    REALLY_INLINE typename RCList<T>::TYPE RCList<T>::last() const
    {
        return reinterpret_cast<TYPE>(m_list.last());
    }

    template<class T>
    REALLY_INLINE void RCList<T>::replace(uint32_t index, TYPE value)
    {
        m_list.replace(index, value);
    }

    template<class T>
    REALLY_INLINE void RCList<T>::set(uint32_t index, TYPE value)
    {
        m_list.set(index, value);
    }

    template<class T>
    REALLY_INLINE void RCList<T>::add(TYPE value)
    {
        m_list.add(value);
    }

    template<class T>
    REALLY_INLINE void RCList<T>::add(const RCList<T>& that)
    {
        m_list.add(that.m_list);
    }

    template<class T>
    REALLY_INLINE void RCList<T>::insert(uint32_t index, TYPE value, uint32_t count)
    {
        m_list.insert(index, value, count);
    }

    template<class T>
    REALLY_INLINE void RCList<T>::insert(uint32_t index, const TYPE* args, uint32_t argc)
    {
        m_list.insert(index, args, argc);
    }

    template<class T>
    REALLY_INLINE void RCList<T>::splice(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, const TYPE* args)
    {
        m_list.splice(insertPoint, insertCount, deleteCount, args);
    }

    template<class T>
    REALLY_INLINE void RCList<T>::splice(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, const RCList<T>& args, uint32_t argsOffset)
    {
        m_list.splice(insertPoint, insertCount, deleteCount, args, argsOffset);
    }

    template<class T>
    REALLY_INLINE void RCList<T>::reverse()
    {
        m_list.reverse();
    }

    template<class T>
    REALLY_INLINE void RCList<T>::clear()
    {
        m_list.clear();
    }

    template<class T>
    REALLY_INLINE int32_t RCList<T>::indexOf(TYPE value) const
    {
        return m_list.indexOf(value);
    }

    template<class T>
    REALLY_INLINE int32_t RCList<T>::lastIndexOf(TYPE value) const
    {
        return m_list.lastIndexOf(value);
    }

    template<class T>
    REALLY_INLINE typename RCList<T>::TYPE RCList<T>::removeAt(uint32_t index)
    {
        return reinterpret_cast<TYPE>(m_list.removeAt(index));
    }

    template<class T>
    REALLY_INLINE typename RCList<T>::TYPE RCList<T>::removeFirst()
    {
        return reinterpret_cast<TYPE>(m_list.removeFirst());
    }

    template<class T>
    REALLY_INLINE typename RCList<T>::TYPE RCList<T>::removeLast()
    {
        return reinterpret_cast<TYPE>(m_list.removeLast());
    }

    template<class T>
    REALLY_INLINE typename RCList<T>::TYPE RCList<T>::operator[](uint32_t index) const
    {
        return reinterpret_cast<TYPE>(m_list[index]);
    }

    template<class T>
    REALLY_INLINE void RCList<T>::ensureCapacity(uint32_t cap)
    {
        m_list.ensureCapacity(cap);
    }

    template<class T>
    REALLY_INLINE uint64_t RCList<T>::bytesUsed() const
    {
        return m_list.bytesUsed();
    }

    template<class T>
    REALLY_INLINE void RCList<T>::destroy()
    {
        m_list.destroy();
    }

    template<class T>
    REALLY_INLINE bool RCList<T>::isDestroyed() const
    {
        return m_list.isDestroyed();
    }

    template<class T>
    REALLY_INLINE void RCList<T>::gcTrace(MMgc::GC* gc)
    {
        m_list.gcTrace(gc);
    }
    
    
    // ----------------------------

    template<class T>
    REALLY_INLINE UnmanagedPointerList<T>::UnmanagedPointerList(MMgc::GC* gc, uint32_t capacity, const T* args)
        : m_list(gc, capacity, (const UnmanagedPointer*)args)
    {
        // We must not be a GC/RCObject.
        MMGC_STATIC_ASSERT(TypeSniffer<T>::isGCObject::value == false &&
                            TypeSniffer<T>::isGCFinalizedObject::value == false &&
                            TypeSniffer<T>::isRCObject::value == false &&
                            TypeSniffer<T>::isGCTraceableObject::value == false);

        // BUT we MUST be a pointer type.
        MMGC_STATIC_ASSERT(TypeSniffer<T>::isNonPointer::value == false);
    }

    template<class T>
    REALLY_INLINE bool UnmanagedPointerList<T>::isEmpty() const
    {
        return m_list.isEmpty();
    }

    template<class T>
    REALLY_INLINE uint32_t UnmanagedPointerList<T>::length() const
    {
        return m_list.length();
    }

    template<class T>
    REALLY_INLINE uint32_t UnmanagedPointerList<T>::capacity() const
    {
        return m_list.capacity();
    }

    template<class T>
    REALLY_INLINE T UnmanagedPointerList<T>::get(uint32_t index) const
    {
        return (T)m_list.get(index);
    }

    template<class T>
    REALLY_INLINE T UnmanagedPointerList<T>::first() const
    {
        return (T)m_list.first();
    }

    template<class T>
    REALLY_INLINE T UnmanagedPointerList<T>::last() const
    {
        return (T)m_list.last();
    }

    template<class T>
    REALLY_INLINE void UnmanagedPointerList<T>::replace(uint32_t index, T value)
    {
        m_list.replace(index, (UnmanagedPointer)value);
    }

    template<class T>
    REALLY_INLINE void UnmanagedPointerList<T>::set(uint32_t index, T value)
    {
        m_list.set(index, (UnmanagedPointer)value);
    }

    template<class T>
    REALLY_INLINE void UnmanagedPointerList<T>::add(T value)
    {
        m_list.add((UnmanagedPointer)value);
    }

    template<class T>
    REALLY_INLINE void UnmanagedPointerList<T>::add(const UnmanagedPointerList<T>& that)
    {
        m_list.add(that.m_list);
    }

    template<class T>
    REALLY_INLINE void UnmanagedPointerList<T>::insert(uint32_t index, T value, uint32_t count)
    {
        m_list.insert(index, (UnmanagedPointer)value, count);
    }

    template<class T>
    REALLY_INLINE void UnmanagedPointerList<T>::insert(uint32_t index, const T* args, uint32_t argc)
    {
        m_list.insert(index, args, argc);
    }

    template<class T>
    REALLY_INLINE void UnmanagedPointerList<T>::splice(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, const T* args)
    {
        m_list.splice(insertPoint, insertCount, deleteCount, args);
    }

    template<class T>
    REALLY_INLINE void UnmanagedPointerList<T>::splice(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, const UnmanagedPointerList<T>& args, uint32_t argsOffset)
    {
        m_list.splice(insertPoint, insertCount, deleteCount, args, argsOffset);
    }

    template<class T>
    REALLY_INLINE void UnmanagedPointerList<T>::reverse()
    {
        m_list.reverse();
    }

    template<class T>
    REALLY_INLINE void UnmanagedPointerList<T>::clear()
    {
        m_list.clear();
    }

    template<class T>
    REALLY_INLINE int32_t UnmanagedPointerList<T>::indexOf(T value) const
    {
        return m_list.indexOf((UnmanagedPointer)value);
    }

    template<class T>
    REALLY_INLINE int32_t UnmanagedPointerList<T>::lastIndexOf(T value) const
    {
        return m_list.lastIndexOf((UnmanagedPointer)value);
    }

    template<class T>
    REALLY_INLINE T UnmanagedPointerList<T>::removeAt(uint32_t index)
    {
        return (T)m_list.removeAt(index);
    }

    template<class T>
    REALLY_INLINE T UnmanagedPointerList<T>::removeFirst()
    {
        return (T)m_list.removeFirst();
    }

    template<class T>
    REALLY_INLINE T UnmanagedPointerList<T>::removeLast()
    {
        return (T)m_list.removeLast();
    }

    template<class T>
    REALLY_INLINE T UnmanagedPointerList<T>::operator[](uint32_t index) const
    {
        return (T)m_list[index];
    }

    template<class T>
    REALLY_INLINE void UnmanagedPointerList<T>::ensureCapacity(uint32_t cap)
    {
        m_list.ensureCapacity(cap);
    }

    template<class T>
    REALLY_INLINE uint64_t UnmanagedPointerList<T>::bytesUsed() const
    {
        return m_list.bytesUsed();
    }

    template<class T>
    REALLY_INLINE void UnmanagedPointerList<T>::destroy()
    {
        m_list.destroy();
    }

    template<class T>
    REALLY_INLINE bool UnmanagedPointerList<T>::isDestroyed() const
    {
        return m_list.isDestroyed();
    }

    template<class T>
    REALLY_INLINE void UnmanagedPointerList<T>::gcTrace(MMgc::GC* gc)
    {
        m_list.gcTrace(gc);
    }
    
    // ----------------------------

    template<class T>
    REALLY_INLINE WeakRefList<T>::WeakRefList(MMgc::GC* gc, uint32_t capacity, const TYPE* args)
        : m_list(gc, capacity, (MMgc::GCObject* const*)args)
    {
        MMGC_STATIC_ASSERT(TypeSniffer<T>::isGCObject::value ||
                            TypeSniffer<T>::isGCFinalizedObject::value ||
                            TypeSniffer<T>::isRCObject::value ||
                            TypeSniffer<T>::isGCTraceableObject::value);
        MMGC_STATIC_ASSERT(TypeSniffer<T>::isNonPointer::value == true);
    }

    template<class T>
    REALLY_INLINE bool WeakRefList<T>::isEmpty() const
    {
        return m_list.isEmpty();
    }

    template<class T>
    REALLY_INLINE uint32_t WeakRefList<T>::length() const
    {
        return m_list.length();
    }

    template<class T>
    REALLY_INLINE uint32_t WeakRefList<T>::capacity() const
    {
        return m_list.capacity();
    }

    template<class T>
    REALLY_INLINE typename WeakRefList<T>::TYPE WeakRefList<T>::get(uint32_t index) const
    {
        return reinterpret_cast<TYPE>(m_list.get(index));
    }

    template<class T>
    REALLY_INLINE typename WeakRefList<T>::TYPE WeakRefList<T>::first() const
    {
        return reinterpret_cast<TYPE>(m_list.first());
    }

    template<class T>
    REALLY_INLINE typename WeakRefList<T>::TYPE WeakRefList<T>::last() const
    {
        return reinterpret_cast<TYPE>(m_list.last());
    }

    template<class T>
    REALLY_INLINE void WeakRefList<T>::replace(uint32_t index, TYPE value)
    {
        m_list.replace(index, to_gc(value));
    }

    template<class T>
    REALLY_INLINE void WeakRefList<T>::set(uint32_t index, TYPE value)
    {
        m_list.set(index, reinterpret_cast<MMgc::GCObject*>(value));
    }

    template<class T>
    REALLY_INLINE void WeakRefList<T>::add(TYPE value)
    {
        m_list.add(reinterpret_cast<MMgc::GCObject*>(value));
    }

    template<class T>
    REALLY_INLINE void WeakRefList<T>::add(const WeakRefList<T>& that)
    {
        m_list.add(that.m_list);
    }

    template<class T>
    REALLY_INLINE void WeakRefList<T>::insert(uint32_t index, TYPE value, uint32_t count)
    {
        m_list.insert(index, reinterpret_cast<MMgc::GCObject*>(value), count);
    }

    template<class T>
    REALLY_INLINE void WeakRefList<T>::insert(uint32_t index, const TYPE* args, uint32_t argc)
    {
        m_list.insert(index, args, argc);
    }

    template<class T>
    REALLY_INLINE void WeakRefList<T>::splice(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, const TYPE* args)
    {
        m_list.splice(insertPoint, insertCount, deleteCount, args);
    }

    template<class T>
    REALLY_INLINE void WeakRefList<T>::splice(uint32_t insertPoint, uint32_t insertCount, uint32_t deleteCount, const WeakRefList<T>& args, uint32_t argsOffset)
    {
        m_list.splice(insertPoint, insertCount, deleteCount, args, argsOffset);
    }

    template<class T>
    REALLY_INLINE void WeakRefList<T>::reverse()
    {
        m_list.reverse();
    }

    template<class T>
    REALLY_INLINE void WeakRefList<T>::clear()
    {
        m_list.clear();
    }

    template<class T>
    REALLY_INLINE int32_t WeakRefList<T>::indexOf(TYPE value) const
    {
        return m_list.indexOf(reinterpret_cast<MMgc::GCObject*>(value));
    }

    template<class T>
    REALLY_INLINE int32_t WeakRefList<T>::lastIndexOf(TYPE value) const
    {
        return m_list.lastIndexOf(reinterpret_cast<MMgc::GCObject*>(value));
    }

    template<class T>
    REALLY_INLINE typename WeakRefList<T>::TYPE WeakRefList<T>::removeAt(uint32_t index)
    {
        return reinterpret_cast<TYPE>(m_list.removeAt(index));
    }

    template<class T>
    REALLY_INLINE typename WeakRefList<T>::TYPE WeakRefList<T>::removeFirst()
    {
        return reinterpret_cast<TYPE>(m_list.removeFirst());
    }

    template<class T>
    REALLY_INLINE typename WeakRefList<T>::TYPE WeakRefList<T>::removeLast()
    {
        return reinterpret_cast<TYPE>(m_list.removeLast());
    }

    template<class T>
    REALLY_INLINE typename WeakRefList<T>::TYPE WeakRefList<T>::operator[](uint32_t index) const
    {
        return reinterpret_cast<TYPE>(m_list[index]);
    }

    template<class T>
    REALLY_INLINE void WeakRefList<T>::ensureCapacity(uint32_t cap)
    {
        m_list.ensureCapacity(cap);
    }

    template<class T>
    REALLY_INLINE uint64_t WeakRefList<T>::bytesUsed() const
    {
        return m_list.bytesUsed();
    }

    template<class T>
    REALLY_INLINE void WeakRefList<T>::destroy()
    {
        m_list.destroy();
    }

    template<class T>
    REALLY_INLINE bool WeakRefList<T>::isDestroyed() const
    {
        return m_list.isDestroyed();
    }

    template<class T>
    REALLY_INLINE uint32_t WeakRefList<T>::removeCollectedItems()
    {
        return m_list.removeNullItems();
    }

    template<class T>
    REALLY_INLINE void WeakRefList<T>::gcTrace(MMgc::GC* gc)
    {
        m_list.gcTrace(gc);
    }

    // ----------------------------

    template<class T, uintptr_t align>
    REALLY_INLINE DataList<T, align>::DataList(MMgc::GC* gc, uint32_t capacity, const T* args)
        : ListImpl< T, DataListHelper<T, align> >(gc, capacity, args)
    {
        // We must not be a pointer type.
        MMGC_STATIC_ASSERT(TypeSniffer<T>::isNonPointer::value == true);

        // If you make a NonGCObject list that holds GC/RCObjects, you are almost
        // certainly making a mistake. (Redundant to the above check, but left in
        // for emphasis.)
        MMGC_STATIC_ASSERT(TypeSniffer<T>::isGCObject::value == false &&
                           TypeSniffer<T>::isGCFinalizedObject::value == false &&
                           TypeSniffer<T>::isRCObject::value == false &&
                           TypeSniffer<T>::isGCTraceableObject::value == false);
    }

    // ----------------------------

    template<class T, uintptr_t align>
    REALLY_INLINE DataListAccessor<T, align>::DataListAccessor(DataList<T, align>* v) : m_list(v)
    {
    }
    
    template<class T, uintptr_t align>
    REALLY_INLINE T* DataListAccessor<T, align>::addr()
    {
        if (m_list == NULL)
            return (T*)NULL;
        if (align == 0)
            return m_list->m_data->entries;
        return (T*)((uintptr_t(m_list->m_data->entries) + (align-1)) & ~(align-1));
    }

    template<class T, uintptr_t align>
    REALLY_INLINE uint32_t DataListAccessor<T, align>::length()
    {
        return (m_list != NULL) ? m_list->length() : 0;
    }
    
    // ----------------------------

    template<class T>
    REALLY_INLINE HeapList<T>::HeapList(MMgc::GC* gc, uint32_t capacity, const typename T::TYPE* args)
        : list(gc, capacity, args)
    {
    }
}

#endif /* __avmplus_List_inlines__ */
