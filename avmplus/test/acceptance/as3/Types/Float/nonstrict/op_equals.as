/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "floatUtil.as";


// var SECTION = "6.8.1";
// var VERSION = "AS3";
// var TITLE   = "The == operator";


var zerof:float = 0.0;
var onef:float = 1.0;
var nd:Number = 1.111111111119;
var nf:float = nd; // rounds up
var ns:String = "1.111111111119";
var nd_down:Number = 0.51;
var nf_down:float = 0.51; // rounds down
var ns_down:String = "0.51"; // rounds down;
var no = new MyStringObject(nf);

function equals(val1, val2)
{
    return (val1 == val2);
}

// Step 1g+
/*
   g+. If Type(x) is float, then
       i.   If x is float.NaN, return false.
       ii.  If y is float.NaN, return false.
       iii. If x is the same float value as y, return true.
       iv.  If x is +0f and y is -0f, return true.
       v.   If x is -0f and y is +0f, return true.
       vi.  Return false
*/

// i.   If x is float.NaN, return false.
Assert.expectEq("float.NaN == 1f", false, (float.NaN == onef));
Assert.expectEq("float.NaN == float.NaN", false, (float.NaN == float.NaN));
Assert.expectEq("float.NaN == float.POSITIVE_INFINITY+float.NEGATIVE_INFINITY", false, (float.NaN==float.POSITIVE_INFINITY+float.NEGATIVE_INFINITY));
Assert.expectEq("float.NaN == zerof", false, (float.NaN == zerof));

// ii.  If y is float.NaN, return false.
Assert.expectEq("1f == float.NaN", false, (1f == float.NaN));
Assert.expectEq("float.MIN_VALUE == float.NaN", false, (float.MIN_VALUE == float.NaN));
Assert.expectEq("float.POSITIVE_INFINITY+float.NEGATIVE_INFINITY==float.NaN", false, (float.POSITIVE_INFINITY+float.NEGATIVE_INFINITY==float.NaN));
Assert.expectEq("zerof == float.NaN", false, (zerof == float.NaN));

// iii. If x is the same float value as y, return true.
Assert.expectEq("float.POSITIVE_INFINITY == float.POSITIVE_INFINITY", true, (float.POSITIVE_INFINITY == float.POSITIVE_INFINITY));
Assert.expectEq("float.POSITIVE_INFINITY == -float.NEGATIVE_INFINITY", true, (float.POSITIVE_INFINITY == -float.NEGATIVE_INFINITY));
Assert.expectEq("float.POSITIVE_INFINITY == float.POSITIVE_INFINITY-float.MAX_VALUE ", true, (float.POSITIVE_INFINITY == float.POSITIVE_INFINITY-float.MAX_VALUE ));
Assert.expectEq("float.NEGATIVE_INFINITY == float.NEGATIVE_INFINITY", true, (float.NEGATIVE_INFINITY == float.NEGATIVE_INFINITY));
Assert.expectEq("-float.POSITIVE_INFINITY == float.NEGATIVE_INFINITY", true, (-float.POSITIVE_INFINITY == float.NEGATIVE_INFINITY));
Assert.expectEq("float.NEGATIVE_INFINITY == float.NEGATIVE_INFINITY+float.MAX_VALUE ", true, (float.NEGATIVE_INFINITY == float.NEGATIVE_INFINITY+float.MAX_VALUE ));
Assert.expectEq("zerof == zerof", true, (zerof == zerof));
Assert.expectEq("-zerof == -zerof", true, (-zerof == -zerof));
Assert.expectEq("1f == onef", true, (1f == onef));

// iv.  If x is +0f and y is -0f, return true.
Assert.expectEq("zerof == -zerof", true, (zerof == -zerof));
Assert.expectEq("0f == -0f", true, (0f == -0f));
Assert.expectEq("equals() 0f == -0f", true, equals(0f, -0f));

