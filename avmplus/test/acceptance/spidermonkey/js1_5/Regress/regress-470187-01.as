/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-470187-01.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 470187;
var summary = 'Do not assert: entry->kpc == (jsbytecode*) atoms[index]';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  for (var j=0;j<3;++j) ({valueOf: function(){return 2}}) - /x/;

  Assert.expectEq(summary, expect, actual);


}

