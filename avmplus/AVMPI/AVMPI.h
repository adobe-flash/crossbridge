/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __avmplus_AVMPI__
#define __avmplus_AVMPI__

#include "VMPI.h"

// Include feature settings.  These also perform platform sniffing if platform
// settings are not provided eg on the command line.

#ifdef AVMSHELL_BUILD
  #include "avmshell-features.h"
#else
  /* The embedding host must provide this file in some directory that's included in
   * header search paths.  It must define a value (0 or 1) for every feature
   * required by avmplus, see shell/avmshell-features.h for a model, also see
   * core/avmfeatures.as for documentation of the feature system.
   */
  #include "avmhost-features.h"
#endif

// Include the feature system here so that the platform files can depend on
// the internal (VMCFG_, etc) names rather than feature names.  Eases
// maintainability.

#include "avmfeatures.h"

#if AVMSYSTEM_WIN32
  #include "win32/win32-platform.h"
#elif AVMSYSTEM_UNIX
  #include "unix/unix-platform.h"
#elif AVMSYSTEM_MAC
  #include "mac/mac-platform.h"
#elif AVMSYSTEM_SYMBIAN
  #include "symbian/symbian-platform.h"
#endif

/* wchar is our version of wchar_t, since wchar_t is different sizes
 on different platforms, but we want to use UTF-16 uniformly. */
typedef uint16_t wchar;

#include "AvmAssert.h"

/**
* Method to retrieve number of VM pages in virtual address space of a process
* @return number of pages
* @see VMPI_getVMPageSize()
*/
extern size_t       AVMPI_getPrivateResidentPageCount();

/**
* Method to find whether the platform supports merging of contiguous memory regions from heap
* @return true if heap merging is supported, false otherwise
*/
extern bool         AVMPI_canMergeContiguousHeapRegions();

/**
 * @return true if AVMPI_commitMemory can operate on a memory range that contains a
 * mixture of decommitted and already-committed memory, /and/ if AVMPI_decommitMemory
 * similarly can operate on a memory range that contains a mixture of committed
 * and already-decommitted memory.
 *
 * A no-op if virtual memory is not to be used.
 *
 * True on most platforms, known to be false on Symbian.
 */
extern bool AVMPI_canCommitAlreadyCommittedMemory();

/**
* This method is used to reserve region(s) of memory, i.e. one or more memory pages, in the system's virtual address space
* This method is recommended to be supported on platforms with virtual memory
* @param address optional argument, indicating the base address of the region to be reserved.
* The system should treat this address as a hint and try to reserve a region as close to this address as possible
* If NULL, the system can determine the address of the region.
* @param size size of region, in bytes, to reserve
* @return point to base address of the reserved region, NULL otherwise
* @see AVMPI_ReleaseMemoryRegion(), AVMPI_CommitMemory()
*/
extern void*        AVMPI_reserveMemoryRegion(void* address, size_t size);

/**
* This method is called to release region(s) of memory previously reserved.
* @param address base address of the region to be released
* @param size size, in bytes, of the region
* @return true if the function succeeds, false otherwise
*/
extern bool         AVMPI_releaseMemoryRegion(void* address, size_t size);

/**
* This method is used to commit a pages(s) of memory in a previously reserved region
* @param address base address of the memory region to commit
* @param size size, in bytes, of the memory to commit
* @return true if the function succeeds, false otherwise
* @see AVMPI_ReserveMemoryRegion()
*/
extern bool         AVMPI_commitMemory(void* address, size_t size);

/**
* This method is used to de-commit a pages(s) of memory that were previously commited
* @param address base address of the memory region to decommit
* @param size, in bytes, of the memory region to decommit
* @return true if the function succeeds, false otherwise
* @see AVMPI_CommitMemory()
*/
extern bool         AVMPI_decommitMemory(char *address, size_t size);

