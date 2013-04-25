/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Array allocation, ten int properties, but by assignment not initializer";
include "driver.as"

function allocloop():uint {
    var v:Array;
    for ( var i:uint=0 ; i < 100000 ; i++ ) {
        v = [];
        v[0] = i;
        v[1] = 1;
        v[2] = 2;
        v[3] = 3;
        v[4] = 4;
        v[5] = 5;
        v[6] = 6;
        v[7] = 7;
        v[8] = 8;
        v[9] = 9;
    }
    return i;
}

TEST(allocloop, "alloc-11");
