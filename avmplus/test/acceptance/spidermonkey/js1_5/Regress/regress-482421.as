/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-482421.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 482421;
var summary = 'TM: Do not assert: vp >= StackBase(fp)';
var actual = '';
var expect = '';

//printBugNumber(BUGNUMBER);
//printStatus (summary);



function f()
{
  var x;
  for (var z = 0; z < 2; ++z) { new Object(new String(this), x)};

}
f();



Assert.expectEq(summary, expect, actual);

