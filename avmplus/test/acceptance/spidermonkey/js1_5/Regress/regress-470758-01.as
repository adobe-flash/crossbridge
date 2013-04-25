/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/licenses/publicdomain/
 * Contributor: Blake Kaplan
 */
import com.adobe.test.Assert
var gTestfile = 'regress-470758-01.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 470758;
var summary = 'Do not crash with eval upvars';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  (function() { var k; for (var k in {}); })()

  Assert.expectEq(summary, expect, actual);


}

