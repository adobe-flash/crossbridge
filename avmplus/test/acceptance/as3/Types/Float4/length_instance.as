/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.6.4";
// var VERSION = "AS3";
// var TITLE   = "const length : int";


var flt4:float4 = new float4(1f);
Assert.expectEq("flt4.length is int", "int", getQualifiedClassName(flt4.length));
Assert.expectEq("flt4.length is 4", 4, flt4.length);


