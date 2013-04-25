/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-356250.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 356250;
var summary = 'Do not assert: !fp->fun || !(fp->fun->flags & JSFUN_HEAVYWEIGHT) || fp->callobj';
var actual = 'No Crash';
var expect = 'No Crash';

(function() { a=function() { } })();
Assert.expectEq(summary + ': nested 0', expect, actual);

//-----------------------------------------------------------------------------
test1();
test2();
//-----------------------------------------------------------------------------

function test1()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  (function() { a=function() { } })();

  Assert.expectEq(summary + ': nested 1', expect, actual);


}

function test2()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  (function () {(a=function() { b=function() { } })();})();

  Assert.expectEq(summary + ': nested 2', expect, actual);


}

