/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/licenses/publicdomain/
 * Contributor: Bob Clary
 */

import com.adobe.test.Assert;

var gTestfile = 'regress-58116.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 58116;
var summary = 'Compute Daylight savings time correctly regardless of year';
var actual = '';
var expect = '';
var status;

//printBugNumber(BUGNUMBER);
//printStatus (summary);

expect = (new Date(2005, 7, 1).getTimezoneOffset());
  
//status = summary + ' ' + inSection(1) + ' 1970-07-1 ';
actual = (new Date(1970, 7, 1).getTimezoneOffset());
Assert.expectEq(summary, expect, actual);
   
//status = summary + ' ' + inSection(2) + ' 1965-07-1 ';
actual = (new Date(1965, 7, 1).getTimezoneOffset());
Assert.expectEq(summary, expect, actual);
   
//status = summary + ' ' + inSection(3) + ' 0000-07-1 ';
actual = (new Date(0, 7, 1).getTimezoneOffset());
Assert.expectEq(summary, expect, actual);
  
//status = summary + ' ' + inSection(4) + ' 1911-07-1 ';
actual = (new Date(1911, 7, 1).getTimezoneOffset());
Assert.expectEq(summary, expect, actual);
  
//status = summary + ' ' + inSection(5) + ' 1944-07-1 ';
actual = (new Date(1944, 7, 1).getTimezoneOffset());
Assert.expectEq(summary, expect, actual);
  
//status = summary + ' ' + inSection(6) + ' 2038-07-1 ';
actual = (new Date(2038, 7, 1).getTimezoneOffset());
Assert.expectEq(summary, expect, actual);

