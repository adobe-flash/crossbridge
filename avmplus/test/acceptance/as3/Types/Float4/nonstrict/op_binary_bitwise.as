/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.7";
// var VERSION = "AS3";
// var TITLE   = "The binary bitwise operators &, ^ and | augmented by float4 values";



Assert.expectEq("float4(1f) & float4(1f)", 0, float4(1f) & float4(1f));
Assert.expectEq("7 & float4(1f)", 0, 7 & float4(1f));
Assert.expectEq("float4(1f) & 3", 0, float4(1f) & 3);


Assert.expectEq("float4(1f) | float4(1f)", 0, float4(1f) | float4(1f));
Assert.expectEq("7 | float4(1f)", 7, 7 | float4(1f));
Assert.expectEq("float4(1f) | 3", 3, float4(1f) | 3);


Assert.expectEq("float4(1f) ^ float4(1f)", 0, float4(1f) ^ float4(1f));
Assert.expectEq("7 ^ float4(1f)", 7, 7 ^ float4(1f));
Assert.expectEq("float4(1f) ^ 3", 3, float4(1f) ^ 3);



