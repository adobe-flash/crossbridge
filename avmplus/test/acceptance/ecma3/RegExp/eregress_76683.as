/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
/*
 * Date: 01 May 2001
 *
 * SUMMARY: Regression test for Bugzilla bug 76683 on Rhino:
 * "RegExp regression (NullPointerException)"
 *
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=76683
 */
//-------------------------------------------------------------------------------------------------
// var SECTION = "eregress_76683";
// var VERSION = "";
// var TITLE   = "Regression test for Bugzilla bug 76683";
// var bug = "76683";

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
 * Rhino (2001-04-19) crashed on the 3rd regular expression below.
 * It didn't matter what the string was. No problem in SpiderMonkey -
 */
string = 'abc';
 //    status = inSection(1);
    pattern = /(<!--([^-]|-[^-]|--[^>])*-->)|(<([\$\w:\.\-]+)((([ ][^\/>]*)?\/>)|(([ ][^>]*)?>)))/;
    actualmatch = string.match(pattern);
    expectedmatch = null;
    array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

 //    status = inSection(2);
    pattern = /(<!--([^-]|-[^-]|--[^>])*-->)|(<(tagPattern)((([ ][^\/>]*)?\/>)|(([ ][^>]*)?>)))/;
    actualmatch = string.match(pattern);
    expectedmatch = null;
    array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

    // This was the one causing a Rhino crash -
 //    status = inSection(3);
    pattern = /(<!--([^-]|-[^-]|--[^>])*-->)|(<(tagPattern)((([ ][^\/>]*)?\/>)|(([ ][^>]*)?>)))|(<\/tagPattern[^>]*>)/;
    actualmatch = string.match(pattern);
    expectedmatch = null;
    array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

    return array;
}
