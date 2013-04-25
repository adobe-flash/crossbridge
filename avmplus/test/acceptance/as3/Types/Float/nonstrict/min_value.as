/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.5.4";
// var VERSION = "AS3";
// var TITLE   = "float.MIN_VALUE";


var STEP = "float.MIN_VALUE (denorm supported)"
var flt_min:float = 1.4012e-45f;
var flt_min_int:int = 1; // 0x1, minimum representable floating point greater than 0
if (flt_min == 0)
{   // ASSUME DENORM NOT SUPPORTED
    STEP = "float.MIN_VALUE (denorm NOT supported)";
    flt_min = 1.17549435e-38f;
    flt_min_int = 8388608; // 0x800000
}
Assert.expectEq(STEP, flt_min_int, FloatRawBits(float.MIN_VALUE));

Assert.expectEq("float.MIN_VALUE - DontDelete", false, delete(float.MIN_VALUE));
Assert.expectEq("float.MIN_VALUE is still ok", flt_min_int, FloatRawBits(float.MIN_VALUE));

Assert.expectEq("float.MIN_VALUE - DontEnum", '',getFloatProp('MIN_VALUE'));
Assert.expectEq("float.MIN_VALUE is no enumberable", false, float.propertyIsEnumerable('MIN_VALUE'));

Assert.expectError("float.MIN_VALUE - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float.MIN_VALUE = 0; });
Assert.expectEq("float.MIN_VALUE is still here", flt_min_int, FloatRawBits(float.MIN_VALUE));



