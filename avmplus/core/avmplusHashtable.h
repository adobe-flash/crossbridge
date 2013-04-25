/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_Hashtable__
#define __avmplus_Hashtable__


namespace avmplus
{
    /**
     * common base class for hashtable-like objects.
     */
    // NOTE NOTE NOTE NOTE
    // NOTE NOTE NOTE NOTE
    // NOTE NOTE NOTE NOTE
    // NOTE NOTE NOTE NOTE
    //
    // InlineHashtable is designed to never have its ctors or dtors called directly; it can't be allocated
    // either on the stack or the heap. For a standalong heap allocation, use HeapHashtable instead.
    //
    // The reason for this oddity is that InlineHashtable can be grafted "inline" to a ScriptObject, and
    // will be manually initialized and destroyed (but not via its ctor/dtor). We also don't want
    // to descend from any class that might force us to have a vtable (eg by way of a virtual dtor) because
    // we neither need one nor want to account for the extra pointer-sized space in every instance of ScriptObject.
    // (It's not large, but it adds up...)
    //
    // NOTE NOTE NOTE NOTE
    // NOTE NOTE NOTE NOTE
    // NOTE NOTE NOTE NOTE
    // NOTE NOTE NOTE NOTE
    class GC_CPP_EXACT(InlineHashtable, MMgc::GCInlineObject)
    {
        friend class HeapHashtable;
        friend class HeapHashtableRC;
        friend class WeakKeyHashtable;
        friend class WeakValueHashtable;
        friend class ScriptEnvMap;

    public:
        /** kDefaultCapacity must be a power of 2 */
        const static int kDefaultCapacity = 2;

        void initialize(MMgc::GC* gc, int capacity = kDefaultCapacity);
        void initializeWithDontEnumSupport(MMgc::GC* gc, int capacity = kDefaultCapacity);
        void destroy();
        void reset();

        uint32_t getSize() const;
        bool needsInitialize() const;

        bool getAtomPropertyIsEnumerable(Atom name) const;
        void setAtomPropertyIsEnumerable(Atom name, bool enumerable);

#ifdef DEBUGGER
        /**
         * Basically the same as AvmPlusScriptableObject::bytesUsed().
         */
        uint64_t bytesUsed() const;
#endif

        Atom get(Atom name) const;
        Atom remove(Atom name);

        // Similar to get(), but keys that are missing return EMPTY
        // rather than undefinedAtom. This is due to the regettable fact that undefinedAtom
        // is a legal value for a hashtable value (while EMPTY is not). (Note that we
        // can't actually use "EMPTY" here due to compiler issues, see isEmpty() for
        // explanation)
        Atom getNonEmpty(Atom name) const;

        // this is here for two reasons:
        // (1) it's conceptually nice to have "EMPTY" be protected.
        // (2) some recalcitrant compilers will fail to inline the constant and give
        // a linkage error if "EMPTY" is used outside this class. (We can heal this
        // by declaring a real definition of EMPTY in avmplusHashtable.cpp but then
        // some compilers generate actual loads for the constant, rather than
        // using immediate 0. Sigh...)
        static bool isEmpty(Atom a);

        bool contains(Atom name) const;

        /**
         * Adds a name/value pair to a hash table.  Automatically
         * grows the hash table if it is full.
         *
         * If the table can't be expanded because the number of elements
         * in the table is at the maximum allowable then one of two
         * things can happen:
         *
         *  - if toplevel is NULL then add() will call GCHeap::SignalObjectTooLarge,
         *    which will call GCHeap::Abort() and shut down the player
         *  - otherwise, add() will throw an Error object with tag kOutOfMemoryError
         *
         * Generally, toplevel should only be non-NULL if the operation originated
         * in AS3 code.
         */
        void add(Atom name, Atom value, Toplevel* toplevel = NULL);

