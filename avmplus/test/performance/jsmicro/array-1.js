/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Sequential reading of array";

// Observe that if int arithmetic/looping are slow then they may mask array access time

var A = new Array();
for ( var i=0 ; i < 100 ; i++ )
    A[i] = true;

function readloop() {
    var a = A;
    var v;
    for ( var i=0 ; i < 1000 ; i++ )
	for ( var j=0 ; j < 100 ; j++ )
	    v = a[j];
    return i;
}

TEST(readloop, "array-1");

