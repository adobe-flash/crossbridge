/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var arguments = "WRONG";
function f(x, y, z) {
    print("ARGUMENTS DEFINED " + arguments);
    print("0 " + arguments[0]);
    print("1 " + arguments[1]);
    print("2 " + arguments[2]);
    print("3 " + arguments[3]);
    print("L " + arguments.length);
    arguments[0] = 12;
    print("ARGUMENTS SHARING " + x);  // should be 12, not 1
    g();
}

function g() {
    //print("ARGUMENTS ON FN " + f.arguments[0]); // should be 12
    //print("ARGUMENTS CALLEE " + (arguments.callee === f) );
    arguments = 27;
    print("ARGUMENTS IS VARIABLE " + arguments);
}

f(1, 2, 3, 4);

print("DONE");
