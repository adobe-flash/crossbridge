/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MMgc__
#define __MMgc__

#include "vmbase.h"

// AVMPI.h includes avmfeatures.h, which detects platforms and sets up most MMGC_ names.
#include "AVMPI.h"
#include "atom.h"

#ifdef MMGC_VALGRIND
    // Valgrind information:
    // The GCHeap, GC and FixedMalloc allocators are instrumented for valgrind's purposes.
    // All memory from the virtual memory API's is unknown to valgrind.   Only when we tell
    // valgrind about the memory using client requests does valgrind track it.
    // See valgrind headers and online manual for client request details.
    #include <valgrind/memcheck.h>
#else
    #define RUNNING_ON_VALGRIND false
    #define VALGRIND_MAKE_MEM_DEFINED(_a,_s)  {}
    #define VALGRIND_MAKE_MEM_UNDEFINED(_a,_s) {}
    #define VALGRIND_MEMPOOL_ALLOC(_p,_a,_s) {}
    #define VALGRIND_MEMPOOL_FREE(_p,_a) {}
    #define VALGRIND_CREATE_MEMPOOL(_a,_rdz,_zer) {}
    #define VALGRIND_DESTROY_MEMPOOL(_p) {}
    #define VALGRIND_MALLOCLIKE_BLOCK(_a,_s,_rdz,_zer) {}
    #define VALGRIND_FREELIKE_BLOCK(_a,rdz) {}
#endif

#ifdef DEBUG
    #define MMGC_DELETE_DEBUGGING
    // Valgrind integration is trickier with fresh memory scribbling and free memory
    // poisoning and its pointless since valgrind will uncover the same problems.
    #ifndef MMGC_VALGRIND
        #define MMGC_MEMORY_INFO
    #endif
#endif

#if defined MMGC_MEMORY_INFO && defined MMGC_MEMORY_PROFILER
    #define MMGC_RC_HISTORY
#endif

#if defined DEBUGGER || defined MMGC_MEMORY_PROFILER || defined MMGC_MEMORY_INFO
    #ifndef MMGC_HOOKS
        #define MMGC_HOOKS
    #endif
#endif

// MMGC_MARKSTACK_ALLOWANCE allows host code to limit the size of the mark stack,
// and thus to test the overflow handling.

#ifdef AVMSHELL_BUILD
    #define MMGC_MARKSTACK_ALLOWANCE
#endif

// Policy profiler settings (for MMgc development mostly).
//
// On MacOS X desktop MMGC_POLICY_PROFILING incurs a 5% execution overhead on the
// benchmark test/performance/mmgc/gcbench.as.
//
// Enabling this and MMGC_MEMORY_INFO at the same time is probably not a good idea.
//
// If AVMSHELL_BUILD is enabled then the output can be enabled/disabled with the -gcbehavior
// switch in the shell; the output is directed to whatever channel is used by GCLog.
//
// If AVMSHELL_BUILD is disabled then the output of policy info is unconditional and is
// directed to a file gcbehavior.txt in the working directory.

#ifdef AVMSHELL_BUILD
    #define MMGC_POLICY_PROFILING
#endif

#ifdef MMGC_POLICY_PROFILING
    // Really for the specially interested!  These switches incur measurable overhead,
    // so be careful when benchmarking.

    // Profile the pointer density of scanned memory
    //#define MMGC_POINTINESS_PROFILING

    // Profile reference count traffic.  This feature adds about 8% execution time overhead
    // to the overhead already added by MMGC_POLICY_PROFILING, on the gcbench benchmark,
    // measured on MacOS X desktop.
    //#define MMGC_REFCOUNT_PROFILING

    // Profile mark stack depth
    //#define MMGC_MARKSTACK_DEPTH
#endif

// this might be good to enable in a heightened DEBUG build, it triggers bug 561402
//#define MMGC_POISON_MEMORY_FROM_OS

#ifdef MMGC_MEMORY_PROFILER
    // For the following profilers you need source information: VMPI_getFunctionNameFromPC
    // and VMPI_getFileAndLineInfoFromPC must work.

    // Internal: profile uses of the conservative marker
    //
    // When MMGC_DELETION_PROFILER is enabled and MMGC_PROFILE=1 is defined in the
    // runtime environment the VM will record all uses of conservative marking and
    // dump the allocation sites that result in conservatively marked storage when
    // the GC is shut down.

    //#define MMGC_CONSERVATIVE_PROFILER

    // Internal: profile uses of explicit deletion
    //
    // When MMGC_DELETION_PROFILER is enabled and MMGC_PROFILE=1 is defined in the
    // runtime environment the VM will record all uses of explicit deletion of managed storage,
    // and dump a profile on GC shutdown.  These profiles are useful because we want to
    // reduce the frequency of those deletions, and tightly control the ones that
    // continue to exist.

    //#define MMGC_DELETION_PROFILER

    // Internal: profile allocations of weak reference allocations
    //
    // When MMGC_WEAKREF_PROFILER is enabled and MMGC_PROFILE=1 is defined in the
    // runtime environment the VM will record all allocations of weak references and dump
    // the allocation sites on GC shutdown.

    //#define MMGC_WEAKREF_PROFILER
#endif

#ifdef MMGC_WEAKREF_PROFILER
    // Internal: measure the lookup traffic in GCHashtable, the statistics are dumped along
    // with the statistics for MMGC_WEAKREF_PROFILER.
    #define MMGC_GCHASHTABLE_PROFILER
#endif

