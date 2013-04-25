/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

var gTestfile = 'regress-378492.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 378492;
var summary = 'namespace_trace/qname_trace should check for null private, ' +
  'WAY_TOO_MUCH_GC';
var actual = 'No Crash';
var expect = 'No Crash';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

x = <x/>;
for each(x.t in x) { }

//reportCompare(expect, actual, summary);
Assert.expectEq(summary, expect, actual);

