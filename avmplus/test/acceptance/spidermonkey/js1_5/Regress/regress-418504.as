/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-418504.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 418504;
var summary = 'Untagged boolean stored in a jsval in JS_ConvertValue';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  expect = false;
  actual = new RegExp().multiline;
  Assert.expectEq('RegExp.multiline', expect, actual);

  expect = false;
  RegExp.multiline = 17;
  actual = new RegExp().multiline;
  Assert.expectEq('RegExp.multiline = 17', expect, actual);

  expect = false;
  RegExp.multiline = 17;
  actual = new RegExp().multiline;
  Assert.expectEq('RegExp.multiline = 17', expect, actual);

  expect = false;
  RegExp.multiline = 17;
  actual = new RegExp().multiline;
  Assert.expectEq('RegExp.multiline = 17', expect, actual);

  expect = false;
  RegExp.multiline = true;
  actual = new RegExp().multiline;
  Assert.expectEq('RegExp.multiline = true', expect, actual);

  expect = false;
  RegExp.multiline = 17;
  actual = new RegExp().multiline;
  Assert.expectEq('RegExp.multiline = 17', expect, actual);
  
  expect = true;
  actual = new RegExp('^bob','m').multiline;
  Assert.expectEq("new RegExp('^bob','m').multiline", expect, actual);

  expect = true;
  actual = /^bob/m.multiline;
  Assert.expectEq("/^bob/m.multiline", expect, actual);
  
  expect = false;
  actual = new RegExp('^bob').multiline;
  Assert.expectEq("new RegExp('^bob').multiline", expect, actual);

  expect = false;
  actual = /^bob/.multiline;
  Assert.expectEq("/^bob/.multiline", expect, actual);
}

