/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

/*
 12.6.4 of ECMA-262 which defines the for-in Statement states that "If a
property that has not yet been visited during enumeration is deleted, then it
will not be visited"
*/


var gTestfile = 'regress-261887.js';
//-----------------------------------------------------------------------------
// testcase from Oscar Fogelberg <osfo@home.se>
var BUGNUMBER = 261887;
var summary = 'deleted properties should not be visited by for in';
var actual = '';
var expect = '';

//printBugNumber(BUGNUMBER);
//printStatus(summary);

var count = 0;
var result = [];
var value = [];

var t = new Object();
t.one = "one";
t.two = "two";
t.three = "three";
t.four = "four";
t.five = "five";

var deletedFive = false;
// need to store in an array and sort as order of for ... in is not guarenteed
for (var prop in t) {
    // since order is not guarenteed, sometimes we iterate through four first, so delete five in that case
    if (count == 0) {
        if (prop != 'four') {
            delete(t.four);
        } else {
            delete(t.five);
            deletedFive = true;
        }
    }
    count++;
    value.push(t[prop]);
    result.push(prop);
}

if (deletedFive == true) {
    expectedValue = ['four', 'one', 'three', 'two'].toString();
    expectedResult = ['four', 'one', 'three', 'two'].toString();
} else {    // deleted four
    expectedValue = ['five', 'one', 'three', 'two'].toString();
    expectedResult = ['five', 'one', 'three', 'two'].toString();
}

Assert.expectEq(summary + ' : value', expectedValue, value.sort().toString());
Assert.expectEq(summary + ' : prop', expectedResult, result.sort().toString());

