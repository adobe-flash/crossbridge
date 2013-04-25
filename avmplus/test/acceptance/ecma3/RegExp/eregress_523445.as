/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import com.adobe.test.Assert;
/*
*
* Date:    21 Oct. 2009
* SUMMARY: RegExp conformance test
* See http://bugzilla.mozilla.org/show_bug.cgi?id=523445
*
*/
//-----------------------------------------------------------------------------

// var SECTION = "eregress_523445";
// var VERSION = "";
// var TITLE   = "RegExp conformance test";
// var bug = "523445";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var status = '';
    var actual = '';
    var expect= '';

 //    status = inSection(1);
    var s : String = "<a>String</a>";
    var r : RegExp = new RegExp("(^<a>)|(<\/a>$)|(^<a\/>$)", "g");

    // This should produce 'String', not 'String</a>'
    s = s.replace(r, "");
    trace(s);
    
    actual = s.replace(r, "");
    expect = 'String';
    array[item++] = Assert.expectEq( status, expect, actual);

    return array;
}