// v.   If x is -0f and y is +0f, return true.
Assert.expectEq("-zerof == zerof", true, (-zerof == zerof));
Assert.expectEq("-0f == 0f", true, (-0f == 0f));
Assert.expectEq("equals() -0f == 0f", true, equals(-0f, 0f));

// vi.  Return false
Assert.expectEq("zerof == float.MIN_VALUE", false, (zerof == float.MIN_VALUE));
Assert.expectEq("-float.MIN_VALUE == zerof", false, ( -float.MIN_VALUE == zerof));
Assert.expectEq("float.POSITIVE_INFINITY == float.MAX_VALUE ", false, (float.POSITIVE_INFINITY == float.MAX_VALUE ));
Assert.expectEq("float.NEGATIVE_INFINITY == -float.MAX_VALUE ", false, (float.NEGATIVE_INFINITY == -float.MAX_VALUE ));
Assert.expectEq("nf == float.MIN_VALUE", false, (nf == float.MIN_VALUE));
Assert.expectEq("-float.MIN_VALUE == float.MIN_VALUE ", false, (-float.MIN_VALUE == float.MIN_VALUE ));



// 5. If type1 is Number and type2 is String or float, return the result of the comparison val1 == ToNumber (val2).
//"Number == Number(float), i.e. '==' calls ToNumber on float; round_up test";
Assert.expectEq("nd == nf", false, (nd == nf));
Assert.expectEq("nf == Number(nf)", true, (Number(nf) == nf));
//"Number == Number(float), i.e. '==' calls ToNumber on float; round_down test";
Assert.expectEq("nd_down == ndfdown", false, (nd_down == nf_down));
Assert.expectEq("Number(nf_down) == nf_down ", true, (Number(nf_down) == nf_down ));
// If this comparision is true, then the largeNumber was converted to float
var largeNumber:Number = 3.40282346638529e+38;
Assert.expectEq("largeNumber == float.POSITIVE_INFINITY", false, (largeNumber == float.POSITIVE_INFINITY));
// If this comparision is true, then Number.MIN_VALUE was converted to float
Assert.expectEq("Number.MIN_VALUE == 0f", false, (Number.MIN_VALUE == 0f));
Assert.expectEq("float(Number.MIN_VALUE) == 0f", true, (float(Number.MIN_VALUE) == 0f));



// 6. If type1 is String or a float and type2 is Number, return the result of the comparison ToNumber (val1) == val2.
//"Number(float) == Number, i.e. '==' calls ToNumber on float; round_up test";
Assert.expectEq("nf == nd", false, (nf == nd));
Assert.expectEq("Number(nf) == nf", true, ( nf == Number(nf)));
//"Number(float) == Number, i.e. '==' calls ToNumber on float; round_down test";
Assert.expectEq("nf_down == nd_down", false, (nf_down == nd_down));
Assert.expectEq("Number(nf_down) == nf_down ", true, (nf_down == Number(nf_down) ));
// If this comparision is true, then the largeNumber was converted to float
var largeNumber:Number = 3.40282346638529e+38;
Assert.expectEq("float.POSITIVE_INFINITY == largeNumber", false, (float.POSITIVE_INFINITY == largeNumber));
// If this comparision is true, then Number.MIN_VALUE was converted to float
Assert.expectEq("0f == Number.MIN_VALUE", false, (0f == Number.MIN_VALUE));
Assert.expectEq("0f == float(Number.MIN_VALUE)", true, (0f == float(Number.MIN_VALUE)));


// 13. If type1 is either String, Number or float and type2 is Object, return the result of the comparison val1 == ToPrimitive(val2).
Assert.expectEq("ns == no", false, (ns == no));
//"float == Object; ToPrimitive should return float, which in turn should compare via ToFloat - hence, equality";
Assert.expectEq("nf == no", true, (nf == no));
//"float == Object; ToPrimitive should return Number - hence, inequality first, equality next";
Assert.expectEq("nf == new MyObject(nd)", false, (nf == new MyObject(nd)));
Assert.expectEq("nf == new MyObject(Number(nf))", true, (nf == new MyObject(Number(nf))));

