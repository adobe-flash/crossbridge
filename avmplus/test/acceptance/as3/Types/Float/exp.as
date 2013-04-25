/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Returns an implementation-dependent approximation to the exponential function
of x (e raised to the power of x, where e is the base of the natural logarithms).
*/

// var SECTION = "4.5.22";
// var VERSION = "AS3";
// var TITLE   = "public function exp(x:float):float";


function check(param:float):float { return float.exp(param); }

AddStrictTestCase("float.exp() returns a float", "float", getQualifiedClassName(float.exp(0)));
AddStrictTestCase("float.exp() length is 1", 1, float.exp.length);

// If x is NaN, the result is NaN.
AddStrictTestCase("float.exp(NaN)", float.NaN, float.exp(float.NaN));
AddStrictTestCase("float.exp(NaN) check()", float.NaN, check(float.NaN));

// If x is +0, the result is 1.
AddStrictTestCase("float.exp(0f)", 1f, float.exp(0f));
AddStrictTestCase("float.exp(0f) check()", 1f, check(0f));

// If x is -0, the result is 1.
AddStrictTestCase("float.exp(-0f)", 1f, float.exp(-0f));
AddStrictTestCase("float.exp(-0f) check()", 1f, check(-0f));

// If x is +Infinity, the result is +Infinity.
AddStrictTestCase("float.exp(float.POSITIVE_INFINITY)", float.POSITIVE_INFINITY, float.exp(float.POSITIVE_INFINITY));
AddStrictTestCase("float.exp(float.POSITIVE_INFINITY) check()", float.POSITIVE_INFINITY, check(float.POSITIVE_INFINITY));

// If x is -Infinity, the result is +0.
AddStrictTestCase("float.exp(float.NEGATIVE_INFINITY)", 0f, float.exp(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.exp(float.NEGATIVE_INFINITY) sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.exp(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.exp(float.NEGATIVE_INFINITY) check()", 0f, check(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.exp(float.NEGATIVE_INFINITY) check() sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/check(float.NEGATIVE_INFINITY));


AddStrictTestCase("float.exp(null)", 1f, float.exp(null));

AddStrictTestCase("float.exp(1)", float.E, float.exp(1f));
AddStrictTestCase("float.exp(1) check()", float.E, check(1f));

AddStrictTestCase("float.exp(float.MIN_VALUE)", 1f, float.exp(float.MIN_VALUE));
AddStrictTestCase("float.exp(float.MAX_VALUE)", float.POSITIVE_INFINITY, float.exp(float.MAX_VALUE));

AddStrictTestCase("float.exp(1.0e+3)", float.POSITIVE_INFINITY, float.exp(1.0e+3));
AddStrictTestCase("float.exp(-1.0e+3)", float(0), float.exp(-1.0e+3));


