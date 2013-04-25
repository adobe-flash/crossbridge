/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_FixedHeapUtils_inlines__
#define __avmplus_FixedHeapUtils_inlines__

namespace avmplus
{

    template <typename TYPE>
    FixedHeapArray<TYPE>::FixedHeapArray(TYPE* values, int length) : values(values), length(length)
    {
    }
    
    template <typename TYPE>
    FixedHeapArray<TYPE>::FixedHeapArray(): values(NULL), length(0)
    {
    }


    template <typename TYPE>
    bool FixedHeapArray<TYPE>::allocate(int length, bool canFail)
    {
        AvmAssert(values == NULL);
        this->length = length;
        this->values = mmfx_new_array_opt(TYPE, length, canFail ? MMgc::kCanFail : MMgc::kNone);
        return this->values != NULL;
    }

    template <typename TYPE>
    bool FixedHeapArray<TYPE>::resize(int newLength, bool canFail) {
        TYPE* oldValues = this->values;
        this->values = mmfx_new_array_opt(TYPE, newLength, canFail ? MMgc::kCanFail : MMgc::kNone);
        if (canFail && this->values == NULL)
            return false;

        for (int i=0; i<this->length; i++) {
            this->values[i] = oldValues[i];
        }
        this->length = newLength;

        if (oldValues != NULL) // if resize used on uninitialized array
            mmfx_delete_array(oldValues);

        return true;
    }

    template <typename TYPE>
    void FixedHeapArray<TYPE>::initialize(const TYPE* source) {
        VMPI_memcpy(values, source, length);
    }
    
    template <typename TYPE>
    void FixedHeapArray<TYPE>::deallocate() 
    {
        if (values) {
            mmfx_delete_array(values);
            values = NULL;
            length = 0;
        }
    }


    template<class T, int TAG>
    /*explicit*/ FixedHeapRef<T, TAG>::FixedHeapRef(): t(NULL)
    {
    }
    
    template<class T, int TAG>
    /*explicit*/ FixedHeapRef<T, TAG>::FixedHeapRef(T* valuePtr): t(NULL)
    {
        set(valuePtr);
    }
    
    template<class T, int TAG>
    FixedHeapRef<T, TAG>::FixedHeapRef(const FixedHeapRef& other): t(NULL)
    {
        set(other.t);
    }
    
    template<class T, int TAG>
    /*virtual*/ FixedHeapRef<T, TAG>::~FixedHeapRef()
    {
        set(NULL);
    }
    
    template<class T, int TAG>
    void FixedHeapRef<T, TAG>::operator=(const FixedHeapRef& other)
    {
        set(other.t);
    }
        
    template<class T, int TAG>
    void FixedHeapRef<T, TAG>::operator=(T* tNew)
    {
        set(tNew);
    }
    
    template<class T, int TAG>
    FixedHeapRef<T, TAG>::operator T*() const
    {
        return t;
    }
        
    template<class T, int TAG>
    T* FixedHeapRef<T, TAG>::operator ->() const
    {
        return t;
    }
    
    template<class T, int TAG>
    T* FixedHeapRef<T, TAG>::operator()() const
    {
        return t;
    }


    template<class T, int TAG>
    void FixedHeapRef<T, TAG>::set(T* tNew)
    {
        if (tNew) {
            int32_t count = tNew->IncrementRef();
            if (TAG > 0) 
                AvmLog("incr %p tag %d to %d\n", (T*)tNew, TAG, count);
        }
        T* tOld = this->t;
        // inc/dec membars must force ordering, so that when t == tNew, rc doesn't go to zero 
        this->t = tNew;
        if (tOld) {
            int32_t count = tOld->DecrementRef();
            if (TAG > 0) 
                AvmLog("decr %p tag %d to %d\n", (T*)tOld, TAG, count);
        }
    }

    template <typename K, typename V>
    FixedHeapHashTable<K,V>::FixedHeapHashTable(size_t size) 
    {
        Init(size);
    }
	
    template <typename K, typename V>
    void FixedHeapHashTable<K,V>::Init(size_t size)
    {
#ifdef DEBUG
        m_inIteration = false;
#endif
        m_buckets = size > 0 ? BucketArrayAlloc(size) : NULL;
        if (m_buckets) {
			memset(m_buckets, 0, size * sizeof(Bucket*));
			m_size = (int) size;
        } else {
            m_size = 0;
        }
        m_numItems = 0;
    }
	
    template <typename K, typename V>
    FixedHeapHashTable<K,V>::~FixedHeapHashTable()
    {
        Destroy();
    }
	
    template <typename K, typename V>
    void FixedHeapHashTable<K,V>::Destroy()
    {
        AvmAssert(!m_inIteration);
        if (m_buckets != NULL) { // idempotent
            Clear();
            BucketArrayFree(m_buckets);
            m_buckets = NULL;
        }
    }
	
	
    template <typename K, typename V>
    void FixedHeapHashTable<K,V>::DestroyItem(K, V)
    {
        // Override me if needed.
    }
	
    template <typename K, typename V>
    void FixedHeapHashTable<K,V>::Clear()
    {
        if (m_numItems) {
            for (int i=0; i<m_size; i++) {
                while (m_buckets[i]) {
					Bucket *bucket = m_buckets[i];
					m_buckets[i] = m_buckets[i]->next;
					DestroyItem(bucket->key, bucket->value);
					BucketFree(bucket);
                }
            }
            m_numItems = 0;
        }
    }
	
