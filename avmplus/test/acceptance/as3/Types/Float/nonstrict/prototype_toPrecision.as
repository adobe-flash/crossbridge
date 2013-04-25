/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.6.7";
// var VERSION = "AS3";
// var TITLE   = "float.prototype.toPrecision ( precision=0 ) : String";


var test_flt:float = 3.14131211f;
Assert.expectEq("float.prototype.toPrecision = function", true, float.prototype.toPrecision is Function );
Assert.expectEq("float.prototype.toPrecision returns a String", "String", getQualifiedClassName(test_flt.public::toPrecision(1)));

var orig_toprec = String.prototype.toPrecision;
String.prototype.toPrecision = float.prototype.toPrecision;
var test_str:String = "3.14131211";
Assert.expectError("float.prototype.toPrecision() on String throws RangeError", Utils.RANGEERROR, function(){ test_str.public::toPrecision() });
Assert.expectEq("float.prototype.toPrecision(7) on String does NOT throw TypeError", test_flt.public::toPrecision(7), test_str.public::toPrecision(7));
// undefined converted to 0, throws RangeError
Assert.expectError("float.prototype.toPrecision - accepts 'undefined' for precision", Utils.RANGEERROR, function(){ test_str.public::toPrecision(undefined);});
String.prototype.toPrecision = orig_toprec;


var expectedRangeErr:String = "???";
try{ test_flt.AS3::toPrecision(0); } catch(e:RangeError){ expectedRangeErr = Utils.rangeError(e.toString()); }
Assert.expectError("float.prototype.toPrecision = invokes AS3::toPrecision", expectedRangeErr, function(){ test_flt.public::toPrecision() });
Assert.expectEq("float.prototype.toPrecision = invokes AS3::toPrecision", test_flt.AS3::toPrecision(4), test_flt.public::toPrecision(4));

Assert.expectEq("float.prototype.toPrecision - DontEnum", "", getFloatProtoProp("toPrecision"));
Assert.expectEq("float.prototype.toPrecision is not enumerable", false, float.prototype.propertyIsEnumerable("toPrecision"));

orig_toprec = float.prototype.toPrecision;
float.prototype.toPrecision = 1.1243174;
Assert.expectEq("float.prototype.toPrecision - Writable", true, orig_toprec != float.prototype.toPrecision);
float.prototype.toPrecision = orig_toprec;

Assert.expectEq("float.prototype.toPrecision - Deletable", true, delete(float.prototype.toPrecision));
Assert.expectEq("float.prototype.toPrecision should now be undefined", undefined, float.prototype.toPrecision);
float.prototype.toPrecision = orig_toprec;


