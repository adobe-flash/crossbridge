/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Returns the greatest (closest to +Infinity) float value that is not greater
than x and is equal to a mathematical integer. If x is already an integer, the
result is x.
*/

// var SECTION = "4.5.23";
// var VERSION = "AS3";
// var TITLE   = "public function floor(x:float):float";


function check(param:float):float { return float.floor(param); }

AddStrictTestCase("float.floor() returns a float", "float", getQualifiedClassName(float.floor(0)));
AddStrictTestCase("float.floor() length is 1", 1, float.floor.length);

// If x is NaN, the result is NaN.
AddStrictTestCase("float.floor(NaN)", float.NaN, float.floor(float.NaN));
AddStrictTestCase("float.floor(NaN)", float.NaN, check(float.NaN));

// If x is +0, the result is +0.
AddStrictTestCase("float.floor(0f)", 0f, float.floor(0f));
AddStrictTestCase("float.floor(0f) sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.floor(0f));
AddStrictTestCase("float.floor(0f) check()", 0f, check(0f));
AddStrictTestCase("float.floor(0f) check() sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/check(0f));
AddStrictTestCase("float.floor(null)", 0f, float.floor(null));

// If x is -0, the result is -0.
AddStrictTestCase("float.floor(-0f)", -0f, float.floor(-0f));
AddStrictTestCase("float.floor(-0f) sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.floor(-0f));
AddStrictTestCase("float.floor(-0f) check()", -0f, check(-0f));
AddStrictTestCase("float.floor(-0f) check() sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/check(-0f));

// If x is +Infinity, the result is +Infinity.
AddStrictTestCase("float.floor(float.POSITIVE_INFINITY)", float.POSITIVE_INFINITY, float.floor(float.POSITIVE_INFINITY));
AddStrictTestCase("float.floor(float.POSITIVE_INFINITY) check()", float.POSITIVE_INFINITY, check(float.POSITIVE_INFINITY));

// If x is -Infinity, the result is -Infinity.
AddStrictTestCase("float.floor(float.NEGATIVE_INFINITY)", float.NEGATIVE_INFINITY, float.floor(float.NEGATIVE_INFINITY));
AddStrictTestCase("float.floor(float.NEGATIVE_INFINITY) check()", float.NEGATIVE_INFINITY, check(float.NEGATIVE_INFINITY));

// If x is greater than 0 but less than 1, the result is +0.
AddStrictTestCase("float.floor(float.MIN_VALUE)", 0f, float.floor(float.MIN_VALUE));
AddStrictTestCase("float.floor(0.5f)", 0f, float.floor(0.5f));
AddStrictTestCase("float.floor(0.5f) sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.floor(0.5f));
AddStrictTestCase("float.floor(0.999f)", 0f, float.floor(0.999f));
AddStrictTestCase("float.floor(0.999f) sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.floor(0.999f));
AddStrictTestCase("float.floor(0.5f) check()", 0f, check(0.5f));
AddStrictTestCase("float.floor(0.5f) check() sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/check(0.5f));

// The value of float.floor(x) is the same as the value of -float.ceil(-x).
AddStrictTestCase("float.floor(3.124f) == -float.ceil(-3.124f)", -float.ceil(-3.124f), float.floor(3.124f));


AddStrictTestCase("float.floor(-float.MIN_VALUE)", -1f, float.floor(-float.MIN_VALUE));
AddStrictTestCase("float.floor(float.MAX_VALUE)", float.MAX_VALUE, float.floor(float.MAX_VALUE));

var myfloat:float = 1f;
AddStrictTestCase("float.floor(myfloat=1f)", float(1f), float.floor(myfloat));
myfloat = 0f;
AddStrictTestCase("float.floor(myfloat=0f)", float(0), float.floor(myfloat));
AddStrictTestCase("float.INFINITY/float.floor(myfloat=0f)", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.floor(myfloat));
myfloat = -0f;
AddStrictTestCase("float.floor(myfloat=-0f)", -float(0), float.floor(myfloat));
AddStrictTestCase("float.INFINITY/float.floor(myfloat=-0f)", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.floor(myfloat));
myfloat = -1f;
AddStrictTestCase("float.floor(myfloat=-1f)", float(-1f), float.floor(myfloat));

AddStrictTestCase("float.floor(1f) FloatLiteral", float(1), float.floor(1f));
AddStrictTestCase("float.floor(0f) FloatLiteral", float(0), float.floor(0f));
AddStrictTestCase("float.floor(-0f) FloatLiteral", -float(0), float.floor(-0f));
AddStrictTestCase("float.floor(-1f) FloatLiteral", -float(1), float.floor(-1f));




