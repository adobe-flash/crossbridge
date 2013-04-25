/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "avmplus.h"

using namespace MMgc;

// WARNING:
// WARNING:
// WARNING:
// Never do "MMgc::GC::GetGC(this)" in the HashTable object.  It is dynamically
// placed at the end of a ScriptObject following traits data which can extend
// greater than 4k off the starting pointer.  This will cause GetGC(this) to fail.
// WARNING:
// WARNING:
// WARNING:

namespace avmplus
{
    void InlineHashtable::initialize(GC *gc, int capacity)
    {
        MMGC_STATIC_ASSERT(sizeof(InlineHashtable) == (sizeof(void*) * 2));

        // isEmpty() requires this, so let's insure it...
        MMGC_STATIC_ASSERT(EMPTY == 0);

        capacity = MathUtils::nextPowerOfTwo(capacity);
        setCapacity(capacity*2);
        AvmAssert(getCapacity());
        MMGC_MEM_TYPE(gc->FindBeginningGuarded(this));
        uint32_t const cap = getCapacity() + (hasIterIndex() ? 2 : 0);
        setAtoms(createAtoms(gc, cap));
    }

    void InlineHashtable::destroy()
    {
        Atom* atoms = getAtoms();
        if(atoms) {
            // deliberately ignore the final two entries if hasIterIndex(), since they aren't Atoms
            AvmCore::decrementAtomRegion(atoms, getCapacity());
            freeAtoms();
        }
        m_atomsAndFlags = 0;
        m_size = 0;
        m_logCapacity = 0;
    }

    void InlineHashtable::setAtoms(InlineHashtable::AtomContainer* newAtoms)
    {
        GC *gc = GC::GetGC(newAtoms);
        uintptr_t newVal = uintptr_t(newAtoms) | (m_atomsAndFlags & kAtomFlags);
        WB(gc, gc->FindBeginningFast(this), &m_atomsAndFlags, (void*)newVal);
    }

    // Return true if the table slot was previously empty, i.e., we have added a
    // new entry.  Return false if we've simply updated the value of an existing entry.
    // If a new entry is added, the caller is responsible expanding the table to
    // maintain at least one free slot, and to prune any weak entries that are dead.

    bool InlineHashtable::put(Atom name, Atom value)
    {
        AvmAssert(name != EMPTY && value != EMPTY);
        AtomContainer* atomContainer = getAtomContainer();
        Atom* atoms = atomContainer->atoms;
        int i = find(name, atoms, getCapacity());
        GC *gc = GC::GetGC(atoms);
        if (removeDontEnumMask(atoms[i]) != name) {
            // Create new entry.
            AvmAssert(!isFull());
            //atoms[i] = name;
            WBATOM(gc, atomContainer, &atoms[i], name);
            m_size++;
            //atoms[i+1] = value;
            WBATOM(gc, atomContainer, &atoms[i+1], value);
            return true;
        } else {
            // Update existing entry.
            //atoms[i+1] = value;
            WBATOM(gc, atomContainer, &atoms[i+1], value);
            return false;
        }
    }

    Atom InlineHashtable::get(Atom name) const
    {
        int i;
        const Atom* atoms = getAtoms();
        return atoms[i = find(name, atoms, getCapacity())] == name ? atoms[i+1] : undefinedAtom;
    }

    int InlineHashtable::find(Stringp x, const Atom *t, uint32_t tLen) const
    {
        AvmAssert(x != NULL);
        return find(x->atom(), t, tLen);
    }

    int InlineHashtable::find(Atom x, const Atom *t, uint32_t m) const
    {
        uintptr_t mask = ~(m_atomsAndFlags & kDontEnumBit);
        x &= mask;

        #if 0 // debug code to print out the strings we're searching for
        static int debug =0;
        if (debug && AvmCore::isString(x))
        {
            Stringp s = AvmCore::atomToString(x);
            AvmDebugMsg (s->c_str(), false);
            AvmDebugMsg ("\n", false);
        }
        #endif

        uintptr_t bitmask = (m - 1) & ~0x1;

        AvmAssert(x != EMPTY && x != DELETED);
        // this is a quadratic probe but we only hit even numbered slots since those hold keys.
        int n = 7 << 1;
        #ifdef _DEBUG
        uint32_t loopCount = 0;
        #endif
        // Note: Mask off MSB to avoid negative indices.  Mask off bottom
        // 3 bits because it doesn't contribute to hash.  Double it
        // because names, values stored adjacently.
        uint32_t i = ((0x7FFFFFF8 & x)>>2) & bitmask;
        Atom k;
        while ((k=t[i]&mask) != x && k != EMPTY)
        {
            i = (i + (n += 2)) & bitmask;       // quadratic probe
            AvmAssert(loopCount++ < m);         // don't scan forever
        }
        AvmAssert(i <= ((m-1)&~0x1));
        return i;
    }

