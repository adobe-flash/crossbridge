/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "floatUtil.as";


// var SECTION = "6.8.2";
// var VERSION = "AS3";
// var TITLE   = "The != operator";


/*
1. If Type(x) is the same as Type(y), then
   a. If Type(x) is XMLList, x and y have the same number of properties, both the order and values of the properties match, return true.
   b. If Type(x) is XML, then [....]
   c. If Type(x) is Namespace and the x.uri == y.uri, return true
   d. If Type(x) is Qname, x.uri == y.uri, and x.localName == y.localName, return true
   e. If Type(x) is undefined, return true.
   f. If Type(x) is null, return true.
   g. If Type(x) is Number (respectively, float), then
       i.   If x is Number.NaN (respectively, float.NaN), return false.
       ii.  If y is Number.NaN (respectively, float.NaN), return false.
       iii. If x is the same Number (or float) value as y, return true.
       iv.  If x is +0 and y is -0, return true.
       v.   If x is -0 and y is +0, return true.
       vi.  Return false
   h. If Type(x) is String, then return true if x and y are exactly the same sequence of characters (same length and same characters in corresponding positions). Otherwise, return false.
   i. If Type(x) is Boolean, return true if x and y are both true or both false. Otherwise, return false.
   j. Return true if x and y refer to the same object. Otherwise, return false.
2. If x is null and y is undefined, return true.
3. If x is undefined and y is null, return true.
4. If Type(x) is Number and Type(y) is String, return the result of the comparison x == ToNumber(y).
5. If Type(x) is String and Type(y) is Number, return the result of the comparison ToNumber(x)== y.
6. If Type(x) is Boolean, return the result of the comparison ToNumber(x) == y.
7. If Type(y) is Boolean, return the result of the comparison x == ToNumber(y).
8. If Type(x) is XML and has simple content, return ToString(x) == ToString(y)
9. If Type(y) is XML and has simple content, r 9. eturn ToString(x) == ToString(y)
10.If Type(x) is XMLList, return true if either x.length is 0 and the other object is undefined or x.length is 1 and the only element of x equals (==) y.
11.If Type(y) is XMLList, return true if either y.length is 0 and the other object is undefined or
x.length is 1 and the only element of x equals (==) y.
12.If Type(x) is either String or Number or float and Type(y) is Object, return the result of the
comparison x == ToPrimitive(y).
13.If Type(x) is Object and Type(y) is either String or Number or float, return the result of the
comparison ToPrimitive(x) == y.
14. If Type(x) is float and Type(y) is String, return the result of the comparison x == ToFloat(y).
15. If Type(x) is String and Type(y) is float, return the result of the comparison ToFloat(x) == y.
16. If Type(x) is float and Type(y) is Number, return the result of the comparison ToNumber(x) == y.
17. If Type(x) is Number and Type(y) is float, return the result of the comparison x == ToNumber(y).
18. Return false.
*/
// TESTING note: we only need to test steps 1g and steps 12-17, as far as float is concerned.

var zerof:float = 0.0;
var onef:float = 1.0;
var nd:Number = 1.111111111119;
var nf:float = nd; // rounds up
var ns:String = "1.111111111119";
var nd_down:Number = 0.51;
var nf_down:float = 0.51; // rounds down
var ns_down:String = "0.51"; // rounds down;
var no = new MyStringObject(nf);

