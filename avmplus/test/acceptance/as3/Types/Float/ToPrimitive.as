/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "9.9";
// var VERSION = "AS3";
// var TITLE   = "The operation ToPrimitive";


/* According to spec, toPrimitive is called (with float argument) only for the binary '+' , and in no other case
   I guess all that we can test is that the result of adding two floats is a float */
var v1:float = 12.345;
var v2:float = 67.89;
/* This actually performs ToNumeric(ToPrimitive(v1))+ ToNumeric(ToPrimitive(v1))... but assuming that ToNumeric works fine,
   this test is still a fair approximation for a ToPrimitive test */
var result = v1+v2;

Assert.expectEq("ToPrimitive() tested through addition; type(float+float)", "float", getQualifiedClassName(result));


