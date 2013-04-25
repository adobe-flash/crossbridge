/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "floatUtil.as";


// var SECTION = "6.8.3";
// var VERSION = "AS3";
// var TITLE   = "The === operator";


var onef:float = 1f;
var zerof:float = 0.0f;
var onen:Number = 1;
var zeron:Number = 0.0;
var nd:Number = 1.111111111119;
var nf:float = nd; // rounds up
var ns:String = "1.111111111119";
var nd_down:Number = 0.51;
var nf_down:float = 0.51; // rounds down
var ns_down:String = "0.51"; // rounds down;
var no = new MyStringObject(nf);

/* Strict equality comparison:
1. If Type(x) is different from Type(y), then
   a. If Type(x) is float and Type(y) is Number, return the result of the comparison ToNumber(x) == y.
   b. If Type(x) is Number and Type(y) is float, return the result of the comparison x == ToNumber(y).
   c. return false
2. If Type(x) is undefined, return true.
3. If Type(x) is null, return true.
4. If Type(x) is Number (respectively, float), then
   a. If x is Number.NaN (respectively, float.NaN), return false.
   b. If y is Number.NaN (respectively, float.NaN), return false.
   c. If x is the same Number (respectively, float) value as y, return true.
   d. If x is +0 and y is -0, return true.
   e. If x is -0 and y is +0, return true.
   f. Return false
5. If Type(x) is String, then return true if x and y are exactly the same sequence of characters (same length and same characters in corresponding positions). Otherwise, return false.
6. If Type(x) is Boolean, return true if x and y are both true or both false; otherwise, return false.
7. Return true if x and y refer to the same object. Otherwise, return false.
*/
//TEST NOTE: we test points "1" and and "4" only.

function strictEquals(val1, val2)
{
    return (val1 === val2);
}

// Let type1=Type(val1) and type2=Type(val2), If type1 is not the same as type2 then return false
Assert.expectEq("1f === 1", false, (onef === onen));
Assert.expectEq("1f FloatLiteral === 1 NumberLiteral", true, (1f === 1));
Assert.expectEq("strictEquals() 1f FloatLiteral === 1 NumberLiteral", false, strictEquals(1f, 1));
Assert.expectEq("1.1f === 1.1", false, (1.1f === 1.1));
Assert.expectEq("strictEquals() 1.1f === 1.1", false, strictEquals(1.1f, 1.1));
Assert.expectEq("float.NaN === NaN", false, (float.NaN === NaN));
Assert.expectEq("zerof === -0.0", true, (zerof === -0.0));
Assert.expectEq("nf === Number(nf)", false, (nf === Number(nf)));
Assert.expectEq("nf_down === Number(nf_down)", false, (nf_down === Number(nf_down)));

// Let type1=Type(val1) and type2=Type(val2), If type1 is not the same as type2 then return false
Assert.expectEq("1 === 1f", false, (onen === onef));
Assert.expectEq("1 NumberLiteral === 1f FloatLiteral", true, (1 === 1f));
Assert.expectEq("strictEquals() 1 NumberLiteral === 1f FloatLiteral", false, strictEquals(1, 1f));
Assert.expectEq("1.1 === 1.1f", false, (1.1 === 1.1f));
Assert.expectEq("strictEquals() 1.1 === 1.1f", false, strictEquals(1.1, 1.1f));
Assert.expectEq("Number.NaN === float.NaN", false, (Number.NaN === float.NaN));
Assert.expectEq("0.0 === -zerof", true, (0.0 === -zerof));
Assert.expectEq("Number(nf) === nf", false, (Number(nf) === nf));
Assert.expectEq("Number(nf_down) === nf_down", false, (Number(nf_down) === nf_down));


