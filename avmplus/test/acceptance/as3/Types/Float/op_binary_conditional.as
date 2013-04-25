/* -*- c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "6.11";
// var VERSION = "AS3";
// var TITLE   = "The conditional operator augmented by float values";


var neg_onef:float = -1f;
var zerof:float = 0f;
var onef:float = 1f;

Assert.expectEq("1f ? 'PASSED' : 'FAILED'", "PASSED", (onef?"PASSED":"FAILED"));
Assert.expectEq("1f FloatLiteral ? 'PASSED' : 'FAILED'", "PASSED", (1f?"PASSED":"FAILED"));
Assert.expectEq("0f ? 'FAILED' : 'PASSED'", "PASSED", (zerof?"FAILED":"PASSED"));
Assert.expectEq("0f FloatLiteral ? 'FAILED' : 'PASSED'", "PASSED", (0f?"FAILED":"PASSED"));
Assert.expectEq("-1f ? 'PASSED' : 'FAILED'", "PASSED", (neg_onef?"PASSED":"FAILED"));
Assert.expectEq("-1f FloatLiteral ? 'PASSED' : 'FAILED'", "PASSED", (-1f?"PASSED":"FAILED"));
Assert.expectEq("float.NaN ? 'FAILED' : 'PASSED'", "PASSED", (float.NaN?"FAILED":"PASSED"));



