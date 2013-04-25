/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "String.prototype.toUpperCase/toLowerCase on a string of 10 ASCII chars"
include "driver.as"

function loop():uint {
    var a:String = "abcdefghij"
    for ( var i:uint=0 ; i < 100000 ; i++ ) {
        a = a.toUpperCase();
        a = a.toLowerCase();
    }
    return a.length;
}

TEST(loop, "string-casechange-1");
