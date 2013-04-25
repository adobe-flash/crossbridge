// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component mmgc
%%category exact

%%prefix

class Cthulhu : public MMgc::GCTraceableObject
{
public:
    static Cthulhu* create(MMgc::GC* gc) {
        return new (gc, MMgc::kExact) Cthulhu();
    }

    virtual bool gcTrace(MMgc::GC* gc, size_t cursor) {
        (void)gc;
        // Incorrectly say that there's more to do even when the object is small
        if (cursor == 0)
            return true;
        // Concede
        return false;
    }
};

// This test triggers a particular edge case in GC::SplitExactGCObject where
// a small object that returns 'true' from its gcTrace method is synchronously
// marked until the method returns 'false'; a 'true' return is only ever expected
// from the gcTrace methods of large objects.

%%test IncorrectlySplitSmallObject
    // Create an object that stays alive so that its tracer will be called
    MMgc::GCObjectLock* lock = core->gc->LockObject(Cthulhu::create(core->gc));
    core->gc->Collect();
    core->gc->Collect();
    core->gc->UnlockObject(lock);

%%verify true