/**
 * Allocate memory for jitted code.
 *
 * This method may have platform-specific constraints, which must be known
 * to the caller and which, if violated, may cause this method to abort the
 * process.  Typical constraints would relate to the maximum size that can be
 * allocated and the allowable size increments (eg, 4KB, 8KB).  This is a
 * security matter and constraints should not be removed willy-nilly.
 *
 * Generally speaking everything is fine if memory is allocated in multiples of
 * the VM page size (so long as the platform does not have constraints on how many
 * pages can be allocated at a time).
 *
 * On some platforms code for JIT memory is separate from other memory; on others,
 * it can be mixed into the heap memory.  On the latter platforms it is explicitly
 * allowed for AVMPI_allocCodeMemory to call GCHeap::Alloc to obtain memory.
 *
 * This method /must/ call GCHeap::SignalCodeMemoryAllocation /before/ it
 * attempts to allocate memory.  The second argument to that function /must/
 * indicate whether GCHeap or native APIs will be used for the allocation.
 * That call may result in out-of-memory handling mechanisms being invoke
 * in order to keep total heap occupancy below preset limits.
 *
 * This method /must/ call GCHeap::SignalOOMEvent if a native API for code
 * allocation fails to allocate memory.  See documentation for that method
 * for how to implement retries.
 *
 * If AVMPI_makeCodeMemoryExecutable(..., true) is a no-op due to platform
 * constraints then the returned memory will be protected RWX.
 *
 * @param size  Size in bytes.
 * @return  A pointer to the allocated memory, never NULL.
 *
 * @exceptions  May abort the process if its constraints are violated.
 */
extern void* AVMPI_allocateCodeMemory(size_t size);

/**
 * Free memory that was allocated by AVMPI_allocateCodeMemory.
 *
 * @param address  The value returned from AVMPI_allocCodeMemory.
 * @param size  The argument that was passed to AVMPI_allocCodeMemory when address
 * was returned.
 *
 * If AVMPI_makeCodeMemoryExecutable(..., false) is a no-op due to platform
 * constraints then this AVMPI_freeCodeMemory should attempt to make the
 * operating system unmap the memory so that executable writable memory is not
 * sitting around unused.
 *
 * If AVMPI_makeCodeMemoryExecutable is called on this memory it must be
 * called again with makeItSo false before calling AVMPI_freeCodeMemory.
 *
 * @exceptions  May abort the process if address was not returned by
 * AVMPI_allocateCodeMemory or size was not the size value passed to
 * AVMPI_allocateCodeMemory.
 */
extern void AVMPI_freeCodeMemory(void* address, size_t size);

/**
 * AVMPI_makeCodeMemoryExecutable makes a range of code memory executable (RX)
 * or not (RW).
 *
 * This operation may be a no-op on some platforms, see comments to
 * AVMPI_allocateCodeMemory and AVMPI_freeCodeMemory.
 *
 * Memory allocated with AVMPI_allocateCodeMemory must be returned to the
 * non-executable state before calling VMP_freeCodeMemory.
 *
 * @param address  Pointer to start of an acceptable protection boundary (typically
 * a VM page) within the block.
 * @param size  Size in bytes of the area to be protected in some acceptable
 * granule (typically one or more VM pages).
 * @param  makeItSo  If true then memory is made executable, otherwise it is made
 * not-executable.
 *
 * @exceptions  May abort the process if its constraints are violated.
 *
 * @note If AVMPI_allocateCodeMemory uses GCHeap for its storage then it is possible
 * that the memory block pointed to by address could be acquired from more than
 * one call to AVMPI_reserveMemoryRegion.  The implementation of this method is
 * expected to handle such cases and identify the region boundaries if the
 * underlying system requires setting the protection flags on blocks individually
 * if they were allocated separately.
 */
extern void AVMPI_makeCodeMemoryExecutable(void *address, size_t size, bool makeItSo);

/**
* This method is used to allocate a block of memory with the base address aligned to the system page size
* This method should be implemented in systems that do not have virtual memory in lieu of APIs to reserve and commit memory regions
* @param size size, in bytes, of the block of memory to allocate
* @return pointer to start of the memory block if allocation was successful, NULL otherwise
* @see AVMPI_ReleaseAlignedMemory()
*/
extern void*        AVMPI_allocateAlignedMemory(size_t size);

