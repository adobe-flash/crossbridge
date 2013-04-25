/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import com.adobe.test.Assert;
/*
*
* Date:    31 August 2002
* SUMMARY: RegExp conformance test
* See http://bugzilla.mozilla.org/show_bug.cgi?id=169497
*
*/
//-----------------------------------------------------------------------------

// var SECTION = "eregress_169497";
// var VERSION = "";
// var TITLE   = "RegExp conformance test";
// var bug = "169497";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var status = '';
    var pattern = '';
    var sBody = '';
    var sHTML = '';
    var string = '';
    var actualmatch = '';
    var expectedmatch = '';

    sBody += '<body onload="alert(event.type);">\n';
    sBody += '<p>Kibology for all<\/p>\n';
    sBody += '<p>All for Kibology<\/p>\n';
    sBody += '<\/body>';

    sHTML += '<html>\n';
    sHTML += sBody;
    sHTML += '\n<\/html>';

 //    status = inSection(1);
    string = sHTML;
    pattern = /<body.*>((.*\n?)*?)<\/body>/i;
    actualmatch = string.match(pattern);
    expectedmatch = Array(sBody, '\n<p>Kibology for all</p>\n<p>All for Kibology</p>\n', '<p>All for Kibology</p>\n');
    array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

    return array;
}
