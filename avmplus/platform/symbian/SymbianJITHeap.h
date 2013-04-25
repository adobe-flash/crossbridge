/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SymbianJITHeap__
#define __SymbianJITHeap__

#include <e32std.h>

/*
* A simple "first pick" allocator.
* Uses a map where each bit points to a 4k chunk.
* It returns always the first free chunk in the map:
* the chunk that was previously freed or the next free chunk from where previous chunk was allocated from.
*/

class SymbianJITHeap
{
public:

    SymbianJITHeap();
    ~SymbianJITHeap();

    bool IsInitialized();
    TInt GetNumAllocs();

    void* Alloc();
    void Free(void* ptr);

private:

    enum
    {
        kMaxBitLookup = 256,
        kBitsInTUint = 32,
        kPageSize = 4096,
        kMaxHeapSize = kPageSize*16384, // 64Mb, should be enough for Symbian for now
        kHeapAdjustSize  = kPageSize*8, // it is likely to be cheaper to not adjust after each allocation
        kMaxMap = kMaxHeapSize / kPageSize / kBitsInTUint // kMaxMap is 512 when kMaxHeapSize is 64Mb
    };

    void SetupBitLookup();
    int GetEmptyBit(int arrayPos);
    void ClearBit(int mapPos);
    void SetBit(int mapPos);
    int GetEmptyChunkIndex();
    void SetEmptyChunkIndex(int index);

    TInt m_previousFree;
    TInt m_currentMaxMem;
    TUint m_map[kMaxMap];
    TUint8 m_bitLookup[kMaxBitLookup];
    RChunk m_chunk;
    TUint m_startAddr;
    TInt m_numAllocs;
};


#endif __SymbianJITHeap__
