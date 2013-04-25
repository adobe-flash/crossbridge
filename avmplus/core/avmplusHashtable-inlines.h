/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Hashtable_inlines__
#define __avmplus_Hashtable_inlines__


namespace avmplus
{
    REALLY_INLINE InlineHashtable::InlineHashtable() :
        m_atomsAndFlags(0), m_size(0), m_logCapacity(0)
    {
        // nothing, here only for HeapHashtable, which explicitly calls initialize()
    }

    // do NOT make this virtual; we want InlineHashtable to NOT have ANY virtual methods, not even a dtor
    REALLY_INLINE InlineHashtable::~InlineHashtable()
    {
        // nothing, here only for HeapHashtable, which explicitly calls destroy()
    }

    REALLY_INLINE void InlineHashtable::initializeWithDontEnumSupport(MMgc::GC* gc, int capacity)
    {
        initialize(gc, capacity);
        m_atomsAndFlags |= kDontEnumBit;
    }

    REALLY_INLINE uint32_t InlineHashtable::getSize() const
    {
        return m_size;
    }

    REALLY_INLINE bool InlineHashtable::needsInitialize() const
    {
        return m_logCapacity == 0;
    }

    REALLY_INLINE uint32_t InlineHashtable::getCapacity() const
    {
        return m_logCapacity ? 1UL<<(m_logCapacity-1) : 0;
    }

    REALLY_INLINE bool InlineHashtable::hasDontEnumSupport() const
    {
        return (m_atomsAndFlags & kDontEnumBit) != 0;
    }

    REALLY_INLINE Atom InlineHashtable::removeDontEnumMask(Atom a) const
    {
        return Atom(uintptr_t(a) & ~(m_atomsAndFlags & kDontEnumBit));
    }

    REALLY_INLINE bool InlineHashtable::enumerable(Atom a) const
    {
        return a != EMPTY && a != DELETED && !(uintptr_t(a) & (m_atomsAndFlags & kDontEnumBit));
    }

    REALLY_INLINE bool InlineHashtable::hasIterIndex() const
    {
        return (m_atomsAndFlags & kHasIterIndex) != 0;
    }

#ifdef DEBUGGER
    REALLY_INLINE uint64_t InlineHashtable::bytesUsed() const
    {
        return (getCapacity() + (hasIterIndex() ? 2 : 0)) * sizeof(Atom);
    }
#endif

    REALLY_INLINE bool InlineHashtable::hasDeletedItems() const
    {
        return (m_atomsAndFlags & kHasDeletedItems) != 0;
    }

    REALLY_INLINE void InlineHashtable::reset()
    {
        MMgc::GC* gc = MMgc::GC::GetGC(getAtoms());
        destroy();
        initialize(gc);
    }

    REALLY_INLINE Atom InlineHashtable::getNonEmpty(Atom name) const
    {
        const Atom* const atoms = getAtoms();
        int const i = find(name, atoms, getCapacity());
        return isEmpty(atoms[i]) ? 0 : atoms[i+1];
    }

    REALLY_INLINE /*static*/ bool InlineHashtable::isEmpty(Atom a)
    {
        return a == 0; // can't use EMPTY unless you like link errors.
    }

    REALLY_INLINE bool InlineHashtable::contains(Atom name) const
    {
        const Atom* atoms = getAtoms();
        return removeDontEnumMask(atoms[find(name, atoms, getCapacity())]) == name;
    }

    REALLY_INLINE int InlineHashtable::find(Atom x) const
    {
        return find(x, getAtoms(), getCapacity());
    }

    REALLY_INLINE void InlineHashtable::setHasDeletedItems()
    {
        m_atomsAndFlags |= kHasDeletedItems;
    }

    REALLY_INLINE void InlineHashtable::clrHasDeletedItems()
    {
        m_atomsAndFlags &= ~kHasDeletedItems;
    }