    void InlineHashtable::deletePairAt(int i)
    {
        Atom* atoms = getAtoms();
        // decrement refcount as necessary.
        AvmCore::atomWriteBarrier_dtor(&atoms[i]);
        AvmCore::atomWriteBarrier_dtor(&atoms[i+1]);
        // calls above set the slot to 0, we want DELETED
        atoms[i] = DELETED;
        atoms[i+1] = DELETED;
        setHasDeletedItems();
    }

    Atom InlineHashtable::remove(Atom name)
    {
        Atom* atoms = getAtoms();
        int i = find(name, atoms, getCapacity());
        Atom val = undefinedAtom;
        if (removeDontEnumMask(atoms[i]) == name)
        {
            val = atoms[i+1];
            deletePairAt(i);
        }
        return val;
    }

    int InlineHashtable::rehash(const Atom *oldAtoms, int oldlen, Atom *newAtoms, int newlen) const
    {
        int newSize = 0;
        for (int i=0, n=oldlen; i < n; i += 2)
        {
            Atom oldAtom;
            if ((oldAtom=oldAtoms[i]) != EMPTY && oldAtom != DELETED)
            {
                // inlined & simplified version of put()
                int j = find(oldAtom, newAtoms, newlen);
                newAtoms[j] = oldAtom;
                newAtoms[j+1] = oldAtoms[i+1];
                newSize++;
            }
        }

        return newSize;
    }

    /**
        * load factor is 0.75 so we're full if size >= M*0.75
        * where M = atoms.length/2<br>
        *     size >= M*3/4<br>
        *     4*size >= 3*M<br>
        *     4*size >= 3*atoms.length/2<br>
        *     8*size >= 3*atoms.length<br>
        *     size<<3 >= 3*atoms.length
        */
    /**
        * load factor is 0.9 so we're full if size >= M*0.9
        * where M = atoms.length/2<br>
        *     size >= M*9/10<br>
        *     10*size >= 9*M<br>
        *     10*size >= 9*atoms.length/2<br>
        *     20*size >= 9*atoms.length<br>
        */
    bool InlineHashtable::isFull() const
    {
        // This seems to work very well and the Intel compiler converts both the multiplies
        // into shift+add operations.
        return uint32_t((5*(m_size+1))) >= uint32_t(2*getCapacity()); // 0.80
        #if 0
        //return ((size<<3)+1) >= 3*getCapacity(); // 0.75
        //return ((40*size)+1) >= 19*getCapacity(); // 0.95
        //return ((40*size)+1) >= 18*getCapacity(); // 0.90
        //return ((40*size)+1) >= 17*getCapacity(); // 0.85
        //return ((40*size)+1) >= 15*getCapacity(); // 0.75
        //Edwins suggestion - if (size > max - max>>N)) grow  (N = 5, 4, 3, 2)
        //#define SHIFTFACTOR 4
        //NOT CORRECT
        //return ( ( size << SHIFTFACTOR ) + size > (getCapacity() << (SHIFTFACTOR-1)) );
        #endif
    }

    void InlineHashtable::add(Atom name, Atom value, Toplevel* toplevel)
    {
        if (put(name, value)) {
            if (isFull()) {
                grow(toplevel);
            }
        }
    }

