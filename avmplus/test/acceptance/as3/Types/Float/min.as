/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
Given zero or more arguments, calls ToFloat on each of the arguments and
returns the smallest of the resulting values.
*/

// var SECTION = "4.5.26";
// var VERSION = "AS3";
// var TITLE   = "public function min(...xs):float";


function checkEmpty():float { return float.min(); }

AddStrictTestCase("float.min() returns a float", "float", getQualifiedClassName(float.min(0)));
AddStrictTestCase("float.min() length is 0", 0, float.min.length);

// If no arguments are given, the result is +Infinity.
AddStrictTestCase("float.min()", float.POSITIVE_INFINITY, float.min());
AddStrictTestCase("float.min() checkEmpty()", float.POSITIVE_INFINITY, checkEmpty());

// If any value is NaN, the result is NaN.
// undefined, "String", Number.NaN, float.NaN, in first, second and in rest args should return float.NaN
AddStrictTestCase("float.min(undefined, 2.1f, 3.2f)", float.NaN, float.min(undefined, 2.1f, 3.2f));
AddStrictTestCase("float.min(2.1f, undefined, 3.2f)", float.NaN, float.min(2.1f, undefined, 3.2f));
AddStrictTestCase("float.min(2.1f, 3.2f, undefined)", float.NaN, float.min(2.1f, 3.2f, undefined));

AddStrictTestCase("float.min('string', 2.1f, 3.2f)", float.NaN, float.min('string', 2.1f, 3.2f));
AddStrictTestCase("float.min(2.1f, 'string', 3.2f)", float.NaN, float.min(2.1f, 'string', 3.2f));
AddStrictTestCase("float.min(2.1f, 3.2f, 'string')", float.NaN, float.min(2.1f, 3.2f, 'string'));

AddStrictTestCase("float.min(Number.NaN, 2.1f, 3.2f)", float.NaN, float.min(Number.NaN, 2.1f, 3.2f));
AddStrictTestCase("float.min(2.1f, Number.NaN, 3.2f)", float.NaN, float.min(2.1f, Number.NaN, 3.2f));
AddStrictTestCase("float.min(2.1f, 3.2f, Number.NaN)", float.NaN, float.min(2.1f, 3.2f, Number.NaN));

AddStrictTestCase("float.min(float.NaN, 2.1f, 3.2f)", float.NaN, float.min(float.NaN, 2.1f, 3.2f));
AddStrictTestCase("float.min(2.1f, float.NaN, 3.2f)", float.NaN, float.min(2.1f, float.NaN, 3.2f));
AddStrictTestCase("float.min(2.1f, 3.2f, float.NaN)", float.NaN, float.min(2.1f, 3.2f, float.NaN));


// The comparison of values to determine the largest value is done as in 11.8.5 except that +0 is considered to be larger than -0.
AddStrictTestCase("float.min(1f, 1f)", 1f, float.min(1f, 1f));
AddStrictTestCase("float.min(1f, 0f)", 0f, float.min(1f, 0f));
AddStrictTestCase("float.min(0f, 1f)", 0f, float.min(0f, 1f));

/*
Do the following combinations, treating 1=0 and 0=-0. This will check that handling -0 is correct
for both x and y, PLUS that the rest args are checked properly.
    1, 1
    0, 0
    1, 0
    0, 1
*/
// The comparison of values to determine the largest value is done as in 11.8.5 except that +0 is considered to be larger than -0.
function isPositive(param:float):Boolean
{
    return float.POSITIVE_INFINITY/param == float.POSITIVE_INFINITY;
}
AddStrictTestCase("float.min( 0f,  0f)", 0f, float.min(0f, 0f));
AddStrictTestCase("float.min( 0f,  0f) check sign", true, isPositive(float.min(0f, 0f)));
AddStrictTestCase("float.min(-0f, -0f)", -0f, float.min(-0f, -0f));
AddStrictTestCase("float.min(-0f, -0f) check sign", false, isPositive(float.min(-0f, -0f)));
AddStrictTestCase("float.min( 0f, -0f)", 0f, float.min(0f, -0f));
AddStrictTestCase("float.min( 0f, -0f) check sign", false, isPositive(float.min(0f, -0f)));
AddStrictTestCase("float.min(-0f,  0f)", 0f, float.min(-0f, 0f));
AddStrictTestCase("float.min(-0f,  0f) check sign", false, isPositive(float.min(-0f, 0f)));

AddStrictTestCase("float.min(null, 1f)", 0f, float.min(null, 1f));
AddStrictTestCase("float.min(-1f, null)", -1f, float.min(-1f, null));
AddStrictTestCase("float.min(false, true)", 0f, float.min(false, true));

AddStrictTestCase("float.min(float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY)", float.NEGATIVE_INFINITY, float.min(float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY));
AddStrictTestCase("float.min(float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY)", float.NEGATIVE_INFINITY, float.min(float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY));
AddStrictTestCase("float.min(float.MIN_VALUE, 0f)", 0f, float.min(float.MIN_VALUE, 0f));
AddStrictTestCase("float.min(float.MIN_VALUE, 1.401298464324819e-45f)", float.MIN_VALUE, float.min(float.MIN_VALUE, 1.401298464324819e-45f));
AddStrictTestCase("float.min(float.MIN_VALUE, 1.401298464324820e-45f)", float.MIN_VALUE, float.min(float.MIN_VALUE, 1.401298464324820e-45f));

AddStrictTestCase("float.min(Number.MAX_VALUE, float.MAX_VALUE)", float.MAX_VALUE, float.min(Number.MAX_VALUE, float.MAX_VALUE));
AddStrictTestCase("float.min(Number.MIN_VALUE, float.MIN_VALUE)", float(Number.MIN_VALUE), float.min(Number.MIN_VALUE, float.MIN_VALUE));

AddStrictTestCase("float.min(float.POSITIVE_INFINITY, float.MAX_VALUE)", float.MAX_VALUE, float.min(float.POSITIVE_INFINITY, float.MAX_VALUE));
AddStrictTestCase("float.min(float.NEGATIVE_INFINITY, float.MIN_VALUE)", float.NEGATIVE_INFINITY, float.min(float.NEGATIVE_INFINITY, float.MIN_VALUE));


AddStrictTestCase("float.min(float.MIN_VALUE, -float.MIN_VALUE)", -float.MIN_VALUE, float.min(float.MIN_VALUE, -float.MIN_VALUE));

var myfloat:float = -3.124f;
var myBool:Boolean = true;
var myNum:Number = 3.1;
var myString:String = "2";
AddStrictTestCase("float.min('2', true, 3.1, -3.124f)", myfloat, float.min(myString, myBool, myNum, myfloat));
AddStrictTestCase("float.min('2', true, 3.1, -3.124f) literals", myfloat, float.min('2', true, 3.1, -3.124f));
AddStrictTestCase("float.min('2', true, 3.1, -3.124f, '-24')", -24f, float.min(myString, myBool, myNum, myfloat, String('-24')));
AddStrictTestCase("float.min('2', true, 3.1, -3.124f, '24') literals", myfloat, float.min('2', true, 3.1, -3.124f, '24'));