        /**
         * The next()/keyAt()/valueAt() methods allow a caller to enumerate all entries in the table.
         * Note that the value passed in is not an index, per se; it's an integral value
         * that indicates that next entry, but is not guaranteed to increase
         * monotonically, or allow for random access: instead, it should be treated
         * as a "magic cookie". Proper usage is (conceptually) as follows:
         *
         *  for (int i = ht->next(0); i != 0; i = ht->next(i))
         *      key = ht->keyAt(i)
         *      value = ht->valueAt(i)
         *
         * i.e., get an initial value by calling next() with a value of 0;
         * continue calling next() until 0 is returned.
         *
         * All nonzero values of the next() int arg are reserved. Don't assume what they mean.
         */

        int next(int index);
        Atom keyAt(int index);
        Atom valueAt(int index);

    protected:

        /* See CPP for load factor variants. */
        bool isFull() const;

        /**
         * Called to grow the InlineHashtable, particularly by add.
         *
         * - Calculates the needed size for the new InlineHashtable
         *   (typically 2X the current size)
         * - Creates a new array of Atoms
         * - Rehashes the current table into the new one
         * - Deletes the old array of Atoms and sets the Atom
         *   pointer to our new array of Atoms
         *
         * If the table can't be expanded because the number of elements
         * in the table is at the maximum allowable then one of two
         * things can happen:
         *
         *  - if toplevel is NULL then add() will call GCHeap::SignalObjectTooLarge,
         *    which will call GCHeap::Abort() and shut down the player
         *  - otherwise, grow() will throw an Error object with tag kOutOfMemoryError
         *
         * Generally, toplevel should only be non-NULL if the operation originated
         * in AS3 code.
         */
        void grow(Toplevel* toplevel=NULL);

        void setHasDeletedItems();
        void clrHasDeletedItems();

    private:

        // -------------- private constants --------------
        /**
         * since identifiers are always interned strings, they can't be 0,
         * so we can use 0 as the empty value.
         */
        static const Atom EMPTY = atomNotFound;

        /** DELETED is stored as the key for deleted items */
        static const Atom DELETED = undefinedAtom;

        // kDontEnumBit is or'd into atoms to indicate that the property is {DontEnum},
        // and ALSO or'd into m_atomsAndFlags to indicate that the InlineHashtable as a whole supports DontEnum.
        static const uintptr_t kDontEnumBit     = 0x01;
        // kHasDeletedItems is or'd into m_atomsAndFlags (but not individual atoms)
        static const uintptr_t kHasDeletedItems = 0x02;
        // if kHasIterIndex is set, there are two extra Atoms allocated at the end of our
        // array (giving it capacity+2) that are used to cache the current value of next()
        static const uintptr_t kHasIterIndex    = 0x04;
        static const uintptr_t kAtomFlags       = (kDontEnumBit | kHasDeletedItems | kHasIterIndex);

        // if an iter index has a key of kIntptrType, we set this bit; this allows us to
        // continue iteration in the expected order even if the current item is deleted
        // during iteration. Since iter indices can't be negative or >2^31, this is a safe value.
        static const int kIterIndexIsIntptr = 0x80000000;

        //
        // "capacity" is the total number of atoms we allocate.
        // we use that capacity as name-value pairs, thus the maximum size at any time is always half the capacity.
        // for 32-bit builds, we want to limit the maximum number of entries to (1<<27)-1,
        // thus we the max capacity we need is (1<<28)-2. but since capacity is always limited
        // to a power of two, we'll actually limit capacity to 1<<27... which in turn will limit
        // maximum number of entries to (1<<26)-1. this has the downside of halving our maximum size,
        // but the upside of avoiding the need to check m_size for overflow on every put (we only
        // need to check capacity for overflow on every grow).
        //
        // (note: for consistency between 32 and 64-bit builds, we'll artificially limit 64-bit systems
        // to the same size, even though the m_size field can hold more.)
        //
        // How does this compare with pre-existing behavior on 32-bit systems?
        // theoretically, capacity could have been an allocation of 1<<32 == 4GB max (it's always a power of two)...
        // but some memory is of course used for other purposes, thus effectively 2GB max.
        // divide by sizeof(Atom) == 4 to find we had an actual max-capacity of 1<<29 entries.
        // So it is mathematically possible that a hashtable that was previously possible
        // will now prematurely run out of memory... but extraordinarily unlikely.
        //
        // (In practice, Win32 limits each process to 2GB, so we can halve the above for those systems,
        // thus "portable" ABC/SWF code could only rely on a max capacity of 1<<28 entries.)
        //
        //
        static const uint32_t MAX_CAPACITY = (1UL<<27);

