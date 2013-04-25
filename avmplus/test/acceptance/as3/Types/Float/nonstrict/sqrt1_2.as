/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.5.13";
// var VERSION = "AS3";
// var TITLE   = "float.SQRT1_2";



var flt_sqrt1_2:float = float(7.0710677e-1);

Assert.expectEq("float.SQRT1_2", flt_sqrt1_2, float.SQRT1_2);
Assert.expectEq("typeof float.SQRT1_2", "float", getQualifiedClassName(float.SQRT1_2));

Assert.expectEq("float.SQRT1_2 - DontDelete", false, delete(float.SQRT1_2));
Assert.expectEq("float.SQRT1_2 is still ok", flt_sqrt1_2, float.SQRT1_2);

Assert.expectEq("float.SQRT1_2 - DontEnum", '',getFloatProp('SQRT1_2'));
Assert.expectEq("float.SQRT1_2 is no enumberable", false, float.propertyIsEnumerable('SQRT1_2'));

Assert.expectError("float.SQRT1_2 - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float.SQRT1_2 = 0; });
Assert.expectEq("float.SQRT1_2 is still here", flt_sqrt1_2, float.SQRT1_2);


