/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-477733.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 477733;
var summary = 'TM: Do not assert: !(fp->flags & JSFRAME_POP_BLOCKS)';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);



  function g() {
    var x = [];
  }

  try {
    d.d.d;
  } catch(e) {
    void (function(){});
  }

  for (var o in [1, 2, 3]) {
    g();
  }



  Assert.expectEq(summary, expect, actual);


}

