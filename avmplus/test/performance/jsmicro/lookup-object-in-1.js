/* -*- indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Check index prop present in object via operator 'in', compare to operator[]";

// Bugzilla 555982
// Keep all of following in sync:
// lookup-object-fetch-1.as, lookup-object-in-1.as,
//  lookup-array-fetch-1.as,  lookup-array-in-1.as
//
// The "object" variants are timing lookup on constructions via object literals;
// and "array" via array literals.
//
// The "in" variants are timing lookup via the in operator;
// and "fetch" via operator[] (yields undefined if key absent).

function fresh_store() {
    return {};
}

var entry_count = 100000;
var sparse_factor = 107;
function populate_dense(store) {
    for (var i = 0; i < entry_count; i++) {
        store[i] = "d.constant"
    }
    return store;
}

function populate_sparse(store) {
    for (var i = 0; i < entry_count*sparse_factor; i += sparse_factor) {
        store[i] = "s.constant"
    }
    return store;
}

function iterate_lookup(store) {
    var hits = 0;
    for (var i = 0; i < 5; i++) {
        for (var j = 0; j < 100000; j += 50) {
            if (j in store) {
                hits += 1;
            }
        }
    }
    return hits;
}

var D = fresh_store();
var S = fresh_store();

D = populate_dense(D);
S = populate_sparse(S);

function do_lookup_both() {
    iterate_lookup(D);
    iterate_lookup(S);
}
function do_lookup_dense() {
    iterate_lookup(D);
}
function do_lookup_sparse() {
    iterate_lookup(S);
}

TEST(do_lookup_both, "lookup-object-in-1");
// TEST(do_lookup_dense,  "lookup-object-in-1:dense");
// TEST(do_lookup_sparse, "lookup-object-in-1:sparse");
