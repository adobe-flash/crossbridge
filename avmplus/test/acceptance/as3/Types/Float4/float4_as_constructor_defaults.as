/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.3.2";
// var VERSION = "AS3";
// var TITLE   = "The float4 method called as a constructor new float4 (x, y, z, w)";



var result = new float4(1f, 1f, 1f, 1f);
Assert.expectEq("float4(1f, 1f, 1f, 1f)", "float4", getQualifiedClassName(result));

var result = new float4(1, 1, 1, 1);
Assert.expectEq("float4(1, 1, 1, 1)", "float4", getQualifiedClassName(result));


