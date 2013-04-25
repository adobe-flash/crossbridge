/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __GCTests__
#define __GCTests__


namespace MMgc
{
    // If you call this you may regress https://bugzilla.mozilla.org/show_bug.cgi?id=472852, which
    // is about spurious asserts that are triggered possibly by stray references on the stack to
    // weakref data.  Beware!
    void RunGCTests(GC *gc);
}

#endif /* __GCTests__ */
