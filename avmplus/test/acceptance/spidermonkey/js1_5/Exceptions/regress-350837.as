/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-350837.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 350837;
var summary = 'clear cx->throwing in finally';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  expect = 'F';

  function f()
  {
    actual = "F";
  }

  try
  {
    try {
      throw 1;
    } finally {
      f.call(this);
    }
  }
  catch(ex)
  {
    Assert.expectEq(summary, 1, ex);
  }

  Assert.expectEq(summary, expect, actual);


}

