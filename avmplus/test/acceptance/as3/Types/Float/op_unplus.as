/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.3.4";
// var VERSION = "AS3";
// var TITLE   = "The numeric identity operator +";



var flt:float = -2.1e-3f;
AddStrictTestCase("unary plus on float", flt, +flt);

var u = +flt;
Assert.expectEq("unary plus on float returns float", "float", typeof(u));
u = +Number(u);
Assert.expectEq("unary plus on number returns number, not float", "number", typeof(u));

var neg_zerof:float = -0.0f;
Assert.expectEq("unary plus on -0.0f", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/+neg_zerof);
Assert.expectEq("unary plus on -0.0f FloatLiteral", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/+(-0.0f));

flt = new float(-12.375f);
var pos = 1095106560;
var neg = 3242590208;
Assert.expectEq("float binary form match", neg.toString(2), uint(FloatRawBits(flt)).toString(2));
Assert.expectEq("unary plus on float", neg.toString(2), uint(FloatRawBits(+flt)).toString(2));

Assert.expectEq("unary plus on float.POSITIVE_INFINITY", float.POSITIVE_INFINITY, +float.POSITIVE_INFINITY);
Assert.expectEq("unary plus on float.NEGATIVE_INFINITY", float.NEGATIVE_INFINITY, +float.NEGATIVE_INFINITY);
Assert.expectEq("unary plus on float.MAX_VALUE", float.MAX_VALUE, +float.MAX_VALUE);
Assert.expectEq("unary plus on float.MIN_VALUE", float.MIN_VALUE, +float.MIN_VALUE);
Assert.expectEq("unary plus on float.NaN", float.NaN, +float.NaN);


