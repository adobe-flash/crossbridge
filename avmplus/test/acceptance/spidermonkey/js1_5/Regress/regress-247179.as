/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-247179.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 247179;
var summary = 'RegExp \\b should not recognize non-ASCII alphanumerics as word characters';
var actual = '';
var expect = '';

//printBugNumber(BUGNUMBER);
//printStatus (summary);
 
expect = 3;
actual = "m\ucc44nd".split(/\b/).length;
 
Assert.expectEq(summary, expect, actual);

expect = 4;
actual = "m\ucc44nd".split(/\w/).length;
 
Assert.expectEq('RegExp \\w should not recognize non-ASCII alphanumerics as word characters', expect, actual);

