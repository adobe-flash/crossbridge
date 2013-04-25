/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "SymbianJITHeap.h"

#include "MMgc.h" // for AvmAssert
#include "AvmDebug.h" // for AvmAssert

SymbianJITHeap::SymbianJITHeap()
{
    m_numAllocs = 0;
    m_startAddr = 0;
    m_currentMaxMem = 0;
    m_previousFree = 0;

    memset(m_map, 0, sizeof(TUint)*kMaxMap);
    SetupBitLookup();

    if(m_chunk.CreateLocalCode(0, kMaxHeapSize) == KErrNone)
    {
        m_startAddr = (TUint)m_chunk.Base();
    }
}

SymbianJITHeap::~SymbianJITHeap()
{
    m_chunk.Close();
}

bool SymbianJITHeap::IsInitialized()
{
    return m_startAddr != 0;
}

TInt SymbianJITHeap::GetNumAllocs()
{
    return m_numAllocs;
}

void* SymbianJITHeap::Alloc()
{
    int chunkIndex = GetEmptyChunkIndex();
    // FIXME https://bugzilla.mozilla.org/show_bug.cgi?id=571407
    // It is really the task of VMPI_allocateCodeMemory to shut down the Player
    // in the event of a failure (not this function).
    if(chunkIndex == -1)
        return 0; // shut down Flash Player after this!
    TUint maxMem = (chunkIndex+1) * kPageSize; // could use shift in places like this but this is easier to read
    if(maxMem > m_currentMaxMem)
    {
        AvmAssert( maxMem == ((m_numAllocs+1) * kPageSize) ); // There should not be any empty slots
        maxMem += kHeapAdjustSize; // call Adjust for more than one chunk a time to improve performance
        if(m_chunk.Adjust(maxMem) != KErrNone)
        {
            // FIXME https://bugzilla.mozilla.org/show_bug.cgi?id=571407
            // It is really the task of VMPI_allocateCodeMemory to shut down the Player
            // in the event of a failure (not this function).
            // shut down Flash Player after this!
            return 0;
        }
        m_currentMaxMem = maxMem;
    }
    SetBit(chunkIndex); // mark as allocated
    void* ptr = (void*)(m_startAddr + chunkIndex * kPageSize);
    memset(ptr, 0, kPageSize);
    m_numAllocs ++;
    return ptr;
}

void SymbianJITHeap::Free(void* ptr)
{
    // assert that it's smaller than kMaxMap
    int chunkIndex = (((TUint)ptr)-m_startAddr) / kPageSize; // // could use shift in places like this but this is easier to read
    SetEmptyChunkIndex(chunkIndex);
    m_numAllocs --;
}

void SymbianJITHeap::SetupBitLookup()
{
    for(int i=0;i<256;i++)
    {
        int v = i;
        for(int j=0;j<8;j++)
        {
            if(!(v&1))
            {
                m_bitLookup[i] = j;
                break;
            }
            v >>= 1;
        }
    }
}

int SymbianJITHeap::GetEmptyBit(int arrayPos)
{
    TUint v = m_map[arrayPos];
    for(int i=0;i<4;i++) // max 4 iterations instead of 32 because of using a lookup
    {
        TUint8 b = v & 0xff;
        if(b != 0xff)
        {
            int bit = m_bitLookup[b];
            bit = bit + (i<<3); // *8
            return bit;
        }
        v >>= 8;
    }
    return -1;
}

void SymbianJITHeap::ClearBit(int mapPos)
{
    int arrayPos = mapPos>>5; // / kBitsInTUint
    int bit = mapPos & 31;
    int v = m_map[arrayPos];
    int mask = 1<<bit;
    v &= ~mask;
    m_map[arrayPos] = v;
}

void SymbianJITHeap::SetBit(int mapPos)
{
    int arrayPos = mapPos>>5; // / kBitsInTUint
    int bit = mapPos & 31;
    int v = m_map[arrayPos];
    int mask = 1<<bit;
    v |= mask;
    m_map[arrayPos] = v;
}

int SymbianJITHeap::GetEmptyChunkIndex()
{
    int i = m_previousFree;
    while(i<kMaxMap)
    {
        if(m_map[i] != 0xffffffff) // this loop can compare 128k a time because each bit = 4k chunk
            break;
        i++;
    }
    if(i==kMaxMap)
        return -1;

    m_previousFree = i;

    int bit = GetEmptyBit(i);

    return ((i<<5)+bit);
}

void SymbianJITHeap::SetEmptyChunkIndex(int index)
{
    if(index < m_previousFree)
        m_previousFree = index;
    ClearBit(index);
}
