/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-354246.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 354246;
var summary = 'calling Error constructor with object with bad toString';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  expect = '13';

  actual += '1';
  try
  {
    new Error({toString: function() { x.y } });
  }
  catch(e)
  {
  }
  actual += '3';
  Assert.expectEq(summary, expect, actual);


}

