/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
/*
*
* Date:    04 Feb 2002
* SUMMARY: regexp backreferences must hold |undefined| if not used
*
* See http://bugzilla.mozilla.org/show_bug.cgi?id=123437 (SpiderMonkey)
* See http://bugzilla.mozilla.org/show_bug.cgi?id=123439 (Rhino)
*
*/
//-----------------------------------------------------------------------------

// var SECTION = "eregress_123437";
// var VERSION = "";
// var TITLE   = "regexp backreferences must hold |undefined| if not used";
// var bug = "123437";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var status = '';
    var pattern = '';
    var string = '';
    var actualmatch = '';
    var expectedmatch = '';


    pattern = /(a)?a/;
    string = 'a';
 //    status = inSection(1);
    actualmatch = string.match(pattern);
    expectedmatch = Array('a', undefined);
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

    pattern = /a|(b)/;
    string = 'a';
 //    status = inSection(2);
    actualmatch = string.match(pattern);

    expectedmatch = Array('a', undefined);

    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

    pattern = /(a)?(a)/;
    string = 'a';
 //    status = inSection(3);
    actualmatch = string.match(pattern);
    expectedmatch = Array('a', undefined, 'a');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

    return array;
}
