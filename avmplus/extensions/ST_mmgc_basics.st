// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component mmgc
%%category basics

%%methods
using namespace MMgc;
class DeleteInFinalizer : public GCFinalizedObject {
 public:
  // note "small" is a reserved identifier in Visual C++ for Windows Mobile (#defined to be 'char')
  DeleteInFinalizer(GCFinalizedObject *big, GCFinalizedObject *small_) : big(big), small_(small_) {};
  ~DeleteInFinalizer() { delete big; delete small_; }
 private:
  GCFinalizedObject *big;
  GCFinalizedObject *small_;
};

// Any small object would do
class AllocInFinalizer2 : public GCObject {
public:
    void* dummy;
};

class AllocInFinalizer : public GCFinalizedObject {
public:
    AllocInFinalizer() {}
    ~AllocInFinalizer() { new (GC::GetGC(this)) AllocInFinalizer2(); }
};

class LockableObject : public GCFinalizedObject {
public:
    LockableObject(int* counter) : counter(counter) {}
    virtual ~LockableObject() {
        *counter = *counter + 1;
    }
    int * const counter;
};

class LockerAndUnlocker
{
public:   
    static const int numlocked = 100;
    static GCObjectLock* lock[numlocked];
    static GCObjectLock* lock2[numlocked];
    static int counter;
    
    static bool createAndLockObjects(GC* gc) {
        counter = 0;
        for ( int i=0 ; i < numlocked ; i++ )
            lock[i] = gc->LockObject(new (gc) LockableObject(&counter));
        return true;
    }

    static bool lockLevel2(GC* gc) {
        for ( int i=0 ; i < numlocked ; i++ )
            lock2[i] = gc->LockObject(gc->GetLockedObject(lock[i]));
        return true;
    }

    static bool testLocksHeld(GC* gc, int level) {
        if (counter != 0)
            return false;

        int held = 0;
        for ( int i=0 ; i < numlocked ; i++ )
            held += bool(gc->GetLockedObject(lock[i]) != NULL);
        if (level > 1)
            for ( int i=0 ; i < numlocked ; i++ )
                held += bool(gc->GetLockedObject(lock2[i]) != NULL);

        if (held != level*numlocked)
            return false;
            
        return true;
    }
    
    static bool testLocksNotHeld(GC*) {
        // At least some of the destructors should have run...
        if (counter < numlocked/2)
            return false;
        return true;
    }

    static bool unlockLevel2(GC* gc) {
        for ( int i=0 ; i < numlocked ; i++ )
            gc->UnlockObject(lock2[i]);
        return true;
    }

    static bool unlockLevel1(GC* gc) {
        for ( int i=0 ; i < numlocked ; i++ ) {
#ifdef MMGC_HEAP_GRAPH
            gc->addToBlacklist(gc->GetLockedObject(lock[i]));
#endif
            gc->UnlockObject(lock[i]);
        }
        return true;
    }
};

GCObjectLock* LockerAndUnlocker::lock[numlocked];
GCObjectLock* LockerAndUnlocker::lock2[numlocked];
int LockerAndUnlocker::counter = 0;

%%decls
private:
    MMgc::GC *gc;
    MMgc::FixedAlloc *fa;
    MMgc::FixedMalloc *fm;

%%prologue
    MMgc::GCConfig config;
    gc=new MMgc::GC(MMgc::GCHeap::GetGCHeap(), config);
    if (gc==NULL) {
        MMgc::GCHeap::Init();
        gc=new MMgc::GC(MMgc::GCHeap::GetGCHeap(), config);
    }

%%epilogue
delete gc;

%%test create_gc_instance
    %%verify gc != NULL

%%test create_gc_object
    MMGC_GCENTER(gc);
    MyGCObject *mygcobject;
    mygcobject = (MyGCObject *)new (gc) MyGCObject();
    %%verify mygcobject!=NULL
    mygcobject->i=10;
    %%verify mygcobject->i==10

%%test get_bytesinuse
    MMGC_GCENTER(gc);
    MyGCObject *mygcobject;
    int inuse=(int)gc->GetBytesInUse();
    mygcobject = (MyGCObject *)new (gc) MyGCObject();
