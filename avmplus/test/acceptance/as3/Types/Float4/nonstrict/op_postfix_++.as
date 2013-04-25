/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.1.8";
// var VERSION = "AS3";
// var TITLE   = "The postfix ++ operator";



var flt4:float4 = new float4(1.125f);
var flt4_plus_1 = flt4 + float4(1.0f);

AddStrictTestCase("postfix ++ on float4", flt4, flt4++);
AddStrictTestCase("postfix ++ on float4", flt4_plus_1, flt4);

flt4 = --flt4;
var o = new MyObject(flt4);
var u = o++;

Assert.expectEq("postfix ++ on Object (that converts to float4 through ToPrimitive)", u, flt4)
Assert.expectEq("postfix ++ on Object (that converts to float4 through ToPrimitive)", o, flt4_plus_1)
Assert.expectEq("postfix ++ on Object returns a float4", "float4", getQualifiedClassName(u));
Assert.expectEq("postfix ++ on Object returns a float4", "float4", getQualifiedClassName(o));


