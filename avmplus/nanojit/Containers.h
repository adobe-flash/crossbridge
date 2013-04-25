/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __nanojit_Containers__
#define __nanojit_Containers__

namespace nanojit
{
    /** simple linear bit array, memory taken from Allocator
     *  warning: when bit array grows, old memory is wasted since it
     *  was allocated from Allocator.  pre-size the bitmap when possible
     *  by passing nbits to the constructor. */
    class BitSet {
        Allocator &allocator;
        int cap;
        int64_t *bits;
        static const int64_t ONE = 1;
        static const int SHIFT = 6;

        inline int bitnum2word(int i) const {
            return i >> 6;
        }
        inline int64_t bitnum2mask(int i) const {
            return ONE << (i & 63);
        }

        /** keep doubling array to fit at least w words */
        void grow(int w);

    public:
        BitSet(Allocator& allocator, int nbits=128);

        /** clear all bits */
        void reset();

        /** allocates new bits and clears them;  any old bits are lost and will
         * be freed according to their allocator's policy. */
        void resetAndAlloc();

        /** perform a bitwise or with BitSet other, return true if
         *  this bitset was modified */
        bool setFrom(BitSet& other);

        /** return bit i as a bool */
        bool get(int i) const {
            NanoAssert(i >= 0);
            int w = bitnum2word(i);
            if (w < cap)
                return (bits[w] & bitnum2mask(i)) != 0;
            return false;
        }

        /** set bit i */
        void set(int i) {
            NanoAssert(i >= 0);
            int w = bitnum2word(i);
            if (w >= cap)
                grow(w);
            NanoAssert(w < cap);
            bits[w] |= bitnum2mask(i);
        }

        /** clear bit i */
        void clear(int i) {
            NanoAssert(i >= 0);
            int w = bitnum2word(i);
            if (w < cap)
                bits[w] &= ~bitnum2mask(i);
        }
    };

    /** Seq is a single node in a linked list */
    template<class T> class Seq {
    public:
        Seq(T head, Seq<T>* tail=NULL) : head(head), tail(tail) {}
        T       head;
        Seq<T>* tail;
    };

    /** SeqBuilder is used to create a linked list of Seq<T> by inserting
     *  nodes either at the beginning, with insert(), or at the end, with
     *  add().  Once built, the actual list can be retained while this
     *  SeqBuilder can be discarded.  */
    template<class T> class SeqBuilder {
    public:
        SeqBuilder(Allocator& allocator)
            : allocator(allocator)
            , items(NULL)
            , last(NULL)
        { }

        /** add item to beginning of list */
        void insert(T item) {
            Seq<T>* e = new (allocator) Seq<T>(item, items);
            if (last == NULL)
                last = e;
            items = e;
        }

        /** add item to end of list */
        void add(T item) {
            Seq<T>* e = new (allocator) Seq<T>(item);
            if (last == NULL)
                items = e;
            else
                last->tail = e;
            last = e;
        }

        /** return first item in sequence */
        Seq<T>* get() const {
            return items;
        }

        /** self explanitory */
        bool isEmpty() const {
            return items == NULL;
        }

        /** de-reference all items */
        void clear() {
            items = last = NULL;
        }

    private:
        Allocator& allocator;
        Seq<T>* items;
        Seq<T>* last;
    };

#ifdef NANOJIT_64BIT
    static inline size_t murmurhash(const void *key, size_t len) {
        const uint64_t m = 0xc6a4a7935bd1e995;
        const int r = 47;
        uint64_t h = 0;

        const uint64_t *data = (const uint64_t*)key;
        const uint64_t *end = data + (len/8);

        while(data != end)
            {
                uint64_t k = *data++;

                k *= m;
                k ^= k >> r;
                k *= m;

                h ^= k;
                h *= m;
            }

        const unsigned char *data2 = (const unsigned char*)data;

        switch(len & 7) {
        case 7: h ^= uint64_t(data2[6]) << 48;
        case 6: h ^= uint64_t(data2[5]) << 40;
        case 5: h ^= uint64_t(data2[4]) << 32;
        case 4: h ^= uint64_t(data2[3]) << 24;
        case 3: h ^= uint64_t(data2[2]) << 16;
        case 2: h ^= uint64_t(data2[1]) << 8;
        case 1: h ^= uint64_t(data2[0]);
            h *= m;
        };

        h ^= h >> r;
        h *= m;
        h ^= h >> r;

        return (size_t)h;
    }
#else
    static inline size_t murmurhash(const void * key, size_t len) {
        const uint32_t m = 0x5bd1e995;
        const int r = 24;
        uint32_t h = 0;

        const unsigned char * data = (const unsigned char *)key;
        while(len >= 4) {
            uint32_t k = *(size_t *)(void*)data;

            k *= m;
            k ^= k >> r;
            k *= m;

            h *= m;
            h ^= k;

            data += 4;
            len -= 4;
        }

        switch(len) {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0];
            h *= m;
        };

