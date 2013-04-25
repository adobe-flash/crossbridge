/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import avmplus.*;
import com.adobe.test.Assert;

// var SECTION = "8.4";
// var VERSION = "AS3";
// var TITLE   = "Verify type-specialized implementations of relational operators involving float4";

// Code coverage test for type-specialized comparisions, e.g., cmpOptimization() in CodegenLIR.
// Verify that specialized comparisons produce the same result as untyped comparisons.  We're
// not looking to verify the underlying mathematics of the comparisions, but rather that the
// case analysis for the type specialization is correct and the correct coercions are performed.


function Cmp_Float4_Float4(op:String, x:float4, y:float4):Boolean
{
    var res:Boolean = false;
    switch (op) {
    case "<":   return x < y;   break;
    case "<=":  return x <= y;  break;
    case ">":   return x > y;   break;
    case ">=":  return x >= y;  break;
    case "==":  return x == y;  break;
    case "!=":  return x != y;  break;
    case "===": return x === y; break;
    case "!==": return x !== y; break;
    }
    return res;
}

function Cmp_Float4_Float(op:String, x:float4, y:float):Boolean
{
    var res:Boolean = false;
    switch (op) {
    case "<":   return x < y;   break;
    case "<=":  return x <= y;  break;
    case ">":   return x > y;   break;
    case ">=":  return x >= y;  break;
    case "==":  return x == y;  break;
    case "!=":  return x != y;  break;
    case "===": return x === y; break;
    case "!==": return x !== y; break;
    }
    return res;
}

function Cmp_Float_Float4(op:String, x:float, y:float4):Boolean
{
    var res:Boolean = false;
    switch (op) {
    case "<":   return x < y;   break;
    case "<=":  return x <= y;  break;
    case ">":   return x > y;   break;
    case ">=":  return x >= y;  break;
    case "==":  return x == y;  break;
    case "!=":  return x != y;  break;
    case "===": return x === y; break;
    case "!==": return x !== y; break;
    }
    return res;
}

function Cmp_Float4_Number(op:String, x:float4, y:Number):Boolean
{
    var res:Boolean = false;
    switch (op) {
    case "<":   return x < y;   break;
    case "<=":  return x <= y;  break;
    case ">":   return x > y;   break;
    case ">=":  return x >= y;  break;
    case "==":  return x == y;  break;
    case "!=":  return x != y;  break;
    case "===": return x === y; break;
    case "!==": return x !== y; break;
    }
    return res;
}

function Cmp_Number_Float4(op:String, x:Number, y:float4):Boolean
{
    var res:Boolean = false;
    switch (op) {
    case "<":   return x < y;   break;
    case "<=":  return x <= y;  break;
    case ">":   return x > y;   break;
    case ">=":  return x >= y;  break;
    case "==":  return x == y;  break;
    case "!=":  return x != y;  break;
    case "===": return x === y; break;
    case "!==": return x !== y; break;
    }
    return res;
}

function Cmp_Float4_Date(op:String, x:float4, y:Date):Boolean
{
    var res:Boolean = false;
    switch (op) {
    case "<":   return x < y;   break;
    case "<=":  return x <= y;  break;
    case ">":   return x > y;   break;
    case ">=":  return x >= y;  break;
    case "==":  return x == y;  break;
    case "!=":  return x != y;  break;
    case "===": return x === y; break;
    case "!==": return x !== y; break;
    }
    return res;
}

function Cmp_Date_Float4(op:String, x:Date, y:float4):Boolean
{
    var res:Boolean = false;
    switch (op) {
    case "<":   return x < y;   break;
    case "<=":  return x <= y;  break;
    case ">":   return x > y;   break;
    case ">=":  return x >= y;  break;
    case "==":  return x == y;  break;
    case "!=":  return x != y;  break;
    case "===": return x === y; break;
    case "!==": return x !== y; break;
    }
    return res;
}

function Cmp_Float4_String(op:String, x:float4, y:String):Boolean
{
    var res:Boolean = false;
    switch (op) {
    case "<":   return x < y;   break;
    case "<=":  return x <= y;  break;
    case ">":   return x > y;   break;
    case ">=":  return x >= y;  break;
    case "==":  return x == y;  break;
    case "!=":  return x != y;  break;
    case "===": return x === y; break;
    case "!==": return x !== y; break;
    }
    return res;
}

function Cmp_String_Float4(op:String, x:String, y:float4):Boolean
{
    var res:Boolean = false;
    switch (op) {
    case "<":   return x < y;   break;
    case "<=":  return x <= y;  break;
    case ">":   return x > y;   break;
    case ">=":  return x >= y;  break;
    case "==":  return x == y;  break;
    case "!=":  return x != y;  break;
    case "===": return x === y; break;
    case "!==": return x !== y; break;
    }
    return res;
}

function Cmp_Float4_Any(op:String, x:float4, y):Boolean
{
    var res:Boolean = false;
    switch (op) {
    case "<":   return x < y;   break;
    case "<=":  return x <= y;  break;
    case ">":   return x > y;   break;
    case ">=":  return x >= y;  break;
    case "==":  return x == y;  break;
    case "!=":  return x != y;  break;
    case "===": return x === y; break;
    case "!==": return x !== y; break;
    }
    return res;
}

