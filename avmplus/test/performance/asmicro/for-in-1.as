/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "'for-in' loop over dense array";

// It is probably not appropriate to compare this to 'for-1', but a good implementation
// might approach that level of performance, so why not...
include "driver.as"

var A:Array = new Array();
for ( var i:uint=0 ; i < 100 ; i++ )
    A.push(true);

function for_in_loop():uint {
    var a:Array = A;
    for ( var i:uint=0 ; i < 100000 ; )
        for ( var j in a ) // saying "var j:String" may slow this down
            i++;
    return i;
}

TEST(for_in_loop, "for-in-1");
