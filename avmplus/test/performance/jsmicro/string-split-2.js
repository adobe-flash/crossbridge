/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "String.prototype.split a string into 10 pieces, multi-character separator"

function loop() {
    var a = "0--1--2--3--4--5--6--7--8--9";
    var v;
    for ( var i=0 ; i < 100000 ; i++ )
	v = a.split("--");
    return a.length + v.length;
}

TEST(loop, "string-split-2");
