/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.1.2";
// var VERSION = "AS3";
// var TITLE   = "The numeric identity operator +";



var flt4:float4 = new float4(-2.1e-3f);
AddStrictTestCase("unary plus on float4", flt4, +flt4);

var u = +flt4;
Assert.expectEq("unary plus on float4 returns float4", "float4", typeof(u));


