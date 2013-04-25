/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-253150.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 253150;
var summary = 'Do not warn on detecting properties';
var actual = '';
var expect = 'No warning';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

var testobject = {};

try
{
  var testresult = testobject.foo;
  actual = 'No warning';
}
catch(ex)
{
  actual = ex + '';
}

Assert.expectEq(summary + ': 1', expect, actual);

try
{
  if (testobject.foo)
  {
    ;
  }
  actual = 'No warning';
}
catch(ex)
{
  actual = ex + '';
}

Assert.expectEq(summary + ': 2', expect, actual);

try
{
  if (typeof testobject.foo == 'undefined')
  {
    ;
  }
  actual = 'No warning';
}
catch(ex)
{
  actual = ex + '';
}

Assert.expectEq(summary + ': 3', expect, actual);

try
{
  if (testobject.foo == null)
  {
    ;
  }
  actual = 'No warning';
}
catch(ex)
{
  actual = ex + '';
}

Assert.expectEq(summary + ': 4', expect, actual);

try
{
  if (testobject.foo == undefined)
  {
    ;
  }
  actual = 'No warning';
}
catch(ex)
{
  actual = ex + '';
}

Assert.expectEq(summary + ': 3', expect, actual);

