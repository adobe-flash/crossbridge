/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import com.adobe.test.Assert;
/*
*
* Date:    09 July 2002
* SUMMARY: RegExp conformance test
*
*   These testcases are derived from the examples in the ECMA-262 Ed.3 spec
*   scattered through section 15.10.2.
*
*/
//-----------------------------------------------------------------------------

// var SECTION = "e15_10_2_1";
// var VERSION = "";
// var TITLE   = "RegExp conformance test";
// var bug = "(none)";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var status = '';
    var pattern = '';
    var string = '';
    var actualmatch = '';
    var expectedmatch = '';


 //    status = inSection(1);
    pattern = "a|ab";
    var regexp = new RegExp(pattern);
    string = 'abc';
    var regexp2 = pattern.toString();
    actualmatch = string.match(regexp2);
    expectedmatch = Array('a');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(2);
    pattern = "((a)|(ab))((c)|(bc))";
    string = 'abc';
    var regexp = new RegExp(pattern);
    regexp2 = pattern.toString();
    actualmatch = string.match(regexp2);
    expectedmatch = Array('abc', 'a', 'a', undefined, 'bc', undefined, 'bc');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(3);
    pattern = "a[a-z]{2,4}";
    var regexp = new RegExp(pattern);
    string = 'abcdefghi';
    regexp2 = pattern.toString();
    actualmatch = string.match(regexp2);
    expectedmatch = Array('abcde');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(4);
    pattern = "a[a-z]{2,4}?";
    string = 'abcdefghi';
    var regexp = new RegExp(pattern);
    regexp2 = pattern.toString();
    actualmatch = string.match(regexp2);
    expectedmatch = Array('abc');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(5);
    pattern = "(aa|aabaac|ba|b|c)*";
    var regexp = new RegExp(pattern);
    string = 'aabaac';
    regexp2 = pattern.toString();
    actualmatch = string.match(regexp2);
    expectedmatch = Array('aaba', 'ba');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());



 //    status = inSection(6);
    pattern = "(z)((a+)?(b+)?(c))*";
    string = 'zaacbbbcac';
    regexp = pattern.toString();
    actualmatch = string.match(regexp);
    expectedmatch = Array('zaacbbbcac', 'z', 'ac', 'a', undefined, 'c');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(7);
    pattern = "(a*)*";
    string = 'b';
    regexp = pattern.toString();
    actualmatch = string.match(regexp);
    expectedmatch = Array('', "");
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());



 //    status = inSection(8);
    pattern = "(?=(a+))";
    string = 'baaabac';
    regexp = pattern.toString();
    actualmatch = string.match(regexp);
    expectedmatch = Array('', 'aaa');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());



 //    status = inSection(9);
    pattern = "(.*?)a(?!(a+)b\2c)\2(.*)";
    string = 'baaabaac';
    regexp = pattern.toString();
    actualmatch = string.match(regexp);

    /* This will need to be changed when bug 110184 is fixed
    //////////////////////////////
    */

    //expectedmatch = Array('baaabaac', 'ba', undefined, 'abaac');
    expectedmatch = null;

    /*
    //////////////////////////////
    */
    array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

 //    status = inSection(10);
    pattern = "(?=(a+))";
    string = 'baaabac';
    var regexp = pattern.toString();
    actualmatch = string.match(regexp);
    expectedmatch = Array('', 'aaa');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

    return array;
}