// If this comparision is true, then the largeNumber was converted to float
var largeNumber:Number = 3.40282346638529e+38;
Assert.expectEq("float.POSITIVE_INFINITY == new MyObject(largeNumber)", false, (float.POSITIVE_INFINITY == new MyObject(largeNumber)));
// If this comparision is true, then Number.MIN_VALUE was converted to float
Assert.expectEq("0f == new MyObject(Number.MIN_VALUE)", false, (0f == new MyObject(Number.MIN_VALUE)));

//"Number == Object; but the float.ToString rendering is precise, so the equality should fail.";
Assert.expectEq("nd == no", false, (nd == no));



// 14. If type1 is Object and type2 is either String, Number or float, return the result of the comparison ToPrimitive(val1) == val2.
Assert.expectEq("no == ns", false, (no == ns));
//"Object == float; ToPrimitive should return float, which in turn should compare via ToFloat - hence, equality";
Assert.expectEq("no == nf", true, (no == nf));
//"Object == float; ToPrimitive should return Number - hence, inequality first, equality next";
Assert.expectEq("new MyObject(nd) == nf", false, (new MyObject(nd) == nf));
Assert.expectEq("new MyObject(Number(nf)) == nf", true, (new MyObject(Number(nf)) == nf));

// If this comparision is true, then the largeNumber was converted to float
var largeNumber:Number = 3.40282346638529e+38;
Assert.expectEq("new MyObject(largeNumber) == float.POSITIVE_INFINITY", false, (new MyObject(largeNumber) == float.POSITIVE_INFINITY));
// If this comparision is true, then Number.MIN_VALUE was converted to float
Assert.expectEq("new MyObject(Number.MIN_VALUE) == 0f", false, (new MyObject(Number.MIN_VALUE) == 0f));

//"Object == Number; but the float.ToString rendering is precise, so the equality should fail.";
Assert.expectEq("no == nd", false, (no == nd));

//"float == String; ToFloat called on String (round_up test)";
// nf == ToNumeric(ns) (8.a) -> nf = ToNumber(ns)
Assert.expectEq("nf == ns", false, (nf == ns));
//"float == String; ToFloat called on String (round_down test)";
Assert.expectEq(" ns_down == nf_down", false, ( ns_down == nf_down));
Assert.expectEq(" ns_down == nf_down", true, ( nf_down == nf_down));


/// some last extra tests
Assert.expectEq("zerof == null", false, (zerof == null));
Assert.expectEq("null == zerof", false, (null == zerof));
Assert.expectEq("undefined == zerof", false, (undefined == zerof));
Assert.expectEq("zerof == undefined", false, (zerof == undefined));


// Section 6.8.1 - 6.8.2:  evaluation order (first operand evaluated first):
var nvo = new MyValueAlteringObject(nf);
Assert.expectEq(" '==' , evaluation order", true, (0+nvo)==(nvo-1)); // (0+1)==(2-1)


Assert.expectEq("new MyObject(float(0xFFFFFF00)) == 0xFFFFFF01)", false, (new MyObject(float(0xFFFFFF00)) == 0xFFFFFF01));

// Weirdo case for Step 14
var v3 = new MyObject(float(0xFFFFFF00));
var v4 = "4294967041"; // i.e. 0xFFFFFF01
// typeof v3.valueOf() -> float
// but v4 is a string so it will be a Number
Assert.expectEq("Weirdo spec behaviour: Object==String, when ToPrimitive(Object)=float", false, v3==v4);
v3++;
v4++;
// After post-increment, these should become Nubmer, and no longer equal
Assert.expectEq("Weirdo spec behaviour: after post-increment", true, v3!=v4);



