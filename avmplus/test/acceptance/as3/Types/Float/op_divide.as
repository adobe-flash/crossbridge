/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.4.2";
// var VERSION = "AS3";
// var TITLE   = "The / operation agumented by float values";



var f1:float = 1.2345678e9; // TODO: use float literals
var f2:float = 6.543210987; // TODO: use float literals
// c double 188679197.7903248965740204
// c float  188679200
AddStrictTestCase("float division result", float(1.886792e+08), f1/f2);
AddStrictTestCase("FloatLiteral division result", float(1.886792e+08), 1.2345678e9f/6.543210987f);
Assert.expectEq("division of 2 floats returns a float", "float", getQualifiedClassName(f1/f2));


var val1:float = 4.734612e3f;
var val2:float = 5.417242e5f;
AddStrictTestCase("multiplication of 2 floats", 2.564853760e9f, val1*val2);
var val3 = 4.734612e3f * 5.417242e5f;
AddStrictTestCase("multiplication of 2 FloatLiterals", 2.564853760e9f, val3);

var onef:float = 1f;
var neg_onef:float = -1f;
var zerof:float = 0f;
var neg_zerof:float = -0f;

function check(val1:*, val2:*):*
{
    return (val1 / val2);
}

AddStrictTestCase("float / Number", 1, check(onef, 1));
AddStrictTestCase("FloatLiteral / Number", 1, check(1f, 1));
AddStrictTestCase("Number / float", 1, check(1, onef));
AddStrictTestCase("Number / FloatLiteral", 1, check(1, 1f));
AddStrictTestCase("Boolean / float", 1, check(true, onef));
AddStrictTestCase("String / float", 1, check("1", onef));


// If either is NaN then NaN
Assert.expectEq("float.NaN / float", float.NaN, float.NaN / onef);
Assert.expectEq("isNaN float.NaN / float", true, isNaN(float.NaN / onef));
Assert.expectEq("float / float.NaN", float.NaN, onef / float.NaN);
Assert.expectEq("isNaN float / float.NaN", true, isNaN(onef / float.NaN));
Assert.expectEq("check(float.NaN, float)", float.NaN, check(float.NaN, onef));
Assert.expectEq("isNaN check(float.NaN, float)", true, isNaN(check(float.NaN, onef)));
Assert.expectEq("check(float, float.NaN)", float.NaN, check(onef, float.NaN));
Assert.expectEq("isNaN check(float, float.NaN)", true, isNaN(check(onef, float.NaN)));

Assert.expectEq("float.NaN / FloatLiteral", float.NaN, float.NaN / 1f);
Assert.expectEq("isNaN float.NaN / FloatLiteral", true, isNaN(float.NaN / 1f));
Assert.expectEq("FloatLiteral / float.NaN", float.NaN, 1f / float.NaN);
Assert.expectEq("isNaN FloatLiteral / float.NaN", true, isNaN(1f / float.NaN));
Assert.expectEq("check(float.NaN, FloatLiteral)", float.NaN, check(float.NaN, 1f));
Assert.expectEq("isNaN check(float.NaN, FloatLiteral)", true, isNaN(check(float.NaN, 1f)));
Assert.expectEq("check(FloatLiteral, float.NaN)", float.NaN, check(1f, float.NaN));
Assert.expectEq("isNaN check(FloatLiteral, float.NaN)", true, isNaN(check(1f, float.NaN)));

// result is positive if both have same sign, negative if they differ
AddStrictTestCase("float / float", 1f, 1f / 1f);
AddStrictTestCase("-float / float", -1f, -1f / 1f);
AddStrictTestCase("float / -float", -1f, 1f / -1f);
AddStrictTestCase("-float / -float", 1f, -1f / -1f);

AddStrictTestCase("check(float, float)", 1f, check(1f, 1f));
AddStrictTestCase("check(-float, float)", -1f, check(-1f, 1f));
AddStrictTestCase("check(float, -float)", -1f, check(1f, -1f));
AddStrictTestCase("check(-float, -float)", 1f, check(-1f, -1f));

