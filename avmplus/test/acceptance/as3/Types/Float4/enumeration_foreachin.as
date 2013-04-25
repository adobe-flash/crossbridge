/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.6.5.2";
// var VERSION = "AS3";
// var TITLE   = "Enumeration for each..in";


var expected:* = 4;
var result:* = 0;
var flt4:float4 = new float4(1.125f);
for each ( var i in flt4 )
{
    AddStrictTestCase("for each .. in loop iterant is value NOT index", 1.125f, i);
    result++;
}
Assert.expectEq("for each .. in loop over float4 should iterate 4 times", expected, result);




