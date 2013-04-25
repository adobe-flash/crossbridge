/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_FixedHeapUtils__
#define __avmplus_FixedHeapUtils__

namespace avmplus
{

    /*
     * Utility collection, allocated on the stack or as a member field, passed by value.
     */
    template <typename TYPE>
    struct FixedHeapArray {
        TYPE* values;
        int length;
        FixedHeapArray(TYPE* values, int length);
        FixedHeapArray();
        
        bool allocate(int length, bool canFail = false);
        bool resize(int newLength, bool canFail = false);
        void initialize(const TYPE* source);
        void deallocate();
    };


#if defined(WIN32)  // really a MSVC 
#define TYPENAME_KLUDGE typename 
#else 
#define TYPENAME_KLUDGE
#endif


    // HashTable implementation borrowed from the FP runtime.
    template <typename K, typename V>
    class FixedHeapHashTable
    {
    public:
        enum { kDefaultSize = 127 };
        
        FixedHeapHashTable(size_t size = kDefaultSize);
        virtual ~FixedHeapHashTable();
        
        bool InsertItem (K key, V value);
        bool RemoveItem(K key);
        bool LookupItem(K key, V*value);
        int GetNumItems() const;
        void Clear();


        class Iterator 
        {
        public:
            virtual void each(TYPENAME_KLUDGE K key, TYPENAME_KLUDGE V item) = 0;
            virtual ~Iterator();
        };

        void ForEach(TYPENAME_KLUDGE Iterator& iterator);
        
        struct Bucket 
        {
            Bucket *next;
            K key;
            V value;
        };

        // helper methods so subclasses can build/tear down with their vtables (and hence allocator
        // functions in tact)
    protected:
        void Init(size_t size);
    public:
        void Destroy();


    protected:
        virtual Bucket *BucketAlloc();
        virtual void BucketFree(Bucket *b);
        
        virtual Bucket **BucketArrayAlloc(size_t size);
        virtual void BucketArrayFree(Bucket **buckets);
        
        virtual uintptr_t HashKey(K key) const;
        virtual bool KeysEqual(K key1, const K key2) const;
        virtual void DestroyItem(K /*key*/, V /*value*/);

        bool Expand();        

        Bucket **m_buckets;
        int m_numItems;
        int m_size;
#ifdef DEBUG
        bool m_inIteration;
#endif

    };


    


    class FixedHeapRCObject 
    {
    public:
        FixedHeapRCObject();
        
        int32_t IncrementRef();
        int32_t DecrementRef();
        int32_t RefCount();
        
        virtual void destroy() = 0;
        virtual ~FixedHeapRCObject();

        
    protected:
        vmbase::AtomicCounter32 m_rcount;
    };


    template<class T, int TAG = 0>
    class FixedHeapRef
    {
    public:
        
        explicit FixedHeapRef();
        explicit FixedHeapRef(T* valuePtr);
        FixedHeapRef(const FixedHeapRef& other);
        virtual ~FixedHeapRef();

        void operator=(const FixedHeapRef& other);
        void operator=(T* tNew);
        operator T*() const;
        T* operator ->() const;
        T* operator()() const;

    private:
        void set(T* tNew);
        T* value() const;
        T* const* location() const;
        T* t;
    };

}
#endif
