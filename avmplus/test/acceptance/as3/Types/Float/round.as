/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Returns the float value that is closest to x and is equal to a mathematical
integer. If two integer float values are equally close to x, then the result
is the float value that is closer to +Infinity. If x is already an integer,
the result is x.
*/

// var SECTION = "4.5.29";
// var VERSION = "AS3";
// var TITLE   = "public function round(x:float):float";


function check(param:float):float { return float.round(param); }

AddStrictTestCase("float.round() returns a float", "float", getQualifiedClassName(float.round(12.345f)));
AddStrictTestCase("float.round() length is 1", 1, float.round.length);

// If x is NaN, the result is NaN.
AddStrictTestCase("float.round(NaN)", float.NaN, float.round(float.NaN));
AddStrictTestCase("float.round(NaN) check()", float.NaN, check(float.NaN));

// If x is +0, the result is +0.
AddStrictTestCase("float.round(0f)", 0f, float.round(0f));
AddStrictTestCase("float.round(0f) is +0f", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.round(0f));
AddStrictTestCase("float.round(0f) check()", 0f, check(0f));
AddStrictTestCase("float.round(0f) is +0f check()", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/check(0f));
AddStrictTestCase("float.round(null)", 0f, float.round(null));

// If x is -0, the result is -0.
AddStrictTestCase("float.round(-0f)", -0f, float.round(-0f));
AddStrictTestCase("float.round(-0f) sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.round(-0f));
AddStrictTestCase("float.round(-0f) check()", -0f, check(-0f));
AddStrictTestCase("float.round(-0f) check() sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/check(-0f));

// If x is +Infinity, the result is +Infinity.
AddStrictTestCase("float.round(float.POSITIVE_INFINITY)", float.POSITIVE_INFINITY, float.round(float.POSITIVE_INFINITY));
AddStrictTestCase("float.round(float.POSITIVE_INFINITY) check()", float.POSITIVE_INFINITY, check(float.POSITIVE_INFINITY));

// If x is -Infinity, the result is -Infinity.
AddStrictTestCase("float.round(float.NEGATIVE_INFINITY)", float.NEGATIVE_INFINITY, float.round(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.round(float.NEGATIVE_INFINITY) check()", float.NEGATIVE_INFINITY, check(float.NEGATIVE_INFINITY));

// If x is greater than 0 but less than 0.5, the result is +0.
AddStrictTestCase("float.round(0.49f)", 0f, float.round(0.49f));
AddStrictTestCase("float.round(0.49999f)", 0f, float.round(0.49999f));
AddStrictTestCase("float.round(49.999e-2f)", 0f, float.round(49.999e-2f));
AddStrictTestCase("float.round(0.49f) check()", 0f, check(0.49f));
AddStrictTestCase("float.round(49.999e-2f) check()", 0f, check(49.999e-2f));
AddStrictTestCase("float.round(float.MIN_VALUE)", 0f, float.round(float.MIN_VALUE));

// If x is less than 0 but greater than or equal to -0.5, the result is -0.
AddStrictTestCase("float.round(-0.49f)", -0f, float.round(-0.49f));
AddStrictTestCase("float.round(-0.49f) sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.round(-0.49f));
AddStrictTestCase("float.round(-0.49f) check()", -0f, check(-0.49f));
AddStrictTestCase("float.round(-0.49f) check() sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/check(-0.49f));
AddStrictTestCase("float.round(-0.49999f)", -0f, float.round(-0.49999f));
AddStrictTestCase("float.round(-0.49999f) sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.round(-0.49999f));
AddStrictTestCase("float.round(-4.9999e-1f)", -0f, float.round(-4.9999e-1f));
AddStrictTestCase("float.round(-4.9999e-1f) sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.round(4.9999e-1f));
AddStrictTestCase("float.round(-float.MIN_VALUE)", -0f, float.round(-float.MIN_VALUE));
AddStrictTestCase("float.round(-float.MIN_VALUE) sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.round(-float.MIN_VALUE));
AddStrictTestCase("float.round(-0.5f)", -0f, float.round(-0.5f));
AddStrictTestCase("float.round(-0.5f) sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.round(-0.5f));

// NOTE 1 float.round(3.5) returns 4, but float.round(–3.5) returns –3.
AddStrictTestCase("float.round(3.5f)", 4f, float.round(3.5f));
AddStrictTestCase("float.round(3.5f) check()", 4f, check(3.5f));
AddStrictTestCase("float.round(-3.5f)", -3f, float.round(-3.5f));
AddStrictTestCase("float.round(-3.5f) check()", -3f, check(-3.5f));

/* FIXME: Need to investigate if this is valid for floats
// NOTE 2 The value of float.round(x) is the same as the value of float.floor(x+0.5),
// except when x is -0 or is less than 0 but greater than or equal to -0.5; for these
// cases float.round(x) returns -0, but float.floor(x+0.5) returns +0.
var x:float = 5.125f;
AddStrictTestCase("x=5.125f float.round(x) == float.floor(x+0.5f)", float.floor(x+0.5f), float.round(x));
x = -0f;
var resRound = float.POSITIVE_INFINITY/float.round(x);
var resFloor = float.POSITIVE_INFINITY/float.floor(x+0.5f);
AddStrictTestCase("x=-0f float.round(x) != float.floor(x+0.5f)", true, resRound != resFloor);
x = -0.49f;
var resRound = float.POSITIVE_INFINITY/float.round(x);
var resFloor = float.POSITIVE_INFINITY/float.floor(x+0.5f);
AddStrictTestCase("x=-0.49f float.round(x) != float.floor(x+0.5f)", true, resRound != resFloor);
*/

AddStrictTestCase("float.round(-5.000001e-1f)", -1f, float.round(-5.000001e-1f));
AddStrictTestCase("float.round(0.5f)", 1f, float.round(0.5f));
AddStrictTestCase("float.round(5.000001e-1f)", 1f, float.round(5.000001e-1f));

var myFloat:float = 3.124f;
AddStrictTestCase("float.round(3.124f)", 3f, float.round(myFloat));
AddStrictTestCase("float.round(3.124f) FloatLiteral", 3f, float.round(3.124f));

AddStrictTestCase("float.round(float.MAX_VALUE)", float.MAX_VALUE, float.round(float.MAX_VALUE));