#ifdef MMGC_REFCOUNT_PROFILING
    #define REFCOUNT_PROFILING_ONLY(x) x
    #define REFCOUNT_PROFILING_ARG(x) , x
#else
    #define REFCOUNT_PROFILING_ONLY(x)
    #define REFCOUNT_PROFILING_ARG(x)
#endif

#ifdef MMGC_MEMORY_PROFILER
    #define MMGC_MEMORY_PROFILER_ARG(x) , x
#else
    #define MMGC_MEMORY_PROFILER_ARG(x)
#endif

#ifdef MMGC_MEMORY_INFO
    #define MEMORY_INFO_ARG(x) , x
#else
    #define MEMORY_INFO_ARG(x)
#endif


// Bugzilla 754281: MMGC_HAS_TRUSTWORTHY_GET_STACK_ENTER,
// when defined, tells the GC that we can use GC::GetStackEnter()
// (whose state is maintained by the GCAutoEnter ctor/dtor via the
// MMGC_GCENTER macros) as the basis for GC::GetStackTop().  When
// undefined, the GC will unconditionally resort to the more
// conservative (and expensive) AVMPI_getThreadStackBase().
//
// Note: If you are seeing time- or space-regressions injected
// by the more conservative AVMPI_getThreadStackBase(), then
// it may be feasible to turn MMGC_HAS_TRUSTWORTHY_GET_STACK_ENTER
// back on for your target.  However, before committing to that
// approach, you should:
// 1.) Carefully review the notes/discussion on Bugzilla 754281,
// 2.) Verify that your target always lays out stack-allocated objects in the
//     order that they are declared in the code, *and*
// 3.) Discuss the change with the MMgc team.

//#define MMGC_HAS_TRUSTWORTHY_GET_STACK_ENTER


// MMGC_FASTBITS enables a potentially faster representation of the per-block bit table. 
// The representation is fairly well hidden from most of the GC code; only GC::GetGCBits
// and GCAlloc / GCLargeAlloc take it into account.
//
// The new representation uses more space than the old because there are gaps in the
// bit table.
//
// Do not disable this.  The #define will be removed and the feature will always be
// enabled when we're certain that the effects of the feature are positive.

#define MMGC_FASTBITS

#include "GCDebug.h"
#include "GCLog.h"

/*
 * If _GCHeapLock is defined, a spin lock is used for thread safety
 * on all public API's (Alloc, Free)
 *
 * Warning:
 * We may use GCHeap for allocation on other threads, hence the
 * spinlock, but the MMgc garbage collector in general is not
 * thread-safe.
 */

#ifdef MMGC_LOCKING
#define MMGC_LOCK(_x) MMgc::GCAcquireSpinlock _lock(&_x)
#include "GCSpinLock.h"
#else
#define MMGC_LOCK(_x)
#endif

// This reverts back to the original pagemap representation.
// https://bugzilla.mozilla.org/show_bug.cgi?id=581070
//#define MMGC_USE_UNIFORM_PAGEMAP

#ifdef MMGC_HEAP_GRAPH
    #define HEAP_GRAPH_ARG(x) , x
#else
    #define HEAP_GRAPH_ARG(x)
#endif

namespace MMgc
{
    class GC;
    class RCObject;
    class GCWeakRef;
    class GCFinalizedObject;
    class GCObject;
    class Cleaner;
    class GCAlloc;
    class GCHeap;
    class GCTraceableBase;
}

#define CAPACITY(T)  (uint32_t(GCHeap::kBlockSize) / uint32_t(sizeof(T)))

#include "StaticAssert.h"
#include "GCTypes.h"
#include "AllocationMacros.h"
#include "OOM.h"
#include "GCStack.h"
#include "GCThreads.h"
#include "GCAllocObject.h"
#include "GCHashtable.h"
#include "GCMemoryProfiler.h"
#include "GCThreadLocal.h"
#include "FixedAlloc.h"
#include "FixedMalloc.h"
#include "GCGlobalNew.h"
#include "BasicList.h"
#include "GCHeap.h"
#include "PageMap.h"
#include "GCAlloc.h"
#include "GCLargeAlloc.h"
#include "ZCT.h"
#include "HeapGraph.h"
#include "GCPolicyManager.h"
#include "WriteBarrier.h"
#include "GCRef.h"
#include "GCMemberBase.h"
#include "GC.h"
#include "GCObject.h"
#include "GCWeakRef.h"

#include "Shared-inlines.h"
#include "GCHashtable-inlines.h"
#include "FixedAlloc-inlines.h"
#include "FixedMalloc-inlines.h"
#include "PageMap-inlines.h"
#include "GCHeap-inlines.h"
#include "GCAlloc-inlines.h"
#include "GCLargeAlloc-inlines.h"
#include "ZCT-inlines.h"
#include "GCPolicyManager-inlines.h"
#include "GC-inlines.h"
#include "GCStack-inlines.h"
#include "WriteBarrier-inlines.h"
#include "GCMember-inlines.h"
#include "GCRef-inlines.h"

// remove these when the player stops using them
#define MMGC_DRC
#define WRITE_BARRIERS

// DRC_TRIVIAL_DESTRUCTOR is an #ifdef you can use to mark destructors
// of subclasses of RCObject that /only/ zero out fields, including those
// that store NULL pointers in RC fields.  Those destructors will be 
// removed if and when that type is made not to inherit from RCObject.
#define DRC_TRIVIAL_DESTRUCTOR

#endif /* __MMgc__ */

