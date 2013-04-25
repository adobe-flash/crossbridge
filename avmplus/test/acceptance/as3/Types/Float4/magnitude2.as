/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.4.16.28";
// var VERSION = "AS3";
// var TITLE   = "Math and geometric methods public function magnitude2(arg:float4):float";


var flt4:float4 = new float4(1f);
Assert.expectEq("float4.magnitude2() returns a float4", "float", getQualifiedClassName(float4.magnitude2(flt4)));
Assert.expectEq("float4.magnitude2() length is 1", 1, float4.magnitude2.length);

AddStrictTestCase("float4.magnitude2(4f, 3f, 3f, 1f)", 5f, float4.magnitude2(float4(4f, 3f, 3f, 1f)));


