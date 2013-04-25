/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCStack__
#define __GCStack__

namespace MMgc
{
    /**
     * Stack for the marking garbage collector.
     *
     * Stack items are strongly typed and items of different types are pushed and
     * popped using different methods (Push_GCObject, Push_StackMemory, ...; Pop_GCObject,
     * Pop_StackMemory, ...).  Calling a Pop method without knowing that the item on
     * top of the stack is of the correct type is an error.
     *
     * A method PeekTypetag() is provided for reading the type of the top item.
     *
     * As the most common item type is a simple object pointer, Pop_GCObject() does not
     * require PeekTypetag to be used to sniff the type before it is called, but instead
     * returns NULL if the top item is not a plain object pointer.
     */
    class GCMarkStack
    {
    public:
        /**
         * Stack item classification, as returned by PeekTypetag.
         *
         * The "Data" comment summarizes the data carried by the item; see doc block
         * further down for more detailed information.
         */
        enum TypeTag
        {
            // Data: Pointer to the beginning of a GC object (GCTraceableBase, GCObject).
            kGCObject = 0,

            // Data: Pointer to the beginning of a GC object; cursor value.
            kLargeExactObjectTail = 1,
            
            // Data: Pointer into a stack segment; length of the rest of the segment; base pointer.
            kStackMemory = 2,

            // Data: Pointer into a large object; length of the rest of the object; base pointer.
            kLargeObjectChunk = 3,

            // Data: Pointer into a split root; length of the rest of the root; base pointer.
            kLargeRootChunk = 4,
            
            // Data: Pointer to root object that's protected against deletion by this item.
            kRootProtector = 5,

            // Data: Pointer to the large object that's protected against deletion by this item.
            kLargeObjectProtector = 6
        };

    public:
#ifdef MMGC_MARKSTACK_ALLOWANCE
        GCMarkStack(int32_t allowance);    // Pass 0 for allowance to request "unlimited", never negative
#else
        GCMarkStack();
#endif
        ~GCMarkStack();

        /**
         * Initialization: Provide the stack with an item to use for clearing dead slots.
         * 'item' must point to a managed GC object (not a root) that the GC promises to 
         * keep alive.  It must not be NULL.
         */
        void SetDeadItem(void* item);

        /** Push a GC item; return true if successful, false if OOM */
        bool Push_GCObject(const void *p);
        
        /** Push a the tail of a large exactly traced GC item; return true if successful, false if OOM */
        bool Push_LargeExactObjectTail(const void *p, size_t cursor);

        /** Push a stack chunk; return true if successful, false if OOM */
        bool Push_StackMemory(const void *p, uint32_t size, const void* baseptr);

        /** Push a large-object chunk; return true if successful, false if OOM */
        bool Push_LargeObjectChunk(const void *p, uint32_t size, const void* baseptr);

        /** Push a large-root chunk; return true if successful, false if OOM */
        bool Push_LargeRootChunk(const void *p, uint32_t size, const void* baseptr);
        
        /** Push an item that protects a split large object from being deleted; return true if successful, false if OOM */
        bool Push_LargeObjectProtector(const void *p);
        
        /** Push an item that protects a split large root from being deleted; return true if successful, false if OOM */
        bool Push_RootProtector(const void *p);

        /** Get the mark stack typetag for the top stack element. */
        TypeTag PeekTypetag();
        
        /** If the top item is a GC item then pop it off the non-empty stack and return it, otherwise return null.  */
        const void* Pop_GCObject();
        
        /** Pop one large exactly traced tail GC item off the non-empty stack and return it.  Type tag must match. */
        void Pop_LargeExactObjectTail(const void * &p, size_t &cursor);
        
        /** Pop one stack chunk item off the non-empty stack and return it.  Type tag must match. */
        void Pop_StackMemory(const void * &p, uint32_t &size, const void * &baseptr);

        /** Pop one large object chunk item off the non-empty stack and return it.  Type tag must match. */
        void Pop_LargeObjectChunk(const void * &p, uint32_t &size, const void * &baseptr);

        /** Pop one large root chunk item off the non-empty stack and return it.  Type tag must match. */
        void Pop_LargeRootChunk(const void * &p, uint32_t &size, const void * &baseptr);
        
        /** Pop one large object protector item off the non-empty stack and return it.  Type tag must match. */
        void Pop_LargeObjectProtector(const void * &p);

        /** Pop one root protector item off the non-empty stack and return it.  Type tag must match. */
        void Pop_RootProtector(const void * &p);

