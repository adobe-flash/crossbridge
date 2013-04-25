/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "4.5.4";
// var VERSION = "AS3";
// var TITLE   = "float4.prototype.valueOf () : float4";


var test_flt4:float4 = new float4(3.14131211f);
Assert.expectEq("float4.prototype.valueOf = function", true, float4.prototype.valueOf is Function );

var orig_valof = Object.prototype.valueOf;
Object.prototype.valueOf = float4.prototype.valueOf;
var test_obj:Object = {};
Assert.expectError("float4.prototype.valueOf() on Object throws TypeError ", Utils.TYPEERROR, function(){ test_obj.public::valueOf();});
Object.prototype.valueOf = orig_valof;

var orig_valof = Number.prototype.valueOf;
Number.prototype.valueOf = float4.prototype.valueOf;
var test_num:Number = 3.14131211;
Assert.expectError("float4.prototype.valueOf() on Number throws TypeError", Utils.TYPEERROR, function(){ test_num.public::valueOf();});
// we test that it throws TypeError, not ArgumentError
Assert.expectError("float4.prototype.valueOf(357) on Number throws TypeError", Utils.TYPEERROR, function(){ test_num.public::valueOf(357);});
Number.prototype.valueOf = orig_valof;

Assert.expectEq("float4.prototype.valueOf returns a float4", "float4", typeof(test_flt4.public::valueOf()));
Assert.expectEq("float4.prototype.valueOf - returns this float4", test_flt4, test_flt4.public::valueOf());

Assert.expectEq("float4.prototype.valueOf - DontEnum", "", getFloat4ProtoProp("valueOf"));
Assert.expectEq("float4.prototype.valueOf is not enumerable", false, float4.prototype.propertyIsEnumerable("valueOf"));

orig_valof = float4.prototype.valueOf;
float4.prototype.valueOf = 1.1243174;
Assert.expectEq("float4.prototype.valueOf - Writable", true, orig_valof != float4.prototype.valueOf);
float4.prototype.valueOf = orig_valof;

Assert.expectEq("float4.prototype.valueOf - Deletable", true, delete(float4.prototype.valueOf));
// it should now find "Object" as valueOf, on the prototype chain
Assert.expectEq("float4.prototype.valueOf should now be Object.prototype.valueOf", Object.prototype.valueOf, float4.prototype.valueOf);
float4.prototype.valueOf = orig_valof;


