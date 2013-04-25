// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component mmgc
%%category gcheap

%%methods
using namespace MMgc;

%%test largeAlloc
       GCHeap *heap = GCHeap::GetGCHeap();
       for(int i=1;i<4;i++) {
          void *item = heap->Alloc(GCHeap::kOSAllocThreshold*i);
          %%verify heap->Size(item) == GCHeap::kOSAllocThreshold*i
          heap->Free(item);
       }
       %%verify true

%%test largeAllocAlignment
       GCHeap *heap = GCHeap::GetGCHeap();
       for(int i=1;i<10;i++) {
          void *item = heap->Alloc(GCHeap::kOSAllocThreshold*i, GCHeap::flags_Alloc, 1<<i);
          %%verify heap->Size(item) == GCHeap::kOSAllocThreshold*i
          %%verify uintptr_t(((GCHeap::kBlockSize<<(i-1))-1) & uintptr_t(item)) == 0
          heap->Free(item);
       }

