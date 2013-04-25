/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Returns reciprocal square root of scalars.
Testmedia based on edge points in float.sqrt().
*/

// var SECTION = "4.5.31";
// var VERSION = "AS3";
// var TITLE   = "public function rsqrt(x:float):float";


function check(param:float):float { return float.rsqrt(param); }

AddStrictTestCase("float.rsqrt() returns a float", "float", getQualifiedClassName(float.rsqrt(12.345f)));
AddStrictTestCase("float.rsqrt() length is 1", 1, float.rsqrt.length);

// If x is NaN, the result is NaN. (1/NaN == NaN)
AddStrictTestCase("float.rsqrt(float.NaN)", float.NaN, float.rsqrt(float.NaN));
AddStrictTestCase("float.rsqrt(float.NaN) check()", float.NaN, check(float.NaN));

// If x is less than 0, the result is NaN. (1/NaN == NaN)
AddStrictTestCase("float.rsqrt(-1.125f)", float.NaN, float.rsqrt(-1.125f));
AddStrictTestCase("float.rsqrt(-1.125f) check()", float.NaN, check(-1.125f));
AddStrictTestCase("float.rsqrt(float.NEGATIVE_INFINITY)", float.NaN, float.rsqrt(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.rsqrt(float.NEGATIVE_INFINITY) check()", float.NaN, check(float.NEGATIVE_INFINITY));

// If x is +0, the result is +0. (1/0 == Infinity)
AddStrictTestCase("float.rsqrt(0f)", float.POSITIVE_INFINITY, float.rsqrt(0f));
AddStrictTestCase("float.rsqrt(0f) check()", float.POSITIVE_INFINITY, check(0f));
AddStrictTestCase("float.rsqrt(null)", float.POSITIVE_INFINITY, float.rsqrt(null));

// If x is -0, the result is -0. (1/-0 == -Infinity)
AddStrictTestCase("float.rsqrt(-0f)", float.NEGATIVE_INFINITY, float.rsqrt(-0f));
AddStrictTestCase("float.rsqrt(-0f) check()", float.NEGATIVE_INFINITY, check(-0f));

// If x is +Infinity, the result is +Infinity. (1/Infinity == 0)
AddStrictTestCase("float.rsqrt(float.POSITIVE_INFINITY)", 0f, float.rsqrt(float.POSITIVE_INFINITY));
AddStrictTestCase("float.rsqrt(float.POSITIVE_INFINITY) check()", 0f, check(float.POSITIVE_INFINITY));
AddStrictTestCase("float.rsqrt(float.POSITIVE_INFINITY) sign check",
                  float.POSITIVE_INFINITY,
                  float.POSITIVE_INFINITY/float.rsqrt(float.POSITIVE_INFINITY));
AddStrictTestCase("float.rsqrt(float.POSITIVE_INFINITY) check() sign check",
                  float.POSITIVE_INFINITY,
                  float.POSITIVE_INFINITY/check(float.POSITIVE_INFINITY));


// float.rsqrt(x) == 1f/float.sqrt(x)
var x:float = 3.124f;
AddStrictTestCase("x=3.124f float.rsqrt(x) == 1f/float.sqrt(x)", 1f/float.sqrt(x), float.rsqrt(x));

AddStrictTestCase("float.rsqrt(64)", float(0.125), float.rsqrt(64));


