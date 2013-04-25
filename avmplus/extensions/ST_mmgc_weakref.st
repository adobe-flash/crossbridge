// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component mmgc
%%category weakref

%%prefix
using namespace MMgc;

class C : public GCFinalizedObject
{
public:
    C(int key) : key(key) {}
    ~C() { key = -1; }
    int key;
    GCMember<C> next;
};

class MyCallback : public GCCallback
{
public:
    MyCallback(GC* gc, GCWeakRef** refs, C** objs, int n)
        : GCCallback(gc)
        , refs(refs)
        , objs(objs)
        , n(n)
    {
    }

    virtual void presweep() {
    for ( int i=1 ; i < 1000 ; i+=2 )
        objs[i]->next = (C*)(void *)(refs[i-1]->get());
    }

private:
    GCWeakRef** refs;
    C** objs;
    int n;
};

class D : public GCFinalizedObject
{
public:
    ~D() { GC::GetWeakRef(this); }
};

%%test unmarked_object_presweep

  // Bugzilla 572331 - it's possible for a weak reference to yield up a pointer to an unmarked
  // object in a presweep handler, and for that pointer to be stored into the heap even with
  // a WB macro, without the barrier triggering.

{
    GC* gc = core->gc;

    // Stack allocated storage, so automatically these arrays are roots

    GCWeakRef* refs[1000];
    C* objs[1000];

    // Create weak and strong references to 1000 objects

    for ( int i=0 ; i < 1000 ; i++ ) {
        objs[i] = new (gc) C(i);
        refs[i] = objs[i]->GetWeakRef();
    }

    // Get the collector into a reasonable state.

    gc->Collect();
    gc->Collect();

    // Remove the even-numbered strong refs.

    for ( int i=0 ; i < 1000 ; i+=2 )
        objs[i] = NULL;

    // Introduce mischief.  The presweep callback will extract
    // pointers from the even-numbered weak refs and insert them into
    // the objects in the odd-numbered slots (which are strongly
    // held).  Most of the objects referenced from the even-numbered
    // weak refs should be weakly held and unmarked.  We hope to catch
    // the collector picking up an unmarked object.

    MyCallback* cb = new MyCallback(gc, refs, objs, 1000);

    // Now trigger the collector again.

    gc->Collect();

    // Prevent more mischief from happening.

    delete cb;

    // Now reference all the odd-numbered objects and check the
    // integrity of their 'next' objects.  There's a chance this may
    // crash.

    for ( int i=1 ; i < 1000 ; i+= 2 ) {
        %%verify objs[i]->next->key == i-1
    }

    // Clean up
    VMPI_memset(refs, 0, sizeof(refs));
    VMPI_memset(objs, 0, sizeof(objs));
}

%%explicit unmarked_object_finalize

  // Bugzilla 647155 - ditto as the previous test, but now it's the object's destructor that tries
  // to store a pointer to an unmarked object (in this case the object itself) into a weak ref.
  // This will assert in debug builds, so the test is marked "explicit" for that reason: in a
  // debug build we want to verify that the assert is hit, in a release build we want to verify that
  // the test does not crash.
  
{
    GC* gc = core->gc;

    // Stack allocated storage, so automatically these arrays are roots

    D* objs[1000];

    // Create strong references to 1000 objects

    for ( int i=0 ; i < 1000 ; i++ ) {
        objs[i] = new (gc) D();
    }

    // Get the collector into a reasonable state.

    gc->Collect();
    gc->Collect();

    // Remove the even-numbered strong refs.

    for ( int i=0 ; i < 1000 ; i+=2 )
        objs[i] = NULL;

    // Now trigger the collector again.

    gc->Collect();

%%verify true
}
