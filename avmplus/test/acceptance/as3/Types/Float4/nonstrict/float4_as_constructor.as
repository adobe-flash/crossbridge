/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.3.1";
// var VERSION = "AS3";
// var TITLE   = "The float4 method called as a constructor new float4 (x)";


// FIXME: Bug https://bugzilla.mozilla.org/show_bug.cgi?id=708623
var myObject:Object = {1:1};
var object_float4 = new float4(myObject);
Assert.expectEq("Float4 as constructor, with 'Object' arg", "float4", getQualifiedClassName(object_float4));


