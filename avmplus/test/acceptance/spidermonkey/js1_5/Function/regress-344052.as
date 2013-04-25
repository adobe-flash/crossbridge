/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-344052.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 344052;
var summary = 'Function prototype - simple shared property';
var actual = '';
var expect = 'true';

Function.prototype.foo = true;
function y(){};

//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  actual = String(y.foo);

  Assert.expectEq(summary, expect, actual);


}