function notEquals(val1, val2)
{
    return (val1 != val2);
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
Assert.expectEq("float.NaN != 1f", true, (float.NaN != onef));
Assert.expectEq("float.NaN != float.NaN", true, (float.NaN != float.NaN));
Assert.expectEq("float.NaN != float.POSITIVE_INFINITY+float.NEGATIVE_INFINITY", true, (float.NaN!=float.POSITIVE_INFINITY+float.NEGATIVE_INFINITY));
Assert.expectEq("float.NaN != zerof", true, (float.NaN != zerof));

// ii.  If y is float.NaN, return false.
Assert.expectEq("1f != float.NaN", true, (1f != float.NaN));
Assert.expectEq("float.MIN_VALUE != float.NaN", true, (float.MIN_VALUE != float.NaN));
Assert.expectEq("float.POSITIVE_INFINITY+float.NEGATIVE_INFINITY != float.NaN", true, (float.POSITIVE_INFINITY+float.NEGATIVE_INFINITY!=float.NaN));
Assert.expectEq("zerof != float.NaN", true, (zerof != float.NaN));

// iii. If x is the same float value as y, return true.
Assert.expectEq("float.POSITIVE_INFINITY != float.POSITIVE_INFINITY", false, (float.POSITIVE_INFINITY != float.POSITIVE_INFINITY));
Assert.expectEq("float.POSITIVE_INFINITY != -float.NEGATIVE_INFINITY", false, (float.POSITIVE_INFINITY != -float.NEGATIVE_INFINITY));
Assert.expectEq("float.POSITIVE_INFINITY != float.POSITIVE_INFINITY-float.MAX_VALUE ", false, (float.POSITIVE_INFINITY != float.POSITIVE_INFINITY-float.MAX_VALUE ));
Assert.expectEq("float.NEGATIVE_INFINITY != float.NEGATIVE_INFINITY", false, (float.NEGATIVE_INFINITY != float.NEGATIVE_INFINITY));
Assert.expectEq("-float.POSITIVE_INFINITY != float.NEGATIVE_INFINITY", false, (-float.POSITIVE_INFINITY != float.NEGATIVE_INFINITY));
Assert.expectEq("float.NEGATIVE_INFINITY != float.NEGATIVE_INFINITY+float.MAX_VALUE ", false, (float.NEGATIVE_INFINITY != float.NEGATIVE_INFINITY+float.MAX_VALUE ));
Assert.expectEq("zerof != zerof", false, (zerof != zerof));
Assert.expectEq("-zerof != -zerof", false, (-zerof != -zerof));
Assert.expectEq("1f != onef", false, (1f != onef));

// iv.  If x is +0f and y is -0f, return true.
Assert.expectEq("zerof != -zerof", false, (zerof != -zerof));
Assert.expectEq("0f != -0f", false, (0f != -0f));
Assert.expectEq("notEquals() 0f != -0f", false, notEquals(0f, -0f));

// v.   If x is -0f and y is +0f, return true.
Assert.expectEq("-zerof != zerof", false, (-zerof != zerof));
Assert.expectEq("-0f != 0f", false, (-0f != 0f));
Assert.expectEq("notEquals() -0f != 0f", false, notEquals(-0f, 0f));

// vi.  Return false
Assert.expectEq("zerof != float.MIN_VALUE", true, (zerof != float.MIN_VALUE));
Assert.expectEq("-float.MIN_VALUE != zerof", true, ( -float.MIN_VALUE != zerof));
Assert.expectEq("float.POSITIVE_INFINITY != float.MAX_VALUE ", true, (float.POSITIVE_INFINITY != float.MAX_VALUE ));
Assert.expectEq("float.NEGATIVE_INFINITY != -float.MAX_VALUE ", true, (float.NEGATIVE_INFINITY != -float.MAX_VALUE ));
Assert.expectEq("nf != float.MIN_VALUE", true, (nf != float.MIN_VALUE));
Assert.expectEq("-float.MIN_VALUE == float.MIN_VALUE ", false, (-float.MIN_VALUE == float.MIN_VALUE ));



// 5. If type1 is Number and type2 is String or float, return the result of the comparison val1 != ToNumber (val2).
//"Number != Number(float), i.e. '!=' calls ToNumber on float; round_up test";
Assert.expectEq("nd != nf", true, (nd != nf));
Assert.expectEq("nf != Number(nf)", false, (Number(nf) != nf));
//"Number != Number(float), i.e. '!=' calls ToNumber on float; round_down test";
Assert.expectEq("nd_down != ndfdown", true, (nd_down != nf_down));
Assert.expectEq("Number(nf_down) != nf_down ", false, (Number(nf_down) != nf_down ));
// If this comparision is false, then the largeNumber was converted to float
var largeNumber:Number = 3.40282346638529e+38;
Assert.expectEq("largeNumber != float.POSITIVE_INFINITY", true, (largeNumber != float.POSITIVE_INFINITY));
// If this comparision is false, then Number.MIN_VALUE was converted to float
Assert.expectEq("Number.MIN_VALUE != 0f", true, (Number.MIN_VALUE != 0f));
Assert.expectEq("float(Number.MIN_VALUE) != 0f", false, (float(Number.MIN_VALUE) != 0f));

// 6. If type1 is String or a float and type2 is Number, return the result of the comparison ToNumber (val1) != val2.
//"Number(float) != Number, i.e. '!=' calls ToNumber on float; round_up test";
Assert.expectEq("nf != nd", true, (nf != nd));
Assert.expectEq("Number(nf) != nf", false, ( nf != Number(nf)));
//"Number(float) != Number, i.e. '!=' calls ToNumber on float; round_down test";
Assert.expectEq("nf_down != nd_down", true, (nf_down != nd_down));
Assert.expectEq("Number(nf_down) != nf_down ", false, (nf_down != Number(nf_down) ));
// If this comparision is false, then the largeNumber was converted to float
var largeNumber:Number = 3.40282346638529e+38;
Assert.expectEq("float.POSITIVE_INFINITY != largeNumber", true, (float.POSITIVE_INFINITY != largeNumber));
// If this comparision is false, then Number.MIN_VALUE was converted to float
Assert.expectEq("0f != Number.MIN_VALUE", true, (0f != Number.MIN_VALUE));
Assert.expectEq("0f != float(Number.MIN_VALUE)", false, (0f != float(Number.MIN_VALUE)));



// 13. If type1 is either String, Number or float and type2 is Object, return the result of the comparison val1 == ToPrimitive(val2).
Assert.expectEq("ns != no", true, (ns != no));
//"Object != float; ToPrimitive should return float, which in turn should compare via ToFloat - hence, equality";
Assert.expectEq("nf != no", false, (nf != no));
//"Object != float; ToPrimitive should return Number - hence, inequality first, equality next";
Assert.expectEq("nf != new MyObject(nd)", true, (nf != new MyObject(nd)));
Assert.expectEq("nf == new MyObject(Number(nf))", true, (nf == new MyObject(Number(nf))));

// If this comparision is true, then the largeNumber was converted to float
var largeNumber:Number = 3.40282346638529e+38;
Assert.expectEq("float.POSITIVE_INFINITY == new MyObject(largeNumber)", false, (float.POSITIVE_INFINITY == new MyObject(largeNumber)));
// If this comparision is true, then Number.MIN_VALUE was converted to float
Assert.expectEq("0f == new MyObject(Number.MIN_VALUE)", false, (0f == new MyObject(Number.MIN_VALUE)));

//"Object != Number; but the float.ToString rendering is precise, so the equality should fail.";
Assert.expectEq("nd != no", true, (nd != no));


// 14. If type1 is Object and type2 is either String, Number or float, return the result of the comparison ToPrimitive(val1) == val2.
Assert.expectEq("no != ns", true, (no != ns));
//"Object != float; ToPrimitive should return float, which in turn should compare via ToFloat - hence, equality";
Assert.expectEq("no != nf", false, (no != nf));
//"Object != float; ToPrimitive should return Number - hence, inequality first, equality next";
Assert.expectEq("new MyObject(Number(nf)) != nf", false, (new MyObject(Number(nf)) != nf));
//"Object == float; ToPrimitive should return Number - hence, inequality first, equality next";
Assert.expectEq("new MyObject(nd) == nf", false, (new MyObject(nd) == nf));
Assert.expectEq("new MyObject(Number(nf)) == nf", true, (new MyObject(Number(nf)) == nf));

// If this comparision is true, then the largeNumber was converted to float
var largeNumber:Number = 3.40282346638529e+38;
Assert.expectEq("new MyObject(largeNumber) == float.POSITIVE_INFINITY", false, (new MyObject(largeNumber) == float.POSITIVE_INFINITY));
// If this comparision is true, then Number.MIN_VALUE was converted to float
Assert.expectEq("new MyObject(Number.MIN_VALUE) == 0f", false, (new MyObject(Number.MIN_VALUE) == 0f));

//"Object != Number; but the float.ToString rendering is precise, so the equality should fail.";
Assert.expectEq("no != nd", true, (no != nd));



//"float != String; ToFloat called on String (round_up test)";
Assert.expectEq("nf != ns", true, (nf != ns));
//"float != String; ToFloat called on String (round_down test)";
Assert.expectEq(" ns_down != nf_down", true, ( ns_down != nf_down));
Assert.expectEq(" nf_down != ns_down", true, ( nf_down != ns_down));


/// some last extra tests
Assert.expectEq("zerof != null", true, (zerof != null));
Assert.expectEq("null != zerof", true, (null != zerof));
Assert.expectEq("undefined != zerof", true, (undefined != zerof));
Assert.expectEq("zerof != undefined", true, (zerof != undefined));


// Step 16-17:
//"float(number) != number, i.e. '!=' calls ToNumber on float; round_up test";
Assert.expectEq("nd != nf", true, (nd != nf));
Assert.expectEq("nf != Number(nf)", false, (nf != Number(nf)));
//"float(number) != number, i.e. '!=' calls ToNumber on float; round_down test";
Assert.expectEq("nf_down != nd_down", true, (nf_down != nd_down));
Assert.expectEq("Number(nf_down) != nf_down ", false, (Number(nf_down) != nf_down ));





// Section 6.8.1 - 6.8.2:  evaluation order (first operand evaluated first):
var nvo = new MyValueAlteringObject(nf);
Assert.expectEq(" '!=' , evaluation order", false, (nvo+1) != (0+nvo) ); // (0+1)!=(2-1)

Assert.expectEq("new MyObject(float(0xFFFFFF00)) != 0xFFFFFF01)", true, (new MyObject(float(0xFFFFFF00)) != 0xFFFFFF01));

// Weirdo case
var v3 = new MyObject(float(0xFFFFFF00));
var v4 = "4294967041"; // i.e. 0xFFFFFF01
// typeof v3.valueOf() -> float
// but v4 is a string so it will be a Number
Assert.expectEq("Weirdo spec behaviour: Object!=String, when ToPrimitive(Object)=float", true, v3!=v4);
v3++;
v4++;
// After post-increment, these should become Nubmer, and no longer equal
Assert.expectEq("Weirdo spec behaviour: after post-increment", true, v3!=v4);


