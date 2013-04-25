/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "9.1";
// var VERSION = "AS3";
// var TITLE   = "The operation ToNumber";


var expected:Array = DoubleRawBits(Number.POSITIVE_INFINITY);
var actual:Array = DoubleRawBits(float.POSITIVE_INFINITY);
Assert.expectEq("ToNumber(float.POSITIVE_INFINITY) 0", expected[0], actual[0]);
Assert.expectEq("ToNumber(float.POSITIVE_INFINITY) 1", expected[1], actual[1]);

expected = DoubleRawBits(Number.NEGATIVE_INFINITY);
actual   = DoubleRawBits(float.NEGATIVE_INFINITY);
Assert.expectEq("ToNumber(float.NEGATIVE_INFINITY) 0", expected[0], actual[0]);
Assert.expectEq("ToNumber(float.NEGATIVE_INFINITY) 1", expected[1], actual[1]);

expected = DoubleRawBits(12345);
actual   = DoubleRawBits(12345f);
Assert.expectEq("ToNumber(12345.0f) - test widening) 0", expected[0], actual[0]);
Assert.expectEq("ToNumber(12345.0f) - test widening) 1", expected[1], actual[1]);

expected = DoubleRawBits(-123456782336); /* that's the actual int value for -123.45678e9f */
actual   = DoubleRawBits(-123.45678e9f);
Assert.expectEq("ToNumber(-123.45678e9f) - test widening) 0", expected[0], actual[0]);
Assert.expectEq("ToNumber(-123.45678e9f) - test widening) 1", expected[1], actual[1]);

expected = DoubleRawBits(3.4028234663852886e+38); /* that's the actual exact value of float.MAX_VALUE extended to double */
actual   = DoubleRawBits(float.MAX_VALUE);
Assert.expectEq("ToNumber(float.MAX_VALUE) 0", expected[0], actual[0]);
Assert.expectEq("ToNumber(float.MAX_VALUE) 1", expected[1], actual[1]);


/* This is tricky, since float.MIN_VALUE may change from platform to platform. Check that going to Number & back preserves the value */
var n:Number = float.MIN_VALUE;
Assert.expectEq("ToNumber(float.MIN_VALUE)", true, float(n)===float.MIN_VALUE);


