/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCHashtable__
#define __GCHashtable__

namespace MMgc
{
    /* Currently only simple types are supported as value in the hashtable.
     * 0 assigment needs to be valid for the value type (e.g. struct type are
     * not supported as 0 assigment will throw warnings for all the members of 
     * the struct not getting zero initialized)
     * 
     * See https://bugzilla.mozilla.org/show_bug.cgi?id=585694 for more info.
     */
    template <typename V>
    struct HashTableEntry {
        V value;
        const void * key;
    };
    
    class GCHashtableKeyHandler;
    class GCHashtableAllocHandler_new;
    /**
    * simplified version of avmplus hashtable
    */
    template <typename VAL = const void*, class KEYHANDLER = GCHashtableKeyHandler, class ALLOCHANDLER=GCHashtableAllocHandler_new>
    class GCHashtableBase
    {
    public:
        static uint32_t const kDefaultSize = 16;

        GCHashtableBase(uint32_t capacity = kDefaultSize);
        ~GCHashtableBase();

        void clear();

        REALLY_INLINE VAL get(const void* key) { return table[find(key, table, tableSize)].value; }
        REALLY_INLINE const void* get(intptr_t key) { return get((const void*)key); }
        VAL remove(const void* key, bool allowRehash=true);
        // updates value if present, adds and grows if necessary if not
        void put(const void* key, VAL value);
        REALLY_INLINE void add(const void* key, VAL value) { put(key, value); }
        REALLY_INLINE void add(intptr_t key, VAL value) { put((const void*)key, value); }
        REALLY_INLINE uint32_t count() const { return numValues - numDeleted; }

        int32_t nextIndex(int32_t index);
        const void* keyAt(int32_t index) const { return table[index].key; }
        VAL valueAt(int32_t index) const { return table[index].value; }

        // Useful to call this after a lot of removals with allowRehash==false
        void prune();

        class Iterator
        {
        public:
            Iterator(GCHashtableBase* _ht) : ht(_ht), index(-1)
            {
#ifdef _DEBUG
                // Bug 637993: iterators stack allocated to ensure clean up.
                GCAssert(IsAddressOnStack(this));
                ht->numIterators++;
#endif
            }

            ~Iterator()
            {
#ifdef _DEBUG
                ht->numIterators--;
#endif
            }

            const void* nextKey()
            {
                do {
                    index += 1;
                } while(index < (int32_t)ht->tableSize && ht->table[index].key <= GCHashtableBase::DELETED);

                return (index < (int32_t)ht->tableSize) ? ht->table[index].key : NULL;
            }

            VAL value()
            {
                GCAssert(ht->table[index].key != NULL);
                return ht->table[index].value;
            }

        private:
            GCHashtableBase* volatile ht;
            int32_t index;
        };

    private:
        typedef struct HashTableEntry<VAL> Entry;
        uint32_t find(const void* key, Entry * table, uint32_t tableSize);

        void grow(bool isRemoval);

        static const void* const DELETED;
        static Entry EMPTY[4];
     protected:
        Entry* table;
        uint32_t tableSize;     // capacity
        uint32_t numValues;     // size of table array
        uint32_t numDeleted;    // number of delete items
#ifdef _DEBUG
        uint32_t numIterators;  // number of active iterators
#endif
#ifdef MMGC_GCHASHTABLE_PROFILER
    public:
        uint64_t probes;
        uint64_t accesses;
#endif
    };

    // --------------------------------

    template <typename VAL, class KEYHANDLER, class ALLOCHANDLER>
    /*static*/ const void* const GCHashtableBase<VAL,KEYHANDLER,ALLOCHANDLER>::DELETED = (const void*)1;

    template <typename VAL, class KEYHANDLER, class ALLOCHANDLER>
    /*static*/ typename GCHashtableBase<VAL,KEYHANDLER,ALLOCHANDLER>::Entry GCHashtableBase<VAL,KEYHANDLER,ALLOCHANDLER>::EMPTY[4] = { {0,0}, {0,0}, {0,0}, {0,0} };
    
    // --------------------------------

    /*
        Why do we need two allocator options?

        On some platforms FixedMalloc is known to be faster than the system (VMPI) allocator,
        but really its about OOM: when we run out of memory we can shutdown in a
        leak proof manner by zapping the GCHeap; if we use system memory we can't do
        that (short of using a malloc zone). Also, it's about getting the right
        profiler data: we want the WeakRef hashtable to show up in the memory profile
        results.
    */
    class GCHashtableAllocHandler_VMPI
    {
    public:
        static void* alloc(size_t size, bool canFail);
        static void free(void* ptr);
    };

    class GCHashtableAllocHandler_new
    {
    public:
        static void* alloc(size_t size, bool canFail);
        static void free(void* ptr);
    };

    // --------------------------------

    class GCHashtableKeyHandler
    {
    public:
        REALLY_INLINE static uint32_t hash(const void* k)
        {
            return uint32_t((uintptr_t(k)>>3) ^ (uintptr_t(k)<<29)); // move the low 3 bits higher up since they're often 0
        }

        REALLY_INLINE static bool equal(const void* k1, const void* k2)
        {
            return k1 == k2;
        }
    };

    typedef GCHashtableBase<const void*, GCHashtableKeyHandler, GCHashtableAllocHandler_new> GCHashtable;
    typedef GCHashtableBase<const void*, GCHashtableKeyHandler, GCHashtableAllocHandler_VMPI> GCHashtable_VMPI;

    // --------------------------------

    class GCStringHashtableKeyHandler
    {
    public:
        REALLY_INLINE static uint32_t hash(const void* k)
        {
            uint32_t hash = 0;
            const char* s = (const char*)k;
            while (*s++)
            {
                hash = (hash >> 28) ^ (hash << 4) ^ ((uintptr_t)*s << ((uintptr_t)s & 0x3));
            }
            return hash;
        }

        REALLY_INLINE static bool equal(const void* k1, const void* k2)
        {
            if (k1 == k2)
                return true;
            if (k1 && k2)
                return VMPI_strcmp((const char*)k1, (const char*)k2) == 0;
            return false;
        }
    };

    typedef GCHashtableBase<const void*, GCStringHashtableKeyHandler, GCHashtableAllocHandler_new> GCStringHashtable;
    typedef GCHashtableBase<const void*, GCStringHashtableKeyHandler, GCHashtableAllocHandler_VMPI> GCStringHashtable_VMPI;
}

#endif

