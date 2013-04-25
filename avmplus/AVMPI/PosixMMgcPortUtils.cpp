/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//
// These implementations depend on MMgc (via avmplus.h)
//
#include "avmplus.h"

#include <sys/mman.h>

bool AVMPI_isMemoryProfilingEnabled()
{
    //read the mmgc profiling option switch
    const char *env = getenv("MMGC_PROFILE");
    return (env && (VMPI_strncmp(env, "1", 1) == 0));
}

// Constraint: nbytes must be a multiple of the VM page size.
//
// The returned memory will be aligned on a VM page boundary and cover
// an integral number of VM pages.  This is necessary in order for
// AVMPI_makeCodeMemoryExecutable to work properly - it too operates
// on entire VM pages.
//
// This function is duplicated in the Windows port utils, if you
// fix a bug here be sure to fix the bug there.

void *AVMPI_allocateCodeMemory(size_t nbytes)
{
    MMgc::GCHeap* heap = MMgc::GCHeap::GetGCHeap();
    size_t pagesize = VMPI_getVMPageSize();

    if (nbytes % pagesize != 0) {
#ifdef DEBUG
        char buf[256];
        VMPI_snprintf(buf,
                      sizeof(buf),
                      "AVMPI_allocateCodeMemory invariants violated: request=%lu pagesize=%lu\nAborting.\n",
                      (unsigned long)nbytes,
                      (unsigned long)pagesize);
        VMPI_log(buf);
#endif
        VMPI_abort();
    }

    size_t nblocks = nbytes / MMgc::GCHeap::kBlockSize;

    heap->SignalCodeMemoryAllocation(nblocks, true);
    return heap->Alloc(nblocks, MMgc::GCHeap::flags_Alloc, pagesize/MMgc::GCHeap::kBlockSize);
}

// Constraint: address must have been returned from VMPI_allocateCodeMemory
// and nbytes must be the size of the allocation.  We can't quite check
// this, so we check that the address points to a page boundary and that
// the size is given as an integral number of VM pages and that the size
// corresponds to GCHeap's notion of the size.
//
// Usage note: on Posix, where the memory goes back into the common pool
// and isn't unmapped by the OS, it is very bad form for the client to
// free executable memory, we do not try to detect that (in DEBUG mode)
// but we probably should.
//
// This function is duplicated in the Windows port utils, if you
// fix a bug here be sure to fix the bug there.

void AVMPI_freeCodeMemory(void* address, size_t nbytes)
{
    MMgc::GCHeap* heap = MMgc::GCHeap::GetGCHeap();
    size_t pagesize = VMPI_getVMPageSize();
    size_t nblocks = heap->Size(address);
    size_t actualBytes = nblocks * MMgc::GCHeap::kBlockSize;

    if ((uintptr_t)address % pagesize != 0 || nbytes % pagesize != 0 || nbytes != actualBytes) {
#ifdef DEBUG
        char buf[256];
        VMPI_snprintf(buf,
                      sizeof(buf),
                      "AVMPI_freeCodeMemory invariants violated: address=%lu provided=%lu actual=%lu\nAborting.\n",
                      (unsigned long)address,
                      (unsigned long)nbytes,
                      (unsigned long)actualBytes);
        VMPI_log(buf);
#endif
        VMPI_abort();
    }

    heap->Free(address);
    heap->SignalCodeMemoryDeallocated(nblocks, true);
}

// Constraint: address must point into a block returned from VMPI_allocateCodeMemory
// that has not been freed, it must point to a VM page boundary, and the number of
// bytes to protect must be an integral number of VM pages.  We can't check that
// the memory was returned from VMPI_allocateCodeMemory though and we don't check
// that the memory is currently allocated.
//
// GCHeap may return memory that overlaps the boundary between two separately
// committed regions.  If that causes problems for you there are two options: either
// don't use GCHeap memory for code memory, or turn off VM support.

void AVMPI_makeCodeMemoryExecutable(void *address, size_t nbytes, bool makeItSo)
{
    size_t pagesize = VMPI_getVMPageSize();
    
    if ((uintptr_t)address % pagesize != 0 || nbytes % pagesize != 0) {
#ifdef DEBUG
        char buf[256];
        VMPI_snprintf(buf,
                      sizeof(buf),
                      "AVMPI_makeCodeMemoryExecutable invariants violated: address=%lu size=%lu pagesize=%lu\nAborting.\n",
                      (unsigned long)address,
                      (unsigned long)nbytes,
                      (unsigned long)pagesize);
        VMPI_log(buf);
#endif
        VMPI_abort();
    }
    
    int flags = makeItSo ? PROT_EXEC|PROT_READ : PROT_WRITE|PROT_READ;
    int retval = mprotect((maddr_ptr)address, (unsigned int)nbytes, flags);
    AvmAssert(retval == 0);
    (void)retval;
}
