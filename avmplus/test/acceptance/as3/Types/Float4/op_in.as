/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.5.3";
// var VERSION = "AS3";
// var TITLE   = "The in operator augmented by float4 values";


var flt4:float4 = new float4(1f, 2f, 3f, 4f);

Assert.expectEq("0 in float4", true, 0 in flt4);
Assert.expectEq("1 in float4", true, 1 in flt4);
Assert.expectEq("2 in float4", true, 2 in flt4);
Assert.expectEq("3 in float4", true, 3 in flt4);
Assert.expectEq("-1 in float4", false, -1 in flt4);
Assert.expectEq("4 in float4", false, 4 in flt4);

Assert.expectEq("'0' in float4", true, '0' in flt4);
Assert.expectEq("'1' in float4", true, '1' in flt4);
Assert.expectEq("'2' in float4", true, '2' in flt4);
Assert.expectEq("'3' in float4", true, '3' in flt4);
Assert.expectEq("'-1' in float4", false, '-1' in flt4);
Assert.expectEq("'4' in float4", false, '4' in flt4);


Assert.expectEq("'x' in float4", true, 'x' in flt4);
Assert.expectEq("'y' in float4", true, 'y' in flt4);
Assert.expectEq("'z' in float4", true, 'z' in flt4);
Assert.expectEq("'w' in float4", true, 'w' in flt4);
Assert.expectEq("'X' in float4", false, 'W' in flt4);
Assert.expectEq("'Y' in float4", false, 'Y' in flt4);
Assert.expectEq("'Z' in float4", false, 'Z' in flt4);
Assert.expectEq("'W' in float4", false, 'W' in flt4);


Assert.expectEq("'b' in float4", false, 'b' in flt4);
Assert.expectEq("'xyza' in float4", false, 'xyza' in flt4);



