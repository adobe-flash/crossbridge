/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.1.5";
// var VERSION = "AS3";
// var TITLE   = "The logical not ! operator";



// Float4 converts to false always in a boolean context.

var flt4:float4 = new float4(1f);
AddStrictTestCase("!float4(1f) all values convert to true", true, !flt4);

flt4 = new float4(0f);
AddStrictTestCase("!float4(0f) all values convert to false", true, !flt4);

flt4 = new float4(9.125f, 0f, 6.234f, 1f);
AddStrictTestCase("!float4(9.125f, 0f, 6.234f, 1f) not all values are false, so convert to true", true, !flt4);

var u = !flt4;
Assert.expectEq("returns a boolean", "boolean", typeof(u));