//    AvmLog("bytes in use before %d after %d\n",inuse,(int)gc->GetBytesInUse());
    %%verify gc->GetBytesInUse()==inuse + sizeof(MyGCObject) + DebugSize()
    delete mygcobject;

%%test collect
    MMGC_GCENTER(gc);
    MyGCObject *mygcobject;
    int inuse=(int)gc->GetBytesInUse();
    mygcobject = (MyGCObject *)new (gc) MyGCObject();
    %%verify (int)gc->GetBytesInUse()>inuse
    delete mygcobject;
//    AvmLog("collect: inuse=%d current=%d\n",inuse,(int)gc->GetBytesInUse());
    gc->Collect();
//    AvmLog("collect: inuse=%d current=%d\n",inuse,(int)gc->GetBytesInUse());
    %%verify (int)gc->GetBytesInUse()<=inuse

%%test getgcheap
    %%verify gc->GetGCHeap()!=NULL

%%test fixedAlloc
    MMgc::FixedAlloc *fa;
    fa=new MMgc::FixedAlloc(2048,MMgc::GCHeap::GetGCHeap());
    %%verify (int)fa->GetMaxAlloc()==0
    %%verify (int)fa->GetNumBlocks()==0
    void *data1=fa->Alloc(2048);
    %%verify MMgc::FixedAlloc::GetFixedAlloc(data1)==fa
    %%verify fa->GetBytesInUse()==DebugSize()+2048
    %%verify fa->GetItemSize()==2048
    void *data2=fa->Alloc(2048);
    %%verify MMgc::FixedAlloc::GetFixedAlloc(data2)==fa
//    AvmLog("fa->GetItemSize=%d\n",(int)fa->GetItemSize());
    %%verify (int)fa->GetItemSize()==2048
    fa->Free(data1);
    %%verify (int)fa->GetItemSize()==2048
    %%verify (int)fa->GetMaxAlloc()==1
    %%verify (int)fa->GetNumBlocks()==1
    fa->Free(data2);
    delete fa;

%%test fixedMalloc
    fm=MMgc::FixedMalloc::GetFixedMalloc();
    int start=(int)fm->GetBytesInUse();
    int starttotal=(int)fm->GetTotalSize();
//    AvmLog("fm->GetBytesInUse()=%d\n",(int)fm->GetBytesInUse());
    %%verify (int)fm->GetBytesInUse()==start
//    AvmLog("fm->GetTotalSize()=%d\n",(int)fm->GetTotalSize());
    %%verify (int)fm->GetTotalSize()==starttotal
    void *obj=fm->Alloc(8192);
//    AvmLog("fm->GetBytesInUse()=%d\n",(int)fm->GetBytesInUse());
//    %%verify fm->GetBytesInUse()==start + 8192 + MMgc::DebugSize()
//    AvmLog("fm->GetTotalSize()=%d\n",(int)fm->GetTotalSize());
//    %%verify (int)fm->GetTotalSize()==starttotal+2
    fm->Free(obj);
//    AvmLog("fm->GetBytesInUse()=%d\n",(int)fm->GetBytesInUse());
    %%verify (int)fm->GetBytesInUse()==start
//    AvmLog("fm->GetTotalSize()=%d\n",(int)fm->GetTotalSize());
    %%verify (int)fm->GetTotalSize()==starttotal
    obj=fm->Calloc(1024,10);
//    AvmLog("fm->GetBytesInUse()=%d\n",(int)fm->GetBytesInUse());
// FixedMalloc is currently (as of redux 3229) tracking large allocs using a list of
// small objects, in some debug modes.  So we can't have a precise test here.
    %%verify (int)fm->GetBytesInUse()>=start+1024*12 && (int)fm->GetBytesInUse()<=start+1024*12+64
//    AvmLog("fm->GetTotalSize()=%d\n",(int)fm->GetTotalSize());
    %%verify (int)fm->GetTotalSize()==starttotal+3
    fm->Free(obj);
    %%verify (int)fm->GetBytesInUse()==start
    %%verify (int)fm->GetTotalSize()==starttotal

