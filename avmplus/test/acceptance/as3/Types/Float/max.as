/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

/*
Given zero or more arguments, calls ToFloat on each of the arguments and
returns the largest of the resulting values.
*/

// var SECTION = "4.5.25";
// var VERSION = "AS3";
// var TITLE   = "public function max(...xs):float";


function checkEmpty():float { return float.max(); }

AddStrictTestCase("float.max() returns a float", "float", getQualifiedClassName(float.max(0)));
AddStrictTestCase("float.max() length is 0", 0, float.max.length);

AddStrictTestCase("float.max()", float.NEGATIVE_INFINITY, float.max());

// If no arguments are given, the result is -Infinity.
AddStrictTestCase("float.max()", float.NEGATIVE_INFINITY, float.max());
AddStrictTestCase("float.max() checkEmpty()", float.NEGATIVE_INFINITY, checkEmpty());

// If any value is NaN, the result is NaN.
// undefined, "String", Number.NaN, float.NaN, in first, second and in rest args should return float.NaN
AddStrictTestCase("float.max(undefined, 2.1f, 3.2f)", float.NaN, float.max(undefined, 2.1f, 3.2f));
AddStrictTestCase("float.max(2.1f, undefined, 3.2f)", float.NaN, float.max(2.1f, undefined, 3.2f));
AddStrictTestCase("float.max(2.1f, 3.2f, undefined)", float.NaN, float.max(2.1f, 3.2f, undefined));

AddStrictTestCase("float.max('string', 2.1f, 3.2f)", float.NaN, float.max('string', 2.1f, 3.2f));
AddStrictTestCase("float.max(2.1f, 'string', 3.2f)", float.NaN, float.max(2.1f, 'string', 3.2f));
AddStrictTestCase("float.max(2.1f, 3.2f, 'string')", float.NaN, float.max(2.1f, 3.2f, 'string'));

AddStrictTestCase("float.max(Number.NaN, 2.1f, 3.2f)", float.NaN, float.max(Number.NaN, 2.1f, 3.2f));
AddStrictTestCase("float.max(2.1f, Number.NaN, 3.2f)", float.NaN, float.max(2.1f, Number.NaN, 3.2f));
AddStrictTestCase("float.max(2.1f, 3.2f, Number.NaN)", float.NaN, float.max(2.1f, 3.2f, Number.NaN));

AddStrictTestCase("float.max(float.NaN, 2.1f, 3.2f)", float.NaN, float.max(float.NaN, 2.1f, 3.2f));
AddStrictTestCase("float.max(2.1f, float.NaN, 3.2f)", float.NaN, float.max(2.1f, float.NaN, 3.2f));
AddStrictTestCase("float.max(2.1f, 3.2f, float.NaN)", float.NaN, float.max(2.1f, 3.2f, float.NaN));

// The comparison of values to determine the largest value is done as in 11.8.5 except that +0 is considered to be larger than -0.
AddStrictTestCase("float.max(1f, 1f)", 1f, float.max(1f, 1f));
AddStrictTestCase("float.max(1f, 0f)", 1f, float.max(1f, 0f));
AddStrictTestCase("float.max(0f, 1f)", 1f, float.max(0f, 1f));

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
AddStrictTestCase("float.max( 0f,  0f)", 0f, float.max(0f, 0f));
AddStrictTestCase("float.max( 0f,  0f) check sign", true, isPositive(float.max(0f, 0f)));
AddStrictTestCase("float.max(-0f, -0f)", -0f, float.max(-0f, -0f));
AddStrictTestCase("float.max(-0f, -0f) check sign", false, isPositive(float.max(-0f, -0f)));
AddStrictTestCase("float.max( 0f, -0f)", 0f, float.max(0f, -0f));
AddStrictTestCase("float.max( 0f, -0f) check sign", true, isPositive(float.max(0f, -0f)));
AddStrictTestCase("float.max(-0f,  0f)", 0f, float.max(-0f, 0f));
AddStrictTestCase("float.max(-0f,  0f) check sign", true, isPositive(float.max(-0f, 0f)));


AddStrictTestCase("float.max(null, 1)", 1f, float.max(null, 1f));
AddStrictTestCase("float.max(-1, null)", 0f, float.max(-1f, null));
AddStrictTestCase("float.max(false, true)", 1f, float.max(false, true));


AddStrictTestCase("float.max(float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY)", float.POSITIVE_INFINITY, float.max(float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY));
AddStrictTestCase("float.max(float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY)", float.POSITIVE_INFINITY, float.max(float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY));
AddStrictTestCase("float.max(float.MIN_VALUE, 0f)", float.MIN_VALUE, float.max(float.MIN_VALUE, 0f));

AddStrictTestCase("float.max(float.MIN_VALUE, 1.401298464324816e-45f)", float.MIN_VALUE, float.max(float.MIN_VALUE, 1.401298464324816e-45f));
AddStrictTestCase("float.max(float.MIN_VALUE, 1.401298464324818e-45f)", float.MIN_VALUE, float.max(float.MIN_VALUE, 1.401298464324818e-45f));

AddStrictTestCase("float.max(Number.MAX_VALUE, float.MAX_VALUE)", float.POSITIVE_INFINITY, float.max(Number.MAX_VALUE, float.MAX_VALUE));
AddStrictTestCase("float.max(Number.MIN_VALUE, float.MIN_VALUE)", float.MIN_VALUE, float.max(Number.MIN_VALUE, float.MIN_VALUE));

Assert.expectEq("float.max(float.POSITIVE_INFINITY, float.MAX_VALUE)", float.POSITIVE_INFINITY, float.max(float.POSITIVE_INFINITY, float.MAX_VALUE));

var myfloat:float = 3.124f;
var myBool:Boolean = true;
var myNum:Number = 3.1;
var myString:String = "2";
AddStrictTestCase("float.max('2', true, 3.1, 3.124f)", myfloat, float.max(myString, myBool, myNum, myfloat));
AddStrictTestCase("float.max('2', true, 3.1, 3.124f) literals", myfloat, float.max('2', true, 3.1, 3.124f));
AddStrictTestCase("float.max('2', true, 3.1, 3.124f, '24')", 24f, float.max(myString, myBool, myNum, myfloat, String('24')));
AddStrictTestCase("float.max('2', true, 3.1, 3.124f, '24') literals", 24f, float.max('2', true, 3.1, 3.124f, '24'));


