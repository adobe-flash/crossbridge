/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Array.prototype.pop from array of length 10..1";
include "driver.as"

var arrays:Array = [];
var alen:uint = 10;

function makeArrays():void {
    function makeArray(n:uint):Array {
        var a:Array = [];
        for ( var i:uint=0 ; i < n ; i++ )
            a[i] = i;
        return a;
    }
    for ( var i:uint=0 ; i < 100000/alen ; i++ )
        arrays[i] = makeArray(alen);
}

function poploop():uint {
    var k:uint=alen;
    var t:uint=0;
    for ( var i:uint=0 ; i < 100000 ; i+=k ) {
        var a:Array = arrays[t++];
        for ( var j:uint=0 ; j < k ; j++ )
            a.pop();
    }
    return i;
}

TEST3(makeArrays, poploop, "array-pop-1");
