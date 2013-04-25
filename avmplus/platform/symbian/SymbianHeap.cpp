/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "SymbianHeap.h"

SymbianHeap::SymbianHeap(TInt size)
{
    m_startAddr = 0;
    m_endAddr = 0;
    m_next = 0;
    if(m_chunk.CreateDisconnectedLocal(0, 0, size, EOwnerProcess) == KErrNone)
    {
        m_startAddr = (TUint)m_chunk.Base();
        m_endAddr = m_startAddr + size;
    }
}

SymbianHeap::~SymbianHeap()
{
    m_chunk.Close();
}

bool SymbianHeap::Commit(TUint addr, TUint size)
{
    if(m_chunk.Commit(addr - m_startAddr, size) == KErrNone)
        return true;
    else
        return false;
}

bool SymbianHeap::Decommit(TUint addr, TUint size)
{
    if(m_chunk.Decommit(addr - m_startAddr, size) == KErrNone)
        return true;
    else
        return false;
}

TUint SymbianHeap::GetStart()
{
    return m_startAddr;
}

TUint SymbianHeap::GetEnd()
{
    return m_endAddr;
}

/* static */
SymbianHeap* SymbianHeap::FindHeap(TUint addr, SymbianHeap* heap)
{
    while(heap)
    {
        if((addr >= heap->GetStart()) && (addr < heap->GetEnd()))
        {
            return heap;
        }
        heap = heap->m_next;
    }
    return NULL;
}

