/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.5.6";
// var VERSION = "AS3";
// var TITLE   = "float.NEGATIVE_INFINITY";


var flt_neg_inf:float = float(-Infinity);
Assert.expectEq("float.NEGATIVE_INFINITY", flt_neg_inf, float.NEGATIVE_INFINITY);

var neg_inf_fl = -1f/0f;
Assert.expectEq("float.NEGATIVE_INFINITY is -1f/0f FloatLiteral", neg_inf_fl, float.NEGATIVE_INFINITY);

var neg_inf_f:float = -1f/0f;
Assert.expectEq("float.NEGATIVE_INFINITY is -1f/0f", neg_inf_f, float.NEGATIVE_INFINITY);

Assert.expectEq("float.NEGATIVE_INFINITY - DontDelete", false, delete(float.NEGATIVE_INFINITY));
Assert.expectEq("float.NEGATIVE_INFINITY is still ok", flt_neg_inf, float.NEGATIVE_INFINITY);

Assert.expectEq("float.NEGATIVE_INFINITY - DontEnum", '',getFloatProp('NEGATIVE_INFINITY'));
Assert.expectEq("float.NEGATIVE_INFINITY is no enumberable", false, float.propertyIsEnumerable('NEGATIVE_INFINITY'));

Assert.expectError("float.NEGATIVE_INFINITY - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float.NEGATIVE_INFINITY = 0; });
Assert.expectEq("float.NEGATIVE_INFINITY is still here", flt_neg_inf, float.NEGATIVE_INFINITY);


