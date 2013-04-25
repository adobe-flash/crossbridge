/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.6.2";
// var VERSION = "AS3";
// var TITLE   = "float.prototype.toString ( radix=10 ) : String";


var test_flt:float = 3.14131211f;
Assert.expectEq("float.prototype.toString = function", true, float.prototype.toString is Function );
Assert.expectEq("float.prototype.toString returns a String", "String", getQualifiedClassName(test_flt.public::toString()));

var orig_tostr = Object.prototype.toString;
Object.prototype.toString = float.prototype.toString;
var test_obj:Object = {};
Assert.expectError("float.prototype.toString() on Object throws TypeError ", Utils.TYPEERROR, function(){ test_obj.public::toString();});
// we test that it throws TypeError, not RangeError
Assert.expectError("float.prototype.toString(357) on Object throws TypeError", Utils.TYPEERROR, function(){ test_obj.public::toString(357);});
Object.prototype.toString = orig_tostr;


var orig_tostr = Number.prototype.toString;
Number.prototype.toString = float.prototype.toString;
var test_num:Number = 3.14131211;
Assert.expectError("float.prototype.toString() on Number throws TypeError", Utils.TYPEERROR, function(){ test_num.public::toString();});
Assert.expectError("float.prototype.toString() on Number throws TypeError", Utils.TYPEERROR, function(){ Number.prototype.toString.AS3::call(test_num);});
// we test that it throws TypeError, not RangeError
Assert.expectError("float.prototype.toString(357) on Number throws TypeError", Utils.TYPEERROR, function(){ test_num.public::toString(357);});
Assert.expectError("float.prototype.toString() on Number throws TypeError", Utils.TYPEERROR, function(){ Number.prototype.toString.AS3::call(test_num, 357);});
Number.prototype.toString = orig_tostr;


Assert.expectEq("float.prototype.toString = invokes AS3::toString", test_flt.AS3::toString(), test_flt.public::toString());
Assert.expectEq("float.prototype.toString(5) = invokes AS3::toString(5)", test_flt.AS3::toString(5), test_flt.public::toString(5));

Assert.expectEq("float.prototype.toString - DontEnum", "", getFloatProtoProp("toString"));
Assert.expectEq("float.prototype.toString is not enumerable", false, float.prototype.propertyIsEnumerable("toString"));

var orig_tostr = float.prototype.toString;
float.prototype.toString = 1.1243174;
Assert.expectEq("float.prototype.toString - Writable", true, orig_tostr != float.prototype.toString);
float.prototype.toString = orig_tostr;

Assert.expectEq("float.prototype.toString - Deletable", true, delete(float.prototype.toString));
// it should now find "Object" as toString, on the prototype chain
Assert.expectEq("float.prototype.toString should now be Object.prototype.toString", Object.prototype.toString, float.prototype.toString);
float.prototype.toString = orig_tostr;

Assert.expectEq("float.prototype.toString: implicit radix is 10", test_flt.public::toString(10), test_flt.public::toString());



