/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-286216.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 286216;
var summary = 'Do not crash tracing a for-in loop';
var actual = 'No Crash';
var expect = 'No Crash';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

if (typeof tracking == 'function')
{
  tracing(true);
}
var mytest = 3;
var dut = { 'x' : mytest };
var ob = [];
for (ob[0] in dut) {
  print(dut[ob[0]]);
}

if (typeof tracing == 'function')
{
  tracing(false);
}
Assert.expectEq(summary, expect, actual);

