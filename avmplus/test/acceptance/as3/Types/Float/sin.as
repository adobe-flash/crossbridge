/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Returns an implementation-dependent approximation to the sine of x. The argument
is expressed in radians.
*/

// var SECTION = "4.5.30";
// var VERSION = "AS3";
// var TITLE   = "public function sin(x:float):float";


function check(param:float):float { return float.sin(param); }

AddStrictTestCase("float.sin() returns a float", "float", getQualifiedClassName(float.sin(0)));
AddStrictTestCase("float.sin() length is 1", 1, float.sin.length);

// If x is NaN, the result is NaN.
AddStrictTestCase("float.sin(float.NaN)", float.NaN, float.sin(float.NaN));
AddStrictTestCase("float.sin(float.NaN) check()", float.NaN, check(float.NaN));


// If x is +0, the result is +0.
var zero:float = 0f;
AddStrictTestCase("float.sin(zero=0f)", 0f, float.sin(zero));
AddStrictTestCase("float.sin(zero=0f) sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.sin(zero));
AddStrictTestCase("float.sin(0f)", 0f, float.sin(0f));
AddStrictTestCase("float.sin(0f) sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.sin(0f));
AddStrictTestCase("float.sin(0f) check()", 0f, check(0f));
AddStrictTestCase("float.sin(0f) check() sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/check(0f));
AddStrictTestCase("float.sin(null)", 0f, float.sin(null));

// If x is -0, the result is -0.
var neg_zero:float = -0;
AddStrictTestCase("float.sin(zero=-0f)", -0f, float.sin(neg_zero));
AddStrictTestCase("float.sin(zero=-0f) sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.sin(neg_zero));
AddStrictTestCase("float.sin(-0f) FloatLiteral", -0f, float.sin(-0f));
AddStrictTestCase("float.sin(-0f) FloatLiteral sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.sin(-0f));
AddStrictTestCase("float.sin(-0f) check()", -0f, check(-0f));
AddStrictTestCase("float.sin(-0f) check() sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/check(-0f));


// If x is +Infinity or -Infinity, the result is NaN.
AddStrictTestCase("float.sin(float.POSITIVE_INFINITY)", float.NaN, float.sin(float.POSITIVE_INFINITY));
AddStrictTestCase("float.sin(float.NEGATIVE_INFINITY)", float.NaN, float.sin(float.NEGATIVE_INFINITY));

AddStrictTestCase("float.sin(float.PI/4f)", 0.7071067811865134f, float.sin(float.PI/4f));
AddStrictTestCase("float.sin(float.PI/2f)", 1f, float.sin(float.PI/2f));
AddStrictTestCase("float.sin(2.356194490192f)", float(0.7071067811867916), float.sin(2.356194490192f));
AddStrictTestCase("float.sin(float.PIf)", -8.74227766e-8f, float.sin(float.PI));


