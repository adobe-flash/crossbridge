/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <e32std.h>
#include <hal.h>
#include "MMgc.h"
#include "AvmDebug.h" // for AvmAssert
#include "SymbianHeap.h"

size_t VMPI_getVMPageSize()
{
    TInt pageSize;
    HAL::Get(HAL::EMemoryPageSize, pageSize);
    return pageSize;
}

bool AVMPI_canMergeContiguousRegions()
{
    return false;
}

bool AVMPI_canCommitAlreadyCommittedMemory()
{
    return false;
}

bool AVMPI_useVirtualMemory()
{
    return true;
}

bool AVMPI_areNewPagesDirty()
{
    return true;
}

static SymbianHeap* symbianHeapListRoot = 0;

void* AVMPI_reserveMemoryRegion(void* address, size_t size)
{
    // After reading the RChunk API it looks like we can not allocate contiguous memory.
    // Create a new heap when address is null. Otherwise return null which MMgc should handle.

    // Open question is could we instead just create one SymbianHeap instance and always allocate
    // from there and ignore reserverMemoryRegion and releaseMemoryRegion. However as each region
    // should be contiguous it's maybe easier just to create multiple regions = heaps.
    void* ptr = 0;
    if(address == NULL)
    {
        SymbianHeap* newHeap = new SymbianHeap(size);
        if(newHeap && newHeap->GetStart())
        {
            newHeap->m_next = symbianHeapListRoot;
            symbianHeapListRoot = newHeap;
            ptr = (void*)newHeap->GetStart();
        } else
        {
            delete newHeap;
        }
    }
    return ptr;
}

bool AVMPI_releaseMemoryRegion(void* address, size_t size)
{
    SymbianHeap* heap = SymbianHeap::FindHeap((TUint)address, symbianHeapListRoot);
    if(heap)
    {
        // remove from list
        SymbianHeap* list = symbianHeapListRoot;
        SymbianHeap* prev = NULL;
        while(list)
        {
            if(list == heap)
            {
                if(prev)
                {
                    prev->m_next = heap->m_next;
                } else
                {
                    symbianHeapListRoot = heap->m_next;
                }
                delete heap;
                break;
            }
            prev = list;
            list = list->m_next;
        }
        return true;
    }
    return false;
}

bool AVMPI_commitMemory(void* address, size_t size)
{
    bool result = false;
    SymbianHeap* heap = SymbianHeap::FindHeap((TUint)address, symbianHeapListRoot);
    if(heap)
    {
        result = heap->Commit((TUint)address, size);
        // FIXME https://bugzilla.mozilla.org/show_bug.cgi?id=571407
        // This needs to be investigated, rather than a simple band-aid applied.
        // TEMPORARY FIX: MMgc should zero initialize the memory, but it does not seem to do it in all cases.
        if(result)
        {
            VMPI_memset(address, 0, size);
        } else
        {
            VMPI_debugLog("Failed to allocate normal memory.\n");
        }
    }
    return result;
}

bool AVMPI_decommitMemory(char *address, size_t size)
{
    bool result = false;
    SymbianHeap* heap = SymbianHeap::FindHeap((TUint)address, symbianHeapListRoot);
    if(heap)
    {
        result = heap->Decommit((TUint)address, size);
    }
    return result;
}

#if 0
void* AVMPI_allocateAlignedMemory(size_t size)
{
    char *ptr, *ptr2, *aligned_ptr;
    size_t align_size = VMPI_getVMPageSize();
    size_t align_mask = align_size - 1;

    size_t alloc_size = size + align_size + sizeof(int);
    ptr = (char *)malloc(alloc_size);

    if(ptr==NULL) return(NULL);

    ptr2 = ptr + sizeof(int);
    aligned_ptr = ptr2 + (align_size - ((size_t)ptr2 & align_mask));

    ptr2 = aligned_ptr - sizeof(int);
    *((int *)ptr2)=(int)(aligned_ptr - ptr);

    return(aligned_ptr);

}
void AVMPI_releaseAlignedMemory(void* address)
{
    int *ptr2=(int *)address - 1;
    char *unaligned_ptr = (char*) address - *ptr2;
    free(unaligned_ptr);
}
#endif // 0

void AVMPI_releaseAlignedMemory(void* address)
{
}

void* AVMPI_allocateAlignedMemory(size_t)
{
    return 0;
}

size_t AVMPI_getPrivateResidentPageCount()
{
    size_t pageSize = VMPI_getVMPageSize();
    if(pageSize > 0)
    {
        TInt freeRAM;
        HAL::Get(HAL::EMemoryRAMFree, freeRAM);
        return freeRAM/pageSize;
    } else
    {
        return 0;
    }
}

void AVMPI_cleanStack(size_t amt)
{
    // TODO - According to Tommy it does not kill if we don't implement on Symbian.
    // It could be possible to implement using ARM assembler.
}

void AVMPI_setupPCResolution()
{
    // TODO
}

void AVMPI_desetupPCResolution()
{
    // TODO
}

uintptr_t AVMPI_getThreadStackBase()
{
    TThreadStackInfo info;
    RThread mythread;
    mythread.StackInfo(info);
    return uintptr_t(info.iBase);
}

#ifdef MMGC_MEMORY_PROFILER

    bool AVMPI_captureStackTrace(uintptr_t* buffer, size_t bufferSize, uint32_t framesToSkip)
    {
        (void) buffer;
        (void) bufferSize;
        (void) framesToSkip;
        return false;
    }

    bool AVMPI_getFunctionNameFromPC(uintptr_t pc, char *buffer, size_t bufferSize)
    {
        (void)pc;
        (void)buffer;
        (void)bufferSize;
        return false;
    }

    bool AVMPI_getFileAndLineInfoFromPC(uintptr_t /*pc*/, char */*buffer*/, size_t /*bufferSize*/, uint32_t* /*lineNumber*/)
    {
        return false;
    }

#endif //MEMORY_PROFILER
