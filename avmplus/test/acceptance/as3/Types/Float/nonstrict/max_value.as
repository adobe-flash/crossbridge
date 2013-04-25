/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.5.3";
// var VERSION = "AS3";
// var TITLE   = "float.MAX_VALUE";


var flt_max:float = 3.402823466e+38f; //TODO: spec says 3.4028236e38 !! (but this leads to"Infinity")
Assert.expectEq("float.MAX_VALUE", flt_max, float.MAX_VALUE);

Assert.expectEq("float.MAX_VALUE - DontDelete", false, delete(float.MAX_VALUE));
Assert.expectEq("float.MAX_VALUE is still ok", flt_max, float.MAX_VALUE);

Assert.expectEq("float.MAX_VALUE - DontEnum", '',getFloatProp('MAX_VALUE'));
Assert.expectEq("float.MAX_VALUE is no enumberable", false, float.propertyIsEnumerable('MAX_VALUE'));

Assert.expectError("float.MAX_VALUE - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float.MAX_VALUE = 0; });
Assert.expectEq("float.MAX_VALUE is still here", flt_max, float.MAX_VALUE);



