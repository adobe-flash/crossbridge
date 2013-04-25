/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Utils;

// var SECTION = "XXX";
// var VERSION = "AS3";
// var TITLE   = "delete operator";


var flt4:float4 = new float4(1,2,3,4);

Assert.expectError("delete an out-of-range element of a float4 (property)", "ReferenceError: Error #1120", function () { delete flt4.zappa });

