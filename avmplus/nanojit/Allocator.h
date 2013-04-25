/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __nanojit_Allocator__
#define __nanojit_Allocator__

namespace nanojit
{
    /**
     * Allocator is a bump-pointer allocator with an SPI for getting more
     * memory from embedder-implemented allocator, such as malloc()/free().
     *
     * alloc() never returns NULL.  The implementation of allocChunk()
     * is expected to perform a longjmp or exception when an allocation can't
     * proceed.  fallibleAlloc() (and fallibleAllocChunk()) may return NULL.
     * They should be used for large allocations whose failure can be handled
     * without too much difficulty.
     */
    class Allocator {
    public:
        Allocator();
        ~Allocator();

        // Usable space in the minimum chunk size;  there are also a few bytes
        // used for administration.
        static const size_t MIN_CHUNK_SZB = 2000;

        void reset();

        /** alloc memory, never return null. */
        void* alloc(size_t nbytes, size_t align = 8) {
            const size_t align_mask = align - 1;
            NanoAssert((align & align_mask) == 0); // check alignment is power of 2
            char* p = (char*)(((uintptr_t)current_top + align_mask) & ~align_mask);
            if (p + nbytes <= current_limit) {
                current_top = p + nbytes;
            } else {
                p = allocSlow(nbytes, align_mask, /* fallible = */false);
                NanoAssert(p);
            }
            return (void*)p;
        }

        /** alloc memory, maybe return null. */
        void* fallibleAlloc(size_t nbytes, size_t align = 8) {
            const size_t align_mask = align - 1;
            NanoAssert((align & align_mask) == 0); // check alignment is power of 2
            char* p = (char*)(((uintptr_t)current_top + align_mask) & ~align_mask);
            if (p + nbytes <= current_limit) {
                current_top = p + nbytes;
            } else {
                p = allocSlow(nbytes, align_mask, /* fallible = */true);
            }
            return (void*)p;
        }

        size_t getBytesAllocated(size_t(*my_malloc_usable_size)(void *));

    protected:
        char* allocSlow(size_t nbytes, size_t align_mask, bool fallible = false);
        bool fill(size_t minbytes, bool fallible);

        class Chunk {
        public:
            Chunk* prev;
            size_t size;
            int64_t data[1]; // int64_t forces 8-byte alignment.
        };

        Chunk* current_chunk;
        char* current_top;
        char* current_limit;

        // allocator SPI

        /** allocate another block from a host provided allocator */
        void* allocChunk(size_t nbytes, bool fallible);

        /** free back to the same allocator */
        void freeChunk(void*);

        /** hook for post-reset action. */
        void postReset();
    };
}

// function for computing type alignment
template <typename T>
inline size_t alignof() {
    struct Wrapper { char c; T t; private: Wrapper() {} };
    return sizeof(Wrapper) - sizeof(T);
}

/** global new overload enabling this pattern:  new (allocator, align) T(...) */
inline void* operator new(size_t size, nanojit::Allocator &a, size_t align) {
    return a.alloc(size, align);
}

inline void* operator new(size_t size, nanojit::Allocator *a, size_t align) {
    return a->alloc(size, align);
}

/** global new overload enabling this pattern:  new (allocator) T(...) */
inline void* operator new(size_t size, nanojit::Allocator &a) {
    return a.alloc(size);
}
inline void* operator new(size_t size, nanojit::Allocator *a) {
    return a->alloc(size);
}

/** global new[] overload enabling this pattern: new (allocator) T[] */
inline void* operator new[](size_t size, nanojit::Allocator& a) {
    return a.alloc(size);
}
inline void* operator new[](size_t size, nanojit::Allocator* a) {
    return a->alloc(size);
}

/** global new[] overload enabling this pattern: new (allocator,align) T[] */
inline void* operator new[](size_t size, nanojit::Allocator& a, size_t align) {
    return a.alloc(size, align);
}
inline void* operator new[](size_t size, nanojit::Allocator* a, size_t align) {
    return a->alloc(size, align);
}

#endif // __nanojit_Allocator__
