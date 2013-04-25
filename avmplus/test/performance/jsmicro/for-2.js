/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "'for' loop over Number values with int bound, update";

// It is highly appropriate to compare this to 'while-1' and 'do-1'.

function forloop() {
    for ( var i=0.5 ; i < 100000 ; i++ )
	;
    return i;
}

TEST(forloop, "for-2");

