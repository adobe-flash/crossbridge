/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-465137.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 465137;
var summary = '!NaN is not false';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  expect = 'falsy,falsy,falsy,falsy,falsy,';
  actual = '';



  for (var i=0;i<5;++i) actual += (!(NaN) ? "falsy" : "truthy") + ',';



  Assert.expectEq(summary, expect, actual);


}