        /** Return the number of elements on the stack. */
        uint32_t Count();

        /** Return true iff the stack is empty. */
        bool IsEmpty();

        /** Pop all elements off the stack and discard any cached memory. */
        void Clear();

        /** Return an "index" denoting the item currently on the mark stack top. */
        uintptr_t Top();

        /**
         * Clear the item at the given "index".  The item at that index must not have been
         * popped since the index was obtained.
         */
        void ClearItemAt(uintptr_t index);

        /**
         * The item at "index" must be a RootProtector for "rootptr".  Clear the item, and search
         * the mark stack upward for any LargeRootChunk item that belongs to the same root, and if
         * it's found then clear that too.  The item at index must not have been popped since
         * index was obtained.
         */
        void ClearRootProtectorAndChunkAbove(uintptr_t index, const void* rootptr);

        /**
         * @return the number of inactive mark stack segments (obscure, but useful for moving work 
         * from the barrier stack to the mark stack).
         */
        uint32_t InactiveSegments();

        /**
         * Move one inactive segment from "other" and insert it into our segment list.  The stack
         * "other" must not have any protector items or split items; "other" will normally be
         * the barrier stack.  OOM conditions are ignored, but at the end both stacks are in a
         * consistent state.
         */
        void TransferOneInactiveSegmentFrom(GCMarkStack& other);

        /**
         * Move everything from "other" and insert into this stack.    The stack "other" must not
         * have any protector items or split items; "other" will normally be the barrier stack.
         * Return true if the transfer was successful, false if an out-of-memory condition
         * prevented reestablishing invariants in "other" following the transfer.  (In the latter
         * case the stacks may have changed, but they will be consistent.)
         */
        bool TransferEverythingFrom(GCMarkStack& other);

        /**
         * Move at least one item from "other" and insert into this
         * stack.  The same conditions apply here as for
         * GCMarkStack::TransferEverythingFrom: the stack "other" must
         * not have any protector items or split items.  Return true
         * if the transfer was successful (or if other was empty);
         * returns false if an out-of-memory condition prevented
         * reestablishing invariants in this or "other" following the
         * transfer, in which case the item transfer is rolled back.
         */
        bool TransferSomethingFrom(GCMarkStack& other);

#ifdef MMGC_MARKSTACK_DEPTH
        /** Return the number of elements on the stack when its depth was the greatest. */
        uint32_t MaxCount();
#endif
    private:
        /**
         * If the top item is a GC item then return it without popping,
         * otherwise return null.
         * @see Pop_GCObject
         */
        const void* Peek_GCObject();

    private:
        // No implementation of copy constructors and assignment operators

        GCMarkStack(const GCMarkStack& other);
        GCMarkStack& operator=(const GCMarkStack& other);
        
    private:
        /**
         * Mark stack representation:
         *
         * The stack is a sequence of machine words; one or more words are grouped to
         * form a single item.  Every word is tagged in the low two bits.  The low
         * bit is the "first-word" bit: if *clear* the word is the first word of the item,
         * and if *set* the word is a subsequent word of the item.  The high bit is the
         * "end" bit: if *clear* this is the last word of the item, and if *set* then it is not.
         *
         * The tag 00 is reserved for single-word "GCObject" items that hold pointers to 
         * managed objects; every other item uses more than one word.  For multi-word items
         * the payload of the first word is always a tag (shifted left two bits), from the
         * set of typetags above.  "size" fields are always a multiple of four bytes; pointers
         * are always at least 4-byte aligned.
         *
         * Managed object
         *    object pointer               | 00
         *
         * Large exact object tail:
         *    (kLargeExactObjectTail << 2) | 10
         *    object pointer               | 11
         *    (cursor << 2)                | 01
         *
         * Stack memory:
         *    (kStackMemory << 2)          | 10
         *    pointer                      | 11
         *    size                         | 11
         *    basepointer                  | 01
         *
         * Large object chunk:
         *    (kLargeObjectChunk << 2)     | 10
         *    pointer                      | 11
         *    size                         | 11
         *    basepointer                  | 01
         *
         * Large root chunk:
         *    (kLargeRootChunk << 2)       | 10
         *    pointer                      | 11
         *    size                         | 11
         *    basepointer                  | 01
         *
         * Root protector:
         *    (kRootProtector << 2)        | 10
         *    pointer                      | 01
         *
         * Large object protector:
         *    (kLargeObjectProtector << 2) | 10
         *    pointer                      | 01
         *
         * When a multi-word item is placed on the stack it will never be split across
         * two stack chunks.
         */

