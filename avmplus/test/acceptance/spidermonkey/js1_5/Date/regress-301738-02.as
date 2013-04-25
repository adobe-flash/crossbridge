/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


var gTestfile = 'regress-301738-02.js';
//-----------------------------------------------------------------------------
var BUGNUMBER = 301738;
var summary = 'Date parse compatibilty with MSIE';
var actual = '';
var expect = '';

//printBugNumber(BUGNUMBER);
//printStatus (summary);

/*
    Case 2. The input string is of the form "f/m/l" where f, m and l are
    integers, e.g. 7/16/45.
    Adjust the mon, mday and year values to achieve 100% MSIE
    compatibility.
    a. If 0 <= f < 70, f/m/l is interpreted as month/day/year.
    i.  If year < 100, it is the number of years after 1900
    ii. If year >= 100, it is the number of years after 0.
    b. If 70 <= f < 100
    i.  If m < 70, f/m/l is interpreted as
    year/month/day where year is the number of years after
    1900.
    ii. If m >= 70, the date is invalid.
    c. If f >= 100
    i.  If m < 70, f/m/l is interpreted as
    year/month/day where year is the number of years after 0.
    ii. If m >= 70, the date is invalid.
*/

var f;
var m;
var l;

function newDate(f, m, l)
{
  return new Date(f + '/' + m + '/' + l);
}

function newDesc(f, m, l)
{
  return f + '/' + m + '/' + l;
}

// Tamarin does not support these ie date formats
/*
// 2.a.i
f = 0;
m = 0;
l = 0;

expect = (new Date(l, f-1, m)).toDateString();
actual = newDate(f, m, l).toDateString();
Assert.expectEq(newDesc(f, m, l), expect, actual);

f = 0;
m = 0;
l = 100;

expect = (new Date(l, f-1, m)).toDateString();
actual = newDate(f, m, l).toDateString();
Assert.expectEq(newDesc(f, m, l), expect, actual);

// 2.a.ii
f = 0;
m = 24;
l = 100;

expect = (new Date(l, f-1, m)).toDateString();
actual = newDate(f, m, l).toDateString();
Assert.expectEq(newDesc(f, m, l), expect, actual);

f = 0;
m = 24;
l = 2100;

expect = (new Date(l, f-1, m)).toDateString();
actual = newDate(f, m, l).toDateString();
Assert.expectEq(newDesc(f, m, l), expect, actual);


// 2.b.i
f = 70;
m = 24;
l = 100;

expect = (new Date(f, m-1, l)).toDateString();
actual = newDate(f, m, l).toDateString();
Assert.expectEq(newDesc(f, m, l), expect, actual);

*/

f = 99;
m = 12;
l = 1;

expect = (new Date(f, m-1, l)).toDateString();
actual = newDate(f, m, l).toDateString();
Assert.expectEq(newDesc(f, m, l), expect, actual);

// 2.b.ii.

f = 99;
m = 70;
l = 1;

expect = true;
actual = isNaN(newDate(f, m, l));
Assert.expectEq(newDesc(f, m, l) + ' is an invalid date', expect, actual);

// 2.c.i

f = 100;
m = 12;
l = 1;

expect = (new Date(f, m-1, l)).toDateString();
actual = newDate(f, m, l).toDateString();
Assert.expectEq(newDesc(f, m, l), expect, actual);

// 2.c.ii

f = 100;
m = 70;
l = 1;

expect = true;
actual = isNaN(newDate(f, m, l));
Assert.expectEq(newDesc(f, m, l) + ' is an invalid date', expect, actual);



