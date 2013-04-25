/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
/*
 * Date: 06 February 2001
 *
 * SUMMARY:  Arose from Bugzilla bug 78156:
 * "m flag of regular expression does not work with $"
 *
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=78156
 *
 * The m flag means a regular expression should search strings
 * across multiple lines, i.e. across '\n', '\r'.
 */
//-------------------------------------------------------------------------------------------------
// var SECTION = "eregress_78156";
// var VERSION = "";
// var TITLE   = "Testing regular expressions with  ^, $, and the m flag -";
// var bug = "78156";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

var status = '';
var pattern = '';
var string = '';
var actualmatch = '';
var expectedmatch = '';

/*
 * All patterns have an m flag; all strings are multiline.
 * Looking for digit characters at beginning/end of lines.
 */

string = 'aaa\n789\r\nccc\r\n345';
 //    status = inSection(1);
    pattern = /^\d/gm;
    actualmatch = string.match(pattern);
    expectedmatch = ['7', '3'];
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(2);
    pattern = /\d$/gm;
    actualmatch = string.match(pattern);
    expectedmatch = ['9','5'];
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

string = 'aaa\n789\r\nccc\r\nddd';
 //    status = inSection(3);
    pattern = /^\d/gm;
    actualmatch = string.match(pattern);
    expectedmatch = ['7'];
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

 //    status = inSection(4);
    pattern = /\d$/gm;
    actualmatch = string.match(pattern);
    expectedmatch = ['9'];
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

    return array;
}
