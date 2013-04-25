/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
/*
 * Date: 22 June 2001
 *
 * SUMMARY:  Regression test for Bugzilla bug 87231:
 * "Regular expression /(A)?(A.*)/ picks 'A' twice"
 *
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=87231
 * Key case:
 *
 *            pattern = /^(A)?(A.*)$/;
 *            string = 'A';
 *            expectedmatch = Array('A', '', 'A');
 *
 *
 * We expect the 1st subexpression (A)? NOT to consume the single 'A'.
 * Recall that "?" means "match 0 or 1 times". Here, it should NOT do
 * greedy matching: it should match 0 times instead of 1. This allows
 * the 2nd subexpression to make the only match it can: the single 'A'.
 * Such "altruism" is the only way there can be a successful global match...
 */
//-------------------------------------------------------------------------------------------------

// var SECTION = "eregress_87231";
// var VERSION = "";
// var TITLE   = "Testing regular expression /(A)?(A.*)/";
// var bug = "87231";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

var cnEmptyString = '';
var status = '';
var pattern = '';
var string = '';
var actualmatch = '';
var expectedmatch = '';


pattern = /^(A)?(A.*)$/;
 //    status = inSection(1);
    string = 'AAA';
    actualmatch = string.match(pattern);
    expectedmatch = Array('AAA', 'A', 'AA');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(2);
    string = 'AA';
    actualmatch = string.match(pattern);
    expectedmatch = Array('AA', 'A', 'A');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(3);
    string = 'A';
    actualmatch = string.match(pattern);
    expectedmatch = Array('A', undefined, 'A'); // 'altruistic' case: see above
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());


pattern = /(A)?(A.*)/;
var strL = 'zxcasd;fl\\\  ^';
var strR = 'aaAAaaaf;lrlrzs';

 //    status = inSection(4);
    string =  strL + 'AAA' + strR;
    actualmatch = string.match(pattern);
    expectedmatch = Array('AAA' + strR, 'A', 'AA' + strR);
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(5);
    string =  strL + 'AA' + strR;
    actualmatch = string.match(pattern);
    expectedmatch = Array('AA' + strR, 'A', 'A' + strR);
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(6);
    string =  strL + 'A' + strR;
    actualmatch = string.match(pattern);
    expectedmatch = Array('A' + strR, undefined, 'A' + strR); // 'altruistic' case: see above
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

    return array;
}
