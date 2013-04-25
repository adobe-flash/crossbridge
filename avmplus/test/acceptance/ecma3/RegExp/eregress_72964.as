/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
/*
 * Date: 2001-07-17
 *
 * SUMMARY: Regression test for Bugzilla bug 72964:
 * "String method for pattern matching failed for Chinese Simplified (GB2312)"
 *
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=72964
 */
//-----------------------------------------------------------------------------

// var SECTION = "eregress_72964";
// var VERSION = "";
// var TITLE   = "Testing regular expressions containing non-Latin1 characters";
// var bug = "72964";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var cnSingleSpace = ' ';
    var status = '';
    var pattern = '';
    var string = '';
    var actualmatch = '';
    var expectedmatch = '';


    pattern = /[\S]+/;
        // 4 low Unicode chars = Latin1; whole string should match
     //    status = inSection(1);
        string = '\u00BF\u00CD\u00BB\u00A7';
        actualmatch = string.match(pattern);
        expectedmatch = Array(string);
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

        // Now put a space in the middle; first half of string should match
     //    status = inSection(2);
        string = '\u00BF\u00CD \u00BB\u00A7';
        actualmatch = string.match(pattern);
        expectedmatch = Array('\u00BF\u00CD');
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());


        // 4 high Unicode chars = non-Latin1; whole string should match
     //    status = inSection(3);
        string = '\u4e00\uac00\u4e03\u4e00';
        actualmatch = string.match(pattern);
        expectedmatch = Array(string);
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

        // Now put a space in the middle; first half of string should match
     //    status = inSection(4);
        string = '\u4e00\uac00 \u4e03\u4e00';
        actualmatch = string.match(pattern);
        expectedmatch = Array('\u4e00\uac00');
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

    return array;
}
