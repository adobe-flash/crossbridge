/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Array.prototype.sort on arrays of length 1000, string values, custom predicate";
include "driver.as"

var arrays:Array = [];
var alen:uint = 1000;

function makeArrays():void {
    function makeArray(n:uint):Array {
        var a:Array = [];
        for ( var i:uint=0 ; i < n ; i++ )
            a[i] = Math.floor(Math.random()*alen).toString();
        return a;
    }
    for ( var i:uint=0 ; i < 100000/alen ; i++ )
        arrays[i] = makeArray(alen);
}

function loop():uint {
    var k:uint=alen;
    var t:uint=0;
    function cmp(a:String,b:String) { if (a < b) return -1; if (a == b) return 0; return 1; }
    for ( var i:uint=0 ; i < 100000 ; i+=k ) {
        var a:Array = arrays[t++];
        a.sort(cmp);
    }
    return i;
}

TEST3(makeArrays, loop, "array-sort-4");
