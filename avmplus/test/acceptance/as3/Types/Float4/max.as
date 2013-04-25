/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.4.16.11";
// var VERSION = "AS3";
// var TITLE   = "Math and geometric methods public function max(arg1:float4, arg2:float4):float4";


var flt4:float4 = new float4(1f);
Assert.expectEq("float4.max() returns a float4", "float4", getQualifiedClassName(float4.max(flt4, flt4)));
Assert.expectEq("float4.max() length is 2", 2, float4.max.length);

var flt4_a:float4 = new float4(1f);
var flt4_b:float4 = new float4(0f);
AddStrictTestCase("float4.max((1f, 1f, 1f, 1f), (0f, 0f, 0f, 0f))", new float4(1f), float4.max(flt4_a, flt4_b));
flt4_b = new float4(2f);
AddStrictTestCase("float4.max((1f, 1f, 1f, 1f), (2f, 2f, 2f, 2f))", new float4(2f), float4.max(flt4_a, flt4_b));

