/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.4.16.26";
// var VERSION = "AS3";
// var TITLE   = "Math and geometric methods public function magnitude(arg:float4):float";


var flt4:float4 = new float4(1f);
Assert.expectEq("float4.magnitude() returns a float4", "float", getQualifiedClassName(float4.magnitude(flt4)));
Assert.expectEq("float4.magnitude() length is 1", 1, float4.magnitude.length);

AddStrictTestCase("float4.magnitude(1f, 1f, 1f, 1f)", 2f, float4.magnitude(flt4));



