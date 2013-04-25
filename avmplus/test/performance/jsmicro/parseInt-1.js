/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "<global>.parseInt on '37'";

function libloop() {
    return libloop2("37");
}

function libloop2(s) {
    var sum = 0;
    for ( var i=0 ; i < 100000 ; i++ )
	sum += parseInt(s);
    return sum;
}

TEST(libloop, "parseInt-1");
