/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-345961.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 345961;
var summary = 'Array.prototype.shift should preserve holes';
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

  var array = new Array(2);
  
  // Note that an empty array should not have a defined property at 0
  actual = array.hasOwnProperty(0);
  Assert.expectEq(summary+' before shift', expect, actual);
  Assert.expectEq('verify Array(2) length', 2, array.length);
  
  
  array.shift();
  Assert.expectEq('after shift - verify Array(2) length', 1, array.length);
  
  actual = array.hasOwnProperty(0);
  Assert.expectEq(summary+' after shift', expect, actual);

  array=Array(1);
  Assert.expectEq('verify Array(1) length', 1, array.length);
  
  array.shift(1);
  Assert.expectEq('after shift - verify Array(1) length', 0, array.length);
  
  actual = array.hasOwnProperty(1);
  Assert.expectEq(summary+' shift array len 1' , expect, actual);


}

