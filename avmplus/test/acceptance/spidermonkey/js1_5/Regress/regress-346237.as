/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-346237.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 346237;
var summary = 'RegExp - /(|)??x/g.exec("y")';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  /(|)??x/g.exec("y");

  Assert.expectEq(summary + ': /(|)??x/g.exec("y")', expect, actual);


}

