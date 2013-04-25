/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Array allocation, ten int properties by initializer";
include "driver.as"

function allocloop():uint {
    var v;
    for ( var i:uint=0 ; i < 100000 ; i++ )
        v = [i,1,2,3,4,5,6,7,8,9];
    return i;
}

TEST(allocloop, "alloc-5");