        // -------------- private methods --------------

        InlineHashtable();
        // do NOT make this virtual; we want InlineHashtable to NOT have ANY virtual methods, not even a dtor
        ~InlineHashtable();

        class GC_CPP_EXACT(AtomContainer, MMgc::GCTraceableObject)
        {
        public:
            /**
             * In theory size should come from m_size but that isn't
             * accessible to us here so we just scan all the atoms
             * that could possibly be there based on GC::Size.
             */
            size_t count() const;
            GC_DATA_BEGIN(AtomContainer);
            Atom GC_ATOMS(atoms[1], "count()");
            GC_DATA_END(AtomContainer);
        };
    
        bool hasDeletedItems() const;
        void setCapacity(uint32_t cap);
        bool put(Atom name, Atom value);
        int rehash(const Atom *oldAtoms, int oldlen, Atom *newAtoms, int newlen) const;
        void throwFailureToGrow(AvmCore* core);
        void setAtoms(AtomContainer* atoms);
        AtomContainer* getAtomContainer() const;
        Atom* getAtoms();
        const Atom* getAtoms() const;
        AtomContainer* createAtoms(MMgc::GC* gc, int capacity) const;
        void freeAtoms();
        static uint32_t FindOneBit(uint32_t value);
        Atom removeDontEnumMask(Atom a) const;
        bool enumerable(Atom a) const;
        uint32_t getCapacity() const;
        bool hasDontEnumSupport() const;
        bool hasIterIndex() const;
        const Atom* expandForIterIndex();
        int publicIterIndexToRealIndex(int i);
        void removeKeyValuePairAtPublicIndex(int i);


        void deletePairAt(int i);

        /**
         * Finds the hash bucket corresponding to the key x
         * in the hash table starting at t, containing tLen
         * atoms.
         *
         * This is a quadratic probe, but we only hit even-numbered
         * slots since those hold keys.
         */
        int find(Atom x, const Atom *t, uint32_t tLen) const;
        int find(Stringp x, const Atom *t, uint32_t tLen) const;
        int find(Atom x) const;


    // ------------------------ DATA SECTION BEGIN
    private:
        GC_DATA_BEGIN(InlineHashtable);
        uintptr_t GC_POINTER(m_atomsAndFlags);  /** property hashtable, this has no GCMember<> on purpose, setAtoms contains the WB */
        GC_DATA_END(InlineHashtable);
    #ifdef AVMPLUS_64BIT
        // on 64-bit systems, padding will force us to 16 bytes here anyway, so let's just use unpacked ints
        uint32_t m_size;            // number of properties
        uint32_t m_logCapacity;     // (log2 of capacity) + 1
    #else
        uint32_t m_size:27;         // number of properties
        uint32_t m_logCapacity:5;   // (log2 of capacity) + 1 -- gives us enough space for 2^32 entries
    #endif
    // ------------------------ DATA SECTION END
    };

    class GC_CPP_EXACT(HeapHashtable, MMgc::GCFinalizedObject)
    {
        GC_DATA_BEGIN(HeapHashtable)
        
    protected:
        InlineHashtable GC_STRUCTURE(ht);
        
        GC_DATA_END(HeapHashtable)

