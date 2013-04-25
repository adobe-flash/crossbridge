/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Returns an implementation-dependent approximation to the square root of x.
*/

// var SECTION = "4.5.31";
// var VERSION = "AS3";
// var TITLE   = "public function sqrt(x:float):float";


function check(param:float):float { return float.sqrt(param); }

AddStrictTestCase("float.sqrt() returns a float", "float", getQualifiedClassName(float.sqrt(0)));
AddStrictTestCase("float.sqrt() length is 1", 1, float.sqrt.length);

// If x is NaN, the result is NaN.
AddStrictTestCase("float.sqrt(float.NaN)", float.NaN, float.sqrt(float.NaN));
AddStrictTestCase("float.sqrt(float.NaN) check()", float.NaN, check(float.NaN));

// If x is less than 0, the result is NaN.
AddStrictTestCase("float.sqrt(-0.1f)", float.NaN, float.sqrt(-0.1f));
AddStrictTestCase("float.sqrt(-0.1f) check()", float.NaN, check(-0.1f));
AddStrictTestCase("float.sqrt(-float.MIN_VALUE)", float.NaN, float.sqrt(-float.MIN_VALUE));
AddStrictTestCase("float.sqrt(float.NEGATIVE_INFINITY)", float.NaN, float.sqrt(float.NEGATIVE_INFINITY));

// If x is +0, the result is +0.
var zero:float = 0f;
AddStrictTestCase("float.sqrt(zero=0f)", 0f, float.sqrt(zero));
AddStrictTestCase("float.sqrt(0f) FloatLiteral", 0f, float.sqrt(0f));
AddStrictTestCase("float.sqrt(0f) sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/float.sqrt(0f));
AddStrictTestCase("float.sqrt(0f) check()", 0f, check(0f));
AddStrictTestCase("float.sqrt(0f) check() sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/check(0f));
AddStrictTestCase("float.sqrt(null)", 0f, float.sqrt(null));

// If x is -0, the result is -0.
var neg_zero:float = 0f;
AddStrictTestCase("float.sqrt(neg_zero=-0f)", -0f, float.sqrt(neg_zero));
AddStrictTestCase("float.sqrt(-0f) FloatLiteral", -0f, float.sqrt(-0f));
AddStrictTestCase("float.sqrt(-0f) sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/float.sqrt(-0f));
AddStrictTestCase("float.sqrt(-0f) check()", -0f, check(-0f));
AddStrictTestCase("float.sqrt(-0f) check() sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/check(-0f));


// If x is +Infinity, the result is +Infinity.
AddStrictTestCase("float.sqrt(float.POSITIVE_INFINITY)", float.POSITIVE_INFINITY, float.sqrt(float.POSITIVE_INFINITY));
AddStrictTestCase("float.sqrt(float.POSITIVE_INFINITY) check()", float.POSITIVE_INFINITY, check(float.POSITIVE_INFINITY));


AddStrictTestCase("float.sqrt(2f)", float.SQRT2, float.sqrt(2f));
AddStrictTestCase("float.sqrt(0.5f)", float.SQRT1_2, float.sqrt(0.5f));

var x:float = 3f;
for( var i:int = 0; i < 20; i++ )
{
    AddStrictTestCase("float.sqrt("+x+")", float(1.73205080756887719318).toFixed(i),  float.sqrt(x).toFixed(i));
}
x = 2f;
for( var i:int = 0; i < 20; i++ )
{
    AddStrictTestCase("float.sqrt("+x+")", float(1.4142135623730951455).toFixed(i),  float.sqrt(x).toFixed(i));
}