    void InlineHashtable::grow(Toplevel* toplevel)
    {
        // grow the table by 2N+1
        //     new = 2*old+1 ; old == o.atoms.length/2
        //         = 2*(o.atoms.length/2)+1
        //         = o.atoms.length + 1
        // If we have deleted slots, we don't grow our HT because our rehash will clear
        // out spots for us.
        const uint32_t oldCapacity = getCapacity();
        const uint32_t newCapacity = hasDeletedItems() ? oldCapacity : MathUtils::nextPowerOfTwo(oldCapacity+1);
        if (newCapacity > MAX_CAPACITY)
        {
            if (toplevel != NULL)
                toplevel->throwError(kOutOfMemoryError);
            else
                GCHeap::SignalObjectTooLarge();
        }
        const Atom* atoms = getAtoms();
        GC* gc = GC::GetGC(atoms);
        MMGC_MEM_TYPE(gc->FindBeginningGuarded(this));
        const uint32_t newCapacityAlloc = newCapacity + (hasIterIndex() ? 2 : 0);
        AtomContainer* atomContainer = createAtoms(gc, newCapacityAlloc);
        Atom* newAtoms = atomContainer->atoms;
        // WE#2621977 -- the behavior of growing-during-enumeration has always been ill-defined,
        // but this change caused that to (effectively) terminate the enumeration. This is a hack:
        // if we have an iter index, copy it over to preserve the previous oddball behavior. (srj)
        if (m_atomsAndFlags & kHasIterIndex)
        {
            intptr_t* oldIterIndices = (intptr_t*)(atoms + oldCapacity);
            intptr_t* newIterIndices = (intptr_t*)(newAtoms + newCapacity);
            newIterIndices[0] = oldIterIndices[0];
            newIterIndices[1] = oldIterIndices[1];
        }
        m_size = rehash(atoms, oldCapacity, newAtoms, newCapacity);
        freeAtoms();
        setAtoms(atomContainer);
        setCapacity(newCapacity);
        clrHasDeletedItems();
    }

    // this function is slow -- however, for "normal", well-mannered ABC code, it should
    // be called only once per iteration, at the start. (For handcrafted ABC it could be
    // called repeatedly, which would dramatically slow such cases, but that's acceptable.)
    int InlineHashtable::publicIterIndexToRealIndex(int publicIndex)
    {
        // keep in mind that a "public" index goes from 1...N (0 is the start-and-finish value)
        AvmAssert(publicIndex > 0);

        // for "well-formed" (ie, generated with ASC or other reasonable compiler),
        // we should only end up calling this function once per iteration, with publicIndex == 1 ....
        // however, we don't assert here because it *is* legal to call it with other values
        // (in fact, our abcasm tests do so), but such usage is now dramatically slower than
        // it used to be.
        // AvmAssert(publicIndex == 1);

        const Atom* const atoms = getAtoms();
        int const cap = getCapacity();
        uintptr_t const dontEnumMask = (m_atomsAndFlags & kDontEnumBit);
        for (int i = 0; i < cap; i += 2)
        {
            Atom const a = atoms[i];

            // don't need to check against EMPTY and DELETED since they won't be kIntptrType
            MMGC_STATIC_ASSERT(atomKind(EMPTY) != kIntptrType);
            MMGC_STATIC_ASSERT(atomKind(DELETED) != kIntptrType);
            // don't need to check for dontEnumMask because it will make kIntptrType appear as kDoubleType
            MMGC_STATIC_ASSERT((kIntptrType | kDontEnumBit) != kIntptrType);

            if (atomKind(a) == kIntptrType)
            {
                if (--publicIndex == 0)
                    return i | kIterIndexIsIntptr;
            }
        }
        for (int i = 0; i < cap; i += 2)
        {
            Atom const a = atoms[i];
            if (a != EMPTY && a != DELETED && atomKind(a) != kIntptrType && !(uintptr_t(a) & dontEnumMask))
            {
                if (--publicIndex == 0)
                    return i;
            }
        }

        // well-formed code can get here if there are no enumerable keys in the table.
        // return a value that is always > getCapacity;
        // since all callers check vs. cap this neuters the result
        return cap+1;
    }

    const Atom* InlineHashtable::expandForIterIndex()
    {
        const Atom* atoms = getAtoms();
        int const cap = getCapacity();
        if (!hasIterIndex())
        {
            GC* gc = GC::GetGC(atoms);
            MMGC_MEM_TYPE(gc->FindBeginningGuarded(this));
            AtomContainer* atomContainer = createAtoms(gc, cap+2);
            Atom* newAtoms = atomContainer->atoms;
            // no need to rehash: size is the same as far as atoms are concerned
            VMPI_memcpy(newAtoms, atoms, cap*sizeof(Atom));
            freeAtoms();
            m_atomsAndFlags |= kHasIterIndex;
            setAtoms(atomContainer);
            return newAtoms;
        }
        return atoms;
    }

