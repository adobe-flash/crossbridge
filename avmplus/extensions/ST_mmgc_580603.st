// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Bugzilla 580603 - adversarial mmgc: dispersive w.r.t. address space
// Bugzilla 445780 - Page map needs to be sparse
// 
// Tommy: "problems in 64-bit linux b/c VMPI_allocateAlignedMemory provides
// memory from low 32 bit-addresses, and mmap is giving us addresses from
// 0x7fxxxxxxxxxxxxxx so we need a 4 GB page map."
//
// This self-test attempts to replicate the problem described above
// (by coordinating with an adversarial gcheap) thus illustrating the
// need for a sparse page map.

%%component mmgc
%%category bugzilla_580603

// (Test only meaningful for 64-bit; more notes in prologue block below.)
%%ifdef AVMPLUS_64BIT
// (Test only meaningful for DEBUG, b/c dispersive mode only kicks in then.)
%%ifdef DEBUG

%%prefix
using namespace MMgc;

%%methods

%%decls

GC *gc;
GCHeap *heap;

size_t saved_dispersiveAdversarial;
size_t saved_heapLimit;

char *min_addr;
char *max_start;
char *max_addr;
size_t min_size;
size_t max_size;

static const int m_num_sizes = 17;
static const int m_num_iters =  2;
static const int m_init_size = 16;
static const int m_dfactor   =  2;

void print_minmax()
{
    intptr_t delta = max_addr - min_addr;
    intptr_t v = delta;
    unsigned r = 0; // r will be floor(log_2(v))

    while (v >>= 1)
        r++;

    printf("min_size: 0x%15lx max_size: 0x%15lx\n",
           (unsigned long)min_size, (unsigned long)max_size);
    printf("min_addr: 0x%15lx max_addr: 0x%15lx delta:% 15ld (>= 2^%u)\n",
           (unsigned long)min_addr, (unsigned long)max_addr, delta, r);
    fflush(NULL);
}

enum alloc_method_t { via_gc, via_heap };

const char* method_string(alloc_method_t m)
{
    switch (m) {
    case via_gc:     return "via_gc";
    case via_heap:   return "via_heap";
    default: return 0;
    }
}

char *my_alloc(size_t size, alloc_method_t m)
{
    char *ret;

    // Took heap-zeroing and heap-profiling out of flags;
    // avoid wasting time mapping in memory to initialize to 0.
    int heapFlags = (MMgc::GCHeap::kExpand);
    size_t sizeInPages = (size+(GCHeap::kBlockSize-1))/GCHeap::kBlockSize;

    switch (m) {
    case via_gc:     ret = (char*)gc->Alloc(size); break;
    case via_heap:
        ret = (char*)heap->Alloc(sizeInPages, heapFlags);
        break;
    default: ret = 0; break;
    }

    if (size < min_size)
        min_size = size;
    if (size > max_size)
        max_size = size;
    if (ret < min_addr)
        min_addr = ret;
    if (ret > max_start)
        max_start = ret;
    if (ret+size > max_addr)
        max_addr = ret+size;

    // printf("my_alloc(%10ld, %10s) => %p\n", size, method_string(m), ret);
    // fflush(NULL);
    return ret;
}

void my_free(char *p, alloc_method_t m)
{
    switch (m) {
    case via_gc:     gc->Free(p); return;
    case via_heap:   heap->FreeNoProfile(p); return;
    default: AvmAssert(p == 0); return;
    }
}

// fills recv[] with the addresses of a series of N allocated and
// freed blocks, where N = (m_num_iters * m_num_sizes).
//
// For each iteration:
//
// 1. allocate m_num_sizes blocks of distinct (and exponentially
//    growing) size.
// 2. after finishing the allocations from (1.), free the blocks
//    allocated in the *previous* iteration.
//
// The intention is to allocate blocks covering a wide range of
// sizes in order to tickle the memory subsystem's ability to
// track blocks of different sizes.  Keeping each iteration's
// blocks alive across the next iteration is meant to ensure
// that the set of allocated addresses have many wide gaps,
// stressing the memory subsystem's ability to represent
// the meta-data for disparately allocated ranges.
//
// The loop attempts to keep the block sizes distinct both within any
// particular iteration (the exponential growth) and also across the
// distinct iterations.  This was an artifact of trying to force the
// memory subsystem to be unable to return back blocks that had been
// previously freed.  It may or may not still be necessary with the
// addition of the dispersiveAdversarial gcheap config mode; it does
// not seem to hurt the effectiveness of this test, since one can
// observe OOM failures when using PageMap::Uniform using this test;
// see notes in prologue code below.

