/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Sanity: returing out of "with" does what it's supposed to do
var x = 20;
var obj = { x: 10 }
function f() {
    with (obj)
        return x;
}
print("SHOULD BE 10: " + f());
print("SHOULD BE 20: " + x);
print("DONE");