    // call this method using the previous value returned
    // by this method starting with 0, until 0 is returned.
    int InlineHashtable::next(int index)
    {
        // we need our iter-index fields to iterate.
        // since most objects don't ever iterate this way,
        // they aren't always preallocated... if we get here and
        // they aren't, reallocate first.

        const Atom* atoms = expandForIterIndex();
        AvmAssert(hasIterIndex()); // @todo
        int const cap = getCapacity();

        intptr_t* iterIndices = (intptr_t*)(atoms + cap);
        //
        // iterIndices[0] == the expected public index we receive as arg
        // iterIndices[1] == the real internal index that corresponds to it,
        //  with bit 31 set if the entry was an intptr.
        //
        int publicIndex = int(iterIndices[0]);
        int realIndex = int(iterIndices[1]);
        if (!index)
        {
            // if index == 0, we're starting the iteration; prime the pump by finding
            // the first enumerable int atom (or first enumerable non-int if we have no ints)
            // in this case we ignore whatever is currently in iterIndices
            publicIndex = 1;
            realIndex = publicIterIndexToRealIndex(1);
            if ((realIndex & ~kIterIndexIsIntptr) >= cap)
            {
                publicIndex = realIndex = 0;
                goto done;
            }
        }
        else
        {
            // we'll never return an index larger than cap, but malicious ABC could try to feed us one
            if (index > cap)
            {
                // the spec doesn't explicitly say this is an error, handle gracefully
                publicIndex = realIndex = 0;
                goto done;
            }

            if (publicIndex == 0 || publicIndex != index)
            {
                // if publicIndex == 0, it's an uninitialized value (eg post-grow()),and realIndex is garbage.
                //
                // if publicIndex is nonzero but doesn't match index, someone is passing
                // us an arbitrary value, rather than the value we
                // last returned; this won't happen with "normal" ABC but hand-assembled
                // ABC could do so, so be prepared to do something reasonable.
                publicIndex = index;
                realIndex = publicIterIndexToRealIndex(index);
                if ((realIndex & ~kIterIndexIsIntptr) >= cap)
                {
                    publicIndex = realIndex = 0;
                    goto done;
                }
            }
            else
            {
                // You'd think this assert would be appropriate, but you'd be wrong:
                // if code iterates over properties and marks them dontEnum,
                // this will not hold true
                // AvmAssert(realIndex == publicIterIndexToRealIndex(publicIndex));
            }

            // realIndex should now always point to a valid key
            AvmAssert((realIndex & ~kIterIndexIsIntptr) < cap);
            AvmAssert(!(realIndex & 1));

            // ok, advance to the next candidate... if we are currently on an int atom,
            // advance to the next int atom. if we run out, advance to non-ints. Note,
            // test kIterIndexIsIntptr in realIndex to determine the type,
            // since the value there could have been changed to DELETED since we
            // were last called...

            publicIndex += 1; // assume we'll find something
            uintptr_t dontEnumMask = (m_atomsAndFlags & kDontEnumBit);
            if (realIndex & kIterIndexIsIntptr)
            {
                realIndex &= ~kIterIndexIsIntptr;
                for (realIndex += 2; realIndex < cap; realIndex += 2)
                {
                    Atom const a = atoms[realIndex];
                    // don't need to check against EMPTY and DELETED since they won't be kIntptrType.
                    // don't need to check for dontEnumMask because it will make kIntptrType appear as kDoubleType
                    if (atomKind(a) == kIntptrType)
                    {
                        realIndex |= kIterIndexIsIntptr;
                        goto done;
                    }
                }
                // reset for search thru object list (-2 so we start at zero)
                realIndex = -2;
            }

            // No enumerable kIntptrType entries left, try the non-kIntptrType

            for (realIndex += 2; realIndex < cap; realIndex += 2)
            {
                Atom const a = atoms[realIndex];
                if (a != EMPTY && a != DELETED && atomKind(a) != kIntptrType && !(uintptr_t(a) & dontEnumMask))
                {
                    goto done;
                }
            }

            // oh well, nothing left
            publicIndex = realIndex = 0;
        }

    done:
        iterIndices[0] = publicIndex;
        iterIndices[1] = realIndex;
        return publicIndex;
    }

