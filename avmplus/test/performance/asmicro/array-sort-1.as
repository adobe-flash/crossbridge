/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Array.prototype.sort on arrays of length 10, integer values, default predicate";
include "driver.as"

var arrays:Array = [];
var alen:uint = 10;

function makeArrays():void {
    function makeArray(n:uint):Array {
        var a:Array = [];
        for ( var i:uint=0 ; i < n ; i++ )
            a[i] = Math.random() >= 0.5 ? 1 : 0;
        return a;
    }
    for ( var i:uint=0 ; i < 100000/alen ; i++ )
        arrays[i] = makeArray(alen);
}

function loop():uint {
    var k:uint=alen;
    var t:uint=0;
    for ( var i:uint=0 ; i < 100000 ; i+=k ) {
        var a:Array = arrays[t++];
        a.sort();
    }
    return i;
}

TEST3(makeArrays, loop, "array-sort-1");
