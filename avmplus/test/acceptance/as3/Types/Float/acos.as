/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Returns an implementation-dependent approximation to the arc cosine of x.
The result is expressed in radians and ranges from +0 to +PI.
*/

// var SECTION = "4.5.16";
// var VERSION = "AS3";
// var TITLE   = "public function acos(x:float):float";


function check(param:float):float { return float.acos(param); }

AddStrictTestCase("float.acos() returns a float", "float", getQualifiedClassName(float.acos(0)));
AddStrictTestCase("float.acos() length is 1", 1, float.acos.length);

// If x is NaN, the result is NaN.
AddStrictTestCase("float.acos(float.NaN)", float.NaN, float.acos(float.NaN));
AddStrictTestCase("float.acos(float.NaN) check()", float.NaN, check(float.NaN));

// If x is greater than 1, the result is NaN.
AddStrictTestCase("float.acos(1.125f)", float.NaN, float.acos(1.125f));
AddStrictTestCase("float.acos(1.125f) check()", float.NaN, check(1.125f));

// If x is less than -1, the result is NaN.
AddStrictTestCase("float.acos(-1.125f)", float.NaN, float.acos(-1.125f));
AddStrictTestCase("float.acos(-1.125f) check", float.NaN, check(-1.125f));

// If x is exactly 1, the result is +0.
AddStrictTestCase("float.acos(1f)", 0f, float.acos(1f));
AddStrictTestCase("Ensure that float.acos(1f) returns +0", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.acos(1f));


AddStrictTestCase("float.acos(null)", float.PI/2.0f, float.acos(null));


var myfloat:float = 0f;
AddStrictTestCase("float.acos(myfloat=0f)", float(float.PI/2.0f), float.acos(myfloat));
myfloat = 1f;
AddStrictTestCase("float.acos(myfloat=1f)", float(0), float.acos(myfloat));
myfloat = -1f;
AddStrictTestCase("float.acos(myfloat=-1f)", float.PI, float.acos(myfloat));
myfloat = -0f;
AddStrictTestCase("float.acos(myfloat=-0f)", float(float.PI/2.0f), float.acos(myfloat));

AddStrictTestCase("float.acos(0f) FloatLiteral", float(float.PI/2.0f), float.acos(0f));
AddStrictTestCase("float.acos(1f) FloatLiteral", float(0), float.acos(1f));
AddStrictTestCase("float.acos(-1f) FloatLiteral", float.PI, float.acos(-1f));
AddStrictTestCase("float.acos(-0f) FloatLiteral", float(float.PI/2.0f), float.acos(-0f));

AddStrictTestCase("float.acos(float.SQRT1_2)", float(float.PI/4.0f), float.acos(float.SQRT1_2));
AddStrictTestCase("float.acos(-float.SQRT1_2)", float(float.PI/4.0f*3.0f), float.acos(-float.SQRT1_2));