// Division of an infinity by an infinity results in NaN.
Assert.expectEq("float.POSITIVE_INFINITY / float.POSITIVE_INFINITY", float.NaN, float.POSITIVE_INFINITY / float.POSITIVE_INFINITY);
Assert.expectEq("isNaN float.POSITIVE_INFINITY / float.POSITIVE_INFINITY", true, isNaN(float.POSITIVE_INFINITY / float.POSITIVE_INFINITY));
Assert.expectEq("float.NEGATIVE_INFINITY / float.POSITIVE_INFINITY", float.NaN, float.NEGATIVE_INFINITY / float.POSITIVE_INFINITY);
Assert.expectEq("isNaN float.NEGATIVE_INFINITY / float.POSITIVE_INFINITY", true, isNaN(float.NEGATIVE_INFINITY / float.POSITIVE_INFINITY));
Assert.expectEq("float.POSITIVE_INFINITY / float.NEGATIVE_INFINITY", float.NaN, float.POSITIVE_INFINITY / float.NEGATIVE_INFINITY);
Assert.expectEq("isNaN float.POSITIVE_INFINITY / float.NEGATIVE_INFINITY", true, isNaN(float.POSITIVE_INFINITY / float.NEGATIVE_INFINITY));
Assert.expectEq("float.NEGATIVE_INFINITY / float.NEGATIVE_INFINITY", float.NaN, float.NEGATIVE_INFINITY / float.NEGATIVE_INFINITY);
Assert.expectEq("isNaN float.NEGATIVE_INFINITY / float.NEGATIVE_INFINITY", true, isNaN(float.NEGATIVE_INFINITY / float.NEGATIVE_INFINITY));

Assert.expectEq("check(float.POSITIVE_INFINITY, float.POSITIVE_INFINITY)", float.NaN, check(float.POSITIVE_INFINITY, float.POSITIVE_INFINITY));
Assert.expectEq("isNaN check(float.POSITIVE_INFINITY, float.POSITIVE_INFINITY)", true, isNaN(check(float.POSITIVE_INFINITY, float.POSITIVE_INFINITY)));
Assert.expectEq("check(float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY)", float.NaN, check(float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY));
Assert.expectEq("isNaN check(float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY)", true, isNaN(check(float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY)));
Assert.expectEq("check(float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY)", float.NaN, check(float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY));
Assert.expectEq("isNaN check(float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY)", true, isNaN(check(float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY)));
Assert.expectEq("check(float.NEGATIVE_INFINITY, float.NEGATIVE_INFINITY)", float.NaN, check(float.NEGATIVE_INFINITY, float.NEGATIVE_INFINITY));
Assert.expectEq("isNaN check(float.NEGATIVE_INFINITY, float.NEGATIVE_INFINITY)", true, isNaN(check(float.NEGATIVE_INFINITY, float.NEGATIVE_INFINITY)));


// Division of an infinity by a zero results in an infinity. The sign is determined by the rule already stated above.
AddStrictTestCase("float.POSITIVE_INFINITY / float", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY / 0f);
AddStrictTestCase("check(float.POSITIVE_INFINITY, float)", float.POSITIVE_INFINITY, check(float.POSITIVE_INFINITY, 0f));
AddStrictTestCase("float.POSITIVE_INFINITY / -float", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY / -0f);
AddStrictTestCase("check(float.POSITIVE_INFINITY, -float)", float.NEGATIVE_INFINITY, check(float.POSITIVE_INFINITY, -0f));
AddStrictTestCase("float.NEGATIVE_INFINITY / float", float.NEGATIVE_INFINITY, float.NEGATIVE_INFINITY / 0f);
AddStrictTestCase("check(float.NEGATIVE_INFINITY, float)", float.NEGATIVE_INFINITY, check(float.NEGATIVE_INFINITY, 0f));
AddStrictTestCase("float.NEGATIVE_INFINITY / -float", float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY / -0f);
AddStrictTestCase("check(float.NEGATIVE_INFINITY, -float)", float.POSITIVE_INFINITY, check(float.NEGATIVE_INFINITY, -0f));


// Division of an infinity by a nonzero finite value results in a signed infinity. The sign is determined by the rule already stated above.
AddStrictTestCase("float.POSITIVE_INFINITY / float", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY / 1f);
AddStrictTestCase("check(float.POSITIVE_INFINITY, float)", float.POSITIVE_INFINITY, check(float.POSITIVE_INFINITY, 1f));
AddStrictTestCase("float.POSITIVE_INFINITY / -float", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY / -1f);
AddStrictTestCase("check(float.POSITIVE_INFINITY, -float)", float.NEGATIVE_INFINITY, check(float.POSITIVE_INFINITY, -1f));
AddStrictTestCase("float.NEGATIVE_INFINITY / float", float.NEGATIVE_INFINITY, float.NEGATIVE_INFINITY / 1f);
AddStrictTestCase("check(float.NEGATIVE_INFINITY, float)", float.NEGATIVE_INFINITY, check(float.NEGATIVE_INFINITY, 1f));
AddStrictTestCase("float.NEGATIVE_INFINITY / -float", float.POSITIVE_INFINITY, float.NEGATIVE_INFINITY / -1f);
AddStrictTestCase("check(float.NEGATIVE_INFINITY, -float)", float.POSITIVE_INFINITY, check(float.NEGATIVE_INFINITY, -1f));


