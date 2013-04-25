/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "4.6.1";
// var VERSION = "AS3";
// var TITLE   = "AS3 function toString ( radix=10 ) : String";


var test_flt4:float4 = new float4(2.7182818284e5f);

Assert.expectEq("float4.AS3::toString = function", true, test_flt4.AS3::toString is Function);
Assert.expectEq("float4.AS3::toString returns a String", "String", getQualifiedClassName(test_flt4.AS3::toString()));

Assert.expectEq("float4.AS3::toString: implicit radix is 10", test_flt4.AS3::toString(10), test_flt4.AS3::toString());

Assert.expectEq("float4.AS3::toString: with string radix", test_flt4.AS3::toString(10), test_flt4.AS3::toString("10"));

test_flt4 = new float4(float.NaN);
Assert.expectEq("No range error for NaN", "NaN,NaN,NaN,NaN", test_flt4..AS3::toString(99));
test_flt4 = new float4(float.POSITIVE_INFINITY);
Assert.expectEq("No range error for +Inf", "Infinity,Infinity,Infinity,Infinity", test_flt4..AS3::toString(99));
test_flt4 = new float4(float.NEGATIVE_INFINITY);
Assert.expectEq("No range error for -Inf", "-Infinity,-Infinity,-Infinity,-Infinity", test_flt4..AS3::toString(99));

test_flt4 = new float4(1f);
Assert.expectError("float4.AS3::toString: range errors toString(37)", Utils.RANGEERROR, function(){test_flt4.AS3::toString(37)});
Assert.expectError("float4.AS3::toString: range errors toString(1)", Utils.RANGEERROR, function(){test_flt4.AS3::toString(1)});
Assert.expectError("float4.AS3::toString: range errors toString(null)", Utils.RANGEERROR, function(){test_flt4.AS3::toString(null)});
Assert.expectError("float4.AS3::toString: range errors toString(undefined)", Utils.RANGEERROR, function(){test_flt4.AS3::toString(undefined)});
Assert.expectError("float4.AS3::toString: range errors toString(float.NaN)", Utils.RANGEERROR, function(){test_flt4.AS3::toString(float.NaN)});
Assert.expectError("float4.AS3::toString: range errors toString(Infinity)", Utils.RANGEERROR, function(){test_flt4.AS3::toString(Infinity)});
Assert.expectError("float4.AS3::toString: range errors toString(-Infinity)", Utils.RANGEERROR, function(){test_flt4.AS3::toString(-Infinity)});

test_flt4 = new float4(2.7182818284e5f);
Assert.expectEq("float4.AS3::toString: base 36", "5tqs,5tqs,5tqs,5tqs", test_flt4.AS3::toString(36)); // should go on to convert only the integral part
Assert.expectEq("float4.AS3::toString: base 2", "1000010010111010100,1000010010111010100,1000010010111010100,1000010010111010100", test_flt4.AS3::toString(2)); // should go on to convert only the integral part
Assert.expectEq("float4.AS3::toString: decimal", "5tqs,5tqs,5tqs,5tqs", test_flt4.AS3::toString(36.2)); // should covert radix to int
Assert.expectEq("float4.AS3::toString: hexadecimal", "5tqs,5tqs,5tqs,5tqs", test_flt4.AS3::toString(0x24)); // should covert radix to int

Assert.expectEq("float4.AS3::toString: NaN, in base 16", "NaN,NaN,NaN,NaN", float4(float.NaN).AS3::toString(16)); // base doesn't matter, still prints 'NaN'

Assert.expectEq("float4.AS3::toString: float.MAX_VALUE, in base 16",
            "ffffff00000000000000000000000000,ffffff00000000000000000000000000,ffffff00000000000000000000000000,ffffff00000000000000000000000000",
            float4(float.MAX_VALUE).AS3::toString(16));

var expected:String = (340282346638528859811704183484516925440).toString(16);
Assert.expectEq("float4.AS3::toString: float.MAX_VALUE, in base 16",
            expected+','+expected+','+expected+','+expected,
            float4(float.MAX_VALUE).AS3::toString(16));


Assert.expectEq("float4.AS3::toString: -float.MAX_VALUE, in base 16",
            "-ffffff00000000000000000000000000,-ffffff00000000000000000000000000,-ffffff00000000000000000000000000,-ffffff00000000000000000000000000",
            float4(-float.MAX_VALUE).AS3::toString(16));

expected = (-340282346638528859811704183484516925440).toString(16);
Assert.expectEq("float4.AS3::toString: -float.MAX_VALUE, in base 16",
            expected+','+expected+','+expected+','+expected,
            float4(-float.MAX_VALUE).AS3::toString(16));

Assert.expectEq("float4.AS3::toString: -float.MIN_VALUE, in base 16",
            "0,0,0,0",
            float4(-float.MIN_VALUE).AS3::toString(16)); // shouldn't be "-0"!

Assert.expectEq("float.AS3::toString: Infinity, in base 16",
            "Infinity,Infinity,Infinity,Infinity",
            float4(float.POSITIVE_INFINITY).AS3::toString(16)); // base doesn't matter, still prints 'Infinity'

Assert.expectEq("float.AS3::toString: -Infinity, in base 16",
            "-Infinity,-Infinity,-Infinity,-Infinity",
            float4(float.NEGATIVE_INFINITY).AS3::toString(16)); // base doesn't matter, still prints '-Infinity'


