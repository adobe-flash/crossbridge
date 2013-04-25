/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.5.2";
// var VERSION = "AS3";
// var TITLE   = "float.prototype";


var flt_proto:Object = float.prototype;
Assert.expectEq("float prototype is non-null", false, float.prototype == null);

Assert.expectEq("float prototype is object", "Object" , getQualifiedClassName(float.prototype));
// TODO: what else? (the prototype properties themselves are tested in section 4.6)

Assert.expectEq("float prototype is not Number prototype", false, Number.prototype == float.prototype);
Assert.expectEq("flt_proto is prototype of float.NaN", true, flt_proto.isPrototypeOf(float.NaN));

Assert.expectEq("Object.prototype is prototype of float", true, Object.prototype.isPrototypeOf(float));

Assert.expectEq("float.prototype - DontDelete", false, delete(float.prototype));
Assert.expectEq("float prototype is still ok", flt_proto, float.prototype);

Assert.expectEq("float.prototype - DontEnum", '',getFloatProp('prototype'));
Assert.expectEq("float.prototype is no enumberable", false, float.propertyIsEnumerable('prototype'));

Assert.expectError("float.prototype - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float.prototype = 10; });
Assert.expectEq("float.prototype is still here", flt_proto , float.prototype );



