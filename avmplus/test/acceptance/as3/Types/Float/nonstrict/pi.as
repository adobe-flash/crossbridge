/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.5.12";
// var VERSION = "AS3";
// var TITLE   = "float.PI";



var flt_pi:float = float(3.1415927);

Assert.expectEq("float.PI", flt_pi, float.PI);
Assert.expectEq("typeof float.PI", "float", getQualifiedClassName(float.PI));

Assert.expectEq("float.PI - DontDelete", false, delete(float.PI));
Assert.expectEq("float.PI is still ok", flt_pi, float.PI);

Assert.expectEq("float.PI - DontEnum", '',getFloatProp('PI'));
Assert.expectEq("float.PI is no enumberable", false, float.propertyIsEnumerable('PI'));

Assert.expectError("float.PI - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float.PI = 0; });
Assert.expectEq("float.PI is still here", flt_pi, float.PI);


