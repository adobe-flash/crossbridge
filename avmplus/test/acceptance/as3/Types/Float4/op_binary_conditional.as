/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "5.9";
// var VERSION = "AS3";
// var TITLE   = "The conditional operator augmented by float4 values";


var neg_onef:* = float4(-1f);
var neg_zerof:* = float4(-0f);
var zerof:* = float4(0f);
var onef:* = float4(1f);


function check(val:*, t:*, f:*):* { return val ? t : f; }

// Float4 converts to false always in a boolean context

Assert.expectEq("float4(1f) ? 'FAILED':'PASSED'", "PASSED", float4(1f) ? 'FAILED':'PASSED');
Assert.expectEq("float4(-1f) ? 'FAILED':'PASSED'", "PASSED", float4(-1f) ? 'FAILED':'PASSED');
Assert.expectEq("float4(0f) ? 'FAILED':'PASSED'", "PASSED", float4(0f) ? 'FAILED':'PASSED');
Assert.expectEq("float4(-0f) ? 'FAILED':'PASSED'", "PASSED", float4(-0f) ? 'FAILED':'PASSED');
Assert.expectEq("float4(float.NaN) ? 'FAILED':'PASSED'", "PASSED", float4(float.NaN) ? 'FAILED':'PASSED');
Assert.expectEq("float4(float.NaN, 0f, -0f, 1f) ? 'FAILED':'PASSED'", "PASSED", float4(float.NaN, 0f, -0f, 1f) ? 'FAILED':'PASSED');
Assert.expectEq("float4(float.NaN, 0f, -0f, 0f) ? 'FAILED':'PASSED'", "PASSED", float4(float.NaN, 0f, -0f, 0f) ? 'FAILED':'PASSED');

Assert.expectEq("check(float4(1f), 'FAILED', 'PASSED')", "PASSED", check(float4(1f), 'FAILED', 'PASSED'));
Assert.expectEq("check(float4(-1f), 'FAILED', 'PASSED')", "PASSED", check(float4(-1f), 'FAILED', 'PASSED'));
Assert.expectEq("check(float4(0f), 'FAILED', 'PASSED')", "PASSED", check(float4(0f), 'FAILED', 'PASSED'));
Assert.expectEq("check(float4(-0f), 'FAILED', 'PASSED')", "PASSED", check(float4(-0f), 'FAILED', 'PASSED'));
Assert.expectEq("check(float4(float.NaN), 'PASSED', 'FAILED')", "PASSED", check(float4(float.NaN), 'FAILED', 'PASSED'));
Assert.expectEq("check(float4(float.NaN, 0f, -0f, 1f), 'FAILED', 'PASSED')", "PASSED", check( float4(float.NaN, 0f, -0f, 1f), 'FAILED', 'PASSED'));
Assert.expectEq("check(float4(float.NaN, 0f, -0f, 0f), 'FAILED', 'PASSED')", "PASSED", check(float4(float.NaN, 0f, -0f, 0f), 'FAILED', 'PASSED'));

