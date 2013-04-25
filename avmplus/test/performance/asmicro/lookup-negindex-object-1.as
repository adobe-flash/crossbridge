/* -*- indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Bugzilla 564167
var DESC="lookup negative index-properties, object receiver, present"

include "driver.as"

function iterate_lookup(store):int {
    var x;
    for (var i:int = -10000 ; i < 0; i++) {
        x = store[i];
    }
    return i;
}

function populate(store) {
    for (var i = -10000; i < 0; i++) {
        store[i] = "constant"
    }
    return store;
}

var s = {}
populate(s);

function do_lookup() {
    iterate_lookup(s);
}

TEST(do_lookup, "lookup-negindex-object-1");
