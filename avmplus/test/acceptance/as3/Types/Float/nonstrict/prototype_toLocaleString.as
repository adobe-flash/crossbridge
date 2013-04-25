/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.6.3";
// var VERSION = "AS3";
// var TITLE   = "float.prototype.toLocaleString ( radix=10 ) : String";


var test_flt:float = 3.14131211f;
Assert.expectEq("float.prototype.toLocaleString = function", true, float.prototype.toLocaleString is Function );
Assert.expectEq("float.prototype.toLocaleString returns a String", "String", getQualifiedClassName(test_flt.public::toLocaleString()));

var orig_tostr = Object.prototype.toString;
Object.prototype.toLocaleString = float.prototype.toLocaleString;
var test_obj:Object = {};
Assert.expectError("float.prototype.toLocaleString() on Object throws TypeError ", Utils.TYPEERROR, function(){ test_obj.public::toLocaleString();});
// we test that it throws TypeError, not RangeError
Assert.expectError("float.prototype.toLocaleString(357) on Object throws TypeError", Utils.TYPEERROR, function(){ test_obj.public::toLocaleString(357);});
Object.prototype.toLocaleString = orig_tostr;

var orig_tostr = Number.prototype.toLocaleString;
var test_num:Number = 3.14131211;
Number.prototype.toLocaleString = float.prototype.toLocaleString;
/// there is no AS3::toLocaleString, this should always throw type error
Assert.expectError("float.prototype.toLocaleString() non-generic= throws TypeError", Utils.TYPEERROR, function(){ test_num.public::toLocaleString();});
Assert.expectError("float.prototype.toLocaleString(357) non-generic= throws TypeError", Utils.TYPEERROR, function(){ test_num.public::toLocaleString(357);});
Number.prototype.toLocaleString = orig_tostr;

Assert.expectEq("float.prototype.toLocaleString = invokes AS3::toString", test_flt.AS3::toString(), test_flt.public::toLocaleString());
Assert.expectEq("float.prototype.toLocaleString(25) = invokes AS3::toString(25)", test_flt.AS3::toString(25), test_flt.public::toLocaleString(25));

Assert.expectEq("float.prototype.toLocaleString - DontEnum", "", getFloatProtoProp("toLocaleString"));
Assert.expectEq("float.prototype.toLocaleString is not enumerable", false, float.prototype.propertyIsEnumerable("toLocaleString"));

orig_tostr = float.prototype.toLocaleString;
float.prototype.toLocaleString = 1.1243174;
Assert.expectEq("float.prototype.toLocaleString - Writable", true, orig_tostr != float.prototype.toLocaleString);
float.prototype.toLocaleString = orig_tostr;

Assert.expectEq("float.prototype.toLocaleString - Deletable", true, delete(float.prototype.toLocaleString));
// it should now find "Object" as toLocaleString, on the prototype chain
Assert.expectEq("float.prototype.toLocaleString should now be Object.prototype.toLocaleString", Object.prototype.toLocaleString, float.prototype.toLocaleString);
float.prototype.toLocaleString = orig_tostr;

Assert.expectEq("float.prototype.toLocaleString: implicit radix is 10", test_flt.public::toLocaleString(10), test_flt.public::toLocaleString());


