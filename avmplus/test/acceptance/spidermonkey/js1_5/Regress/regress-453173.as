/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-453173.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 453173;
var summary = 'Do not Crash with JIT [@ TraceRecorder::record_JSOP_ENDINIT] with "[,]"';
var actual = 'No Crash';
var expect = 'No Crash';

//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  var i;



  for(i=0;i<4;++i) var x=[,];



  Assert.expectEq(summary, expect, actual);


}

