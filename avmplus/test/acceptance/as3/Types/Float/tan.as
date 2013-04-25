/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Returns an implementation-dependent approximation to the tangent of x. The
argument is expressed in radians.
*/

// var SECTION = "4.5.32";
// var VERSION = "AS3";
// var TITLE   = "public function tan(x:float):float";


function check(param:float):float { return float.tan(param); }

AddStrictTestCase("float.tan() returns a float", "float", getQualifiedClassName(float.tan(0)));
AddStrictTestCase("float.tan() length is 1", 1, float.tan.length);

// If x is NaN, the result is NaN.
AddStrictTestCase("float.tan(float.NaN)", float.NaN, float.tan(float.NaN));
AddStrictTestCase("float.tan(float.NaN) check()", float.NaN, check(float.NaN));

// If x is +0, the result is +0.
var zero:float = 0f;
AddStrictTestCase("float.tan(zero=0f)", 0f, float.tan(zero));
AddStrictTestCase("float.tan(0f) FloatLiteral", 0f, float.tan(0f));
AddStrictTestCase("float.tan(0f) sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.tan(0f));
AddStrictTestCase("float.tan(0f) check()", 0f, check(0f));
AddStrictTestCase("float.tan(0f) check() sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/check(0f));
AddStrictTestCase("float.tan(null)", 0f, float.tan(null));

// If x is -0, the result is -0.
var neg_zero:float = -0f;
AddStrictTestCase("float.tan(neg_zero)", -0f, float.tan(neg_zero));
AddStrictTestCase("float.tan(neg_zero) sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.tan(neg_zero));
AddStrictTestCase("float.tan(-0f) FloatLiteral", -0f, float.tan(-0f));
AddStrictTestCase("float.tan(-0f) FloatLiteral sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.tan(-0f));
AddStrictTestCase("float.tan(-0f) check()", -0f, check(-0f));
AddStrictTestCase("float.tan(-0f) check() sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/check(-0f));

// If x is +Infinity or -Infinity, the result is NaN.
AddStrictTestCase("float.tan(float.POSITIVE_INFINITY)", float.NaN, float.tan(float.POSITIVE_INFINITY));
AddStrictTestCase("float.tan(float.NEGATIVE_INFINITY)", float.NaN, float.tan(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.tan(float.POSITIVE_INFINITY) check()", float.NaN, check(float.POSITIVE_INFINITY));
AddStrictTestCase("float.tan(float.NEGATIVE_INFINITY) check()", float.NaN, check(float.NEGATIVE_INFINITY));

AddStrictTestCase("float.tan(float.PI/4f)", float(1), float.tan(float.PI/4f));
AddStrictTestCase("float.tan(3f*float.PI/4f)", float(-1), float.tan(3f*float.PI/4f));
AddStrictTestCase("float.tan(float.PI)", -float.sin(float.PI), float.tan(float.PI));
AddStrictTestCase("float.tan(5f*float.PI/4f)", float(1.0000003576278687), float.tan(5f*float.PI/4f));


