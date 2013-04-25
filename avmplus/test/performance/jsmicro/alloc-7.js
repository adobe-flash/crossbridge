/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "JS-style constructor function, one int property";

function C(x) {
    this.x = x;
}

function allocloop() {
    var v;
    for ( var i=0 ; i < 100000 ; i++ )
	v = new C(i);
    return i;
}

TEST(allocloop, "alloc-7");
