/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = '10.1.6.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 293782;
var summary = 'Local variables can cause predefined function object properties to be undefined';
var actual = '';
var expect = '';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

function f()
{
  var constructor=1;
}

expect = '[class Function]';
actual = f.constructor.toString();
 
Assert.expectEq(summary, expect, actual);