// Division of a finite value by an infinity results in zero. The sign is determined by the rule already stated above.
AddStrictTestCase("float / float.POSITIVE_INFINITY", 0f, 1f / float.POSITIVE_INFINITY);
AddStrictTestCase("float / float.POSITIVE_INFINITY sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY / (1f / float.POSITIVE_INFINITY));
AddStrictTestCase("check(float, float.POSITIVE_INFINITY)", 0f, check(1f, float.POSITIVE_INFINITY));
AddStrictTestCase("-float / float.POSITIVE_INFINITY", -0f, -1f / float.POSITIVE_INFINITY);
AddStrictTestCase("-float / float.POSITIVE_INFINITY sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY / (-1f / float.POSITIVE_INFINITY));
AddStrictTestCase("check(-float, float.POSITIVE_INFINITY)", -0f, check(-1f, float.POSITIVE_INFINITY));
AddStrictTestCase("float / float.NEGATIVE_INFINITY", -0f, 1f / float.NEGATIVE_INFINITY);
AddStrictTestCase("float / float.NEGATIVE_INFINITY sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY / (1f / float.NEGATIVE_INFINITY));
AddStrictTestCase("check(float, float.NEGATIVE_INFINITY)", -0f, check(1f, float.NEGATIVE_INFINITY));
AddStrictTestCase("-float / float.NEGATIVE_INFINITY", 0f, -1f / float.NEGATIVE_INFINITY);
AddStrictTestCase("check(-float, float.NEGATIVE_INFINITY)", 0f, check(-1f, float.NEGATIVE_INFINITY));

// Division of a zero by a zero results in NaN; division of zero by any other finite value results in zero,
// with the sign determined by the rule already stated above.
Assert.expectEq("0f / 0f", float.NaN, zerof / zerof);
Assert.expectEq("isNaN 0f / 0f", true, isNaN(zerof / zerof));
Assert.expectEq("0f / -0f", float.NaN, zerof / neg_zerof);
Assert.expectEq("isNaN 0f / -0f", true, isNaN(zerof / neg_zerof));
Assert.expectEq("-0f / -0f", float.NaN, neg_zerof / neg_zerof);
Assert.expectEq("isNaN -0f / -0f", true, isNaN(neg_zerof / neg_zerof));

AddStrictTestCase("0f / 1f", 0f, 0f / 1f);
AddStrictTestCase("0f / 1f sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY / (0f / 1f));
AddStrictTestCase("check(0f, 1f)", 0f, check(0f, 1f));
AddStrictTestCase("0f / -1f", -0f, 0f / -1f);
AddStrictTestCase("0f / -1f sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY / (0f / -1f));
AddStrictTestCase("check(0f, -1f)", -0f, check(0f, -1f));
AddStrictTestCase("-0f / 1f", -0f, -0f / 1f);
AddStrictTestCase("-0f / 1f sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY / (-0f / 1f));
AddStrictTestCase("check(-0f, 1f)", -0f, check(-0f, 1f));
AddStrictTestCase("-0f / -1f", -0f, -0f / -1f);
AddStrictTestCase("-0f / -1f sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY / (-0f / -1f));
AddStrictTestCase("check(-0f, -1f)", 0f, check(-0f, -1f));

// Division of a nonzero finite value by a zero results in a signed infinity. The sign is determined by the rule already stated above.
AddStrictTestCase("1f / 0f", float.POSITIVE_INFINITY, 1f / 0f);
AddStrictTestCase("-1f / 0f", float.NEGATIVE_INFINITY, -1f / 0f);
AddStrictTestCase("1f / -0f", float.NEGATIVE_INFINITY, 1f / -0f);
AddStrictTestCase("-1f / -0f", float.POSITIVE_INFINITY, -1f / -0f);
AddStrictTestCase("check(1f, 0f)", float.POSITIVE_INFINITY, check(1f, 0f));
AddStrictTestCase("check(-1f, 0f)", float.NEGATIVE_INFINITY, check(-1f, 0f));
AddStrictTestCase("check(1f, -0f)", float.NEGATIVE_INFINITY, check(1f, -0f));
AddStrictTestCase("check(-1f, -0f)", float.POSITIVE_INFINITY, check(-1f, -0f));