    Atom InlineHashtable::keyAt(int i)
    {
        // if we get here without this bit being set, someone called us without
        // calling next() first -- a no-no. We'll still work, but we will have
        // to search for the right index, which is slower.
        AvmAssert(hasIterIndex());
        AvmAssert(i > 0);

        Atom* const atoms = getAtoms();
        int const cap = getCapacity();
        intptr_t* const iterIndices = (intptr_t*)(atoms + cap);
        int realIndex = (hasIterIndex() && iterIndices[0] == i) ?
                            (int)iterIndices[1] :
                            publicIterIndexToRealIndex(i);
        realIndex &= ~kIterIndexIsIntptr;
        return realIndex < cap ? removeDontEnumMask(atoms[realIndex]) : nullStringAtom;
    }

    Atom InlineHashtable::valueAt(int i)
    {
        // if we get here without this bit being set, someone called us without
        // calling next() first -- a no-no. We'll still work, but we will have
        // to search for the right index, which is slower.
        AvmAssert(hasIterIndex());
        AvmAssert(i > 0);

        Atom* const atoms = getAtoms();
        int const cap = getCapacity();
        intptr_t* const iterIndices = (intptr_t*)(atoms + cap);
        int realIndex = (hasIterIndex() && iterIndices[0] == i) ?
                            (int)iterIndices[1] :
                            publicIterIndexToRealIndex(i);
        realIndex &= ~kIterIndexIsIntptr;
        realIndex += 1;
        return realIndex < cap ? atoms[realIndex] : undefinedAtom;
    }

    void InlineHashtable::removeKeyValuePairAtPublicIndex(int i)
    {
        AvmAssert(hasIterIndex());
        AvmAssert(i > 0);

        Atom* const atoms = getAtoms();
        int const cap = getCapacity();
        intptr_t* const iterIndices = (intptr_t*)(atoms + cap);
        int realIndex = (hasIterIndex() && iterIndices[0] == i) ?
                            (int)iterIndices[1] :
                            publicIterIndexToRealIndex(i);
        realIndex &= ~kIterIndexIsIntptr;
        if (realIndex < cap)
        {
            // decrement refcount as necessary.
            AvmCore::atomWriteBarrier_dtor(&atoms[realIndex]);
            AvmCore::atomWriteBarrier_dtor(&atoms[realIndex+1]);
            // calls above set the slot to 0, we want DELETED
            atoms[realIndex] = DELETED;
            atoms[realIndex+1] = DELETED;
            setHasDeletedItems();
        }
    }

