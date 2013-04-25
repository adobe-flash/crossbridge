/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Array.prototype.push single value into array of length 0..9";
include "driver.as"

function pushloop():uint {
    var a:Array = [0,1,2,3,4,5,6,7,8,9];
    for ( var i:uint=0 ; i < 100000 ; i+=10 ) {
        a.length = 0;
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
        a.push(i);
    }
    return a.length;
}

TEST(pushloop, "array-push-1");
