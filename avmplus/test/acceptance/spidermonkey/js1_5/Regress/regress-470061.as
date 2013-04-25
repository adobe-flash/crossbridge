/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-470061.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 470061;
var summary = 'TM: Do not assert: cx->fp->regs->pc == f->ip && f->root == f';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);


 
  function x (w, z) {
    var h = 0;
    var q = 0;
    while (q < 300) {
      while (w) {
      }
      ++q;
      if (q % 4 == 1) {
        h = Math.ceil(z);
      }
    }
  }

  x(false, 40);



  Assert.expectEq(summary, expect, actual);


}

