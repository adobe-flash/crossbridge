/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.4.16.19";
// var VERSION = "AS3";
// var TITLE   = "Math and geometric methods public function sqrt(arg:float4):float4";


var flt4:float4 = new float4(9f);
Assert.expectEq("float4.sqrt() returns a float4", "float4", getQualifiedClassName(float4.sqrt(flt4)));
Assert.expectEq("float4.sqrt() length is 1", 1, float4.sqrt.length);

var flt_sqrt:float = float.sqrt(9f);
AddStrictTestCase("float4.sqrt(9f, 9f, 9f, 9f)", new float4(flt_sqrt), float4.sqrt(flt4));



