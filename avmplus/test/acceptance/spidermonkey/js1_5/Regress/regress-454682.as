/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-454682.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 454682;
var summary = 'Do not crash with JIT in MatchRegExp';
var actual = 'No Crash';
var expect = 'No Crash';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 


  var a = new String("foo");
  for (i = 0; i < 300; i++) {
    a.match(/bar/);
  }



  Assert.expectEq(summary, expect, actual);


}

