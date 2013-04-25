/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_MultinameHashtable__
#define __avmplus_MultinameHashtable__

namespace avmplus
{
    typedef struct Binding_* Binding;
    enum BindingKind
    {
        BKIND_NONE    = 0, // no such binding (id == 0)            000
        BKIND_METHOD  = 1, // disp_id of method                    001
        BKIND_VAR     = 2, // slot number (r/w var)                010
        BKIND_CONST   = 3, // slot number (r/o const)              011
        BKIND_unused  = 4, // not in use                           100
        BKIND_GET     = 5, // get-only property                    101
        BKIND_SET     = 6, // set-only property                    110
        BKIND_GETSET  = 7  // get+set  property                    111
    };

    // Dispatch id: BKIND_METHOD, BKIND_GET, BKIND_SET, and BKIND_GETSET
    // are tags for a disp_id which indexes into VTable::methods[].  The
    // disp_id for a setter (BKIND_SET or BKIND_GETSET) is actually disp_id+1,
    // whether or not a getter exists at [disp_id].

    // A couple of common Binding results that are worth having constants for
    const Binding BIND_AMBIGUOUS = (Binding)-1;
    const Binding BIND_NONE      = (Binding)BKIND_NONE;      // no such binding

    // Quad shall be four words; we want the alignment and the space
    // conservation.  There are static asserts in
    // MultinameHashtable.cpp to check that condition.  See also
    // bugzilla 534317.

    // This can't be an inner class of MultinameHashtable anymore due
    // to template limitations (also encountered with List classes).
    // Specifically, how to declare getNSSet's return value eluded me.
    // Neither MultinameHashtable::Quad (complains about missing
    // template params) nor with the params (syntax error) was
    // acceptable go GCC.
    template <class VALUE_TYPE>
    class Quad
    {
    public:
        Stringp     name;
        Namespacep  ns;
        VALUE_TYPE  value;
        uintptr_t   apiAndMultiNS;
        
        // 'multiNs' is 1 if the given name exists elsewhere with a different namespace.
        // 'api' is the minimum API version.
        //
        // The values share a word for space reasons.  The low bit holds 'multiNS',
        // the word shifted right 1 holds 'apiVersion'.
        //
        // NOTE, some code in MultinameHashtable.cpp operates directly on this
        // representation when the values are updated.
        
        REALLY_INLINE uint32_t multiNS() const { return (uint32_t)(apiAndMultiNS & 1); }
        REALLY_INLINE ApiVersion apiVersion() const { return (ApiVersion)(apiAndMultiNS >> 1); }
        
        // return true iff a lookup with the given ns is compatible with the one in this Quad.
        bool matchNS(Namespacep ns) const;
    };

    template <class VALUE_TYPE>
    class QuadContainer : public MMgc::GCTraceableObject
    {
    private:
        QuadContainer(uint32_t capacity) : capacity(capacity) {}

    public:
        static QuadContainer<VALUE_TYPE>* create(MMgc::GC* gc, uint32_t capacity);

        virtual bool gcTrace(MMgc::GC* gc, size_t cursor);

        const uint32_t capacity;
#ifdef AVMPLUS_64BIT
        uint32_t padding;
#endif
        // Now it should at least be word-aligned.  FIXME - do we want better than that?
        Quad<VALUE_TYPE> quads[1];  // Actual size is capacity
    };
    
    /**
     * Hashtable for mapping <name, ns> pairs to a value, which can be
     * a Binding or a Traits.
     */
    template <class VALUE_TYPE, class VALUE_WRITER>
    class MultinameHashtable : public MMgc::GCTraceableObject
    {
        template<class MNHT>
        friend class StMNHTIterator;

    public:
        typedef VALUE_TYPE TYPE; // for StMNHTIterator

    private:
        /**
         * Finds the hash bucket corresponding to the key <name,ns>
         * in the hash table starting at t, containing tLen
         * quads.
         */
        static int find(Stringp name, Namespacep ns, const Quad<VALUE_TYPE> *t, unsigned tLen);
        void rehash(const Quad<VALUE_TYPE> *oldAtoms, int oldlen, Quad<VALUE_TYPE> *newAtoms, int newlen);

        /**
         * Called to grow the Hashtable, particularly by add.
         *
         * - Calculates the needed size for the new Hashtable
         *   (typically 2X the current size)
         * - Creates a new array of Atoms
         * - Rehashes the current table into the new one
         * - Deletes the old array of Atoms and sets the Atom
         *   pointer to our new array of Atoms
         *
         */
        void grow();

        // Free the m_quads array, clear m_quads
        void freeQuads(MMgc::GC* gc);

