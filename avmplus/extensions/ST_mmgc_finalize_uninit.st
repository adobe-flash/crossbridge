// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */

// Bugzilla 573737 - a throw from an argument to a constructor can
// cause an object to be allocated on the gc-heap before it has been
// fully initialized.  (In particular, its vtable could be missing.)
// Since finalizers use the virtual destructor method, a missing
// vtable is a problem.
//
// Its a little tricky to test this because the "order of evaluation
// to an operator new() to get memory and the evaluation of arguments
// to constructors is undefined"; see details below.

%%component mmgc
%%category finalize_uninit

%%prefix

using namespace MMgc;

class D : public GCFinalizedObject
{
public:
    D(int dummyArgument) { (void)dummyArgument; }
    ~D() { ++finalized; }
    static int finalized_count() { return D::finalized; }
    static void reset_finalized_count() { finalized = 0; }
    // "usual" overload of placement new used in mmgc client code
    void * operator new(size_t size, GC *gc) {
        return GCFinalizedObject::operator new(size, gc);
    }
    // hack to explicitly order new-allocation + ctor-arg eval; see below
    // (but reallys its just good ol' placement new!)
    void * operator new(size_t size, GC *gc, void *raw) {
        (void)size; (void)gc;
        return raw;
    }
private:
    static int finalized;
};

/*static*/
int D::finalized;

int deathValue(AvmCore *core)
{
    core->throwAtom(atomFromIntptrValue(1)); /* won't return */

    return 0; /* silence compiler warnings */
}

// Test checks that finalizers themselves have not broken; here,
// constructor argument subexpression evaluates cleanly.
%%test check_finalizers_still_work
{
    D::reset_finalized_count();
    D* d;
    int i;

    // loop to alloc many (and subsequently reclaim >=expected percentage)
    for (i = 0; i < 100; i++) {
        d = new (core->gc) D(1);
    }
    (void) d;

    core->gc->Collect(); // finish any prior incremental work ...
    core->gc->Collect(); // ... and ensure we got fresh + complete gc.

    // printf("D::finalized_count(): %d\n", D::finalized_count());

    %%verify (D::finalized_count() > 90)
}

// Test illustrates of the kind of code that exposed the original bug;
// here, constructor argument subexpression throws.
%%test original_death
{
    D* d;
    volatile int i;

    // Here, constructor argument subexpression throws

    // Just one allocation attempt alone would risk false retention of
    // intermediate values, so loop a bit to ensure that *some* D's,
    // if allocated at all, will be considered garbage.
    for (i = 0; i < 5; i++) {
        TRY (core, kCatchAction_Ignore) {
            d = new (core->gc) D(deathValue(core));
        }  CATCH (Exception *e) {
            (void)e;
        }
        END_CATCH
        END_TRY
    }
    (void) d;

    // if things go badly, one of the collections below will segfault
    // during finalization.
    core->gc->Collect();
    core->gc->Collect();

    // (not dying is passing.)
    %%verify true
}

// C++ standard says "The order of evaluation to an operator new() to
// get memory and the evaluation of arguments to constructors is
// undefined."
//
// Unfortunately, it is difficult to directly express the particular
// order of evaluation that exposes the bug in question, because
// allocation and construction are tied together.
//
// So, here we manually decompose the tests above to control
// evaluation order of operator new() and constructor arguments, to
// express suitable evil (namely: allocation; args eval; construction)
// for selftest.
//
// The desugaring iteself is:
// desugar[[ new (gc-exp) D(arg-exp) ]]
// ==> mem = D::operator new(sizeof(D), arg), new (gc-exp, mem) D(arg-exp)

// Test illustrates desugaring is "sound"; keep in sync with
// check_finalizers_still_work above.
%%test desugared_check_finalizers_still_work
{
    D::reset_finalized_count();
    D* d;
    int i;

    // loop to alloc many (and subsequently reclaim >=expected percentage)
    for (i = 0; i < 100; i++) {
        // d = new (core->gc) D(s);
        void *mem = D::operator new(sizeof(D), core->gc);
        d = new (core->gc, mem) D(1);
    }
    (void) d;

    core->gc->Collect(); // finish any prior incremental work ...
    core->gc->Collect(); // ... and ensure we got fresh + complete gc.

    // printf("D::finalized_count(): %d\n", D::finalized_count());

    %%verify (D::finalized_count() > 90)
}


// Test forces evil order of evaluation via desugaring of
// construction; keep in sync with original_death above.
%%test desugared_death
{
    D* d;
    volatile int i;

    // Here, constructor argument subexpression throws

    // Just one allocation attempt alone would risk false retention of
    // intermediate values, so loop a bit to ensure that *some* D's,
    // which are forcibly allocated here, will be considered garbage.
    for (i = 0; i < 5; i++) {
        TRY (core, kCatchAction_Ignore) {
            // d = new (core->gc) D(deathValue());
            void *mem = D::operator new(sizeof(D), core->gc);
            d = new (core->gc, mem) D(deathValue(core));
        }  CATCH (Exception *e) {
            (void)e;
        }
        END_CATCH
        END_TRY
    }
    (void) d;

    // if things go badly, one of the collections below will segfault
    // during finalization.
    core->gc->Collect();
    core->gc->Collect();

    // (not dying is passing.)
    %%verify true
}
