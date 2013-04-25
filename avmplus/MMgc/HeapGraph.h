/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MMGC_HEAP_GRAPH__
#define __MMGC_HEAP_GRAPH__

#ifndef MMGC_HEAP_GRAPH

#define MMGC_WB_EDGE(_a, _v)

#else

#define MMGC_WB_EDGE(_a, _v) {if(GCHeap::GetGCHeap()->config.dumpFalsePositives) GetActiveGC()->mutatorGraph.edge(_a, _v); }

namespace MMgc
{
    /**
       HeapGraph represents the edges in the GC heap.
    */
    class HeapGraph
    {
    public:
        HeapGraph() {}
        ~HeapGraph() { clear(); }

        // this is called before the mutation takes place
        void edge(const void *addr, const void *newValue);
        GCHashtable *getPointers(const void *obj);
        void clear();
    private:
        // key = addr, value = container
        GCHashtable_VMPI backEdges;
    };
}
#endif // MMGC_HEAP_GRAPH

#endif /* __MMGC_HEAP_GRAPH__ */
