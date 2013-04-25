// -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5)
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

// Bugzilla 565631 - We occasionally interleave invoking finalizers
// and clearing mark bits in GCAlloc::Finalize; so a finalizer can
// observe a live object that does not have its mark bit set.
//
// This complicates things because we want to ensure that unmarked
// weakly-referenced objects are resurrected by the GC if the weak
// reference is dereferenced during presweep, but we do not want to
// schedule collection work (or set bits that are supposed to be
// unmarked) during finalization.
//
// (Long term we might want to get rid of the interleaving of
// finalization and mark-bit clearing.  Short term, lets just
// try to detect this on our own.)

%%component mmgc
%%category bugzilla_575631

%%prefix
using namespace MMgc;

// Upon destruction, start reading weak refs of "friends" near and far
class Snoopy : public GCFinalizedObject
{
public:
    Snoopy(int key, GCWeakRef** refs, int len)
        : key(key), friends(refs), len(len)
    {
        ++alive_count;
    }
    ~Snoopy();
    static int alive_count;
private:
    int key;
    GCWeakRef** friends;
    int len;
};

// To take D samples from an array of N elems, walk thru by floor(N/D)
// steps (but avoid the pathological case when the floor is zero).
int compute_stride(int numerator, int denominator)
{
    int delta = numerator / denominator;
    return (delta > 0) ? delta : 1;
}

%%decls
// collecting twice is only "sure" way to gc in presence of incrementality
void collect2() { core->gc->Collect(); core->gc->Collect(); }

%%methods

/*static*/ int Snoopy::alive_count = 0;

const int arr_len = 1000;
const int lookups_per_destruct = 10;
const int destructs = 10;
Snoopy::~Snoopy()
{
    int delta = compute_stride(arr_len, lookups_per_destruct);

    for ( int i = 1 ; i < arr_len ; i += delta ) {
        int idx = (key + i) % len;
        // printf("referencing ref[%d] from Snoopy(%d)\n", idx, key);
        friends[idx]->get();
    }
    --alive_count;
}

%%test drizzle
{
    GC* gc = core->gc;

    Snoopy* objs[arr_len];
    GCWeakRef* refs[arr_len];

    // initial setup:
    for (int i=0 ; i < arr_len; ++i ) {
        objs[i] = new (gc) Snoopy(i, refs, arr_len);
        refs[i] = objs[i]->GetWeakRef();
    }

    collect2();

    int delta = compute_stride(arr_len, destructs);

    for (int i=0; i < arr_len; i += delta) {
        objs[i] = NULL;
        collect2();
    }

    // not assert failing within get() is passing the test.
    %%verify 1
          ; // (make my auto-indenter happy)

    // cleanup code; letting ~Snoopy occur outside test extent is big no-no.
    {
        for (int i=0; i < arr_len; ++i ) {
            if (! refs[i]->isNull())
                delete objs[i];
        }

        // if something went wrong above and some Snoopy's are still alive,
        // we'll get burned during their destructors.  Make sure that
        // does not happen.
        %%verify (Snoopy::alive_count == 0)
              ;
    }
}
