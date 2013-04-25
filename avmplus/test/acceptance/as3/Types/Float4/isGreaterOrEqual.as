/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.4.15.2";
// var VERSION = "AS3";
// var TITLE   = "Comparison methods public function float4.isGreaterOrEqual(arg1:float4, arg2:float4):float4";


var flt4_a:float4 = new float4(1f, 1f, 1f, 1f);
var flt4_b:float4 = new float4(2f, 2f, 2f, 2f);
Assert.expectEq("float4.isGreaterOrEqual() returns a float4", "float4", getQualifiedClassName(float4.isGreaterOrEqual(flt4_a, flt4_b)));

flt4_a = new float4(1f, 1f, 1f, 1f);
flt4_b = new float4(2f, 2f, 2f, 2f);
AddStrictTestCase("float4.isGreaterOrEqual((1f, 1f, 1f, 1f), (2f, 2f, 2f, 2f))", new float4(0f, 0f, 0f, 0f), float4.isGreaterOrEqual(flt4_a, flt4_b));
AddStrictTestCase("float4.isGreaterOrEqual((2f, 2f, 2f, 2f), (1f, 1f, 1f, 1f))", new float4(1f, 1f, 1f, 1f), float4.isGreaterOrEqual(flt4_b, flt4_a));

flt4_a = new float4(1f, 1f, 1f, 1f);
flt4_b = new float4(2f, 0f, 0f, 0f);
AddStrictTestCase("float4.isGreaterOrEqual((1f, 1f, 1f, 1f), (2f, 0f, 0f, 0f))", new float4(0f, 1f, 1f, 1f), float4.isGreaterOrEqual(flt4_a, flt4_b));
flt4_a = new float4(1f, 1f, 1f, 1f);
flt4_b = new float4(0f, 2f, 0f, 0f);
AddStrictTestCase("float4.isGreaterOrEqual((1f, 1f, 1f, 1f), (0f, 2f, 0f, 0f))", new float4(1f, 0f, 1f, 1f), float4.isGreaterOrEqual(flt4_a, flt4_b));
flt4_a = new float4(1f, 1f, 1f, 1f);
flt4_b = new float4(0f, 0f, 2f, 0f);
AddStrictTestCase("float4.isGreaterOrEqual((1f, 1f, 1f, 1f), (0f, 0f, 2f, 0f))", new float4(1f, 1f, 0f, 1f), float4.isGreaterOrEqual(flt4_a, flt4_b));
flt4_a = new float4(1f, 1f, 1f, 1f);
flt4_b = new float4(0f, 0f, 0f, 2f);
AddStrictTestCase("float4.isGreaterOrEqual((1f, 1f, 1f, 1f), (0f, 0f, 0f, 2f))", new float4(1f, 1f, 1f, 0f), float4.isGreaterOrEqual(flt4_a, flt4_b));

flt4_a = new float4(1f, 1f, 1f, 1f);
AddStrictTestCase("float4.isGreaterOrEqual((1f, 1f, 1f, 1f), (1f, 1f, 1f, 1f))", new float4(1f, 1f, 1f, 1f), float4.isGreaterOrEqual(flt4_a, flt4_a));

flt4_a = new float4(-0f);
flt4_b = new float4(0f);
AddStrictTestCase("float4.isGreaterOrEqual((-0f), (0f))", new float4(1f), float4.isGreaterOrEqual(flt4_a, flt4_b));
AddStrictTestCase("float4.isGreaterOrEqual((0f), (-0f))", new float4(1f), float4.isGreaterOrEqual(flt4_b, flt4_a));


