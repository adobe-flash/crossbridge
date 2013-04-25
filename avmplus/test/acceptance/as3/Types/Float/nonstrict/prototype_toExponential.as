/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "floatUtil.as";


// var SECTION = "4.6.5";
// var VERSION = "AS3";
// var TITLE   = "float.prototype.toExponential ( fractionDigits=0 ) : String";


var test_flt:float = 3.14131211f;
Assert.expectEq("float.prototype.toExponential = function", true, float.prototype.toExponential is Function );
Assert.expectEq("float.prototype.toExponential returns a String", "String", getQualifiedClassName(test_flt.public::toExponential()));

var orig_toexpo = String.prototype.toExponential;
String.prototype.toExponential = float.prototype.toExponential;
var test_str:String = "3.14131211";
Assert.expectEq("float.prototype.toExponential() on String does NOT throw TypeError", test_flt.public::toExponential(), test_str.public::toExponential());
Assert.expectEq("float.prototype.toExponential(7) on String does NOT throw TypeError", test_flt.public::toExponential(7), test_str.public::toExponential(7));

// shouldn't throw according to Note
Assert.expectEq("float.prototype.toExponential - accepts 'undefined' for fractionDigits", test_flt.public::toExponential(undefined), test_str.public::toExponential(undefined));
String.prototype.toExponential = orig_toexpo;

Assert.expectEq("float.prototype.toExponential = invokes AS3::toExponential", test_flt.AS3::toExponential(), test_flt.public::toExponential());
Assert.expectEq("float.prototype.toExponential = invokes AS3::toExponential", test_flt.AS3::toExponential(6), test_flt.public::toExponential(6));

Assert.expectEq("float.prototype.toExponential - DontEnum", "", getFloatProtoProp("toExponential"));
Assert.expectEq("float.prototype.toExponential is not enumerable", false, float.prototype.propertyIsEnumerable("toExponential"));

orig_toexpo = float.prototype.toExponential;
float.prototype.toExponential = 1.1243174;
Assert.expectEq("float.prototype.toExponential - Writable", true, orig_toexpo != float.prototype.toExponential);
float.prototype.toExponential = orig_toexpo;

Assert.expectEq("float.prototype.toExponential - Deletable", true, delete(float.prototype.toExponential));
Assert.expectEq("float.prototype.toExponential should now be undefined", undefined, float.prototype.toExponential);
float.prototype.toExponential = orig_toexpo;

Assert.expectEq("float.prototype.toExponential default fractionDigits 0", test_flt.public::toExponential(0), test_flt.public::toExponential());


