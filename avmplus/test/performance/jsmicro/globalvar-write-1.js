/* -*- indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Writes of globally-bound free variable";

// Compare against 'for-1' and 'globalvar-read-1'
// (only additional work should be global var write and local var read)

var global_var = 1;

function writeloop() {
    var v = 2;
    for ( var i=0; i < 100000 ; i++ )
        global_var = v;
    return i;
}

TEST(writeloop, "globalvar-write-1");
