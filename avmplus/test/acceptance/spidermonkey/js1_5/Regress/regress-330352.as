/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-330352.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 330352;
var summary = 'Very non-greedy regexp causes crash in jsregexp.c';
var actual = 'No Crash';
var expect = 'No Crash';

//printBugNumber(BUGNUMBER);
//printStatus (summary);


if ("AB".match(/(.*?)*?B/))
{
 // printStatus(RegExp.lastMatch);
}
Assert.expectEq(summary + ': "AB".match(/(.*?)*?B/)', expect, actual);

if ("AB".match(/(.*)*?B/))
{
  //printStatus(RegExp.lastMatch);
}
Assert.expectEq(summary + ': "AB".match(/(.*)*?B/)', expect, actual);

if ("AB".match(/(.*?)*B/))
{
  //printStatus(RegExp.lastMatch);
}
Assert.expectEq(summary + ': "AB".match(/(.*?)*B/)', expect, actual);

