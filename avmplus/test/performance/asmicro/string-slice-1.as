/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "String.prototype.slice a substring of length 8 from an string of length 10";
include "driver.as"

function loop():uint {
    var a:String = "0123456789";
    var v:String;
    for ( var i:uint=0 ; i < 100000 ; i++ )
        v = a.slice(1,9);
    return a.length + v.length;
}

TEST(loop, "string-slice-1");
