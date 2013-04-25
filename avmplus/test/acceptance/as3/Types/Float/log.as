/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Returns an implementation-dependent approximation to the natural logarithm of x.
*/

// var SECTION = "4.5.24";
// var VERSION = "AS3";
// var TITLE   = "public function log(x:float):float";


function check(param:float):float { return float.log(param); }

AddStrictTestCase("float.log() returns a float", "float", getQualifiedClassName(float.log(0)));
AddStrictTestCase("float.log() length is 1", 1, float.log.length);

// If x is NaN, the result is NaN.
AddStrictTestCase("float.log(NaN)", float.NaN, float.log(float.NaN));
AddStrictTestCase("float.log(NaN) check()", float.NaN, check(float.NaN));

// If x is less than 0, the result is NaN.
AddStrictTestCase("float.log(-1f)", float.NaN, float.log(-1f));
AddStrictTestCase("float.log(-1f) check()", float.NaN, check(-1f));
AddStrictTestCase("float.log(float.NEGATIVE_INFINITY)", float.NaN, float.log(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.log(float.NEGATIVE_INFINITY) check()", float.NaN, check(float.NEGATIVE_INFINITY));

// If x is +0 or -0, the result is -Infinity.
AddStrictTestCase("float.log(0f)", float.NEGATIVE_INFINITY, float.log(0f));
AddStrictTestCase("float.log(0f) check()", float.NEGATIVE_INFINITY, check(0f));
AddStrictTestCase("float.log(-0f)", float.NEGATIVE_INFINITY, float.log(-0f));
AddStrictTestCase("float.log(-0f) check()", float.NEGATIVE_INFINITY, check(-0f));

// If x is 1, the result is +0.
AddStrictTestCase("float.log(1f)", 0f, float.log(1f));
AddStrictTestCase("float.log(1f) check()", 0f, check(1f));
AddStrictTestCase("float.log(1f) is +0f", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.log(1f));
AddStrictTestCase("float.log(1f) is +0f check()", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/check(1f));

// If x is +Infinity, the result is +Infinity.
AddStrictTestCase("float.log(float.POSITIVE_INFINITY)", float.POSITIVE_INFINITY, float.log(float.POSITIVE_INFINITY));
AddStrictTestCase("float.log(float.POSITIVE_INFINITY) check()", float.POSITIVE_INFINITY, check(float.POSITIVE_INFINITY));


var myfloat:float = 1.0e-6f;
AddStrictTestCase("float.log(-1.0e-6f)", float.NaN, float.log(-myfloat));
AddStrictTestCase("float.log(1.0e-6f)", -13.8155107498168945f, float.log(myfloat));
AddStrictTestCase("float.log(-1.0e-6f) FloatLiteral", float.NaN, float.log(-1.0e-6f));
AddStrictTestCase("float.log(1.0e-6f) FloatLiteral", -13.8155107498168945f, float.log(1.0e-6f));

myfloat = 1f;
AddStrictTestCase("float.log(-1f)", float.NaN, float.log(-myfloat));
AddStrictTestCase("float.log(1f)", 0f, float.log(myfloat));
AddStrictTestCase("float.log(-1f) FloatLiteral", float.NaN, float.log(-1f));
AddStrictTestCase("float.log(1f) FloatLiteral", 0f, float.log(1f));



