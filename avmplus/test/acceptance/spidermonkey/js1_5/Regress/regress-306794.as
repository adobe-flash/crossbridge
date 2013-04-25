/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/licenses/publicdomain/
 * Contributor: Blake Kaplan
 */
import com.adobe.test.Assert;

var gTestfile = 'regress-306794.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 306794;
var summary = 'Do not assert: parsing foo getter';
var actual = 'No Assertion';
var expect = 'No Assertion';

//printBugNumber(BUGNUMBER);
//printStatus (summary);
 
try
{
  getter;
}
catch(e)
{
}

Assert.expectEq(summary, expect, actual);

