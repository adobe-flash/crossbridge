/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-360969-05.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 360969;
var summary = '2^17: local function';
var actual = 'No Crash';
var expect = 'No Crash';

var global = this;

//-----------------------------------------------------------------------------
addtestcases();

//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  var start = new Date();
  var p;
  var i;
  var limit = 2 << 16;

  for (var i = 0; i < limit; i++)
  {
    this['pf'+i] = function() {};
  }

  Assert.expectEq(summary, expect, actual);

  var stop = new Date();

  //print('Elapsed time: ' + Math.floor((stop - start)/1000) + ' seconds');


}