void do_allocs(char **recv, alloc_method_t m)
{
    size_t init   = m_init_size;
    int num_iters = m_num_iters;
    int num_sizes = m_num_sizes;
    int dfactor   = m_dfactor;
    for (int j=num_iters-1; j >= 0; j--) {
        for (int i=0, factor=1; i < num_sizes; i++, factor*=dfactor) {
            size_t size = factor*init+num_iters-j;
            // size_t size = factor*init;
            char *result = my_alloc(size, m);
            recv[j*num_sizes+i] = result;
        }
        if (j < num_iters - 1) {
            for (int i=0; i < num_sizes; i++) {
                my_free(recv[(j+1)*num_sizes+i], m);
            }
        }
    }
    for (int i=0; i < num_sizes; i++) {
        my_free(recv[0*num_sizes+i], m);
    }
}


%%prologue
{
    min_addr = (char*)(void*)-1;
    min_size = (size_t)(void*)-1;
    max_addr = 0;
    max_size = 0;

    // based on ST_mmgc_basics.st
    heap = MMgc::GCHeap::GetGCHeap();

    // Constants determining test parameters; see comments below.
    const int k1 = 40;
    const int k2 = 15;

    // filler that GCHeap attempts to insert between allocations,
    // in bytes (not that the precise number matters).
    saved_dispersiveAdversarial = heap->Config().dispersiveAdversarial;
    heap->Config().dispersiveAdversarial = size_t(1) << k1;
    // limit is measured in pages; 1 page = kBlockSize bytes.
    // its default value is absurdly large; cut it down to something
    // where we'll see a failure without first hosing the host machine.
    saved_heapLimit = heap->Config().heapLimit;
    heap->Config().heapLimit = ((1 << k2)-1);

    // Some pairs of (k1,k2) for filler = 2**k1 and limit = 2**k2 - 1,
    // where the (decreasing) value of k2 has just passed threshold to
    // expose OOM from PageMap::Uniform (i.e. limit = 2**(k2+1) - 1
    // won't OOM); test bed is 64-bit DEBUG avmshell on Mac OS X 10.6.4.
    //
    // k1, k2
    // ------
    // 43, 18
    // 42, 17
    // 41, 16
    // 40, 15
    // 39, 14
    // 38, 13
    // 37, 12
    // 36, 12
    // 35, 11
    // 34, 11
    //
    // For k1 < 34, test won't OOM for k2 >= 11.
    // For k2 < 11, test will OOM w/o dispersiveAdversarial at all.

    gc = core->gc;
}

%%epilogue
{
    // restore original values to limit disruption to remaining selftests
    heap->Config().heapLimit = this->saved_heapLimit;
    heap->Config().dispersiveAdversarial = saved_dispersiveAdversarial;
}

%%test alloc_loop_mmgc_viaheap_far
{
    char *h[3];
    h[0] = my_alloc( 256*GCHeap::kBlockSize, via_heap);
    h[1] = my_alloc( 256*GCHeap::kBlockSize, via_heap);
    my_free(h[0], via_heap);
    my_free(h[1], via_heap);
    // print_minmax();

    char *g[m_num_iters*m_num_sizes]; (void)g;
    do_allocs(g, via_heap);

    // print_minmax();

    %%verify 1
}

%%test alloc_loop_mmgc_viagc_far
{
    char *h[3];
    h[0] = my_alloc( 256*GCHeap::kBlockSize, via_heap);
    h[1] = my_alloc( 256*GCHeap::kBlockSize, via_heap);
    my_free(h[0], via_heap);
    my_free(h[1], via_heap);
    // print_minmax();

    char *g[m_num_iters*m_num_sizes]; (void)g;
    do_allocs(g, via_gc);

    // print_minmax();

    %%verify 1
}
