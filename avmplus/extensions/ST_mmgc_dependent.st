// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Bugzilla 543560 - here we risk deleting an object that is still on the mark stack because
// of how we perform large-object splitting.  The setup is that user code that deletes the object
// gets to run after the first part of the large object has been popped off the mark stack
// but before the rest has been handled.

%%component mmgc
%%category  dependent

%%prefix
using namespace MMgc;

static const size_t nbytes = 16*1024*1024;

class DependentAllocHolder : public GCFinalizedObject {
public:

  DependentAllocHolder() {
    memory = (char*)FixedMalloc::GetFixedMalloc()->Alloc(nbytes);
    GC::GetGC(this)->SignalDependentAllocation(nbytes);
  }

  virtual ~DependentAllocHolder() {
    FixedMalloc::GetFixedMalloc()->Free(memory);
    memory = NULL;
    GC::GetGC(this)->SignalDependentDeallocation(nbytes);
  }
  private:
    char* memory;
};

%%decls
private:
    MMgc::GC *gc;

%%prologue
    GCConfig config;
    gc = new GC(GCHeap::GetGCHeap(), config);

%%epilogue
    delete gc;

%%test dependent_alloc
    size_t maxheap = 0;
    {
        MMGC_GCENTER(gc);

        int count = 100;
        for (int c=0; c<count; c++) {
            (void)(new (gc) DependentAllocHolder());
            size_t heapsize = gc->policy.blocksOwnedByGC() * GCHeap::kBlockSize;
            // printf("%lu\n", (unsigned long)heapsize);
            if (heapsize > maxheap)
                maxheap = heapsize;
        }
    }

    // This is tricky to get right but for this test the 16MB blocks will dominate
    // completely.  So assume that heap size must stay below L*2*16MB for the
    // L that applies at 32MB.

%%verify size_t(gc->policy.queryLoadForHeapsize(double(2*nbytes)) * 2.0 * double(nbytes)) >= maxheap

%%test dependent_memory_total
#if defined VMCFG_TELEMETRY && defined AVMSHELL_BUILD
    size_t depMem_start,depMem_end;
    depMem_start = depMem_end = 0;
    MMgc::GC* gc = core->gc;

    // Check total dependent memory consistency
    for(int i = 0; i < MMgc::typeCount; i++)
        depMem_start += gc->getDependentMemory((MMgc::DependentMemoryType)i);
    %%verify depMem_start == gc->policy.dependentAllocation

    // Check byteArray type dependent memory
    avmshell::ShellCore* c = (avmshell::ShellCore*)core;
    avmshell::ShellToplevel* top = c->shell_toplevel;

    size_t byteArray_bytes1 = gc->getDependentMemory(MMgc::typeByteArray);
    ByteArrayObject* byteArray = top->byteArrayClass()->constructByteArray();
    byteArray->writeBoolean(false);
    byteArray->writeDouble(3.14);

    // Allocate known type dependent memory
    size_t byteArray_bytes2 = gc->getDependentMemory(MMgc::typeByteArray);
    %%verify byteArray_bytes2 > byteArray_bytes1

    // Allocate unknown dependent memory
    DependentAllocHolder* obj = new (gc) DependentAllocHolder();

    size_t byteArray_bytes3 = gc->getDependentMemory(MMgc::typeByteArray);
    %%verify byteArray_bytes3 == byteArray_bytes2
    byteArray->clear();
 
    %%verify gc->getDependentMemory(MMgc::typeByteArray) == byteArray_bytes1
    delete obj;

    // Consistency check
    for(int i = 0; i < MMgc::typeCount; i++)
        depMem_end += gc->getDependentMemory((MMgc::DependentMemoryType)i);
    %%verify depMem_end == gc->policy.dependentAllocation
    %%verify depMem_end == depMem_start

#if !defined DEBUG && !defined DEBUGGER
    // Get memory for out of bounds type
    // getDependentMemory has assert; skip in debug build
    %%verify gc->getDependentMemory(MMgc::typeCount) == 0
#endif
%%verify true
#endif

%%test dependent_memory_unknown
#if defined VMCFG_TELEMETRY && defined AVMSHELL_BUILD
    // Check unknown type dependent memory
    MMgc::GC* gc = core->gc;
    size_t unknownDependentMem_start = gc->getDependentMemory(MMgc::typeUnknown);
    DependentAllocHolder* obj = new (gc) DependentAllocHolder();
    size_t unknownDependentMem_end = gc->getDependentMemory(MMgc::typeUnknown);
    %%verify (unknownDependentMem_end - unknownDependentMem_start) == nbytes
    
    delete obj;
    %%verify unknownDependentMem_start == gc->getDependentMemory(MMgc::typeUnknown)
#else
%%verify true
#endif

