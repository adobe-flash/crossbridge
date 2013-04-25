/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/licenses/publicdomain/
 * Contributor: Bob Clary
 */

import com.adobe.test.Assert;

var gTestfile = 'regress-162392.js';

//-----------------------------------------------------------------------------
// SUMMARY: 10.1.8 Arguments Object length

var BUGNUMBER = 162392;
var summary = 'arguments.length == 0 when no arguments specified';
var actual = noargslength();
var expect = 0;

function noargslength()
{
    return(arguments.length);

}

//-----------------------------------------------------------------------------
addtestcases();
//-----------------------------------------------------------------------------

function addtestcases()
{

  //printBugNumber(BUGNUMBER);
  //printStatus (summary);
 
  Assert.expectEq(summary, expect, actual);


}

