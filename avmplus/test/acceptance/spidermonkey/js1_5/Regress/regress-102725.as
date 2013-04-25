/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


/*
 * Date: 09 October 2001
 *
 * SUMMARY: Regression test for Bugzilla bug 102725
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=102725
 * "gcc -O2 problems converting numbers to strings"
 *
 */
//-----------------------------------------------------------------------------
var gTestfile = 'regress-102725.js';
var UBound = 0;
var BUGNUMBER = 102725;
var summary = 'Testing converting numbers to strings';
var status = '';
var statusitems = [];
var actual = '';
var actualvalues = [];
var expect= '';
var expectedvalues = [];


/*
 * Successive calls to foo.toString() were producing different answers!
 */
//status = inSection(1);
foo = 1268426729388;
actual = foo.toString();
expect = '1268426729388';
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
  //printStatus (summary);

  for (var i=0; i<UBound; i++)
  {
    Assert.expectEq(statusitems[i], expectedvalues[i], actualvalues[i]);
  }


}

