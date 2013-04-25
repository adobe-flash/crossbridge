/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import avmplus.*;
import com.adobe.test.Assert;


var gTestfile = 'regress-464334.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 464334;
var summary = 'Do not assert: (size_t) (fp->regs->sp - fp->slots) <= fp->script->nslots';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  function g()
  {
    System.forceFullCollection();
  }

  var a = [];
  for (var i = 0; i != 20; ++i)
    a.push(i);
  g.apply(this, a);

  Assert.expectEq(summary, expect, actual);


}

