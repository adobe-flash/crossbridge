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
    pattern = /a|ab/;
    string = 'abc';
    actualmatch = string.match(pattern);
    expectedmatch = Array('a');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(2);
    pattern = /((a)|(ab))((c)|(bc))/;
    string = 'abc';
    actualmatch = string.match(pattern);
    expectedmatch = Array('abc', 'a', 'a', undefined, 'bc', undefined, 'bc');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(3);
    pattern = /a[a-z]{2,4}/;
    string = 'abcdefghi';
    actualmatch = string.match(pattern);
    expectedmatch = Array('abcde');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(4);
    pattern = /a[a-z]{2,4}?/;
    string = 'abcdefghi';
    actualmatch = string.match(pattern);
    expectedmatch = Array('abc');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(5);
    pattern = /(aa|aabaac|ba|b|c)*/;
    string = 'aabaac';
    actualmatch = string.match(pattern);
    expectedmatch = Array('aaba', 'ba');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(6);
    pattern = /^(a+)\1*,\1+$/;
    string = 'aaaaaaaaaa,aaaaaaaaaaaaaaa';
    actualmatch = string.match(pattern);
    expectedmatch = Array('aaaaaaaaaa,aaaaaaaaaaaaaaa', 'aaaaa');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(7);
    pattern = /(z)((a+)?(b+)?(c))*/;
    string = 'zaacbbbcac';
    actualmatch = string.match(pattern);
    expectedmatch = Array('zaacbbbcac', 'z', 'ac', 'a', undefined, 'c');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(8);
    pattern = /(a*)*/;
    string = 'b';
    actualmatch = string.match(pattern);
    expectedmatch = Array('', "");
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(9);
    pattern = /(a*)b\1+/;
    string = 'baaaac';
    actualmatch = string.match(pattern);
    expectedmatch = Array('b', '');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(10);
    pattern = /(?=(a+))/;
    string = 'baaabac';
    actualmatch = string.match(pattern);
    expectedmatch = Array('', 'aaa');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(11);
    pattern = /(?=(a+))a*b\1/;
    string = 'baaabac';
    actualmatch = string.match(pattern);
    expectedmatch = Array('aba', 'a');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(12);
    pattern = /(.*?)a(?!(a+)b\2c)\2(.*)/;
    string = 'baaabaac';
    actualmatch = string.match(pattern);

    expectedmatch = Array('baaabaac', 'ba', undefined, 'abaac');

    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(13);
    pattern = /(?=(a+))/;
    string = 'baaabac';
    actualmatch = string.match(pattern);
    expectedmatch = Array('', 'aaa');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

    return array;
}
