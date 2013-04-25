/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.5.1";
// var VERSION = "AS3";
// var TITLE   = "float4.prototype.constructor";



Assert.expectEq("float4.prototype.constructor = float4", float4, float4.prototype.constructor);
Assert.expectEq("float4.prototype.constructor = function", false, float4.prototype.constructor is Function);

Assert.expectEq("float4.prototype.constructor - DontEnum", "", getFloat4ProtoProp("constructor"));
Assert.expectEq("float4.prototype.constructor is not enumerable", false, float4.prototype.propertyIsEnumerable("constructor"));

var orig_cstr = float4.prototype.constructor;
float4.prototype.constructor = 1.1243174;
Assert.expectEq("float4.prototype.constructor - Writable", true, orig_cstr != float4.prototype.constructor);
float4.prototype.constructor = orig_cstr;

Assert.expectEq("float4.prototype.constructor - Deletable", true, delete(float4.prototype.constructor));
// it should now find "Object" as constructor, on the prototype chain
Assert.expectEq("float4.prototype.constructor should now be Object.prototype.constructor", Object, float4.prototype.constructor);
float4.prototype.constructor = orig_cstr;


