/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

// var SECTION = "6.7.4";
// var VERSION = "AS3";
// var TITLE   = "The instanceof operator augmented by float values";


var onef:float = 1f;
var vf = new Vector.<float>();

Assert.expectEq("float class is not instanceof float", false, float instanceof float);
Assert.expectEq("FloatLiteral is instanceof float", true, 1f instanceof float);
Assert.expectEq("float instance is instanceof float", true, onef instanceof float);
Assert.expectEq("float class is instanceof Object", true, float instanceof Object);
Assert.expectEq("float class is instanceof Class", true, float instanceof Class);
Assert.expectEq("float class is not instanceof Function", false, float instanceof Function);
Assert.expectEq("FloatLiteral is instanceof Object", true, 1f instanceof Object);
Assert.expectEq("float instance is instanceof Object", true, onef instanceof Object);
Assert.expectEq("Vector.<float> is not instanceof float", false, vf instanceof float);


Assert.expectEq("float class is not instanceof Number", false, float instanceof Number);
Assert.expectEq("FloatLiteral is not instanceof Number", false, 1f instanceof Number);
Assert.expectEq("float instance is not instanceof Number", false, onef instanceof Number);
Assert.expectEq("float class is not instanceof int", false, float instanceof int);
Assert.expectEq("FloatLiteral is not instanceof int", false, 1f instanceof int);
Assert.expectEq("float instance is not instanceof int", false, onef instanceof int);
Assert.expectEq("float class is not instanceof uint", false, float instanceof uint);
Assert.expectEq("FloatLiteral is not instanceof uint", false, 1f instanceof uint);
Assert.expectEq("float instance is not instanceof uint", false, onef instanceof uint);
Assert.expectEq("float class is not instanceof String", false, float instanceof String);
Assert.expectEq("FloatLiteral is not instanceof String", false, 1f instanceof String);
Assert.expectEq("float instance is not instanceof String", false, onef instanceof String);

Assert.expectError("AS: TypeError if datatype is not Class or Function", Utils.TYPEERROR, function(){ return 1f instanceof 1f; });



