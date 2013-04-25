/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-330951.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 330951;
var summary = 'Crash in Array.sort on array with undefined value';
var actual = 'No Crash';
var expect = 'No Crash';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

var a = [undefined,1];
a.sort();
 
Assert.expectEq(summary, expect, actual);

