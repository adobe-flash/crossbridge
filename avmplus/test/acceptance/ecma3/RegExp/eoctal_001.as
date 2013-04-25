/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import com.adobe.test.Assert;
/*
*
* Date:    18 July 2002
* SUMMARY: Testing octal sequences in regexps
* See http://bugzilla.mozilla.org/show_bug.cgi?id=141078
*
*/
//-----------------------------------------------------------------------------

// var SECTION = "eoctal_001";
// var VERSION = "";
// var TITLE   = "Testing octal sequences in regexps";
// var bug = "141078";

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
    pattern = /\240/;
    string = 'abc';
    actualmatch = string.match(pattern);
    expectedmatch = null;
    array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

    /*
     * In the following sections, we test the octal escape sequence '\052'.
     * This is character code 42, representing the asterisk character '*'.
     * The Unicode escape for it would be '\u002A', the hex escape '\x2A'.
     */
 //    status = inSection(2);
    pattern = /ab\052c/;
    string = 'ab*c';
    actualmatch = string.match(pattern);
    expectedmatch = Array('ab*c');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(3);
    pattern = /ab\052*c/;
    string = 'abc';
    actualmatch = string.match(pattern);
    expectedmatch = Array('abc');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(4);
    pattern = /ab(\052)+c/;
    string = 'ab****c';
    actualmatch = string.match(pattern);
    expectedmatch = Array('ab****c', '*');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(5);
    pattern = /ab((\052)+)c/;
    string = 'ab****c';
    actualmatch = string.match(pattern);
    expectedmatch = Array('ab****c', '****', '*');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(6);
    pattern = /(?:\052)c/;
    string = 'ab****c';
    actualmatch = string.match(pattern);
    expectedmatch = Array('*c');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

    return array;
}
