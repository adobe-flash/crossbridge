/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.5.9";
// var VERSION = "AS3";
// var TITLE   = "float.LN10";



var flt_ln10:float = float(2.30258509);

Assert.expectEq("float.LN10", flt_ln10, float.LN10);
Assert.expectEq("typeof float.LN10", "float", getQualifiedClassName(float.LN10));

Assert.expectEq("float.LN10 - DontDelete", false, delete(float.LN10));
Assert.expectEq("float.LN10 is still ok", flt_ln10, float.LN10);

Assert.expectEq("float.LN10 - DontEnum", '',getFloatProp('LN10'));
Assert.expectEq("float.LN10 is no enumberable", false, float.propertyIsEnumerable('LN10'));

Assert.expectError("float.LN10 - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float.LN10 = 0; });
Assert.expectEq("float.LN10 is still here", flt_ln10, float.LN10);


