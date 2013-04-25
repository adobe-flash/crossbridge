/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Array allocation, ten int properties by assignment, not starting at 0";

function allocloop() {
    var v;
    for ( var i=0 ; i < 100000 ; i++ ) {
	v = [];
	v[1] = i;
	v[2] = 2;
	v[3] = 3;
	v[4] = 4;
	v[5] = 5;
	v[6] = 6;
	v[7] = 7;
	v[8] = 8;
	v[9] = 9;
	v[10] = 10;
    }
    return i;
}

TEST(allocloop, "alloc-12");
