/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "arguments-object baseline: function that doesn't use the arguments object";

function f(a,b,c,d,e)
{
    return a + b + c + d + e;
}

function loop(f) {
    var res;
    for ( var i=0 ; i < 100000 ; i++ ) {
	res = f(i,2,3,4,5);
    }
    return res;
}

TEST(function () { return loop(f) }, "arguments-1");