%%test gcheap
    MMgc::GCHeap *gh=MMgc::GCHeap::GetGCHeap();
    int startfreeheap=(int)gh->GetFreeHeapSize();
//    %%verify (int)gh->GetTotalHeapSize()==128
//    AvmLog("gh->GetFreeHeapSize()=%d\n",(int)gh->GetFreeHeapSize());
    %%verify (int)gh->GetFreeHeapSize()==startfreeheap
//gh->Config().heapLimit = 1024;
//    %%verify (int)gh->GetTotalHeapSize()==128
//    AvmLog("gh->GetFreeHeapSize()=%d\n",(int)gh->GetFreeHeapSize());
    %%verify (int)gh->GetFreeHeapSize()==startfreeheap
    void *data = gh->Alloc(10,MMgc::GCHeap::kExpand | MMgc::GCHeap::kZero);
    %%verify (int)gh->GetTotalHeapSize()>startfreeheap
//    AvmLog("gh->GetFreeHeapSize()=%d\n",(int)gh->GetFreeHeapSize());
    gh->FreeNoProfile(data);
       
%%test gcheapAlign
    MMgc::GCHeap *gh=MMgc::GCHeap::GetGCHeap();

    // Tricky: try to provoke some internal asserts
    void *d[1000];
    for ( unsigned i=0 ; i < ARRAY_SIZE(d) ; i++ ) {
        d[i] = gh->Alloc(1);
        void *data = gh->Alloc(10,MMgc::GCHeap::flags_Alloc, 4);
        gh->Free(data);
    }
    for ( unsigned i=0 ; i < ARRAY_SIZE(d) ; i++ )
        gh->Free(d[i]);

    // 
    for ( size_t k=2 ; k <= 256 ; k *= 2 ) {
        void *data = gh->Alloc(10,MMgc::GCHeap::flags_Alloc, k);
        %%verify ((uintptr_t)data & (k*MMgc::GCHeap::kBlockSize - 1)) == 0
        %%verify gh->Size(data) == 10
        gh->Free(data);
    }

%%test gcmethods
    MMGC_GCENTER(gc);
    MyGCObject *mygcobject;
    mygcobject = (MyGCObject *)new (gc) MyGCObject();
    %%verify (MyGCObject *)gc->FindBeginningGuarded(mygcobject)==mygcobject
    %%verify (MyGCObject *)gc->FindBeginningFast(mygcobject)==mygcobject

// Bugzilla 542529 - in debug mode we would assert here due to logic flaws in the allocatr
%%test finalizerAlloc
    MMGC_GCENTER(gc);
    new (gc) AllocInFinalizer();
    gc->Collect(false);
    %%verify true

%%test finalizerDelete
    MMGC_GCENTER(gc);
    new (gc) DeleteInFinalizer(new (gc, 100) GCFinalizedObject(), new (gc) GCFinalizedObject());
    //delete m; delete m; // this verifies we crash, it does
    gc->Collect(false);
    %%verify true
    GCFinalizedObject *gcfo = new (gc) GCFinalizedObject();
    gcfo->~GCFinalizedObject();
    gcfo->~GCFinalizedObject(); // this used to be a deleteing dtor and would crash, not anymore



%%test nestedGCs
    GCConfig config;
    GC *gcb = new GC(GCHeap::GetGCHeap(), config);
    MMGC_GCENTER(gc);
    void *a = gc->Alloc(8);
    {
        MMGC_GCENTER(gcb);
        a = gcb->Alloc(8);
        {
            MMGC_GCENTER(gc);
            a = gc->Alloc(8);
        }
        a = gcb->Alloc(8);
    }
    a = gc->Alloc(8);
    // just fishing for asserts/hangs/crashes
    %%verify true
    delete gcb;

%%test collectDormantGC
    {
        GCConfig config;
        GC *gcb = new GC(GCHeap::GetGCHeap(), config);
        {
            MMGC_GCENTER(gcb);
            gcb->Alloc(8);
        }

        // this will cause a Collection in gcb
        GCHeap::SignalExternalFreeMemory();
        delete gcb;

        // just fishing for asserts/hangs/crashes
        %%verify true
    }

