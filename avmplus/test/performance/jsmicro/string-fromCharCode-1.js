/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "String.fromCharCode, one ASCII value";

function loop() {
    var x;
    for ( var i=0 ; i < 100000 ; i++ )
	x = String.fromCharCode(37);
    return x;
}

TEST(loop, "string-fromCharCode-1");
