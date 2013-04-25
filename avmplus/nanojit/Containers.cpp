/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nanojit.h"

#ifdef FEATURE_NANOJIT

namespace nanojit
{
    BitSet::BitSet(Allocator& allocator, int nbits)
        : allocator(allocator)
        , cap((nbits+63)>>6)
    {
        resetAndAlloc();
    }

    void BitSet::resetAndAlloc()
    {
        bits = (int64_t*)allocator.alloc(cap * sizeof(int64_t));
        reset();
    }

    void BitSet::reset()
    {
        for (int i=0, n=cap; i < n; i++)
            bits[i] = 0;
    }

    bool BitSet::setFrom(BitSet& other)
    {
        int c = other.cap;
        if (c > cap)
            grow(c);
        int64_t *bits = this->bits;
        int64_t *otherbits = other.bits;
        int64_t newbits = 0;
        for (int i=0; i < c; i++) {
            int64_t b = bits[i];
            int64_t b2 = otherbits[i];
            newbits |= b2 & ~b; // bits in b2 that are not in b
            bits[i] = b|b2;
        }
        return newbits != 0;
    }

    /** keep doubling the bitset length until w fits */
    void BitSet::grow(int w)
    {
        int cap2 = cap;
        do {
            cap2 <<= 1;
        } while (w >= cap2);
        int64_t *bits2 = (int64_t*) allocator.alloc(cap2 * sizeof(int64_t));
        int j=0;
        for (; j < cap; j++)
            bits2[j] = bits[j];
        for (; j < cap2; j++)
            bits2[j] = 0;
        cap = cap2;
        bits = bits2;
    }
}

#endif // FEATURE_NANOJIT
