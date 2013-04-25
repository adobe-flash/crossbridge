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

// var SECTION = "eregress_169534";
// var VERSION = "";
// var TITLE   = "RegExp conformance test";
// var bug = "169534";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var status = '';
    var actual = '';
    var expect= '';

 //    status = inSection(1);
    var re = /(\|)([\w\x81-\xff ]*)(\|)([\/a-z][\w:\/\.]*\.[a-z]{3,4})(\|)/ig;
    var str = "To sign up click |here|https://www.xxxx.org/subscribe.htm|";
    actual = str.replace(re, '<a href="$4">$2</a>');
    expect = 'To sign up click <a href="https://www.xxxx.org/subscribe.htm">here</a>';
    array[item++] = Assert.expectEq( status, expect, actual);

    return array;
}
