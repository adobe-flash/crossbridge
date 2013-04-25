/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "4.7.4";
// var VERSION = "AS3";
// var TITLE   = "AS3 function toFixed ( fractionDigits=0 ) : String";


var test_flt:float = 2.7182818284e5f;
var test_num:Number;

Assert.expectEq("float.AS3::toFixed = function", true, test_flt.AS3::toFixed is Function);
Assert.expectEq("float.AS3::toFixed returns a String", "String", getQualifiedClassName(test_flt.AS3::toFixed()));

test_flt = Infinity;
test_num = test_flt;
Assert.expectEq("test float.AS3::toFixed Infinity", test_num.AS3::toFixed(), test_flt.AS3::toFixed());
test_flt = -Infinity;
test_num = test_flt;
Assert.expectEq("test float.AS3::toFixed -Infinity", test_num.AS3::toFixed(), test_flt.AS3::toFixed());
test_flt = NaN;
test_num = test_flt;
Assert.expectEq("test float.AS3::toFixed NaN", test_num.AS3::toFixed(), test_flt.AS3::toFixed());
test_flt = float.MAX_VALUE;
test_num = test_flt;
Assert.expectEq("test float.AS3::toFixed float.MAX_VALUE", test_num.AS3::toFixed(), test_flt.AS3::toFixed());
test_flt = float.MIN_VALUE;
test_num = test_flt;
Assert.expectEq("test float.AS3::toFixed float.MIN_VALUE", test_num.AS3::toFixed(), test_flt.AS3::toFixed());
test_flt = new float(2.7182818284e5);
test_num = test_flt;
Assert.expectEq("test float.AS3::toFixed new float()", test_num.AS3::toFixed(), test_flt.AS3::toFixed());
test_flt = 2.7182818284e5f;
test_num = test_flt;
Assert.expectEq("test float.AS3::toFixed float literal", test_num.AS3::toFixed(), 2.7182818284e5f.AS3::toFixed());


Assert.expectEq("float.AS3::toFixed() == float.AS3::toFixed(0)", test_flt.AS3::toFixed(), test_flt.AS3::toFixed(0));

test_num = test_flt;
Assert.expectEq("float.AS3::toFixed(3)", test_num.AS3::toFixed(3), test_flt.AS3::toFixed(3));

test_num = test_flt;
Assert.expectEq("float.AS3::toFixed(20)", test_num.AS3::toFixed(20), test_flt.AS3::toFixed(20));

Assert.expectEq("float.AS3::toFixed(undefined)", test_num.AS3::toFixed(undefined), test_flt.AS3::toFixed(undefined));

Assert.expectEq("float.AS3::toFixed(NaN)", test_num.AS3::toFixed(NaN), test_flt.AS3::toFixed(NaN));

Assert.expectEq("float.AS3::toFixed(undefined) == float.AS3::toFixed(0)", test_flt.AS3::toFixed(undefined), test_flt.AS3::toFixed(0));

Assert.expectError("float.AS3::toFixed(-1): RangeError", Utils.RANGEERROR, function(){ test_flt.AS3::toFixed(-1) });
Assert.expectError("float.AS3::toFixed(21): RangeError", Utils.RANGEERROR, function(){ test_flt.AS3::toFixed(21) });



