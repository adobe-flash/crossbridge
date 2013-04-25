/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// return-past-finally tests

var washit;

/* These don't work; problem with the verifier
function f() {
    try {
        return "RETURNED";
    }
    catch (e) {
    }
    finally {
        washit = "RIGHT";
    }
    return "WACKY";
}
washit = "WRONG";
print( f() );
print( washit );

function g() {
    try {
        throw 0;
    }
    catch (e) {
        return "RETURNED";
    }
    finally {
        washit = "RIGHT";
    }
    return "WACKY";
}
washit = "WRONG";
print( g() );
print( washit );
*/

print("DONE");

