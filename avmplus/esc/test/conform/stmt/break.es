/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var x=2;
while (true) {
    print("LOOP 1");
    x = x - 1;
    if (x == 0)
        break;
}

var x=2;
fnord:
    while (true) {
        while (true) {
            print("LOOP 2");
            x = x - 1;
            if (x == 0)
                break fnord;
        }
    }

var x=2;
feeble:
    if (x > 1) {
        print("NESTED");
        break feeble;
        print("WRONG");
    }

// Check to see if the compiler can handle a labelled stmt 
// even if the stmt can't be broken out of.
/* Doesn't work, parser needs fixing
confusing:
    var x = 10;
*/

print("DONE");
