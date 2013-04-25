/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-459628.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 459628;
var summary = 'Do not assert: JSVAL_IS_VOID(STOBJ_GET_SLOT(obj, map->freeslot))';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 


(function() {
  for (var odjoff = 0; odjoff < 4; ++odjoff) {
    new Date()[0] = 3;
  }
})();



  Assert.expectEq(summary, expect, actual);


}

