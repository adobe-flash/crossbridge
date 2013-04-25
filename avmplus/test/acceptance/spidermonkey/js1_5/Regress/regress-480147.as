/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-480147.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 480147;
var summary = 'TM: Do not assert: cx->bailExit';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);



  var w = [/a/, /b/, /c/, {}];
  for (var i = 0; i < w.length; ++i)
    "".replace(w[i], "");



  Assert.expectEq(summary, expect, actual);


}

