/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
/*
 * Date: 01 May 2001
 *
 * SUMMARY:  Regression test for Bugzilla bug 31316:
 * "Rhino: Regexp matches return garbage"
 *
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=31316
 */
//-------------------------------------------------------------------------------------------------

// var SECTION = "eregress_31316";
// var VERSION = "";
// var TITLE   = "Regression test for Bugzilla bug 31316";
// var bug = "31316";

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


 //    status = inSection(1);
    pattern = /<([^\/<>][^<>]*[^\/])>|<([^\/<>])>/;
    string = '<p>Some<br />test</p>';
    actualmatch = string.match(pattern);
    expectedmatch = Array('<p>', undefined, 'p');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

    return array;
}
