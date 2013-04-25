/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.4.16.1";
// var VERSION = "AS3";
// var TITLE   = "Math and geometric methods public function abs(arg:float4):float4";


var flt4:float4 = new float4(1f);
Assert.expectEq("float4.abs() returns a float4", "float4", getQualifiedClassName(float4.abs(flt4)));
Assert.expectEq("float4.abs() length is 1", 1, float4.abs.length);

flt4 = new float4(-1f, 1f, 1f, 1f);
AddStrictTestCase("float4.abs(-1f, 1f, 1f, 1f)", new float4(1f, 1f, 1f, 1f), float4.abs(flt4));
flt4 = new float4(1f, -1f, 1f, 1f);
AddStrictTestCase("float4.abs(1f, -1f, 1f, 1f)", new float4(1f, 1f, 1f, 1f), float4.abs(flt4));
flt4 = new float4(1f, 1f, -1f, 1f);
AddStrictTestCase("float4.abs(1f, 1f, -1f, 1f)", new float4(1f, 1f, 1f, 1f), float4.abs(flt4));
flt4 = new float4(1f, 1f, 1f, -1f);
AddStrictTestCase("float4.abs(1f, 1f, 1f, -1f)", new float4(1f, 1f, 1f, 1f), float4.abs(flt4));