        static const uintptr_t kFirstWord = 2;      // Tag for the first word of a multi-word item
        static const uintptr_t kMiddleWord = 3;     // Tag for the middle word of a multi-word item
        static const uintptr_t kLastWord = 1;       // Tag for the last word of a multi-word item

        // A "stack segment" is a single heap block (4K bytes) that has two parts: the header
        // and the payload.  The payload starts where the header ends, and ends at the end of
        // the block.
        //
        // In DEBUG builds there are sentinels before and after the payload to catch overwrites
        // and other errors in the stack logic.

        struct StackSegment
        {
            StackSegment();

            uintptr_t*      m_savedTop;  // Saved m_top value when this segment is not topmost
            StackSegment* m_prev;        // The segment below this one
#ifdef DEBUG
            uintptr_t       sentinel1;
            uintptr_t       sentinel2;
#endif
        };
        
        // Useful invariants on stack state:
        //
        //   m_topSegment, m_base, m_top, and m_limit are never NULL following construction
        //   m_base <= m_top <= m_limit
        //   m_base == items(m_topSegment)
        //   m_limit == limit(m_topSegment)
        
        uintptr_t*          m_base;           // First entry in m_topSegment
        uintptr_t*          m_top;            // First free entry in m_topSegment
        uintptr_t*          m_limit;          // First entry following m_topSegment
        StackSegment*       m_topSegment;     // Current stack segment, older segments linked through 'prev'
        uint32_t            m_hiddenCount;    // Number of elements in those older segments
        uint32_t            m_hiddenSegments; // Number of those older segments
        StackSegment*       m_extraSegment;   // Single-element cache to control costs of straddling a segment boundary
        uintptr_t           m_deadItem;       // A managed object that is used to clear out dead slots
#ifdef MMGC_MARKSTACK_ALLOWANCE
        int32_t             m_allowance;      // Allowance for the number of elements
#endif
#ifdef MMGC_MARKSTACK_DEPTH
        uint32_t            m_maxDepth;       // Max depth of mark stack
#endif

        // Allocate n consecutive words on the stack and return a pointer to the one at the 
        // highest address.  Return NULL if space could not be obtained.
        uintptr_t* allocSpace(size_t nwords);

        // Free n consecutive words allocated through allocSpace.
        void freeSpace(size_t nwords);

        // Return the address of the first payload word in the segment.
        static uintptr_t* items(StackSegment* seg);

        // Return the address above the last payload word in the segment.  In DEBUG builds there
        // are two sentinel words starting at this address.
        static uintptr_t* limit(StackSegment* seg);

        // Push a new segment onto the stack, and update all instance vars.  If "mustSucceed" is true
        // then the allocation must not fail - used during initialization only.  If it fails, the
        // normal OOM mechanism will kick in.  Return true if the segment could be pushed or false 
        // if it could not be allocated.
        bool PushSegment(bool mustSucceed=false);

        // The current segment is discarded and the previous segment, if any, reinstated.
        // Update all instance vars.
        void PopSegment();

        // The current segment is discarded if there is a previous segment.
        void PopSegment_UnlessLast();

        // Populate m_extraSegment if NULL, return true if successful, false otherwise.  If
        // "mustSucceed" is true then failure to allocate will trigger normal OOM handling.
        bool PopulateExtraSegment(bool mustSucceed);

        // Allocate a segment, return NULL if it could not be allocated.  If mustSucceed is true
        // then failure to allocate will trigger normal OOM handling.
        void* AllocStackSegment(bool mustSucceed);

        // Free a segment allocated through AllocStackSegment.
        void FreeStackSegment(void* p);

        // cdr down a segment list and return the last one.
        StackSegment* FindLastSegment(StackSegment* first);

        // Given a pointer to an item on the stack, return the one above it (the younger one),
        // or NULL if the item is the top item.
        uintptr_t* GetNextItemAbove(uintptr_t* item);

#ifdef MMGC_MARKSTACK_ALLOWANCE
        // Make sure the stack can grow by nseg segments (relative to its allowance); return true
        // if so and false otherwise.  May free the extra segment if it's present, and will take
        // into account an empty segment that will be popped if segments are inserted underneath
        // it.
        bool MakeSpaceForSegments(int32_t nseg);
#endif

#ifdef _DEBUG
        // Check as many invariants as possible
        bool Invariants();
#endif
    };
}

#endif /* __GCStack__ */
