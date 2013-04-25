/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-450833.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 450833;
var summary = 'TM: Multiple trees per entry point';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  expect = 100;



  function f(i) {
    for (var m = 0; m < 20; ++m)
      for (var n = 0; n < 100; n += i)
        ;
    return n;
  }

  print(actual = f(1));

  Assert.expectEq(summary, expect, actual);


  print(actual = f(.5));

  Assert.expectEq(summary, expect, actual);


}

