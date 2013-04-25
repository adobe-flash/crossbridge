/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Returns an implementation-dependent approximation to the arc sine of x.
The result is expressed in radians and ranges from -PI/2 to +PI/2.
*/

// var SECTION = "4.5.17";
// var VERSION = "AS3";
// var TITLE   = "public function asin(x:float):float";


function check(param:float):float { return float.asin(param); }

AddStrictTestCase("float.asin() returns a float", "float", getQualifiedClassName(float.asin(0)));
AddStrictTestCase("float.asin() length is 1", 1, float.asin.length);

// If x is NaN, the result is NaN.
AddStrictTestCase("float.asin(float.NaN)", float.NaN, float.asin(float.NaN));
AddStrictTestCase("float.asin(float.NaN) check()", float.NaN, check(float.NaN));

// If x is greater than 1, the result is NaN.
AddStrictTestCase("float.asin(1.125f)", float.NaN, float.asin(1.125f));
AddStrictTestCase("float.asin(1.125f) check()", float.NaN, check(1.125f));

// If x is less than –1, the result is NaN.
AddStrictTestCase("float.asin(-1.125f)", float.NaN, float.asin(-1.125f));
AddStrictTestCase("float.asin(-1.125f) check()", float.NaN, check(-1.125f));


// If x is +0, the result is +0.
AddStrictTestCase("float.asin(0f)", 0f, float.asin(0f));
AddStrictTestCase("float.asin(0f) check()", 0f, check(0f));
AddStrictTestCase("Ensure that float.asin(+0f) returns +0f", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.asin(0f));
AddStrictTestCase("Ensure that float.asin(+0f) returns +0f check()", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/check(0f));

// If x is -0, the result is -0.
AddStrictTestCase("float.asin(-0f)", -0f, float.asin(-0f));
AddStrictTestCase("float.asin(-0f) check()", -0f, check(-0f));
AddStrictTestCase("Ensure that float.asin(-0f) returns -0f", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.asin(-0f));
AddStrictTestCase("Ensure that float.asin(-0f) returns -0f check()", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/check(-0f));


AddStrictTestCase("float.asin(null)", 0f, float.asin(null));

var myfloat:float = 1f;
AddStrictTestCase("float.asin(myfloat=1f)", float(float.PI/2.0f), float.asin(myfloat));
myfloat = 0f;
AddStrictTestCase("float.asin(myfloat=0f)", float(0), float.asin(myfloat));
myfloat = -0f;
AddStrictTestCase("float.asin(myfloat=-0f)", -float(0), float.asin(myfloat));
myfloat = -1f;
AddStrictTestCase("float.asin(myfloat=-1f)", -float(float.PI/2.0f), float.asin(myfloat));

AddStrictTestCase("float.asin(1f) FloatLiteral", float(float.PI/2.0f), float.asin(1f));
AddStrictTestCase("float.asin(0f) FloatLiteral", float(0), float.asin(0f));
AddStrictTestCase("float.asin(-0f) FloatLiteral", -float(0), float.asin(-0f));
AddStrictTestCase("float.asin(-1f) FloatLiteral", -float(float.PI/2.0f), float.asin(-1f));

AddStrictTestCase("float.asin(float.SQRT1_2)", 0.7853981256484985f, float.asin(float.SQRT1_2));
AddStrictTestCase("float.asin(-float.SQRT1_2)", -0.7853981256484985f, float.asin(-float.SQRT1_2));



