/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "floatUtil.as";


// var SECTION = "4.6.6";
// var VERSION = "AS3";
// var TITLE   = "float.prototype.toFixed ( fractionDigits=0 ) : String";


var test_flt:float = 3.14131211f;
Assert.expectEq("float.prototype.toFixed = function", true, float.prototype.toFixed is Function );
Assert.expectEq("float.prototype.toFixed returns a String", "String", getQualifiedClassName(test_flt.public::toFixed()));

var orig_tofixed = String.prototype.toFixed;
String.prototype.toFixed = float.prototype.toFixed;
var test_str:String = "3.14131211";
Assert.expectEq("float.prototype.toFixed() - does NOT throw TypeError", test_flt.public::toFixed(), test_str.public::toFixed());
Assert.expectEq("float.prototype.toFixed(7) - does NOT throw TypeError", test_flt.public::toFixed(7), test_str.public::toFixed(7));
// shouldn't throw according to Note
Assert.expectEq("float.prototype.toFixed - accepts 'undefined' for fractionDigits", test_flt.public::toFixed(undefined), test_str.public::toFixed(undefined));
String.prototype.toFixed = orig_tofixed;


Assert.expectEq("float.prototype.toFixed = invokes AS3::toFixed", test_flt.AS3::toFixed(), test_flt.public::toFixed());
Assert.expectEq("float.prototype.toFixed = invokes AS3::toFixed", test_flt.AS3::toFixed(7), test_flt.public::toFixed(7));

Assert.expectEq("float.prototype.toFixed - DontEnum", "", getFloatProtoProp("toFixed"));
Assert.expectEq("float.prototype.toFixed is not enumerable", false, float.prototype.propertyIsEnumerable("toFixed"));

orig_tofixed = float.prototype.toFixed;
float.prototype.toFixed = 1.1243174;
Assert.expectEq("float.prototype.toFixed - Writable", true, orig_tofixed != float.prototype.toFixed);
float.prototype.toFixed = orig_tofixed;

Assert.expectEq("float.prototype.toFixed - Deletable", true, delete(float.prototype.toFixed));
Assert.expectEq("float.prototype.toFixed should now be undefined", undefined, float.prototype.toFixed);
float.prototype.toFixed = orig_tofixed;

Assert.expectEq("float.prototype.toFixed default fractionDigits 0", test_flt.public::toFixed(0), test_flt.public::toFixed());


