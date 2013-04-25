/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.4.3";
// var VERSION = "AS3";
// var TITLE   = "The % operation agumented by float values";



var f1:float = 1.2345678e9;
var f2:float = 6.543210987;
Assert.expectEq("float modulo result", float(7.874126434326171875e-01), f1%f2);
Assert.expectEq("FloatLiteral modulo result", float(7.874126434326171875e-01), 1.2345678e9f%6.543210987f);
 /*
    virgilp: 09/23/2011
    modulo on float & number should THEORETICALLY yield different results
    However, on gcc & VisualC (windows/x86), fmod seems to return identical results with fmodf
    if the input values are identical
    (and no, I didn't get fooled by the "fmod" version with float arguments - I explicitly
    casted the parameters to double)
    I suspect the runtime library implementations perform both operations on 64 bits.
    The thing is, there's no hardware instruction for modulo - except the x87 FPU
    (it has a "FPREM" instruction)
    Bottom line is - the C runtimelibrary makes no diffeernce (not on x86 and x64, Mac/Win, at least)
   */

// ABC Extension spec states that Float modulo can happen in double precision
Assert.expectEq("modulo of 2 floats returns a float", "float", getQualifiedClassName(f1%f2));

var onef:float = 1f;
var neg_onef:float = -1f;
var zerof:float = 0f;
var neg_zerof:float = -0f;

function check(val1:*, val2:*):*
{
    return (val1 % val2);
}

AddStrictTestCase("float % Number", 0, check(onef, 1));
AddStrictTestCase("FloatLiteral % Number", 0, check(1f, 1));
AddStrictTestCase("Number % float", 0, check(1, onef));
AddStrictTestCase("Number % FloatLiteral", 0, check(1, 1f));
AddStrictTestCase("Boolean % float", 0, check(true, onef));
AddStrictTestCase("String % float", 0, check("1", onef));

// the left operand is the dividend and the right operand is the divisor.
// If either operand is NaN, the result is NaN.
Assert.expectEq("float.NaN % float", float.NaN, float.NaN % onef);
Assert.expectEq("isNaN float.NaN % float", true, isNaN(float.NaN % onef));
Assert.expectEq("float % float.NaN", float.NaN, onef % float.NaN);
Assert.expectEq("isNaN float % float.NaN", true, isNaN(onef % float.NaN));
Assert.expectEq("check(float.NaN, float)", float.NaN, check(float.NaN, onef));
Assert.expectEq("isNaN check(float.NaN, float)", true, isNaN(check(float.NaN, onef)));
Assert.expectEq("check(float, float.NaN)", float.NaN, check(onef, float.NaN));
Assert.expectEq("isNaN check(float, float.NaN)", true, isNaN(check(onef, float.NaN)));

Assert.expectEq("float.NaN % FloatLiteral", float.NaN, float.NaN % 1f);
Assert.expectEq("isNaN float.NaN % FloatLiteral", true, isNaN(float.NaN % 1f));
Assert.expectEq("FloatLiteral % float.NaN", float.NaN, 1f % float.NaN);
Assert.expectEq("isNaN FloatLiteral % float.NaN", true, isNaN(1f % float.NaN));
Assert.expectEq("check(float.NaN, FloatLiteral)", float.NaN, check(float.NaN, 1f));
Assert.expectEq("isNaN check(float.NaN, FloatLiteral)", true, isNaN(check(float.NaN, 1f)));
Assert.expectEq("check(FloatLiteral, float.NaN)", float.NaN, check(1f, float.NaN));
Assert.expectEq("isNaN check(FloatLiteral, float.NaN)", true, isNaN(check(1f, float.NaN)));

// The sign of the result equals the sign of the dividend.
AddStrictTestCase("float % float", 1f, 3f % 2f);
AddStrictTestCase("-float % float", -1f, -3f % 2f);
AddStrictTestCase("float % -float", 1f, 3f % -2f);
AddStrictTestCase("-float % -float", -1f, -3f % -2f);

AddStrictTestCase("check(float, float)", 1f, check(3f, 2f));
AddStrictTestCase("check(-float, float)", -1f, check(-3f, 2f));
AddStrictTestCase("check(float, -float)", 1f, check(3f, -2f));
AddStrictTestCase("check(-float, -float)", -1f, check(-3f, -2f));

// If the dividend is an infinity, or the divisor is a zero, or both, the result is NaN.
Assert.expectEq("float.POSITIVE_INFINITY % float", float.NaN, float.POSITIVE_INFINITY % onef);
Assert.expectEq("isNaN float.POSITIVE_INFINITY % float", true, isNaN(float.POSITIVE_INFINITY % onef));
Assert.expectEq("float.NEGATIVE_INFINITY % float", float.NaN, float.NEGATIVE_INFINITY % onef);
Assert.expectEq("isNaN float.NEGATIVE_INFINITY % float", true, isNaN(float.NEGATIVE_INFINITY % onef));
Assert.expectEq("float % 0f", float.NaN, onef % zerof);
AddStrictTestCase("is NaN float % 0f", true, isNaN(onef % zerof));
Assert.expectEq("float % -0f", float.NaN, onef % neg_zerof);
AddStrictTestCase("is NaN float % -0f", true, isNaN(onef % neg_zerof));
Assert.expectEq("float.POSITIVE_INFINITY % 0f", float.NaN, float.POSITIVE_INFINITY % zerof);
Assert.expectEq("isNaN float.POSITIVE_INFINITY % 0f", true, isNaN(float.POSITIVE_INFINITY % zerof));
Assert.expectEq("float.POSITIVE_INFINITY % -0f", float.NaN, float.POSITIVE_INFINITY % neg_zerof);
Assert.expectEq("isNaN float.POSITIVE_INFINITY % -0f", true, isNaN(float.POSITIVE_INFINITY % neg_zerof));

// If the dividend is finite and the divisor is an infinity, the result equals the dividend.
AddStrictTestCase("float % float.POSITIVE_INFINITY", onef, onef % float.POSITIVE_INFINITY);
AddStrictTestCase("check(float % float.POSITIVE_INFINITY)", onef, check(onef, float.POSITIVE_INFINITY));
AddStrictTestCase("float % float.NEGATIVE_INFINITY", onef, onef % float.NEGATIVE_INFINITY);
AddStrictTestCase("check(float % float.NEGATIVE_INFINITY)", onef, check(onef, float.NEGATIVE_INFINITY));

// If the dividend is a zero and the divisor is nonzero and finite, the result is the same as the dividend.
AddStrictTestCase("0f % float", zerof, zerof % onef);
AddStrictTestCase("check(0f, float)", zerof, check(zerof, onef));
AddStrictTestCase("-0f % float", neg_zerof, neg_zerof % onef);
AddStrictTestCase("-0f % float sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY / (neg_zerof % onef));
AddStrictTestCase("check(-0f, float)", neg_zerof, check(neg_zerof, onef));
AddStrictTestCase("check(-0f, float) sign check ", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY / check(neg_zerof, onef));



