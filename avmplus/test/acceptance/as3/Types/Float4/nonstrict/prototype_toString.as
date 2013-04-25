/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "4.5.2";
// var VERSION = "AS3";
// var TITLE   = "float4.prototype.toString ( radix=10 ) : String";


var test_flt4:float4 = new float4(3.14131211f);
Assert.expectEq("float4.prototype.toString = function", true, float4.prototype.toString is Function );
Assert.expectEq("float4.prototype.toString returns a String", "String", getQualifiedClassName(test_flt4.public::toString()));

var orig_tostr = Object.prototype.toString;
Object.prototype.toString = float4.prototype.toString;
var test_obj:Object = {};
Assert.expectError("float4.prototype.toString() on Object throws TypeError ", Utils.TYPEERROR, function(){ test_obj.public::toString();});
// we test that it throws TypeError, not RangeError
Assert.expectError("float4.prototype.toString(357) on Object throws TypeError", Utils.TYPEERROR, function(){ test_obj.public::toString(357);});
Object.prototype.toString = orig_tostr;

var orig_tostr = Number.prototype.toString;
Number.prototype.toString = float4.prototype.toString;
var test_num:Number = 3.14131211;
Assert.expectError("float4.prototype.toString() on Number throws TypeError", Utils.TYPEERROR, function(){ test_num.public::toString();});
Assert.expectError("float4.prototype.toString() on Number throws TypeError", Utils.TYPEERROR, function(){ Number.prototype.toString.AS3::call(test_num);});
// we test that it throws TypeError, not RangeError
Assert.expectError("float4.prototype.toString(357) on Number throws TypeError", Utils.TYPEERROR, function(){ test_num.public::toString(357);});
Assert.expectError("float4.prototype.toString() on Number throws TypeError", Utils.TYPEERROR, function(){ Number.prototype.toString.AS3::call(test_num);});
Number.prototype.toString = orig_tostr;

Assert.expectEq("float4.prototype.toString = invokes AS3::toString", test_flt4.AS3::toString(), test_flt4.public::toString());
Assert.expectEq("float4.prototype.toString(5) = invokes AS3::toString(5)", test_flt4.AS3::toString(5), test_flt4.public::toString(5));

Assert.expectEq("float4.prototype.toString - DontEnum", "", getFloat4ProtoProp("toString"));
Assert.expectEq("float4.prototype.toString is not enumerable", false, float4.prototype.propertyIsEnumerable("toString"));

var orig_tostr = float4.prototype.toString;
float4.prototype.toString = 1.1243174;
Assert.expectEq("float4.prototype.toString - Writable", true, orig_tostr != float4.prototype.toString);
float4.prototype.toString = orig_tostr;

Assert.expectEq("float4.prototype.toString - Deletable", true, delete(float4.prototype.toString));
// it should now find "Object" as toString, on the prototype chain
Assert.expectEq("float4.prototype.toString should now be Object.prototype.toString", Object.prototype.toString, float4.prototype.toString);
float4.prototype.toString = orig_tostr;

Assert.expectEq("float4.prototype.toString: implicit radix is 10", test_flt4.public::toString(10), test_flt4.public::toString());