    REALLY_INLINE InlineHashtable::AtomContainer* InlineHashtable::getAtomContainer() const
    {
        return (AtomContainer*)(m_atomsAndFlags & ~kAtomFlags);
    }

    REALLY_INLINE const Atom* InlineHashtable::getAtoms() const
    {
        return (const Atom*)getAtomContainer()->atoms;
    }

    REALLY_INLINE Atom* InlineHashtable::getAtoms()
    {
        return (Atom*)getAtomContainer()->atoms;
    }
    
    REALLY_INLINE size_t InlineHashtable::AtomContainer::count() const
    {
        size_t count = (MMgc::GC::Size(this) - sizeof(AtomContainer) + sizeof(Atom)) / sizeof(Atom);
        AvmAssert(count > 0);
        size_t powof2 = 1;
        // Power of two floor operation to only scan Atoms and not
        // potential iteration indices which aren't atoms.
        while((count >>= 1) != 1)
            powof2++;
        return size_t(1)<<powof2;
    }

    REALLY_INLINE InlineHashtable::AtomContainer* InlineHashtable::createAtoms(MMgc::GC* gc, int capacity) const
    {
        size_t extra = 0;
        if (capacity > 0)
            extra = MMgc::GCHeap::CheckForCallocSizeOverflow(capacity-1, sizeof(Atom));
        return new (gc, MMgc::kExact, extra) AtomContainer();
    }

    REALLY_INLINE void InlineHashtable::freeAtoms()
    {
        AtomContainer* atoms = getAtomContainer();
        m_atomsAndFlags = m_atomsAndFlags & kAtomFlags;  // Must clear the pointer bits to avoid a dangling pointer
        delete atoms;
    }

    // @todo -- move this mess to VMPI, see https://bugzilla.mozilla.org/show_bug.cgi?id=546354
#if defined(AVMPLUS_IA32) || defined(AVMPLUS_AMD64)
    REALLY_INLINE /*static*/ uint32_t InlineHashtable::FindOneBit(uint32_t value)
    {

    #ifndef __GNUC__
        #if defined(_MSC_VER) && defined(AVMPLUS_64BIT)
        unsigned long index;
        _BitScanReverse(&index, value);
        return (uint32_t)index;
        #elif defined(__SUNPRO_C)||defined(__SUNPRO_CC)
        for (int i=0; i < 32; i++)
            if (value & (1<<i))
                return i;
        // asm versions of this function are undefined if no bits are set
        AvmAssert(false);
        return 0;
        #else
        _asm
        {
            bsr eax,[value];
        }
        #endif
    #else
        // DBC - This gets rid of a compiler warning and matchs PPC results where value = 0
        register int    result = ~0;

        if (value)
        {
            asm (
                "bsr %1, %0"
                : "=r" (result)
                : "m"(value)
                );
        }
        return result;
    #endif
    }

#elif defined(AVMPLUS_PPC)

    REALLY_INLINE /*static*/ uint32_t InlineHashtable::FindOneBit(uint32_t value)
    {
        register int index;
        #ifdef __GNUC__
        asm ("cntlzw %0,%1" : "=r" (index) : "r" (value));
        #else
        register uint32_t in = value;
        asm { cntlzw index, in; }
        #endif
        return 31-index;
    }

#else // generic platform

    REALLY_INLINE /*static*/ uint32_t InlineHashtable::FindOneBit(uint32_t value)
    {
        for (int i=0; i < 32; i++)
            if (value & (1<<i))
                return i;
        // asm versions of this function are undefined if no bits are set
        AvmAssert(false);
        return 0;
    }

#endif

    REALLY_INLINE void InlineHashtable::setCapacity(uint32_t cap)
    {
        m_logCapacity = cap ? (FindOneBit(cap)+1) : 0;
        AvmAssert(getCapacity() == cap);
    }
    
    REALLY_INLINE HeapHashtableRC* HeapHashtableRC::create(MMgc::GC* gc, int32_t capacity /*= InlineHashtable::kDefaultCapacity*/)
    {
        return new (gc, MMgc::kExact) HeapHashtableRC(gc, capacity);
    }

