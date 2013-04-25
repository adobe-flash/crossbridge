/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-343966.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 343966;
var summary = 'ClearScope foo regressed due to bug 343417';
var actual = 'failed';
var expect = 'passed';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  Function["prototype"].inherits=function(a){};
  function foo(){};
  function bar(){};
  foo.inherits(bar);
  actual = "passed";

  Assert.expectEq(summary, expect, actual);


}