    public:
        // Should be protected but there are members inlined in other GCObjects
        /**
         * initialize with a known capacity.  i.e. we can fit minSize
         * elements in without rehashing.
         * @param heap
         * @param capacity  # of logical slots
         */
        HeapHashtable(MMgc::GC* gc, int32_t capacity = InlineHashtable::kDefaultCapacity);


        static HeapHashtable* create(MMgc::GC* gc, int32_t capacity = InlineHashtable::kDefaultCapacity);
        virtual ~HeapHashtable();

        InlineHashtable* get_ht();

        void reset();
        uint32_t getSize() const;

        virtual int next(int index);
        Atom keyAt(int index);
        Atom valueAt(int index);

        virtual void add(Atom name, Atom value, Toplevel* toplevel=NULL);
        virtual Atom get(Atom name);
        virtual Atom remove(Atom name);
        virtual bool contains(Atom name) const;

        virtual bool weakKeys() const;
        virtual bool weakValues() const;

#ifdef DEBUGGER
    public:
        /**
         * Basically the same as AvmPlusScriptableObject::bytesUsed().
         */
        virtual uint64_t bytesUsed() const;
#endif
    };

    // Holds RCObject values, not Atom values.  Otherwise like HeapHashtable.
    class GC_CPP_EXACT(HeapHashtableRC, MMgc::GCFinalizedObject)
    {
        GC_DATA_BEGIN(HeapHashtableRC);
    private:
        InlineHashtable GC_STRUCTURE(ht);
        GC_DATA_END(HeapHashtableRC);

        HeapHashtableRC(MMgc::GC* gc, int32_t capacity = InlineHashtable::kDefaultCapacity);
    public:
        static HeapHashtableRC* create(MMgc::GC* gc, int32_t capacity = InlineHashtable::kDefaultCapacity);
        virtual ~HeapHashtableRC();

        void reset();
        uint32_t getSize() const;

        int next(int index);
        Atom keyAt(int index);
        MMgc::RCObject* valueAt(int index);

        void add(Atom name, MMgc::RCObject* value, Toplevel* toplevel=NULL);
        MMgc::RCObject* get(Atom name);
        MMgc::RCObject* remove(Atom name);
        bool contains(Atom name) const;

        // used by Flash
        size_t getAllocatedSize() const;

    private:
        Atom tagObject(MMgc::RCObject* obj);
        MMgc::RCObject* untagAtom(Atom a);
    };

    /**
     * If key is an object, weak refs are used
     */
    class GC_CPP_EXACT(WeakKeyHashtable, HeapHashtable)
    {
    public:
        // Should be private but there are members inlined in other GCObjects
        WeakKeyHashtable(MMgc::GC* _gc);

    public:
        static WeakKeyHashtable* create(MMgc::GC* gc);
        virtual int next(int index);

        virtual void add(Atom key, Atom value, Toplevel* toplevel=NULL);
        virtual Atom get(Atom key);
        virtual Atom remove(Atom key);
        virtual bool contains(Atom key) const;

        virtual bool weakKeys() const;
    private:
        Atom getKey(Atom key) const;
        void prune();
        
        GC_NO_DATA(WeakKeyHashtable);
    };

    /**
     * If value is an object, weak refs are used
     */
    class GC_CPP_EXACT(WeakValueHashtable, HeapHashtable)
    {
    public:
        // Should be private but there are members inlined in other GCObjects
        WeakValueHashtable(MMgc::GC* _gc);

    public:
        static WeakValueHashtable* create(MMgc::GC* gc);
        virtual void add(Atom key, Atom value, Toplevel* toplevel=NULL);
        virtual Atom get(Atom key);
        virtual Atom remove(Atom key);

        virtual bool weakValues() const;
    private:
        Atom getValue(Atom key, Atom value);
        void prune();
        
        GC_NO_DATA(WeakValueHashtable);
    };
}

#endif /* __avmplus_Hashtable__ */
