/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "4.4.2";
// var VERSION = "AS3";
// var TITLE   = "float4.prototype";


var flt4_proto:Object = float4.prototype;
Assert.expectEq("float4 prototype is non-null", false, float4.prototype == null);

Assert.expectEq("float4 prototype is object", "Object" , getQualifiedClassName(float4.prototype));
// TODO: what else? (the prototype properties themselves are tested in section 4.6)

Assert.expectEq("float4 prototype is not Number prototype", false, Number.prototype == float4.prototype);

Assert.expectEq("Object.prototype is prototype of float4", true, Object.prototype.isPrototypeOf(float4));

Assert.expectEq("float4.prototype - DontDelete", false, delete(float4.prototype));
Assert.expectEq("float4 prototype is still ok", flt4_proto, float4.prototype);

Assert.expectEq("float4.prototype - DontEnum", '',getFloat4Prop('prototype'));
Assert.expectEq("float4.prototype is no enumberable", false, float4.propertyIsEnumerable('prototype'));

Assert.expectError("float4.prototype - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float4.prototype = 10; });
Assert.expectEq("float4.prototype is still here", flt4_proto , float4.prototype );



