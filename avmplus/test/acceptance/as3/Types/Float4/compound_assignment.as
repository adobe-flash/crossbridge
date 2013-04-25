/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.10";
// var VERSION = "AS3";
// var TITLE   = "The assignment operators augmented by float4 values";


var flt4:float4 = new float4(0f);
var tracker:int = 0;
var i:int

flt4 = new float4(2f);
Assert.expectEq("flt4 *= 3", float4(6f), flt4 *= 3);
Assert.expectEq("flt4 *= 2", float4(12f), flt4 *= 2);

flt4 = new float4(13f);
Assert.expectEq("flt4 %= 5", float4(3f), flt4 %= 5);
Assert.expectEq("flt4 %= 2", float4(1f), flt4 %= 2);

flt4 = new float4(12f);
Assert.expectEq("flt4 /= 3", float4(4f), flt4 /= 3);
Assert.expectEq("flt4 /= 2", float4(2f), flt4 /= 2);

flt4 = new float4(0f);
tracker = 0;
for (i = 1; i<5; i++)
{
    tracker += i;
    flt4 += i;
    Assert.expectEq("flt4 += "+i, float4(tracker), flt4);
}

flt4 = new float4(0f);
tracker = 0;
for (i = 1; i<5; i++)
{
    tracker -= i;
    flt4 -= i;
    Assert.expectEq("flt4 -= "+i, float4(tracker), flt4);
}


/*
Assert.expectEq("float4(1f) & float4(1f)", 0, float4(1f) & float4(1f));
Assert.expectEq("7 & float4(1f)", 0, 7 & float4(1f));
Assert.expectEq("float4(1f) & 3", 0, float4(1f) & 3);


Assert.expectEq("float4(1f) | float4(1f)", 0, float4(1f) | float4(1f));
Assert.expectEq("7 | float4(1f)", 7, 7 | float4(1f));
Assert.expectEq("float4(1f) | 3", 3, float4(1f) | 3);


Assert.expectEq("float4(1f) ^ float4(1f)", 0, float4(1f) ^ float4(1f));
Assert.expectEq("7 ^ float4(1f)", 7, 7 ^ float4(1f));
Assert.expectEq("float4(1f) ^ 3", 3, float4(1f) ^ 3);
*/


