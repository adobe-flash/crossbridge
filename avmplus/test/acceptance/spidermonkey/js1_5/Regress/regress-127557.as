/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


/*
 *
 * Date:    06 Mar 2002
 * SUMMARY: Testing cloned function objects
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=127557
 *
 * Before this bug was fixed, this testcase would error when run:
 *
 *             ReferenceError: h_peer is not defined
 *
 * The line |g.prototype = new Object| below is essential: this is
 * what was confusing the engine in its attempt to look up h_peer
 */
//-----------------------------------------------------------------------------
var gTestfile = 'regress-127557.js';
var UBound = 0;
var BUGNUMBER = 127557;
var summary = 'Testing cloned function objects';
var cnCOMMA = ',';
var status = '';
var statusitems = [];
var actual = '';
var actualvalues = [];
var expect= '';
var expectedvalues = [];


function f(x,y)
{
  function h()
  {
    return h_peer();
  }
  function h_peer()
  {
    return (x + cnCOMMA + y);
  }
  return h;
}

if (typeof clone == 'function')
{
  //status = inSection(1);
  var g = clone(f);
  g.prototype = new Object;
  var h = g(5,6);
  actual = h();
  expect = '5,6';
  addThis();
}
else
{
  Assert.expectEq('shell only test requires clone()', 'Test not run', 'Test not run');
}



//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------



function addThis()
{
  statusitems[UBound] = status;
  actualvalues[UBound] = actual;
  expectedvalues[UBound] = expect;
  UBound++;
}


function addtestcases()
{

  //printBugNumber(BUGNUMBER);
//printStatus(summary);

  for (var i=0; i<UBound; i++)
  {
    Assert.expectEq(statusitems[i], expectedvalues[i], actualvalues[i]);
  }


}


