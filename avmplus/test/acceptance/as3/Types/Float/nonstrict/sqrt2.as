/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.5.14";
// var VERSION = "AS3";
// var TITLE   = "float.SQRT2";



var flt_sqrt2:float = float(1.4142135);

Assert.expectEq("float.SQRT2", flt_sqrt2, float.SQRT2);
Assert.expectEq("typeof float.SQRT2", "float", getQualifiedClassName(float.SQRT2));

Assert.expectEq("float.SQRT2 - DontDelete", false, delete(float.SQRT2));
Assert.expectEq("float.SQRT2 is still ok", flt_sqrt2, float.SQRT2);

Assert.expectEq("float.SQRT2 - DontEnum", '',getFloatProp('SQRT2'));
Assert.expectEq("float.SQRT2 is no enumberable", false, float.propertyIsEnumerable('SQRT2'));

Assert.expectError("float.SQR_2 - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float.SQRT2 = 0; });
Assert.expectEq("float.SQRT2 is still here", flt_sqrt2, float.SQRT2);