    template <typename K, typename V>
    bool FixedHeapHashTable<K,V>::InsertItem (K key, V value)
    {
        AvmAssert(!m_inIteration);
        if (!m_buckets) {
            return false;
        }
        
        int index = HashKey(key) % m_size;
        
        // Find any existing item
        Bucket *bucket = m_buckets[index];
        while (bucket) {
            if (KeysEqual(bucket->key, key)) {
                break;
            }
            bucket = bucket->next;
        }
        
        // If no existing bucket, create a new one.
        if (!bucket) {
            bucket = BucketAlloc();
            if ( !bucket )
                return false;
            bucket->next = m_buckets[index];
            m_buckets[index] = bucket;
            m_numItems++;
        } else {
            DestroyItem(bucket->key, bucket->value);
        }
        bucket->key = key;
        bucket->value = value;
        
        if (m_numItems >= m_size / 2) {
            Expand();
        }
        
        return true;
    }
    
    template <typename K, typename V>
    bool FixedHeapHashTable<K,V>::LookupItem(K key, V* value)
    {
        if (!m_buckets) {
            return false;
        }
        
        Bucket *bucket = m_buckets[HashKey(key) % m_size];
        while (bucket) {
            if (KeysEqual(bucket->key, key)) {
                *value = bucket->value;
                return true;
            }
            bucket = bucket->next;
        }
        return false;
    }
    
    template <typename K, typename V>
    int FixedHeapHashTable<K,V>::GetNumItems() const
    {
		return m_numItems;
    }
	
    template <typename K, typename V>
    bool FixedHeapHashTable<K,V>::RemoveItem(K key)
    {
        AvmAssert(!m_inIteration);
        if (!m_buckets) {
            return false;
        }
        
        Bucket **prev = &m_buckets[HashKey(key) % m_size];
        Bucket *bucket = *prev;
        while (bucket) {
            if (KeysEqual(bucket->key, key)) {
                *prev = bucket->next;
                DestroyItem(bucket->key, bucket->value);
                BucketFree(bucket);
                AvmAssert(m_numItems != 0);
                m_numItems--;
                return true;
            }
            prev = &bucket->next;
            bucket = bucket->next;
        }
        return false;
    }
    
    template <typename K, typename V>
    bool FixedHeapHashTable<K,V>::Expand()
    {
        if (!m_buckets) {
            return false;
        }
        
        int newSize = m_size * 2;
        Bucket **newBuckets = BucketArrayAlloc(newSize);
        if (!newBuckets) {
            return false;
        }
        memset(newBuckets, 0, sizeof(Bucket*) * newSize);
        
        // Rehash everything
        for (int i=0; i<m_size; i++) {
            Bucket *bucket = m_buckets[i];
            while (bucket) {
                Bucket *next = bucket->next;		
                int index = HashKey(bucket->key) % newSize;
                bucket->next = newBuckets[index];
                newBuckets[index] = bucket;		
                bucket = next;
            }
        }
        
        BucketArrayFree(m_buckets);
        m_buckets = newBuckets;
        m_size = newSize;
        return true;
    }
	
    template <typename K, typename V>
    void FixedHeapHashTable<K,V>::ForEach(TYPENAME_KLUDGE FixedHeapHashTable<K,V>::Iterator& iterator)
    {
        if (m_numItems) {
#ifdef DEBUG
            m_inIteration = true;
#endif
            // Call the callback function for each item in the hash table
            for (int i = 0; i< m_size; i++) {
                for (Bucket *bucket = m_buckets[i]; bucket; bucket = bucket->next) {
                    iterator.each(bucket->key, bucket->value);
                }
            }
#ifdef DEBUG
            m_inIteration = false;
#endif
			
        }
    }
	
    template <typename K, typename V>
    /*virtual*/ typename FixedHeapHashTable<K,V>::Bucket* FixedHeapHashTable<K,V>::BucketAlloc()
    {
		return mmfx_new0(Bucket);
    }
	
    template <typename K, typename V>
    /*virtual*/ void FixedHeapHashTable<K,V>::BucketFree(Bucket *b)
    {
		mmfx_delete(b);
    }
	
    template <typename K, typename V>
    /*virtual*/ typename FixedHeapHashTable<K,V>::Bucket** FixedHeapHashTable<K,V>::BucketArrayAlloc(size_t size)
    {
		return mmfx_new_array(Bucket*, size);
    }
    
    template <typename K, typename V>
    /*virtual*/ void FixedHeapHashTable<K,V>::BucketArrayFree(Bucket **buckets)
    {
		mmfx_delete_array(buckets);
    }
	
    template <typename K, typename V>
    /*virtual*/ uintptr_t FixedHeapHashTable<K,V>::HashKey(K key) const
    {
		return (uintptr_t)key >> 3;
    }		
    
    template <typename K, typename V>
    /*virtual*/ bool FixedHeapHashTable<K,V>::KeysEqual(K key1, const K key2) const
    {
		return key1 == key2;
    }
	
	
    template <typename K, typename V>
    /*virtual*/ FixedHeapHashTable<K,V>::Iterator::~Iterator()
    {
    }
	
}
#endif