%%test lockObject
    {
        GCConfig config;
        GC *gc = new GC(GCHeap::GetGCHeap(), config);
        MMGC_GCENTER(gc);

        %%verify LockerAndUnlocker::createAndLockObjects(gc)
        gc->Collect();
        gc->Collect();
        %%verify LockerAndUnlocker::testLocksHeld(gc, 1)
        gc->Collect();
        gc->Collect();
        %%verify LockerAndUnlocker::lockLevel2(gc)
        gc->Collect();
        gc->Collect();
        %%verify LockerAndUnlocker::testLocksHeld(gc, 2)
        gc->Collect();
        gc->Collect();
        %%verify LockerAndUnlocker::unlockLevel2(gc)
        gc->Collect();
        gc->Collect();
        %%verify LockerAndUnlocker::testLocksHeld(gc, 1)
        gc->Collect();
        gc->Collect();
        %%verify LockerAndUnlocker::unlockLevel1(gc)

        // Bug 637695: be aware: conservative retention may foil verify below
        gc->Collect();
        gc->Collect();
        %%verify LockerAndUnlocker::testLocksNotHeld(gc)

        delete gc;
    }

%%test regression_551169
    {
        GCConfig config;
        GC *testGC = new GC(GCHeap::GetGCHeap(), config);
        {
            MMGC_GCENTER(testGC);
            testGC->StartIncrementalMark();
            // self test for tricky GCRoot deletion logic
            // do this a bunch, idea is to try to hit GetItemAbove border edge cases
            //GCMarkStack& ms = testGC->m_incrementalWork;
            for(int i=0;i<10000;i++) {
                //GCRoot *fauxRoot = new GCRoot(testGC, new char[GC::kMarkItemSplitThreshold*2], GC::kMarkItemSplitThreshold*2);
                //testGC->MarkAllRoots();
                // tail of fauxRoot is on stack
                //uintptr_t sentinel = fauxRoot->GetMarkStackSentinelPointer();
                //if(sentinel) {
                //    const void* ptr;
                //    ms.Read_RootProtector(sentinel, ptr);
                //    %%verify ptr == fauxRoot
                //    uintptr_t tail = ms.GetItemAbove(sentinel);
                //    %%verify ms.GetEndAt(tail) == fauxRoot->End()
                //    %%verify sentinel != 0
                //}
                //delete [] (char*)fauxRoot->Get();
                //delete fauxRoot;
                //if(sentinel) {
                //    %%verify ms.P(sentinel) == GCMarkStack::kDeadItem
                //    %%verify ms.GetSentinel1TypeAt(ms.GetItemAbove(sentinel)) == GCMarkStack::kDeadItem
                //}
                %%verify true
            }
            testGC->Mark();
            testGC->ClearMarkStack();
            testGC->ClearMarks();
        }
        delete testGC;
    }


%%test blacklisting
#ifdef MMGC_HEAP_GRAPH
    GCConfig config;
    GC *gc = new GC(GCHeap::GetGCHeap(), config);
    MMGC_GCENTER(gc);
    MyGCObject *mygcobject;
    mygcobject = (MyGCObject *)new (gc) MyGCObject();
    gc->addToBlacklist(mygcobject);
    gc->Collect();
    gc->removeFromBlacklist(mygcobject);
    delete gc;
#endif
    %%verify true

%%test get_bytesinusefast
    MMGC_GCENTER(gc);
    MyGCObject *mygcobject;
    mygcobject = (MyGCObject *) new (gc) MyGCObject();
    %%verify gc->GetBytesInUse() == gc->GetBytesInUseFast()
    delete mygcobject;
    %%verify gc->GetBytesInUse() == gc->GetBytesInUseFast()
    gc->Collect();
    %%verify gc->GetBytesInUse() == gc->GetBytesInUseFast()
    MyGCLargeObject *mygclargeobject;
    mygclargeobject = (MyGCLargeObject *) new (gc) MyGCLargeObject();
    %%verify gc->GetBytesInUse() == gc->GetBytesInUseFast()
    delete mygclargeobject;
    %%verify gc->GetBytesInUse() == gc->GetBytesInUseFast()
    gc->Collect();
    %%verify gc->GetBytesInUse() == gc->GetBytesInUseFast() 
     
