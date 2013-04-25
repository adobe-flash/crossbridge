/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-346363.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 346363;
var summary = 'Date.prototype.setFullYear()';
var actual = '';
var expect = true;


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  var d = new Date();
  d.setFullYear();
  d.setFullYear(2006);
  actual = d.getFullYear() == 2006;

  Assert.expectEq(summary, expect, actual);


}

