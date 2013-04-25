/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-457065-03.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 457065;
var summary = 'Do not assert: !fp->callee || fp->thisp == JSVAL_TO_OBJECT(fp->argv[-1])';
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
    new function (){ for (var x = 0; x < 3; ++x){} };
  })();



  Assert.expectEq(summary, expect, actual);


}

