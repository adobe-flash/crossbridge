/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "'for' loop over Number values with Number bound, update";

// Compare this to 'for-2', where there is mixed arithmetic.

function forloop() {
    for ( var i=0.5 ; i < 50000.5 ; i+=0.5 )
	;
    return i;
}

TEST(forloop, "for-3");
