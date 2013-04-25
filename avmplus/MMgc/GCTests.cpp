/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#include "MMgc.h"

#ifdef _MSC_VER
// "behavior change: an object of POD type constructed with an initializer of the form () will be default-initialized"
#pragma warning(disable:4345) // b/c GCObject doesn't have a ctor
#ifdef _DEBUG
// we turn on exceptions in DEBUG builds
#pragma warning(disable:4291) // no matching operator delete found; memory will not be freed if initialization throws an exception
#endif
#endif

#ifdef _DEBUG

namespace MMgc
{
    GC *gc;

    void collect()
    {
        gc->Collect();
    }

    GCWeakRef* createWeakRef(int extra=0)
    {
        // Bogusly use up some extra stack.
        //
        // Certain compilers/platforms require this (at least gcc/MacIntel).
        // A pointer to the temporary GCObject can be left on the stack, so
        // far down that CleanStack doesn't dare touch it.  Leaving the
        // pointer there causes the temporary to be marked, and not collected,
        // which causes tests to fail with assertions.
        ///
        // The extra 64 bytes here causes the temporary to end up higher on
        // the stack (numerically lesser address, on Intel at least) where
        // CleanStack will clobber it.
        //
        char buf[64];
        VMPI_sprintf(buf, "%d", extra);  // don't optimize away buf

        return (new (gc, extra) GCObject())->GetWeakRef();
    }

    void weakRefSweepSmall()
    {
        GCWeakRef *ref = createWeakRef();
        collect();
        gc->CleanStack(true);
        collect();
        (void)ref;
        GCAssert(ref->isNull());
    }

    void weakRefSweepLarge()
    {
        GCWeakRef *ref = createWeakRef(5000);
        collect();
        gc->CleanStack(true);
        collect();
        (void)ref;
        GCAssert(ref->isNull());
    }

    void weakRefFreeSmall()
    {
        GCWeakRef *ref = createWeakRef();
        delete ref->get();
        GCAssert(ref->isNull());
    }

    void weakRefFreeLarge()
    {
        GCWeakRef *ref = createWeakRef(5000);
        delete ref->get();
        GCAssert(ref->isNull());
    }

    class RCObjectAddRefInDtor : public RCObject
    {
    public:
        RCObjectAddRefInDtor(RCObject ** _stackPinners, int _length) : rcs(_stackPinners), length(_length) {}
        ~RCObjectAddRefInDtor()
        {
            // whack these, used create freelist
            for(int i=0, n=length; i<n;i++)
            {
                r1 = rcs[i];
            }

            // add/remove myself (this was the apollo bug)
            r1 = this;
            r1 = NULL;
            rcs = NULL;
            length = 0;
        }
        GCMember<RCObject> r1;

        // naked pointer so I can kick these pinners out out of the ZCT during reap
        RCObject **rcs;
        int length;
    };

    GCWeakRef* createProblem(RCObject **stackPinners)
    {
        // now create one that causes some removes from the dtor
        return (new (gc) RCObjectAddRefInDtor(stackPinners, 3))->GetWeakRef();
    }

    /* see bug 182420 */
    void drcApolloTest()
    {
        // prime ZCT with some pinners
        RCObject *stackPinners[3];
        VMPI_memset(stackPinners, 0, sizeof(stackPinners));

        GCWeakRef *wr = createProblem(stackPinners);

        stackPinners[0] = new (gc) RCObject();
        stackPinners[1] = new (gc) RCObject();
        stackPinners[2] = new (gc) RCObject();

        // force ZCT
        for(int i=0, n=1000;i<n; i++)
        {
            new (gc) RCObject();
        }

        // it may still be alive if we had a dirty stack
        if(!wr->isNull())
            delete wr->get();
    }

    // See comments in header file regarding what might happen if this is called.
    void RunGCTests(GC *g)
    {
        gc = g;
        weakRefSweepSmall();
        weakRefSweepLarge();
        weakRefFreeSmall();
        weakRefFreeLarge();
        drcApolloTest();
    }
}

#endif // _DEBUG
