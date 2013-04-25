/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-480244.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 480244;
var summary = 'Do not assert: isInt32(*p)';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);



  function outer() {
    var v = 10.234;
    for (var i = 0; i < 0xff; ++i) {
      inner(v);
    }
  }

  var g = 0;
  var h = 0;

  function inner() {
    var v = 10;
    for (var k = 0; k < 0xff; ++k) {
      g++;
      if (g & 0xff == 0xff)
        h++;
    }
    return h;
  }

  outer();
  print("g=" + g + " h=" + h);



  Assert.expectEq(summary, expect, actual);


}

