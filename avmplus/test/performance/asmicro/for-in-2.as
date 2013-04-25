/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "'for-in' loop but only every other element defined";
include "driver.as"

var A:Array = new Array();
for ( var i:uint=0 ; i < 200 ; i += 2 )
    A[i] = true;

function for_in_loop():uint {
    var a:Array = A;
    for ( var i:uint=0 ; i < 100000 ; )
        for ( var j in a ) // saying "var j:String" may slow this down
            i++;
    return i;
}

TEST(for_in_loop, "for-in-2");
