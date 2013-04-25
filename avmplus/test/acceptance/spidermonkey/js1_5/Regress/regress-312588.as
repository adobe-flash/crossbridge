/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-312588.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 312588;
var summary = 'Do not crash creating infinite array';
var actual = 'No Crash';
var expect = 'No Crash';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

var a = new Array();

try
{
  while (1)
  {
    (a = new Array(a)).sort();
  }
}
catch(ex)
{
  print(ex + '');
}

Assert.expectEq(summary, expect, actual);

