/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "String.prototype.lastIndexOf, finding character at the end of a string";

function loop() {
    loop2("abcde");
}

function loop2(s) {
    var v;
    for ( var i=0 ; i < 100000 ; i++ )
	v = s.lastIndexOf("e");
    return v;
}

TEST(loop, "string-lastIndexOf-1");
