/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.6.4";
// var VERSION = "AS3";
// var TITLE   = "float.prototype.valueOf () : float";


var test_flt:float = 3.14131211f;
Assert.expectEq("float.prototype.valueOf = function", true, float.prototype.valueOf is Function );

var orig_valof = Object.prototype.valueOf;
Object.prototype.valueOf = float.prototype.valueOf;
var test_obj:Object = {};
Assert.expectError("float.prototype.valueOf() on Object throws TypeError ", Utils.TYPEERROR, function(){ test_obj.public::valueOf();});
Object.prototype.valueOf = orig_valof;


var orig_valof = Number.prototype.valueOf;
Number.prototype.valueOf = float.prototype.valueOf;
var test_num:Number = 3.14131211;
Assert.expectError("float.prototype.valueOf() on Number throws TypeError", Utils.TYPEERROR, function(){ test_num.public::valueOf();});
Number.prototype.valueOf = orig_valof;


Assert.expectEq("float.prototype.valueOf returns a float", "float", typeof(test_flt.public::valueOf()));
Assert.expectEq("float.prototype.valueOf - returns this float", test_flt, test_flt.public::valueOf());
var test_flt_bits:int=1078528834; // 0x40490b42
Assert.expectEq("float.prototype.valueOf - returns this float, bits", test_flt_bits, FloatRawBits(test_flt.public::valueOf()));


Assert.expectEq("float.prototype.valueOf - DontEnum", "", getFloatProtoProp("valueOf"));
Assert.expectEq("float.prototype.valueOf is not enumerable", false, float.prototype.propertyIsEnumerable("valueOf"));

orig_valof = float.prototype.valueOf;
float.prototype.valueOf = 1.1243174;
Assert.expectEq("float.prototype.valueOf - Writable", true, orig_valof != float.prototype.valueOf);
float.prototype.valueOf = orig_valof;

Assert.expectEq("float.prototype.valueOf - Deletable", true, delete(float.prototype.valueOf));
// it should now find "Object" as valueOf, on the prototype chain
Assert.expectEq("float.prototype.valueOf should now be Object.prototype.valueOf", Object.prototype.valueOf, float.prototype.valueOf);
float.prototype.valueOf = orig_valof;


