/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-418128.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 418128;
var summary = 'GC hazard with ++/--';
var actual = 'No Crash';
var expect = 'No Crash';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  var obj = {};
  var id = { toString: function() { return ""+Math.pow(2, 0.1); } }
  obj[id] = { valueOf: unrooter };
  print(obj[id]++);
  System.forceFullCollection();
  print(uneval(obj));

  function unrooter()
  {
    delete obj[id];
    System.forceFullCollection();
    return 10;
  }

  Assert.expectEq(summary, expect, actual);


}

