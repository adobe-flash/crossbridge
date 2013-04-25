/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
include "floatUtil.as";


// var SECTION = "4.5.1";
// var VERSION = "AS3";
// var TITLE   = "float.length";


Assert.expectEq("float.length is 1", 1 , float.length );
Assert.expectEq("float.length is int", "int" , getQualifiedClassName(float.length));

Assert.expectEq("float.length - DontDelete", false, delete(float.length));
Assert.expectEq("float.length is still ok", 1, float.length);

Assert.expectEq("float.length - DontEnum", '', getFloatProp('length'));
Assert.expectEq("float.length is not enumerable", false, float.propertyIsEnumerable('length'));

Assert.expectError("float.length - ReadOnly", Utils.REFERENCEERROR+1074, function(){ float.length = 0; });
Assert.expectEq("float.length is still here", 1 , float.length );