    bool InlineHashtable::getAtomPropertyIsEnumerable(Atom name) const
    {
        if (hasDontEnumSupport())
        {
            const Atom* atoms = getAtoms();
            int i = find(name, atoms, getCapacity());
            if (Atom(atoms[i]&~kDontEnumBit) == name)
            {
                return (atoms[i]&kDontEnumBit) == 0;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return contains(name);
        }
    }

    void InlineHashtable::setAtomPropertyIsEnumerable(Atom name, bool enumerable)
    {
        if (hasDontEnumSupport())
        {
            Atom* atoms = getAtoms();
            int i = find(name, atoms, getCapacity());
            if (Atom(atoms[i]&~kDontEnumBit) == name)
            {
                atoms[i] = (atoms[i]&~kDontEnumBit) | (enumerable ? 0 : kDontEnumBit);
            }
        }
    }

    /*virtual*/ HeapHashtableRC::~HeapHashtableRC()
    {
        ht.destroy();
    }

    /*virtual*/ HeapHashtable::~HeapHashtable()
    {
        ht.destroy();
    }

    /*virtual*/ int HeapHashtable::next(int index)
    {
        return ht.next(index);
    }

    /*virtual*/ void HeapHashtable::add(Atom name, Atom value, Toplevel* toplevel /*=NULL*/)
    {
        ht.add(name, value, toplevel);
    }

    /*virtual*/ Atom HeapHashtable::get(Atom name)
    {
        return ht.get(name);
    }

    /*virtual*/ Atom HeapHashtable::remove(Atom name)
    {
        return ht.remove(name);
    }

    /*virtual*/ bool HeapHashtable::contains(Atom name) const
    {
        return ht.contains(name);
    }

    /*virtual*/ bool HeapHashtable::weakKeys() const
    {
        return false;
    }

    /*virtual*/ bool HeapHashtable::weakValues() const
    {
        return false;
    }

#ifdef DEBUGGER
    /*virtual*/ uint64_t HeapHashtable::bytesUsed() const
    {
        return GC::Size(this) + ht.bytesUsed();
    }
#endif

    /*virtual*/ Atom WeakKeyHashtable::get(Atom key)
    {
        return ht.get(getKey(key));
    }

    /*virtual*/ Atom WeakKeyHashtable::remove(Atom key)
    {
        return ht.remove(getKey(key));
    }

    /*virtual*/ bool WeakKeyHashtable::contains(Atom key) const
    {
        return ht.contains(getKey(key));
    }

    /*virtual*/ bool WeakKeyHashtable::weakKeys() const
    {
        return true;
    }

    Atom WeakKeyHashtable::getKey(Atom key) const
    {
        // this gets a weak ref number, ie double keys, okay I guess
        if(AvmCore::isPointer(key)) {
            // Don't know if key is GCObject or GCFinalizedObject so can't go via the
            // GetWeakRef methods on those classes; go directly to GC
            GCWeakRef *weakRef = GC::GetWeakRef(atomPtr(key));
            key = AvmCore::genericObjectToAtom(weakRef);
        }
        return key;
    }

    /*virtual*/ void WeakKeyHashtable::add(Atom key, Atom value, Toplevel* toplevel)
    {
        if (ht.put(getKey(key), value)) {
            if (ht.isFull()) {
                prune();
                ht.grow(toplevel);
            }
        }
    }

    void WeakKeyHashtable::prune()
    {
        Atom* atoms = ht.getAtoms();
        for(int i=0, n=ht.getCapacity(); i<n; i+=2) {
            if(AvmCore::isGenericObject(atoms[i])) {
                GCWeakRef *ref = (GCWeakRef*)AvmCore::atomToGenericObject(atoms[i]);
                if(ref && ref->isNull()) {
                    ht.deletePairAt(i);
                }
            }
        }
    }

    /*virtual*/ int WeakKeyHashtable::next(int index)
    {
        for (;;)
        {
            index = ht.next(index);
            if (index != 0)
            {
                Atom const a = ht.keyAt(index);
                if (AvmCore::isGenericObject(a))
                {
                    GCWeakRef* weakRef = (GCWeakRef*)AvmCore::atomToGenericObject(a);
                    if (!weakRef->get())
                    {
                        ht.removeKeyValuePairAtPublicIndex(index);
                        continue;
                    }
                }
            }
            return index;
        }
    }

    Atom WeakValueHashtable::getValue(Atom key, Atom value)
    {
        if(AvmCore::isGenericObject(value)) {
            GCWeakRef *wr = (GCWeakRef*)AvmCore::atomToGenericObject(value);
            if(!wr->isNull()) {
                // Note wr could be a pointer to a double, that's what this is for.
                // Use 'peek' to avoid marking the object unless it actually is an
                // RCObject.
                Atom* atoms = ht.getAtoms();
                if(GC::GetGC(atoms)->IsRCObject(wr->peek())) {
                    union {
                        GCObject* o;
                        AvmPlusScriptableObject* so;
                    };
                    o = wr->get();
                    value = so->toAtom();
                } else {
                    AvmAssert(false);
                }
            } else {
                remove(key);
                value = undefinedAtom;
            }
        }
        return value;
    }

    /*virtual*/ void WeakValueHashtable::add(Atom key, Atom value, Toplevel* toplevel)
    {
        if(AvmCore::isPointer(value)) {
            // Don't know if value is GCObject or GCFinalizedObject so can't go via the
            // GetWeakRef methods on those classes; go directly to GC
            GCWeakRef *wf = GC::GetWeakRef(atomPtr(value));
            value = AvmCore::genericObjectToAtom(wf);
        }
        if (ht.put(key, value)) {
            if (ht.isFull()) {
                prune();
                ht.grow(toplevel);
            }
        }
    }

    /*virtual*/ Atom WeakValueHashtable::get(Atom key)
    {
        return getValue(key, ht.get(key));
    }

    /*virtual*/ Atom WeakValueHashtable::remove(Atom key)
    {
        return getValue(key, ht.remove(key));
    }

    /*virtual*/ bool WeakValueHashtable::weakValues() const
    {
        return true;
    }

    void WeakValueHashtable::prune()
    {
        Atom* atoms = ht.getAtoms();
        for(int i=0, n=ht.getCapacity(); i<n; i+=2) {
            if(AvmCore::isPointer(atoms[i+1])) {
                GCWeakRef *ref = (GCWeakRef*)atomPtr(atoms[i+1]);
                if(ref && ref->isNull()) {
                    ht.deletePairAt(i);
                }
            }
        }
    }
}
