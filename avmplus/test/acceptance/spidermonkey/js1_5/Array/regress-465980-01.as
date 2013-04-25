/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-465980-01.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 465980;
var summary = 'Do not crash @ InitArrayElements';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  try
  {
    var a = new Array(4294967294);
    a.push("foo", "bar");
  }
  catch(ex)
  {
  }

  Assert.expectEq(summary, expect, actual);


}

