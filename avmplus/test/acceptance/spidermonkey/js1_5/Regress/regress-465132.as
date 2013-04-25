/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-465132.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 465132;
var summary = 'TM: Mathematical constants should be constant';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);



  var constants = ['E', 'LN10', 'LN2', 'LOG2E', 'LOG10E', 'PI', 'SQRT1_2', 'SQRT2'];

  for (var j = 0; j < constants.length; j++)
  {
    expect = Math[constants[j]];

    for(i=0;i<9;++i) {
        try {
            ++Math[constants[j]];
        } catch (e) {
            print(e)
        }
    }
    
      

    actual = Math[constants[j]];

    Assert.expectEq(summary + ' Math.' + constants[j], expect, actual);
  }




}

