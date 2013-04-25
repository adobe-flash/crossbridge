// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

%%component avmplus
%%category builtins

%%test WeakValueHashTable
    WeakValueHashtable* tbl = WeakValueHashtable::create(core->gc);
    String* fhtagn = String::createLatin1(core, "Fhtagn!");
    MMgc::GCObjectLock* fhtagn_lock = core->gc->LockObject(fhtagn);
    fhtagn = NULL;
    for ( int i=0 ; i < 500 ; i++ ) {
        tbl->add(atomFromIntptrValue(i), (i & 1) ? String::createLatin1(core, "Cthulhu!")->atom() : ((String*)core->gc->GetLockedObject(fhtagn_lock))->atom());
    }
    core->gc->Collect();
    core->gc->Collect();
    int sum = 0;
    for ( int i=0 ; i < 500 ; i++ ) {
        Atom a = tbl->get(atomFromIntptrValue(i));
        if (a != AtomConstants::undefinedAtom)
            sum++;
    }
    core->gc->UnlockObject(fhtagn_lock);
    printf("fhtagn sum: %d\n", sum);

// Retain at least 250, but it would be unreasonable to retain more than 300
%%verify sum >= 250 && sum <= 300
