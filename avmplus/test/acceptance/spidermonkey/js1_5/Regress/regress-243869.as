/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-243869.js';
//-----------------------------------------------------------------------------
// testcase from Alex Vincent
var BUGNUMBER = 243869;
var actual = '';


//printBugNumber(BUGNUMBER);

function bar()
{
  try
  {
    var f = new Error("Test Error", 123);
    throw f;
  }
  catch(e)
  {
    throw e;
  }
}

try
{
  bar();
}
catch(eb)
{
  actual = eb;
}

Assert.expectEq('Rethrown custom Errors should retain message', "Test Error", actual.message);
Assert.expectEq('Rethrown custom Errors should retain id', 123, actual.errorID)

