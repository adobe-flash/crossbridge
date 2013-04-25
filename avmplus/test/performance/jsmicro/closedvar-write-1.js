/* -*- indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Writes of closure-bound free variable, opportunity for local escape analysis";

// Compare against 'globalvar-read-1' and 'closedvar-write-2'
// (only additional work should be closed var write and local var read)

var global_var = 1;

function mkwriteloop(closed_over_var) {
    function writeloop() {
        var v = 2;
	for ( var i=0; i < 100000 ; i++ )
            closed_over_var = v;
        return i;
    }
    return writeloop;
}

TEST(mkwriteloop(3), "closedvar-write-1");
