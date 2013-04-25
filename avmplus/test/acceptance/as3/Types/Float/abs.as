/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Returns the absolute value of x; the result has the same magnitude as x but has positive sign.
*/

// var SECTION = "4.5.15";
// var VERSION = "AS3";
// var TITLE   = "public function abs(x:float):float";


function check(param:float):float { return float.abs(param); }

AddStrictTestCase("float.abs() returns a float", "float", getQualifiedClassName(float.abs(0)));
AddStrictTestCase("float.abs() length is 1", 1, float.abs.length);


// If x is NaN, the result is NaN.
AddStrictTestCase("float.abs(float.NaN)", float.NaN, float.abs(float.NaN));
AddStrictTestCase("float.abs(flaot.NaN) check()", float.NaN, check(float.NaN));

// If x is -0, the result is +0.
AddStrictTestCase("float.abs(0.0f)", 0f, float.abs(0.0f));
AddStrictTestCase("float.POSITIVE_INFINITY/float.abs(0.0f)", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.abs(0.0f));
AddStrictTestCase("float.abs(-0.0f)", 0f, float.abs(-0.0f));
AddStrictTestCase("float.POSITIVE_INFINITY/float.abs(-0.0f)", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.abs(-0.0f));
AddStrictTestCase("float.POSITIVE_INFINITY/check(-0.0f)", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/check(-0.0f));

// If x is -Infinity, the result is +Infinity.
AddStrictTestCase("float.abs(float.NEGATIVE_INFINITY)", float.POSITIVE_INFINITY, float.abs(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.abs(float.POSITIVE_INFINITY)", float.POSITIVE_INFINITY, float.abs(float.POSITIVE_INFINITY));
AddStrictTestCase("float.abs(float.NEGATIVE_INFINITY) check()", float.POSITIVE_INFINITY, check(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.abs(float.POSITIVE_INFINITY) check()", float.POSITIVE_INFINITY, check(float.POSITIVE_INFINITY));

var pi = 3.14f;
AddStrictTestCase("float.abs(-FloatLiteral)", pi, float.abs(-pi));
AddStrictTestCase("float.abs(FloatLiteral)", pi, float.abs(pi));
AddStrictTestCase("float.abs(-FloatLiteral) check()", pi, check(-pi));
AddStrictTestCase("float.abs(FloatLiteral) check()", pi, check(pi));
var pif:float = 3.14f;
AddStrictTestCase("float.abs(-typed)", pif, float.abs(-pif));
AddStrictTestCase("float.abs(typed)", pif, float.abs(pif));
AddStrictTestCase("float.abs(-typed) check()", pif, check(-pif));
AddStrictTestCase("float.abs(typed) check()", pif, check(pif));

AddStrictTestCase("float.abs(null)", 0f, float.abs(null));

AddStrictTestCase("float.abs(-float.MIN_VALUE)", float.MIN_VALUE, float.abs(-float.MIN_VALUE));
AddStrictTestCase("float.abs(-float.MAX_VALUE)", float.MAX_VALUE, float.abs(-float.MAX_VALUE));
AddStrictTestCase("float.abs(float.MIN_VALUE)", float.MIN_VALUE, float.abs(float.MIN_VALUE));
AddStrictTestCase("float.abs(float.MAX_VALUE)", float.MAX_VALUE, float.abs(float.MAX_VALUE));
AddStrictTestCase("float.abs(-float.MIN_VALUE) check()", float.MIN_VALUE, check(-float.MIN_VALUE));
AddStrictTestCase("float.abs(-float.MAX_VALUE) check()", float.MAX_VALUE, check(-float.MAX_VALUE));
AddStrictTestCase("float.abs(float.MIN_VALUE) check()", float.MIN_VALUE, check(float.MIN_VALUE));
AddStrictTestCase("float.abs(float.MAX_VALUE) check()", float.MAX_VALUE, check(float.MAX_VALUE));


