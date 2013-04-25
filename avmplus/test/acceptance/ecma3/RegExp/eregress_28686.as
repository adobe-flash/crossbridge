/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * ***** BEGIN LICENSE BLOCK *****
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "eregress_28686";
// var VERSION = "";
// var TITLE   = "Regression test for Bugzilla bug 28686";
// var bug = "28686";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var str = 'foo "bar" baz';
    // reportCompare ('foo \\"bar\\" baz', str.replace(/([\'\"])/g, "\\$1"),
    //            "str.replace failed.");
    array[item++] = Assert.expectEq(
        "str.replace failed.",
        'foo \\"bar\\" baz',
        str.replace(/([\'\"])/g, "\\$1"));

    return array;
}
