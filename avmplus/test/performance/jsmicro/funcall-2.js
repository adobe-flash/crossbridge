/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Empty function call with three args";

function caller(f) {
    for ( var i=0 ; i < 100000 ; i++ )
	f(1, 2, 3);
}

function emptyfun2(a, b, c) {
}

function emptyfun() {
    caller(emptyfun2);
}

TEST(emptyfun, "funcall-2");
