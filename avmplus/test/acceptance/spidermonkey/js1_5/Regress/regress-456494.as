/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-456494.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 456494;
var summary = 'Do not crash with apply and argc > nargs';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);



  function k(s)
  {
  }
  function f()
  {
    for (i = 0; i < 10; i++)
    {
      k.apply(this, arguments);
    }
  }
  f(1);



  if (typeof this.tracemonkey != 'undefined')
  {
    for (var p in this.tracemonkey)
    {
      print(p + ':' + this.tracemonkey[p]);
    }
  }

  Assert.expectEq(summary, expect, actual);


}