/**
* This method is used to release a block of memory via AVMPI_AllocateAlignedMemory
* @param address pointer to the start of the memory block to be released
* @return none
*/
extern void         AVMPI_releaseAlignedMemory(void* address);

/**
* This method is used to determind should MMgc zero initalize newly allocated memory,
* either allocated with AVMPI_commitMemory or AVMPI_allocateAlignedMemory.
* @return false if the memory is zero initialized by the OS, otherwise true
*/
extern bool         AVMPI_areNewPagesDirty();

/**
* Method to obtain the stack backtrace
* Used by the MMgc memory profiler to get call stack information
* @param buffer buffer to fill the call stack data with
* @param bufferSize size, in bytes, of the buffer passed
* @param framesToSkip number of function frames to skip from the start of trace
* @return true if back trace was captured successfully, false otherwise
*/
extern bool         AVMPI_captureStackTrace(uintptr_t* buffer, size_t bufferSize, uint32_t framesToSkip);

/**
* Method to retrieve the name of the method/function given a specific address in code space
* Used by the MMgc memory profiler to get and display function names
* This method is expected to write a null terminated string representing the function name
* in to the buffer
* @param pc address whose corresponding function name should be returned
* @param buffer buffer to write the function name to
* @param bufferSize size, in bytes, of the buffer passed
* @return true if the function name was retrieved, false otherwise
*/
extern bool         AVMPI_getFunctionNameFromPC(uintptr_t pc, char *buffer, size_t bufferSize);

/**
* Method to retrieve the source filename and line number given a specific address in a code space
* Used by the MMgc memory profiler to display location info of source code
* This method is expected to write a null terminated string representing the file name
* in to the buffer
* @param pc address of code whose corresponding location should be returned
* @param buffer buffer to write the filename to
* @param bufferSize size, in bytes, of the buffer for filename
* @param out param to write the line number to
* @return true if the file and line number info was retrieved successfully, false otherwise
*/
extern bool         AVMPI_getFileAndLineInfoFromPC(uintptr_t pc, char *buffer, size_t bufferSize, uint32_t* lineNumber);

/**
 * can two consecutive calls to AVMPI_reserveMemoryRegion be freed with one VMP_releaseMemoryRegion call?
 * @return true if it can
 */
extern bool AVMPI_canMergeContiguousRegions();

/**
 * are the virtual memory API's implemented and usable for this platform/OS?
 * @return true if they are
 */
extern bool AVMPI_useVirtualMemory();

/**
* Method to check whether MMgc memory profiling is turned on or not
* @return true if profiling is enabled, false otherwise
*/
extern bool AVMPI_isMemoryProfilingEnabled();

/**
 * Method to setup a spy channel on MMgc/avmplus
 * If the platform intends to periodically retrieve information from MMgc/avmplus
 * then it can perform the necessary setup during this function call and return true
 * @return true if the spy is setup successfully else false
 * @see AVMPI_spyCallback
*/
extern bool AVMPI_spySetup();
extern void AVMPI_spyTeardown();
/**
 * Callback method for spy
 * Currently called on every allocation in MMgc if AVMPI_spySetup returned true
 * @return none
*/
extern void AVMPI_spyCallback();

/*
 * Method to perform any initialization activities to assist
 * the program counter to symbols resolution for MMgc memory profiler
 * @return none
 */
extern void AVMPI_setupPCResolution();

/**
 * Method to perform any cleanup of items that were created/setup
 * for program counter to symbols resolution for MMgc memory profiler
 * @return none
 * @see VMPI_setupPCResolution
 */
extern void AVMPI_desetupPCResolution();

/* Compute the highest address of stack memory that the calling thread will use.
 *
 * @note MMgc assumes the stack grows down.
 */
extern uintptr_t AVMPI_getThreadStackBase();

/**
 * Clean from the current stack pointer amt bytes
 * @param the amout of bytes to clear
 */
extern void AVMPI_cleanStack(size_t amt);

/**
 * MEthod to determine whether we have access to symbol information
 * @return a bool indicating whether we have
*/
extern bool AVMPI_hasSymbols();

#endif /* __avmplus_AVMPI__ */
