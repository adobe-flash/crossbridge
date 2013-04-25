/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Array.prototype.sort on arrays of length 1000, integer values (but may be boxed), default predicate";

var arrays = [];
var alen = 1000;

function makeArrays() {
    function makeArray(n) {
	var a = [];
	for ( var i=0 ; i < n ; i++ )
	    a[i] = Math.floor(Math.random()*alen);
	return a;
    }
    for ( var i=0 ; i < 100000/alen ; i++ )
	arrays[i] = makeArray(alen);
}

function loop() {
    var k=alen;
    var t=0;
    for ( var i=0 ; i < 100000 ; i+=k ) {
	var a = arrays[t++];
	a.sort();
    }
    return i;
}

TEST3(makeArrays, loop, "array-sort-2");
