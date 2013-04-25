/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-452742-02.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 452742;
var summary = 'Do not do overzealous eval inside function optimization in BindNameToSlot';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  expect = '';

  var obj = { arguments: [-100] };

  function a()
  {
    with (obj) { return arguments[0]; }
  }

  function b()
  {
    var result;
    with (obj) { result = arguments[0]; };
    return result;
  }

  try
  {
    var result = a();
    if (result !== -100)
      throw "Bad result " + result;

    var result = b();
    if (result !== -100)
      throw "Bad result " + result;
  }
  catch(ex)
  {
    actual = ex + '';
  }
  Assert.expectEq(summary, expect, actual);


}

