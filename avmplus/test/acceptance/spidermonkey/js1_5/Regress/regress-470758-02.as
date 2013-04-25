/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/licenses/publicdomain/
 * Contributor: Blake Kaplan
 */
import com.adobe.test.Assert;

var gTestfile = 'regress-470758-02.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 470758;
var summary = 'Promote evald initializer into upvar';
var actual = '';
var expect = '';


//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  expect = 5;

  (function(){var x;for (x = 0; x < 5; x++);;print(actual = x);})();

  Assert.expectEq(summary, expect, actual);


}