function Cmp_Any_Float4(op:String, x, y:float4):Boolean
{
    var res:Boolean = false;
    switch (op) {
    case "<":   return x < y;   break;
    case "<=":  return x <= y;  break;
    case ">":   return x > y;   break;
    case ">=":  return x >= y;  break;
    case "==":  return x == y;  break;
    case "!=":  return x != y;  break;
    case "===": return x === y; break;
    case "!==": return x !== y; break;
    }
    return res;
}

function Cmp_Any_Any(op:String, x, y):Boolean
{
    var res:Boolean = false;
    switch (op) {
    case "<":   return x < y;   break;
    case "<=":  return x <= y;  break;
    case ">":   return x > y;   break;
    case ">=":  return x >= y;  break;
    case "==":  return x == y;  break;
    case "!=":  return x != y;  break;
    case "===": return x === y; break;
    case "!==": return x !== y; break;
    }
    return res;
}

function AddTest(lht:String, op:String, rht:String, fun, x, y)
{
    var actual:Boolean = fun(op, x, y);
    var expected:Boolean = Cmp_Any_Any(op, x, y);
    var testname:String =  x + ":" + lht + " " + op + " " + y + ":" + rht;
    //var result:String = (actual === expected) ? "PASSED" : ("FAILED, expected " + expected);
    //trace(testname + " => " + result);
    Assert.expectEq(testname, expected, actual);
}

function TestComparisons(xt, yt, fun, x, y)
{
    AddTest(xt, "<",   yt, fun, x, y);
    AddTest(xt, "<=",  yt, fun, x, y);
    AddTest(xt, ">",   yt, fun, x, y);
    AddTest(xt, ">=",  yt, fun, x, y);
    AddTest(xt, "==",  yt, fun, x, y);
    AddTest(xt, "!=",  yt, fun, x, y);
    AddTest(xt, "===", yt, fun, x, y);
    AddTest(xt, "!==", yt, fun, x, y);
}

function TestNumericTypes(x, y)
{
    TestComparisons("float4", "float4",  Cmp_Float4_Float4,  float4(x),    float4(y)   );
    TestComparisons("float4", "float",   Cmp_Float4_Float,   float4(x),    float(y)    );
    TestComparisons("float",  "float4",  Cmp_Float_Float4,   float(x),     float4(y)   );
    TestComparisons("float4", "Number",  Cmp_Float4_Number,  float4(x),    Number(y)   );
    TestComparisons("Number", "float4",  Cmp_Number_Float4,  Number(x),    float4(y)   );
}

function TestObjectTypes(x, y)
{
    TestNumericTypes(x, y);

    TestComparisons("float4", "Date",    Cmp_Float4_Date,    float4(x),    new Date(y) );
    TestComparisons("Date",   "float4",  Cmp_Date_Float4,    new Date(x),  float4(y)   );
    TestComparisons("float4", "String",  Cmp_Float4_String,  float4(x),    String(y)   );
    TestComparisons("String", "float4",  Cmp_String_Float4,  String(x),    float4(y)   );
}

function TestSpecialsAnyX(x, y)
{
    TestComparisons("*",      "float4",  Cmp_Any_Float4,     x,            float4(y)   );
}

function TestSpecialsAnyY(x, y)
{
    TestComparisons("float4",  "*",      Cmp_Float4_Any,     float4(x),     y          );
}

function TestSpecialsTypedX(x, y)
{
    TestSpecialsAnyX(x, y);

    TestComparisons("Date",   "float4",  Cmp_Date_Float4,     x,            float4(y)  );
    TestComparisons("String", "float4",  Cmp_String_Float4,   x,            float4(y)  );
}

function TestSpecialsTypedY(x, y)
{
    TestSpecialsAnyY(x, y);

    TestComparisons("float4", "Date",    Cmp_Float4_Date,     float4(x),     y          );
    TestComparisons("floa4t", "String",  Cmp_Float4_String,   float4(x),     y          );
}

function TestValues()
{
    // Zero is interesting, as it is the result of likely coercions.

    TestObjectTypes(0, 0);
    TestObjectTypes(0, 1);
    TestObjectTypes(1, 0);
    TestObjectTypes(1, 1);

    TestNumericTypes(NaN, 0);
    TestNumericTypes(NaN, 1);
    TestNumericTypes(0,   NaN);
    TestNumericTypes(1,   NaN);
    TestNumericTypes(NaN, NaN);

    TestSpecialsTypedX(null, 0);
    TestSpecialsTypedX(null, 1);

    TestSpecialsTypedY(0, null);
    TestSpecialsTypedY(1, null);

    // Undefined is converted to null when coerced to an object type,
    // e.g., when passed as a Date or String parameter.

    TestSpecialsAnyX(undefined, 0);
    TestSpecialsAnyX(undefined, 1);
    TestSpecialsAnyX(true, 0);
    TestSpecialsAnyX(true, 1);
    TestSpecialsAnyX(false, 0);
    TestSpecialsAnyX(false, 1);

    TestSpecialsAnyY(0, undefined);
    TestSpecialsAnyY(1, undefined);
    TestSpecialsAnyY(0, true);
    TestSpecialsAnyY(1, true);
    TestSpecialsAnyY(0, false);
    TestSpecialsAnyY(1, false);
}

TestValues();

