/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "'for-in' loop but only every other element defined";

var A = new Array();
for ( var i=0 ; i < 200 ; i += 2 )
    A[i] = true;

function for_in_loop() {
    var a = A;
    for ( var i=0 ; i < 100000 ; )
	for ( var j in a )
	    i++;
    return i;
}

TEST(for_in_loop, "for-in-2");
