/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// The point of the ASCII char is to force the conversion of at least
// one character; could do better here by using unicode chars that
// have case conversions, presumably.

var DESC = "String.prototype.toUpperCase/toLowerCase on a string of 9 non-8-bit chars and one ASCII"

function loop() {
    var a = String.fromCharCode(200, 201, 202, 203, 204, 205, 97, 207, 208, 209);
    for ( var i=0 ; i < 100000 ; i++ ) {
	a = a.toUpperCase();
	a = a.toLowerCase();
    }
    return a.length;
}

TEST(loop, "string-casechange-2");
