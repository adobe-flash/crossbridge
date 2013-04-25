/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-416737-01.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 416737;
var summary = 'Do not assert: *pc == JSOP_GETARG';
var actual = '';
var expect = '';

//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  (function() { (function(){ function n(){} })(1) });

  Assert.expectEq(summary, expect, actual);


}


