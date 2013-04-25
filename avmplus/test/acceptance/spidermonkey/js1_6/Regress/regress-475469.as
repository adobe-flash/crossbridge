/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

var gTestfile = 'regress-475469.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 475469;
var summary = 'TM: Do not crash @ FramePCOffset';
var actual = '';
var expect = '';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

try {
    ([1,2,3]).map(/a/gi);
} catch (e) {
    // do nothing
}

//reportCompare(expect, actual, summary);
Assert.expectEq(summary, expect, actual);



