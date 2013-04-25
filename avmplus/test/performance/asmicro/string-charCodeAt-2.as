/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "String.prototype.charCodeAt, characters are all outside the 8-bit range";
include "driver.as"

function loop():void {
    loop2(String.fromCharCode(200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
                              210, 211, 212, 213, 214, 215, 216, 217, 218, 219,
                              220, 221, 222, 223, 224));
}

function loop2(s:String):Number {
    var v:Number;
    for ( var i:uint=0 ; i < 100000 ; i += 25 )
        for ( var j:uint=0 ; j < 25 ; j++ )
            v = s.charCodeAt(j);
    return v;
}

TEST(loop, "string-charCodeAt-2");
