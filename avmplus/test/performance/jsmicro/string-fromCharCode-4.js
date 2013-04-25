/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "String.fromCharCode, 100 ASCII values";

function loop() {
    var x;
    for ( var i=0 ; i < 1000 ; i++ )
	x = String.fromCharCode(65,66,67,68,69,70,71,72,73,74,65,66,67,68,69,70,71,72,73,74,65,66,67,68,69,70,71,72,73,74,
				65,66,67,68,69,70,71,72,73,74,65,66,67,68,69,70,71,72,73,74,65,66,67,68,69,70,71,72,73,74,
				65,66,67,68,69,70,71,72,73,74,65,66,67,68,69,70,71,72,73,74,65,66,67,68,69,70,71,72,73,74,
				65,66,67,68,69,70,71,72,73,74);
    return x;
}

TEST(loop, "string-fromCharCode-4");
