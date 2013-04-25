/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-372364.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 372364;
var summary = 'Incorrect error message "() has no properties"';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  print('See Also bug 365891');
  expect = 'TypeError: Error #1009: Cannot access a property or method of a null object reference.'.substr(0,22);
  try
  {
    function a(){return null;} a(1)[0];
  }
  catch(ex)
  {
    actual = ex.toString().substr(0,22);
  }
  Assert.expectEq(summary, expect, actual);

  try
  {
    /a/.exec("b")[0];
  }
  catch(ex)
  {
    actual = ex.toString().substr(0,22);
  }
  Assert.expectEq(summary, expect, actual);


}

