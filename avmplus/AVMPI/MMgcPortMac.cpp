/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "MMgc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#ifdef MMGC_MEMORY_PROFILER
#include <dlfcn.h>
#include <cxxabi.h>
#include <mach-o/dyld.h>
#endif

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/sysctl.h>

#define MAP_ANONYMOUS MAP_ANON

static const int kOSX105 = 9;

bool AVMPI_canMergeContiguousRegions()
{
    return AVMPI_useVirtualMemory();
}

bool AVMPI_canCommitAlreadyCommittedMemory()
{
    return true;
}

bool AVMPI_areNewPagesDirty()
{
#ifdef MMGC_POISON_MEMORY_FROM_OS
    return true;
#else
    return false;
#endif
}

static int get_major_version()
{
    int mib[2];
    mib[0]=CTL_KERN;
    mib[1]=KERN_OSRELEASE;
    char buf[10];
    size_t siz=sizeof(buf);
    sysctl(mib, 2, &buf, &siz, NULL, 0);
    return strtol(buf, 0, 10);
}

bool AVMPI_useVirtualMemory()
{
#ifdef MMGC_64BIT
    return true;
#else
    return get_major_version() >= kOSX105;
#endif
}

static int get_mmap_fdes(int delta)
{
    // ensure runtime version
    if(get_major_version() >= kOSX105)
        return VM_MAKE_TAG(VM_MEMORY_APPLICATION_SPECIFIC_1+delta);
    else
        return -1;
}

void* AVMPI_reserveMemoryRegion(void *address, size_t size)
{
    void *addr = (char*)mmap(address,
                             size,
                             PROT_NONE,
                             MAP_PRIVATE | MAP_ANONYMOUS,
                             get_mmap_fdes(0), 0);

    // the man page for mmap documents it returns -1 to signal failure.
    if (addr == (void *)-1) return NULL;

    if(address && address != addr) {
        // fail if we didn't get the right address
        AVMPI_releaseMemoryRegion(addr, size);
        return NULL;
    }
    return addr;
}

bool AVMPI_releaseMemoryRegion(void* address, size_t size)
{
    int result = munmap(address, size);
    return (result == 0);
}

bool AVMPI_commitMemory(void* address, size_t size)
{
    char *got = (char*)mmap(address,
                            size,
                            PROT_READ | PROT_WRITE,
                            MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                            get_mmap_fdes(0), 0);
#ifdef MMGC_POISON_MEMORY_FROM_OS
    if(got == address)
        memset(got, MMgc::GCHeap::FXFreshPoison, size);
#endif
    return (got == address);
}

bool AVMPI_decommitMemory(char *address, size_t size)
{
    kern_return_t result = vm_deallocate(mach_task_self(), (vm_address_t)address, size);
    if(result == KERN_SUCCESS)
    {
        result = vm_map(mach_task_self(), (vm_address_t*)&address, size, 0, FALSE, MEMORY_OBJECT_NULL, 0, FALSE, VM_PROT_NONE, VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXECUTE, VM_INHERIT_NONE);
    }
    //  return false; // for testing RemovePartialBlock
    return (result == KERN_SUCCESS);
}

void* AVMPI_allocateAlignedMemory(size_t size)
{
    void *addr = valloc(size);
#ifdef MMGC_POISON_MEMORY_FROM_OS
    GCAssert(size % VMPI_getVMPageSize() == 0);
    memset(addr, MMgc::GCHeap::FXFreshPoison, size);
#endif
    return addr;
}

void AVMPI_releaseAlignedMemory(void* address)
{
    free(address);
}

size_t AVMPI_getPrivateResidentPageCount()
{
    size_t private_pages = 0;
    task_t task = mach_task_self();

    vm_address_t addr = VM_MIN_ADDRESS;
    vm_size_t size = 0;
    while (true)
    {
        mach_msg_type_number_t count = VM_REGION_TOP_INFO_COUNT;
        vm_region_top_info_data_t info;
        mach_port_t object_name;
        kern_return_t ret;

        addr += size;

#if defined(VMCFG_64BIT) || defined(VMCFG_ARM)
        ret = vm_region_64(task, &addr, &size, VM_REGION_TOP_INFO, (vm_region_info_t)&info, &count, &object_name);
#else
        ret = vm_region(task, &addr, &size, VM_REGION_TOP_INFO, (vm_region_info_t)&info, &count, &object_name);
#endif

        if (ret != KERN_SUCCESS)
            break;
        private_pages += info.private_pages_resident;
    }
    return private_pages;
}

void AVMPI_cleanStack(size_t amount)
{
    void *space = alloca(amount);
    if(space)
        VMPI_memset(space, 0, amount);
}

