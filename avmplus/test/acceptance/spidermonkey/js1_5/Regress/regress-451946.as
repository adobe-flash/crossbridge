/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-451946.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 451946;
var summary = 'Do not crash with SELinux execheap protection';
var actual = 'No Crash';
var expect = 'No Crash';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  print('This test is only valid with SELinux targetted policy with exeheap protection');



  var i; for (i = 0; i  < 2000000; i++) {;}



  Assert.expectEq(summary, expect, actual);


}

