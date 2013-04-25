/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.1.1";
// var VERSION = "AS3";
// var TITLE   = "The typeof operator";


var flt4:float4 = new float4(1.125f);
Assert.expectEq("typeof on float4", "float4", typeof(flt4));

var flt4_uninit:float4;
Assert.expectEq("typeof on uninitilized float4", "float4", typeof(flt4_uninit));
Assert.expectEq("typeof on new float4()", "float4", typeof(new float4()));



