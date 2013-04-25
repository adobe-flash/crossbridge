/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-390078.js';

//-----------------------------------------------------------------------------
var BUGNUMBER = 390078;
var summary = 'GC hazard with JSstackFrame.argv[-1]';
var actual = 'No Crash';
var expect = 'No Crash';

//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  var a = new Array(10*1000);
  a[0] = { toString: function() { System.forceFullCollection(); return ".*9"; }};;
  a[1] = "g";

  for (var i = 0; i != 10*1000; ++i) {
    String(new Number(123456789));
  }

  "".match.apply(123456789, a);

  Assert.expectEq(summary, expect, actual);


}