        MultinameHashtable(int capacity);

    public:
        /** kDefaultCapacity must be a power of 2 */
        const static int kDefaultCapacity = 8;

        /**
         * initialize with a known capacity.  i.e. we can fit minSize
         * elements in without rehashing.
         * @param heap
         * @param capacity  # of logical slots
         */
        REALLY_INLINE static MultinameHashtable* create(MMgc::GC* gc, int capacity = kDefaultCapacity)
        {
            return new (gc, MMgc::kExact) MultinameHashtable(capacity);
        }

        virtual bool gcTrace(MMgc::GC* gc, size_t cursor);

        bool isFull() const;

        /**
         * @name operations on name/ns/binding quads
         */
        /*@{*/
        VALUE_TYPE get(Stringp name, Namespacep ns) const;
        VALUE_TYPE get(Stringp name, NamespaceSetp nsset) const;
        // if nsFoundOut is not NULL, then it is set to the Namespace where
        // we found the matching name (or NULL if no matching name was found).
        VALUE_TYPE getName(Stringp name, Namespacep* nsFoundOut = NULL) const;
        VALUE_TYPE getMulti(const Multiname* name) const;
        VALUE_TYPE getMulti(const Multiname& name) const;

        // return the NS that unambigously matches in "match" (or null for none/ambiguous)
        VALUE_TYPE getMulti(const Multiname& name, Namespacep& match) const;
        /*@}*/


        // note: if you are just doing a single iteration thru a single MNHT,
        // it's more efficient (and easier) to use StMNHTIterator instead.
        int FASTCALL next(int index) const;
        Stringp keyAt(int index) const;
        Namespacep nsAt(int index) const;
        VALUE_TYPE valueAt(int index) const;

        size_t allocatedSize() const;

        /**
         * Adds a name/value pair to a hash table.  Automatically
         * grows the hash table if it is full.
         */
        void add(Stringp name, Namespacep ns, VALUE_TYPE value);

        void put(Stringp name, Namespacep ns, VALUE_TYPE value);

    private:
        const Quad<VALUE_TYPE>* getNSSet(Stringp name, NamespaceSetp nsset) const;

    protected:
        void Init(int capacity);

    // ------------------------ DATA SECTION BEGIN
    private:    QuadContainer<VALUE_TYPE>* m_quads; // property hashtable (written with explicit WB)
    public:     int size;               // no. of properties
    public:     int numQuads;           // size of hashtable
    // ------------------------ DATA SECTION END
    };

    class BindingType
    {
    public:
        //        const static Binding NONE = NULL;
        static void store(MMgc::GC*, void *, void **addr, void *value);
    };

    class GCObjectType
    {
    public:
        //        const Traitsp NONE = NULL;
        static void store(MMgc::GC* gc, void *container, void **addr, void *value);
    };

    typedef MultinameHashtable<Binding, BindingType> MultinameBindingHashtable;
    typedef MultinameHashtable<Traitsp, GCObjectType> MultinameTraitsHashtable;
    typedef MultinameHashtable<MethodInfo*, GCObjectType> MultinameMethodInfoHashtable;

    // Note: unlike MNHT::next(), StMNHTIterator::next()
    // doesn't advance past empty entries. it's the caller's
    // responsibility to call StMNHTIterator::key() and skip null
    // entries -- this is a useful optimization since the caller
    // typically is looping and extracting this value anyway -- this is
    // a substantial win since it avoids the loop-within-a-loop scenario.
    // proper usage is:
    //
    //      StMNHTIterator iter(mnht);
    //      while (iter.next()) {
    //          if (!iter.key()) continue;
    //          .. rest of loop ..
    //      }
    //
    template<class MNHT>
    class StMNHTIterator
    {
    private:

        MNHT* const volatile m_mnht; // kept just to ensure it doesn't get collected -- must be volatile!
        const Quad<typename MNHT::TYPE>* m_cur;
        const Quad<typename MNHT::TYPE>* const m_end; // one past the end

    public:
        StMNHTIterator(MNHT* mnht);
        bool next();
        Stringp key() const;
        Namespacep ns() const;
        typename MNHT::TYPE value() const;
        ApiVersion apiVersion() const;
    };

    typedef StMNHTIterator<MultinameBindingHashtable> StMNHTBindingIterator;
    typedef StMNHTIterator<MultinameTraitsHashtable> StMNHTTraitsIterator;
    typedef StMNHTIterator<MultinameMethodInfoHashtable> StMNHTMethodInfoIterator;
}

#endif /* __avmplus_MultinameHashtable__ */
