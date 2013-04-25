/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;


var gTestfile = 'regress-420919.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 420919;
var summary = 'this.u.v = 1 should report this.u is undefined';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  // 1.8 branch reports no properties, trunk reports undefined
  expect = 'TypeError: Error #1010'; // A term is undefined and has no properties.;
  Assert.expectError(summary, expect, function () {this.u.v = 1;});


}

