/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Closure creation, deep environment and many free variables";

var g = function () { return 1; }

function allocloop() {
    var v1=g();
    function f1(a) {
	var v2=g();
	function f2(b) {
	    var v3=g();
	    function f3(c) {
		var v;
		for ( var i=0 ; i < 100000 ; i++ )
		    v = function (x) { return x+v1+v2+v3+a+b+c }
		return i;
	    }
	    return f3(v1);
	}
	return f2(v1);
    }
    return f1(v1);
}

TEST(allocloop, "alloc-10");
