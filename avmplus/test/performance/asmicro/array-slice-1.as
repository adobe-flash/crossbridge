/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Array.prototype.slice a subarray of length 8 from an array of length 10";
include "driver.as"

function loop():uint {
    var a:Array = [0,1,2,3,4,5,6,7,8,9];
    var v:Array;
    for ( var i:uint=0 ; i < 100000 ; i++ )
        v = a.slice(1,9);
    return a.length + v.length;
}

TEST(loop, "array-slice-1");
