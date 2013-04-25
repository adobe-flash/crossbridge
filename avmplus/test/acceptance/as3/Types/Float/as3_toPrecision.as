/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "4.7.5";
// var VERSION = "AS3";
// var TITLE   = "AS3 function toPrecision ( precision=0 ) : String";


var test_flt:float = 2.7182818284e5f;
var test_num:Number;

Assert.expectEq("float.AS3::toPrecision = function", true, test_flt.AS3::toPrecision is Function);
Assert.expectEq("float.AS3::toPrecision returns a String", "String", getQualifiedClassName(test_flt.AS3::toPrecision(1)));

test_flt = Infinity;
test_num = test_flt;
Assert.expectEq("test float.AS3::toPrecision(2) Infinity", test_num.AS3::toPrecision(2), test_flt.AS3::toPrecision(2));
test_flt = -Infinity;
test_num = test_flt;
Assert.expectEq("test float.AS3::toPrecision(2) -Infinity", test_num.AS3::toPrecision(2), test_flt.AS3::toPrecision(2));
test_flt = NaN;
test_num = test_flt;
Assert.expectEq("test float.AS3::toPrecision(2) NaN", test_num.AS3::toPrecision(2), test_flt.AS3::toPrecision(2));
test_flt = float.MAX_VALUE;
test_num = test_flt;
Assert.expectEq("test float.AS3::toPrecision(2) float.MAX_VALUE", test_num.AS3::toPrecision(2), test_flt.AS3::toPrecision(2));
test_flt = float.MIN_VALUE;
test_num = test_flt;
Assert.expectEq("test float.AS3::toPrecision(2) float.MIN_VALUE", test_num.AS3::toPrecision(2), test_flt.AS3::toPrecision(2));
test_flt = new float(2.7182818284e5);
test_num = test_flt;
Assert.expectEq("test float.AS3::toPrecision(2) new float()", test_num.AS3::toPrecision(2), test_flt.AS3::toPrecision(2));
test_flt = 2.7182818284e5f;
test_num = test_flt;
Assert.expectEq("test float.AS3::toPrecision(2) float literal", test_num.AS3::toPrecision(2), 2.7182818284e5f.AS3::toPrecision(2));

var expectedRangeErr:String = "???";
try{ test_flt.AS3::toPrecision(0); } catch(e:RangeError){ expectedRangeErr = Utils.rangeError(e.toString()); }
Assert.expectError("float.AS3::toPrecision() == float.AS3::toPrecision(0)", expectedRangeErr, function(){ return test_flt.AS3::toPrecision();});

test_num = test_flt;
Assert.expectEq("float.AS3::toPrecision(3)", test_num.AS3::toPrecision(3), test_flt.AS3::toPrecision(3));

test_num = test_flt;
Assert.expectEq("float.AS3::toPrecision(21)", test_num.AS3::toPrecision(21), test_flt.AS3::toPrecision(21));

test_num = test_flt;
Assert.expectEq("float.AS3::toPrecision(1)", test_num.AS3::toPrecision(1), test_flt.AS3::toPrecision(1));

Assert.expectError("float.AS3::toPrecision(undefined)", Utils.RANGEERROR, function(){ test_flt.AS3::toPrecision(undefined) });
Assert.expectError("float.AS3::toPrecision(NaN): RangeError", Utils.RANGEERROR, function(){ test_flt.AS3::toPrecision(NaN) });
Assert.expectError("float.AS3::toPrecision(0): RangeError", Utils.RANGEERROR, function(){ test_flt.AS3::toPrecision(0) });
Assert.expectError("float.AS3::toPrecision(22): RangeError ", Utils.RANGEERROR, function(){ test_flt.AS3::toPrecision(22) });


