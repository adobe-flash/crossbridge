/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

print("OUTER");  // before the defn to make sure the defn is allowed

function f(x) {
    print("NESTED");  // before the defn to make sure the nested defn is OK

    function g() {
        return x;
    }

    return g; // testing that it's closed over
}
var v1 = f(10);
print("CLOSURE 1 " + v1());
var v2 = f(20);
print("CLOSURE 2 " + v2());
print("CLOSURE 1 " + v1());  // should be 10 again
print("DONE");
