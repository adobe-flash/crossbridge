/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __SymbianHeap__
#define __SymbianHeap__

#include <e32std.h>

class SymbianHeap
{
public:

    SymbianHeap(TInt size);
    ~SymbianHeap();
    bool Commit(TUint addr, TUint size);
    bool Decommit(TUint addr, TUint size);
    TUint GetStart();
    TUint GetEnd();
    static SymbianHeap* FindHeap(TUint addr, SymbianHeap* heap);

private:

    RChunk m_chunk;
    TUint m_startAddr;
    TUint m_endAddr;

public:

    SymbianHeap* m_next;
};

#endif __SymbianHeap__