uintptr_t AVMPI_getThreadStackBase()
{
    return (uintptr_t)pthread_get_stackaddr_np(pthread_self());
}

#ifdef MMGC_MEMORY_PROFILER

#ifdef MMGC_PPC

bool AVMPI_captureStackTrace(uintptr_t* buffer, size_t len, uint32_t skip)
{
    register int stackp;
    uintptr_t pc;
    asm("mr %0,r1" : "=r" (stackp));
    while(skip--) {
        stackp = *(int*)stackp;
    }
    size_t i=0;
    // save space for 0 terminator
    len--;
    while(i<len && stackp) {
        pc = *((uintptr_t*)stackp+2);
        buffer[i++]=pc;
        stackp = *(int*)stackp;
    }
    buffer[i] = 0;
    return true;
}
#endif

#ifdef MMGC_ARM
bool AVMPI_captureStackTrace(uintptr_t* buffer, size_t bufferSize, uint32_t framesToSkip)
{
    (void) buffer;
    (void) bufferSize;
    (void) framesToSkip;
    return false;
}
#endif

#if (defined(MMGC_IA32) || defined(MMGC_AMD64))

bool AVMPI_captureStackTrace(uintptr_t* buffer, size_t bufferSize, uint32_t skip)
{
    void **ebp;
#ifdef MMGC_IA32
    asm("mov %%ebp, %0" : "=r" (ebp));
#else
    asm("mov %%rbp, %0" : "=r" (ebp));
#endif

    if (skip)
        --skip;

    // our embedder (eg Safari) can have stack frames that aren't formed
    // in the way we expect, which can make us crash. so sniff to ensure we're still
    // inside the stack range, and if not, bail before trying to dereference.
    // Note that pthread_get_stackaddr_np() and pthread_get_stacksize_np() seems
    // to be poorly documented and thus it's not completely clear if they are meant to
    // apply to the current thread vs. the main thread; the fact they take a thread as
    // an argument, and anecdotal evidence online, suggests the former (which is what we want).
    // In any event, doing this check makes for less-crashy code than what we had before,
    // which is good, but if you find misbehavior here, be aware.
    pthread_t const self = pthread_self();
    uintptr_t const stacktop = uintptr_t(pthread_get_stackaddr_np(self));
    uintptr_t const stacksize = pthread_get_stacksize_np(self);
    uintptr_t const stackbot = stacktop - stacksize;

    while(skip--)
    {
        if ((uintptr_t(ebp) - stackbot) >= stacksize)
            break;
        if (!*ebp)
            break;
        ebp = (void**)(*ebp);
    }

    bufferSize--;
    size_t i=0;
    while(i<bufferSize)
    {
        if ((uintptr_t(ebp) - stackbot) >= stacksize)
            break;
        if (!*ebp)
            break;
        buffer[i++] = *((uintptr_t*)ebp+1);
        ebp = (void**)(*ebp);
    }
    buffer[i] = 0;
    return true;
}
#endif

static FILE* atos_file = NULL;

bool startATOSProcess()
{
    char cmd[PATH_MAX];
    VMPI_snprintf(cmd, sizeof(cmd), "atos -p %d 2> /dev/null", getpid());
    // thank you very much http://boinc.berkeley.edu/svn/trunk/boinc/lib/mac/mac_backtrace.cpp
    // thank you very little Apple!
    // this is required for bi-directional I/O to work
    setenv("NSUnbufferedIO", "YES", 1);
    atos_file = popen(cmd, "r+");
    setlinebuf(atos_file);
    return atos_file != NULL;
}

void AVMPI_setupPCResolution()
{
    //attempt launch of atos for the first time
    //if it fails for some reason we never reattempt it
    // silence occasional errors from atos
    if(AVMPI_isMemoryProfilingEnabled())
        startATOSProcess();
}

void AVMPI_desetupPCResolution()
{
    if(atos_file != NULL)
    {
        pclose(atos_file);
        atos_file = NULL;
    }
}

bool AVMPI_getFunctionNameFromPC(uintptr_t pc, char *buffer, size_t bufferSize)
{
    if(atos_file == NULL)
        return false;

    fprintf( atos_file, "%p\n",  (void*)pc);
    if(!fgets( buffer, bufferSize, atos_file))
        return false;

    char *eol = strchr(buffer, '\n');
    if(eol) {
        *eol = '\0';
    }

    return true;
}

bool AVMPI_getFileAndLineInfoFromPC(uintptr_t pc, char *buffer, size_t bufferSize, uint32_t* lineNumber)
{
    (void)buffer;
    (void)pc;
    (void)bufferSize;
    (void)lineNumber;
    return false;
}

#endif //MMGC_MEMORY_PROFILER
