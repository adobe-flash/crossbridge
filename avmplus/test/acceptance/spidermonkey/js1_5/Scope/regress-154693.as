/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


/*
 *
 * Date:    26 Nov 2002
 * SUMMARY: Testing scope
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=154693
 *
 */
//-----------------------------------------------------------------------------
var gTestfile = 'regress-154693.js';
var UBound = 0;
var BUGNUMBER = 154693;
var summary = 'Testing scope';
var status = '';
var statusitems = [];
var actual = '';
var actualvalues = [];
var expect= '';
var expectedvalues = [];


function f()
{
  function nested() {}
  return nested;
}
var f1 = f();
var f2 = f();

//status = inSection(1);
actual = (f1 != f2);
expect = true;
addThis();



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

