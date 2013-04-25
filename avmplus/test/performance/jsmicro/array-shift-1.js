/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Array.prototype.shift from array of length 10..1";

var arrays = [];
var alen = 10;

function makeArrays() {
    function makeArray(n) {
	var a = [];
	for ( var i=0 ; i < n ; i++ )
	    a[i] = i;
	return a;
    }
    for ( var i=0 ; i < 100000/alen ; i++ )
	arrays[i] = makeArray(alen);
}

function shiftloop() {
    var k=alen;
    var t=0;
    for ( var i=0 ; i < 100000 ; i+=k ) {
	var a = arrays[t++];
	for ( var j=0 ; j < k ; j++ )
	    a.shift();
    }
    return i;
}

TEST3(makeArrays, shiftloop, "array-shift-1");