    REALLY_INLINE HeapHashtableRC::HeapHashtableRC(MMgc::GC* gc, int32_t capacity /*= InlineHashtable::kDefaultCapacity*/)
    {
        ht.initialize(gc, capacity);
    }

    REALLY_INLINE void HeapHashtableRC::reset()
    {
        ht.reset();
    }

    REALLY_INLINE uint32_t HeapHashtableRC::getSize() const
    {
        return ht.getSize();
    }

    REALLY_INLINE int HeapHashtableRC::next(int index)
    {
        return ht.next(index);
    }

    REALLY_INLINE Atom HeapHashtableRC::keyAt(int index)
    {
        return ht.keyAt(index);
    }

    REALLY_INLINE MMgc::RCObject* HeapHashtableRC::valueAt(int index)
    {
        return untagAtom(ht.valueAt(index));
    }

    REALLY_INLINE void HeapHashtableRC::add(Atom name, MMgc::RCObject* value, Toplevel* toplevel /*=NULL*/)
    {
        ht.add(name, tagObject(value), toplevel);
    }

    REALLY_INLINE MMgc::RCObject* HeapHashtableRC::get(Atom name)
    {
        return untagAtom(ht.get(name));
    }

    REALLY_INLINE MMgc::RCObject* HeapHashtableRC::remove(Atom name)
    {
        return untagAtom(ht.remove(name));
    }

    REALLY_INLINE bool HeapHashtableRC::contains(Atom name) const
    {
        return ht.contains(name);
    }

    // used by Flash
    REALLY_INLINE size_t HeapHashtableRC::getAllocatedSize() const
    {
        return ht.getCapacity() * sizeof(Atom);
    }

    REALLY_INLINE Atom HeapHashtableRC::tagObject(MMgc::RCObject* obj)
    {
        return (Atom)obj | kObjectType;
    }

    REALLY_INLINE MMgc::RCObject* HeapHashtableRC::untagAtom(Atom a)
    {
        return (MMgc::RCObject*)atomPtr(a);
    }

    REALLY_INLINE HeapHashtable* HeapHashtable::create(MMgc::GC* gc, int32_t capacity /*= InlineHashtable::kDefaultCapacity*/)
    {
        return new (gc, MMgc::kExact) HeapHashtable(gc, capacity);
    }

    REALLY_INLINE HeapHashtable::HeapHashtable(MMgc::GC* gc, int32_t capacity /*= InlineHashtable::kDefaultCapacity*/)
    {
        ht.initialize(gc, capacity);
    }

    REALLY_INLINE InlineHashtable* HeapHashtable::get_ht()
    {
        return &ht;
    }

    REALLY_INLINE void HeapHashtable::reset()
    {
        ht.reset();
    }

    REALLY_INLINE uint32_t HeapHashtable::getSize() const
    {
        return ht.getSize();
    }

    REALLY_INLINE Atom HeapHashtable::keyAt(int index)
    {
        return ht.keyAt(index);
    }

    REALLY_INLINE Atom HeapHashtable::valueAt(int index)
    {
        return ht.valueAt(index);
    }

    REALLY_INLINE WeakKeyHashtable::WeakKeyHashtable(MMgc::GC* _gc) : HeapHashtable(_gc)
    {
    }
    
    REALLY_INLINE WeakKeyHashtable* WeakKeyHashtable::create(MMgc::GC* gc)
    {
        return new (gc, MMgc::kExact) WeakKeyHashtable(gc);
    }

    REALLY_INLINE WeakValueHashtable* WeakValueHashtable::create(MMgc::GC* gc)
    {
        return new (gc, MMgc::kExact) WeakValueHashtable(gc);
    }
    
    REALLY_INLINE WeakValueHashtable::WeakValueHashtable(MMgc::GC* _gc) : HeapHashtable(_gc)
    {
    }
}

#endif /* __avmplus_Hashtable_inlines__ */
