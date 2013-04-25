/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import com.adobe.test.Assert;
/*
*
* Date:    20 Sep 2002
* SUMMARY: RegExp conformance test
* See http://bugzilla.mozilla.org/show_bug.cgi?id=169534
*
*/
//-----------------------------------------------------------------------------

// var SECTION = "eregress_170798";
// var VERSION = "";
// var TITLE   = "RegExp conformance test";
// var bug = "170798";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var status = '';
    var actual = '';
    var expect= '';

    var a:Array = [ /foo/, /bar/ ]

 //    status = inSection(1);
    actual = String(a[0]);
    expect = "/foo/";
    array[item++] = Assert.expectEq( status, expect, actual);

 //    status = inSection(2);
    actual = String(a[1]);
    expect = "/bar/";
    array[item++] = Assert.expectEq( status, expect, actual);

    return array;
}
