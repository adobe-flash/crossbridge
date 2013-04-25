/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nanojit.h"

#ifdef FEATURE_NANOJIT

namespace nanojit
{
    Allocator::Allocator()
        : current_chunk(NULL)
        , current_top(NULL)
        , current_limit(NULL)
    { }

    Allocator::~Allocator()
    {
        reset();
    }

    void Allocator::reset()
    {
        Chunk *c = current_chunk;
        while (c) {
            Chunk *prev = c->prev;
            freeChunk(c);
            c = prev;
        }
        current_chunk = NULL;
        current_top = NULL;
        current_limit = NULL;
        postReset();
    }

    char* Allocator::allocSlow(size_t nbytes, size_t align_mask, bool fallible)
    {
        // We don't know how much padding will be needed for alignment
        // until the new chunk is allocated and we know its address,
        // thus we assume the worst here.
        if (fill(nbytes + align_mask, fallible)) {
            char* p = (char*)(((uintptr_t)current_top + align_mask) & ~align_mask);
            NanoAssert(p + nbytes <= current_limit);
            current_top = p + nbytes;
            return p;
        }
        return NULL;
    }

    bool Allocator::fill(size_t nbytes, bool fallible)
    {
        if (nbytes < MIN_CHUNK_SZB)
            nbytes = MIN_CHUNK_SZB;
        size_t chunkbytes = sizeof(Chunk) + nbytes - sizeof(int64_t);
        void* mem = allocChunk(chunkbytes, fallible);
        if (mem) {
            Chunk* chunk = (Chunk*) mem;
            chunk->prev = current_chunk;
            chunk->size = chunkbytes;
            current_chunk = chunk;
            current_top = (char*)chunk->data;
            current_limit = (char*)mem + chunkbytes;
            return true;
        } else {
            NanoAssert(fallible);
            return false;
        }
    }

    size_t Allocator::getBytesAllocated(size_t(*my_malloc_usable_size)(void *))
    {
        size_t n = 0;
        Chunk *c = current_chunk;
        while (c) {
            size_t usable = my_malloc_usable_size(c);
            n += usable ? usable : c->size;
            c = c->prev;
        }
        return n;
    }
}

#endif // FEATURE_NANOJIT
