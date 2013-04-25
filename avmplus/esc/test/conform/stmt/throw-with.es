/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var obj =  { x : 10 };
var x = 20;

// First test: throwing out of a with to a handler in the same scope should
// pop the scope stack; this is handled by the VM exn machinery.

function f() {
    try {
        with (obj)
            throw x;
    }
    catch (exn) {
        print("HANDLER " + exn); // 10
        print("X " + x);         // 20
    }
}

f();

// Second test: throwing to a handler inside a with needs to restore the with scope
// that is popped by the VM.

function g() {
    with (obj) {
        try {
            throw "foo";
        }
        catch (exn) {
            print("HANDLER " + exn); // foo
            print("X " + x);         // 10
        }
    }
}

g();

print("DONE");