        h ^= h >> 13;
        h *= m;
        h ^= h >> 15;

        return (size_t)h;
    }
#endif

    template<class K> struct DefaultHash {
        static size_t hash(const K &k) {
            // (const void*) cast is required by ARM RVCT 2.2
            return murmurhash((const void*) &k, sizeof(K));
        }
    };

    template<class K> struct DefaultHash<K*> {
        static size_t hash(K* k) {
            uintptr_t h = (uintptr_t) k;
            // move the low 3 bits higher up since they're often 0
            h = (h>>3) ^ (h<<((sizeof(uintptr_t) * 8) - 3));
            return (size_t) h;
        }
    };

    /** The default hash function assumes bitwise equality,
     *  so that is what we implement here.  Note that float,
     *  double, and float4 types would not normally compare
     *  bitwise (due to 0 vs. -0 and NaNs), but that we want
     *  bitwise semantics for CSE and pools, for example.
     */

    template <class K> struct DefaultKeysEqual {
        static bool keysEqual(const K& x, const K& y) {
            return VMPI_memcmp(&x, &y, sizeof(K)) == 0;
        }
    };

    // use this in a hashmap if you want to compare keys by possibly-
    // overloaded operator ==.
    template <class K> struct UseOperEqual {
        static bool keysEqual(const K& x, const K& y) {
            return x == y;
        }
    };

    template <class K> struct DefaultKeysEqual<K*> {
        static bool keysEqual(K* x, K* y) {
            return x == y;
        }
    };

    template <> struct DefaultKeysEqual<int32_t> {
        static bool keysEqual(int32_t x, int32_t y) {
            return x == y;
        }
    };

    template <> struct DefaultKeysEqual<uint32_t> {
        static bool keysEqual(uint32_t x, uint32_t y) {
            return x == y;
        }
    };

    template <> struct DefaultKeysEqual<int64_t> {
        static bool keysEqual(int64_t x, int64_t y) {
            return x == y;
        }
    };

    template <> struct DefaultKeysEqual<uint64_t> {
        static bool keysEqual(uint64_t x, uint64_t y) {
            return x == y;
        }
    };

    /** Bucket hashtable with a fixed # of buckets (never rehash)
     *  Intended for use when a reasonable # of buckets can be estimated ahead of time.
     *  Note that operator== is used to compare keys.
     */
    template<class K, class T, class H=DefaultHash<K>, class E=DefaultKeysEqual<K> > class HashMap {
        Allocator& allocator;
        size_t nbuckets;
        class Node {
        public:
            K key;
            T value;
            Node(K k, T v) : key(k), value(v) { }
        };
        Seq<Node>** buckets;

        /** return the node containing K, and the bucket index, or NULL if not found */
        Node* find(K k, size_t &i) const {
            i = H::hash(k) % nbuckets;
            for (Seq<Node>* p = buckets[i]; p != NULL; p = p->tail) {
                if (E::keysEqual(p->head.key, k))
                    return &p->head;
            }
            return NULL;
        }
    public:
        HashMap(Allocator& a, size_t nbuckets = 16)
            : allocator(a)
            , nbuckets(nbuckets)
            , buckets(new (a) Seq<Node>*[nbuckets])
        {
            NanoAssert(nbuckets > 0);
            clear();
        }

        /** clear all buckets.  Since we allocate all memory from Allocator,
         *  nothing needs to be freed. */
        void clear() {
            VMPI_memset(buckets, 0, sizeof(Seq<Node>*) * nbuckets);
        }

        /** add (k,v) to the map.  If k is already in the map, replace the value */
        void put(const K& k, const T& v) {
            size_t i;
            Node* n = find(k, i);
            if (n) {
                n->value = v;
                return;
            }
            buckets[i] = new (allocator, alignof<K>()) Seq<Node>(Node(k,v), buckets[i]);
        }

        /** return v for element k, or T(0) if k is not present */
        T get(const K& k) const {
            size_t i;
            Node* n = find(k, i);
            return n ? n->value : 0;
        }

        /** returns true if k is in the map. */
        bool containsKey(const K& k) const {
            size_t i;
            return find(k, i) != 0;
        }

        /** remove k from the map, if it is present.  if not, remove()
         *  silently returns */
        void remove(const K& k) {
            size_t i = H::hash(k) % nbuckets;
            Seq<Node>** prev = &buckets[i];
            for (Seq<Node>* p = buckets[i]; p != NULL; p = p->tail) {
                if (E::keysEqual(p->head.key, k)) {
                    (*prev) = p->tail;
                    return;
                }
                prev = &p->tail;
            }
        }

        /** Iter is an iterator for HashMap, intended to be instantiated on
         *  the stack.  Iteration order is undefined.  Mutating the hashmap
         *  while iteration is in progress gives undefined results.  All iteration
         *  state is in class Iter, so multiple iterations can be in progress
         *  at the same time.  for example:
         *
         *  HashMap<K,T>::Iter iter(map);
         *  while (iter.next()) {
         *     K *k = iter.key();
         *     T *t = iter.value();
         *  }
         */
        class Iter {
            friend class HashMap;
            typedef HashMap<K,T,H,E> MapType;
            const MapType &map;
            int bucket;
            const Seq<Node>* current;

        public:
            Iter(MapType& map) : map(map), bucket((int)map.nbuckets-1), current(NULL)
            { }

            /** return true if more (k,v) remain to be visited */
            bool next() {
                if (current)
                    current = current->tail;
                while (bucket >= 0 && !current)
                    current = map.buckets[bucket--];
                return current != NULL;
            }

            /** return the current key */
            const K& key() const {
                NanoAssert(current != NULL);
                return current->head.key;
            }

            /** return the current value */
            const T& value() const {
                NanoAssert(current != NULL);
                return current->head.value;
            }
        };

        /** return true if the hashmap has no elements */
        bool isEmpty() {
            Iter iter(*this);
            return !iter.next();
        }
    };

    /**
     * Simple binary tree.  No balancing is performed under the assumption
     * that the only users of this structure are not performance critical.
     */
    template<class K, class T> class TreeMap {
        Allocator& alloc;
        class Node {
        public:
            Node* left;
            Node* right;
            K key;
            T value;
            Node(K k, T v) : left(NULL), right(NULL), key(k), value(v)
            { }
        };
        Node* root;

        /**
         * helper method to recursively insert (k,v) below Node n or a child
         * of n so that the binary search tree remains well formed.
         */
        void insert(Node* &n, K k, T v) {
            if (!n)
                n = new (alloc) Node(k, v);
            else if (k == n->key)
                n->value = v;
            else if (k < n->key)
                insert(n->left, k, v);
            else
                insert(n->right, k, v);
        }

        /**
         * search for key k below Node n and return n if found, or the
         * closest parent n where k should be inserted.
         */
        Node* find(Node* n, K k) {
            if (!n)
                return NULL;
            if (k == n->key)
                return n;
            if (k < n->key)
                return find(n->left, k);
            if (n->right)
                return find(n->right, k);
            return n;
        }

    public:
        TreeMap(Allocator& alloc) : alloc(alloc), root(NULL)
        { }

        /** set k = v in the map.  if k already exists, replace its value */
        void put(K k, T v) {
            insert(root, k, v);
        }

        /** return the closest key that is <= k, or NULL if k
            is smaller than every key in the Map. */
        K findNear(K k) {
            Node* n = find(root, k);
            return n ? n->key : 0;
        }

        /** returns the value for k or NULL */
        T get(K k) {
            Node* n = find(root, k);
            return (n && n->key == k) ? n->value : 0;
        }

        /** returns true iff k is in the Map. */
        bool containsKey(K k) {
            Node* n = find(root, k);
            return n && n->key == k;
        }

        /** make the tree empty.  trivial since we dont manage elements */
        void clear() {
            root = NULL;
        }
    };
}
#endif // __nanojit_Containers__
