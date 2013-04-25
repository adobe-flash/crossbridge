/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
include "floatUtil.as";


// var SECTION = "4.6.1";
// var VERSION = "AS3";
// var TITLE   = "float.prototype.constructor";



Assert.expectEq("float.prototype.constructor = float", float, float.prototype.constructor);
Assert.expectEq("float.prototype.constructor = function", false, float.prototype.constructor is Function);

Assert.expectEq("float.prototype.constructor - DontEnum", "", getFloatProtoProp("constructor"));
Assert.expectEq("float.prototype.constructor is not enumerable", false, float.prototype.propertyIsEnumerable("constructor"));

var orig_cstr = float.prototype.constructor;
float.prototype.constructor = 1.1243174;
Assert.expectEq("float.prototype.constructor - Writable", true, orig_cstr != float.prototype.constructor);
float.prototype.constructor = orig_cstr;

Assert.expectEq("float.prototype.constructor - Deletable", true, delete(float.prototype.constructor));
// it should now find "Object" as constructor, on the prototype chain
Assert.expectEq("float.prototype.constructor should now be Object.prototype.constructor", Object, float.prototype.constructor);
float.prototype.constructor = orig_cstr;


