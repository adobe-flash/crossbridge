/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-465347.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 465347;
var summary = 'Test integer to id in js_Int32ToId';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  var o;

  o = new Array();

  expect = undefined;
  o[0xffffffff] = 'end';
  actual = o[-1];
  Assert.expectEq(summary + ': 1', expect, actual);

  expect = 42;
  o['42'] = 42;
  actual = o[42];
  Assert.expectEq(summary + ': 2', expect, actual);

  //

  o = new Object();

  expect = undefined;
  o[0xffffffff] = 'end';
  actual = o[-1];
  Assert.expectEq(summary + ': 3', expect, actual);

  expect = 42;
  o['42'] = 42;
  actual = o[42];
  Assert.expectEq(summary + ': 4', expect, actual);


}

