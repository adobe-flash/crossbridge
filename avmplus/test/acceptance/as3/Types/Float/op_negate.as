/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.3.5";
// var VERSION = "AS3";
// var TITLE   = "The negate operatror -";


var flt:float = 0f;
flt = -flt;
Assert.expectEq("unary minus on float zero", -2147483648 /*0x80000000*/, FloatRawBits(flt));
Assert.expectEq("unary minus on float zero sign check", float.NEGATIVE_INFINITY, float.POSITIVE_INFINITY/flt);
flt = -flt;
Assert.expectEq("unary minus on float negative zero", 0, FloatRawBits(flt));
Assert.expectEq("unary minus on float negative zero sign check", float.POSITIVE_INFINITY, float.POSITIVE_INFINITY/flt);
var u = -flt;
Assert.expectEq("returns a float", "float", typeof(u));


flt = float.NaN;
Assert.expectEq("unary minus on float - NaN", 0x7f800000  /*NaN mask 0x7f800000*/, (FloatRawBits(-flt) & 0x7f800000));

flt = new float(-3.1413119f);
Assert.expectEq("unary minus on float - regular number", 1078528833 /*0x40490b42*/,FloatRawBits(-flt));


flt = new float(12.375f);
var pos = 1095106560;
var neg = 3242590208;
Assert.expectEq("float binary form match", pos.toString(2), uint(FloatRawBits(flt)).toString(2));
Assert.expectEq("unary minus on float binary form check", neg.toString(2), uint(FloatRawBits(-flt)).toString(2));

Assert.expectEq("unary minus on float.POSITIVE_INFINITY", float.NEGATIVE_INFINITY, -float.POSITIVE_INFINITY);
Assert.expectEq("unary minus on float.NEGATIVE_INFINITY", float.POSITIVE_INFINITY, -float.NEGATIVE_INFINITY);
Assert.expectEq("unary minus on float.MAX_VALUE", -3.40282346638528e+38f, -float.MAX_VALUE);
Assert.expectEq("unary minus on float.MIN_VALUE", -1.401298464324817e-45f, -float.MIN_VALUE);
Assert.expectEq("unary minus on float.NaN", float.NaN, -float.NaN);


