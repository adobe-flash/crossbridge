/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "4.7.3";
// var VERSION = "AS3";
// var TITLE   = "AS3 function toExponential( fractionDigits=0 ) : String";


var test_flt:float = 2.7182818284e5f;
var test_num:Number;

Assert.expectEq("float.AS3::toExponential = function", true, test_flt.AS3::toExponential is Function);
Assert.expectEq("float.AS3::toExponential returns a String", "String", getQualifiedClassName(test_flt.AS3::toExponential()));


test_flt = Infinity;
test_num = test_flt;
Assert.expectEq("test float.AS3::toExponential Infinity", test_num.AS3::toExponential(), test_flt.AS3::toExponential());
test_flt = -Infinity;
test_num = test_flt;
Assert.expectEq("test float.AS3::toExponential -Infinity", test_num.AS3::toExponential(), test_flt.AS3::toExponential());
test_flt = NaN;
test_num = test_flt;
Assert.expectEq("test float.AS3::toExponential NaN", test_num.AS3::toExponential(), test_flt.AS3::toExponential());
test_flt = float.MAX_VALUE;
test_num = test_flt;
Assert.expectEq("test float.AS3::toExponential float.MAX_VALUE", test_num.AS3::toExponential(), test_flt.AS3::toExponential());
test_flt = float.MIN_VALUE;
test_num = test_flt;
Assert.expectEq("test float.AS3::toExponential float.MIN_VALUE", test_num.AS3::toExponential(), test_flt.AS3::toExponential());
test_flt = new float(2.7182818284e5);
test_num = test_flt;
Assert.expectEq("test float.AS3::toExponential new float()", test_num.AS3::toExponential(), test_flt.AS3::toExponential());
test_flt = 2.7182818284e5f;
test_num = test_flt;
Assert.expectEq("test float.AS3::toExponential float literal", test_num.AS3::toExponential(), 2.7182818284e5f.AS3::toExponential());

Assert.expectEq("float.AS3::toExponential() == float.AS3::toExponential(0)", test_flt.AS3::toExponential(), test_flt.AS3::toExponential(0));

test_num = test_flt;
Assert.expectEq("float.AS3::toExponential(3)", test_num.AS3::toExponential(3), test_flt.AS3::toExponential(3));

test_num = test_flt;
Assert.expectEq("float.AS3::toExponential(20)", test_num.AS3::toExponential(20), test_flt.AS3::toExponential(20));

Assert.expectEq("float.AS3::toExponential(undefined)", test_num.AS3::toExponential(undefined), test_flt.AS3::toExponential(undefined));

Assert.expectEq("float.AS3::toExponential(NaN)", test_num.AS3::toExponential(NaN), test_flt.AS3::toExponential(NaN));

Assert.expectError("float.AS3::toExponential(-1): RangeError", Utils.RANGEERROR, function(){ test_flt.AS3::toExponential(-1) });
Assert.expectError("float.AS3::toExponential(21): RangeError", Utils.RANGEERROR, function(){ test_flt.AS3::toExponential(21) });



