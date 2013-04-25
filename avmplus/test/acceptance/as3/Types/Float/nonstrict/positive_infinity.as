/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.5.7";
// var VERSION = "AS3";
// var TITLE   = "float.POSITIVE_INFINITY";


var flt_inf:float = float(Infinity);
Assert.expectEq("float.POSITIVE_INFINITY", flt_inf, float.POSITIVE_INFINITY);

var inf_fl = 1f/0f;
Assert.expectEq("float.POSITIVE_INFINITY is 1f/0f FloatLiteral", inf_fl, float.POSITIVE_INFINITY);

var inf_f:float = 1f/0f;
Assert.expectEq("float.POSITIVE_INFINITY is 1f/0f", inf_f, float.POSITIVE_INFINITY);

Assert.expectEq("float.POSITIVE_INFINITY - DontDelete", false, delete(float.POSITIVE_INFINITY));
Assert.expectEq("float.POSITIVE_INFINITY is still ok", flt_inf, float.POSITIVE_INFINITY);

Assert.expectEq("float.POSITIVE_INFINITY - DontEnum", '',getFloatProp('POSITIVE_INFINITY'));
Assert.expectEq("float.POSITIVE_INFINITY is no enumberable", false, float.propertyIsEnumerable('POSITIVE_INFINITY'));

Assert.expectError("float.POSITIVE_INFINITY - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float.POSITIVE_INFINITY = 0; });
Assert.expectEq("float.POSITIVE_INFINITY is still here", flt_inf, float.POSITIVE_INFINITY);


