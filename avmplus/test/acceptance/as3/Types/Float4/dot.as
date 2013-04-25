/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.4.16.23";
// var VERSION = "AS3";
// var TITLE   = "Math and geometric methods public function dot(arg1:float4, arg2:float4):float";


var flt4:float4 = new float4(1f);
Assert.expectEq("float4.dot() returns a float", "float", getQualifiedClassName(float4.dot(flt4, flt4)));
Assert.expectEq("float4.dot() length is 2", 2, float4.dot.length);

var flt4_a:float4 = new float4(1f, 2f, 3f, 4f);
var flt4_b:float4 = new float4(4f, 3f, 2f, 1f);
AddStrictTestCase("float4.dot((1f, 2f, 3f, 4f), (4f, 3f, 2f, 1f))", new float(20f), float4.dot(flt4_a, flt4_b));


