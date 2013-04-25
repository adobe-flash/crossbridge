/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-504078.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 504078;
var summary = 'Iterations over global object';
var actual = '';
var expect = '';

var g = (typeof window == 'undefined' ? this : window);

//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);

  function keys(obj) {
    for (var prop in obj) {
    }
  }

  var data = { a : 1, b : 2 };
  var data2 = { a : 1, b : 2 };

  function boot() {
      keys(data);
    keys(g);
    keys(data2); // this call dies with "var prop is not a function"
    print('no error');
  }

  try
  {
    boot();
  }
  catch(ex)
  {
    actual = ex + '';
  }

  Assert.expectEq(summary, expect, actual);


}

