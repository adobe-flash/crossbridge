/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-356693.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 356693;
var summary = 'Do not assert: pn2->pn_op == JSOP_SETCALL';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  expect = 'ReferenceError: Error #1065';   // Variable x is not defined.
  try
  {
    delete (0 ? 3 : x());
  }
  catch(ex)
  {
    actual = ex + '';
  }

  Assert.expectEq(summary, expect, actual.substr(0,expect.length));


}

