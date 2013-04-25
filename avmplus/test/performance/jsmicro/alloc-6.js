/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "simple String allocation, accumulating short/medium-length strings (~250 chars)";

function allocloop() {
    var s;
    for ( var i=0 ; i < 10000 ; i++ ) {
	s = "abracadabra";
	for ( var j=0 ; j < 10 ; j++ ) {
	    s += "supercallifragilisticexpialidocious";
	}
    }
    return i;
}

TEST(allocloop, "alloc-6");
