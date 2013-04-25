/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "4.6.2";
// var VERSION = "AS3";
// var TITLE   = "AS3 function valueOf ( ) : float4";


var test_flt4:float4 = new float4(2.7182818284e5f);

Assert.expectEq("float4.AS3::valueOf = function", true, test_flt4.AS3::valueOf is Function);
Assert.expectEq("float4.AS3::valueOf returns a float4", "float4", getQualifiedClassName(test_flt4.AS3::valueOf()));

Assert.expectEq("float4.AS3::valueOf", test_flt4, test_flt4.AS3::valueOf());


