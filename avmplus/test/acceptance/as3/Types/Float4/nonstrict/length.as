/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "4.4.1";
// var VERSION = "AS3";
// var TITLE   = "float4.length";


Assert.expectEq("float4.length is 4", 4 , float4.length );
Assert.expectEq("float4.length is int", "int" , getQualifiedClassName(float4.length));

Assert.expectEq("float4.length - DontDelete", false, delete(float4.length));
Assert.expectEq("float4.length is still ok", 4, float4.length);

Assert.expectEq("float4.length - DontEnum", '', getFloat4Prop('length'));
Assert.expectEq("float4.length is not enumerable", false, float4.propertyIsEnumerable('length'));

Assert.expectError("float4.length - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float4.length = 0; });
Assert.expectEq("float4.length is still here", 4 , float4.length );


