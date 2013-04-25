/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-465135.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 465135;
var summary = 'true << true';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  expect = '2,2,2,2,2,';
  actual = '';



  for (var i=0;i<5;++i) actual += (true << true) + ',';



  Assert.expectEq(summary, expect, actual);


}

