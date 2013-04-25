/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-456470.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 456470;
var summary = 'TM: Make sure JSOP_DEFLOCALFUN pushes the right function object.';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);



  function x() {
    function a() {
      return true;
    }
    return a();
  }

  for (var i = 0; i < 10; ++i)
    x();


 
  Assert.expectEq(summary, expect, actual);


}