// 2 (c) If type1 is different from type2: return false
Assert.expectEq("undefined === zerof", false, (undefined === zerof));
Assert.expectEq("null === zerof", false, (null === zerof));
Assert.expectEq("false === zerof", false, (false === zerof));
Assert.expectEq("\"0\" === zerof", false, ("0" === zerof));
Assert.expectEq("strictEquals() \"0\" === zerof", false, strictEquals("0", zerof));
Assert.expectEq("nf_down  === String(nf_down)", false, (nf_down  === String(nf_down)));
Assert.expectEq("new MyObject(nf)  === nf", false, (new MyObject(nf)  === nf));

// 6. If type1 is float:
// (a) If val1 is float.NaN return false.
Assert.expectEq("float.NaN === onef", false, (float.NaN === onef));
Assert.expectEq("float.NaN === 1f", false, (float.NaN === 1f));
Assert.expectEq("float.NaN === (float.POSITIVE_INFINITY/float.NEGATIVE_INFINITY)", false, (float.NaN === (float.POSITIVE_INFINITY/float.NEGATIVE_INFINITY)));

// (b) If val2 is float.NaN return false.
Assert.expectEq("onef === float.NaN", false, (onef === float.NaN));
Assert.expectEq("1f === float.NaN", false, (1f === float.NaN));
Assert.expectEq("(float.POSITIVE_INFINITY/float.NEGATIVE_INFINITY) === float.NaN", false, ((float.POSITIVE_INFINITY/float.NEGATIVE_INFINITY) === float.NaN));

// (c) If val1 is the same float value as val2, return true.
Assert.expectEq("float.MIN_VALUE === float.MIN_VALUE-zerof", true, (float.MIN_VALUE === float.MIN_VALUE-zerof));
Assert.expectEq("float.MAX_VALUE === float.MIN_VALUE+float.MAX_VALUE", true, (float.MAX_VALUE === float.MIN_VALUE+float.MAX_VALUE));
Assert.expectEq("float.NEGATIVE_INFINITY === -float.POSITIVE_INFINITY", true, (float.NEGATIVE_INFINITY === -float.POSITIVE_INFINITY));
Assert.expectEq("-float.NEGATIVE_INFINITY === float.POSITIVE_INFINITY", true, (-float.NEGATIVE_INFINITY === float.POSITIVE_INFINITY));
Assert.expectEq("onef === 1f", true, (onef === 1f));
Assert.expectEq("1f === onef", true, (1f === onef));
Assert.expectEq("float(0xFFFFFF00) === float(0xFFFFFF01)", true, (float(0xFFFFFF00) === float(0xFFFFFF01)));
Assert.expectEq("1.1f === 1.100000023841858f", true, (1.1f === 1.100000023841858f));
Assert.expectEq("strictEquals() 1.1f === 1.100000023841858f", true, strictEquals(1.1f, 1.100000023841858f));

// (d) If val1 is +0 and val2 is -0, return true.
Assert.expectEq("zerof === -zerof", true, (zerof === -zerof));
Assert.expectEq("0f === -0f", true, (0f === -0f));
Assert.expectEq("stictEquals() 0f === -0f", true, strictEquals(0f, -0f));

// (e) If val1 is -0 and val2 is +0, return true.
Assert.expectEq("-zerof === zerof", true, (-zerof === zerof));
Assert.expectEq("-0f === 0f", true, (-0f === 0f));
Assert.expectEq("strictEquals() -0f === 0f", true, strictEquals(-0f, 0f));

// (f) Return false
Assert.expectEq("-onef === onef", false, (-onef === onef));
Assert.expectEq("-1f === 1f", false, (-1f === 1f));
Assert.expectEq("strictEquals() -1f === 1f", false, strictEquals(-1f, 1f));

// 8. Return true if val1 and val2 refer to the same object. Otherwise, return false
Assert.expectEq("new MyObject(1f)  === new MyObject(1f)", false, (new MyObject(1f)  === new MyObject(1f)));
var var1 = new MyObject(1f);
Assert.expectEq("var1  === var1", true, (var1  === var1));

var nvo = new MyValueAlteringObject(nf);
Assert.expectEq("Evaluation order: '===' ", true,(zerof+nvo) === (nvo-1f));


