/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Returns an implementation-dependent approximation to the arc tangent of x.
The result is expressed in radians and ranges from iPI/2 to +PI/2.
*/

// var SECTION = "4.5.18";
// var VERSION = "AS3";
// var TITLE   = "public function atan(x:float):float";


function check(param:float):float { return float.atan(param); }

AddStrictTestCase("float.atan() returns a float", "float", getQualifiedClassName(float.atan(0)));
AddStrictTestCase("float.atan() length is 1", 1, float.atan.length);

// If x is NaN, the result is NaN.
AddStrictTestCase("float.atan(NaN)", float.NaN, float.atan(float.NaN));
AddStrictTestCase("float.atan(NaN) check()", float.NaN, check(float.NaN));

// If x is +0, the result is +0.
AddStrictTestCase("float.atan(0f)", 0f, float.atan(0f));
AddStrictTestCase("float.atan(0f) check()", 0f, check(0f));
AddStrictTestCase("Ensure that float.atan(+0f) returns +0f", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.atan(0f));
AddStrictTestCase("Ensure that float.atan(+0f) returns +0f check()", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/check(0f));

// If x is -0, the result is -0.
AddStrictTestCase("float.atan(-0f)", -0f, float.atan(-0f));
AddStrictTestCase("float.atan(-0f) check()", -0f, check(-0f));
AddStrictTestCase("Ensure that float.atan(-0f) returns -0f", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.atan(-0f));
AddStrictTestCase("Ensure that float.atan(-0f) returns -0f check()", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/check(-0f));


// If x is +Infinity, the result is an implementation-dependent approximation to +PI/2.
AddStrictTestCase("float.atan(float.POSITIVE_INFINITY)", float.PI/2f, float.atan(float.POSITIVE_INFINITY));
AddStrictTestCase("float.atan(float.POSITIVE_INFINITY) check()", float.PI/2f, check(float.POSITIVE_INFINITY));

// If x is -Infinity, the result is an implementation-dependent approximation to -PI/2.
AddStrictTestCase("float.atan(float.NEGATIVE_INFINITY)", -float.PI/2f, float.atan(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.atan(float.NEGATIVE_INFINITY) check()", -float.PI/2f, check(float.NEGATIVE_INFINITY));


AddStrictTestCase("float.atan(null)", 0f, float.atan(null));

var myfloat:float = 1f;
AddStrictTestCase("float.atan(myfloat=1f)", float(float.PI/4.0f), float.atan(myfloat));
myfloat = 0f;
AddStrictTestCase("float.atan(myfloat=0f)", float(0), float.atan(myfloat));
myfloat = -0f;
AddStrictTestCase("float.atan(myfloat=-0f)", -float(0), float.atan(myfloat));
myfloat = -1f;
AddStrictTestCase("float.atan(myfloat=-1f)", -float(float.PI/4.0f), float.atan(myfloat));

AddStrictTestCase("float.atan(1f) FloatLiteral", float(float.PI/4.0f), float.atan(1f));
AddStrictTestCase("float.atan(0f) FloatLiteral", float(0), float.atan(0f));
AddStrictTestCase("float.atan(-0f) FloatLiteral", -float(0), float.atan(-0f));
AddStrictTestCase("float.atan(-1f) FloatLiteral", -float(float.PI/4.0f), float.atan(-1f));



