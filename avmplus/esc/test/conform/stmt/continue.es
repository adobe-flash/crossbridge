/* -*- mode: java; tab-width: 4; insert-tabs-mode: nil; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var x=2;
while (x > 0) {
    print("LOOP 1");
    x = x - 1;
    continue;
    print("WRONG");
}

var x=2;
outer:
while(true) {
    print("LOOP 2 OUTER");
    while (x > 0) {
        print("LOOP 2");
        x = x - 1;
        continue outer;
        print("WRONG");
    }
    break;
}

print("DONE");
