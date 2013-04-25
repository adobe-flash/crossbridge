/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-478314.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 478314;
var summary = 'Do not assert: "need a way to EOT now, since this is trace end": 0';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);



  for (var z = 0; z < 2; ++z) { switch(false & 1e-81) {} }



  Assert.expectEq(summary, expect, actual);


}

