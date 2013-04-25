/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-344804.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 344804;
var summary = 'Do not crash iterating over window.Packages';
var actual = 'No Crash';
var expect = 'No Crash';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  if (typeof window != 'undefined')
  {
    for (var p in window.Packages)
      ;
  }
 
  Assert.expectEq(summary, expect, actual);


}

