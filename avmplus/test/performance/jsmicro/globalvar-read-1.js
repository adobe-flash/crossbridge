/* -*- indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Reads of globally-bound free variable";

// Compare against 'for-1'
// (only additional work should be global var read and local var write)

var global_var = -1;

function readloop() {
    var v;
    for ( var i=0; i < 100000 ; i++ )
        v = global_var
    return v;
}

TEST(readloop, "globalvar-read-1");
