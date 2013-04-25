/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "<global>.parseFloat on '3.14159'";

function libloop() {
    return libloop2("3.14159");
}

function libloop2(s) {
    var sum = 0;
    for ( var i=0 ; i < 100000 ; i++ )
	sum += parseFloat(s);
    return sum;
}

TEST(libloop, "parseFloat-1");
