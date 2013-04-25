/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-351515.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 351515;
var summary = 'js17 features must be enabled by version request';
var actual = 'No Error';
var expect = 'No Error';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

yield = 1;
let   = 1;

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  function f(yield, let) { return yield+let; }

  var yield = 1;
  var let = 1;

  function yield() {}

  Assert.expectEq(summary, expect, actual);


}

