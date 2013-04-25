/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "String.prototype.charAt, characters are all ASCII";

function loop() {
    loop2("abcdefghijklmnopqrstuvwxyz");
}

function loop2(s) {
    var v;
    for ( var i=0 ; i < 100000 ; i += 25 )
	for ( var j=0 ; j < 25 ; j++ )
	    v = s.charAt(j);
    return v;
}

TEST(loop, "string-charAt-1");
