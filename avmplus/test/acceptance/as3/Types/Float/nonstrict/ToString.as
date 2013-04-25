/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "floatUtil.as";


// var SECTION = "9.8";
// var VERSION = "AS3";
// var TITLE   = "The operation ToString";


var v:String;

v = float(+0.0); // TODO: use float literals!
Assert.expectEq("ToString(+0.0f)", "0", v);

v = float(-0.0); // TODO: use float literals!
Assert.expectEq("ToString(-0.0f)", "0", v);

v = float.NaN;
Assert.expectEq("ToString(float.NaN)", "NaN", v);

v = float.MIN_VALUE;
Assert.expectEq("ToString(float.MIN_VALUE)", String(Number(float.MIN_VALUE)), v);

// Bug https://bugzilla.mozilla.org/show_bug.cgi?id=699463
v = float.MAX_VALUE;
var nMAX_VALUE = String(Number(float.MAX_VALUE));
var fExponent = v.slice(v.indexOf("+")+1, v.length);
var nExponent = nMAX_VALUE.slice(nMAX_VALUE.indexOf("+")+1, nMAX_VALUE.length);
var fFraction = v.slice(0, v.indexOf("e"));
var nFraction = nMAX_VALUE.slice(0, nMAX_VALUE.indexOf("e"));
Assert.expectEq("ToString(float.MAX_VALUE) exponent match", nExponent, fExponent);
Assert.expectEq("ToString(float.MAX_VALUE) fraction match", nFraction.slice(0, fFraction.length), fFraction);

v = float(-0.23);// todo:use float literals
var N:Number = float(-0.23);// todo:use float literals
var EXPECTED:String = N;
Assert.expectEq("ToString(-0.23f)", EXPECTED, v);

v = float.POSITIVE_INFINITY;
Assert.expectEq("ToString(float.POSITIVE_INFINITY)", "Infinity", v);

v = float.NEGATIVE_INFINITY;
Assert.expectEq("ToString(float.NEGATIVE_INFINITY)", "-Infinity", v);



