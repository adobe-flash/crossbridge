/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import avmplus.*;
import com.adobe.test.Assert;


var gTestfile = 'regress-360681-01.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 360681;
var summary = 'Regression from bug 224128';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  expect = actual = 'No Crash';

  var a = Array(3);
  a[0] = 1;
  a[1] = 2;
  a.sort(function () { System.forceFullCollection(); return 1; });

  Assert.expectEq(summary, expect, actual);


}

