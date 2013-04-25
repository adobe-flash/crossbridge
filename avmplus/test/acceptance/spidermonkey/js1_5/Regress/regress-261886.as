/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-261886.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 261886;
var summary = 'Always evaluate delete operand expression';
var actual = '';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

var o = {a:1};


try
{
  delete (++o.a); // must wrap in paren - see https://bugs.adobe.com/jira/browse/ASC-4023
  actual = o.a;
}
catch(e)
{
  actual = o.a;
  summary += ' ' + e;
}

expect = 2;
Assert.expectEq(summary, expect, actual);

