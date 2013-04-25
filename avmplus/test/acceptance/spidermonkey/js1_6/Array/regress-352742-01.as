/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

var gTestfile = 'regress-352742-01.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 352742;
var summary = 'Array filter on {valueOf: Function}';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
starttest();
//-----------------------------------------------------------------------------

function starttest()
{
  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  expect = 4;
  z = {valueOf: Function};
  actual = 2;
  try {
    ([11]).filter(z);
  }
  catch(e)
  {
    actual = 3;
    print(e);
  }
  actual = 4;

  //reportCompare(expect, actual, summary);
Assert.expectEq(summary, expect, actual);

}
