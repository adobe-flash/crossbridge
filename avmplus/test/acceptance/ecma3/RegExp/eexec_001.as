/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "RegExp/exec-001";
// var VERSION = "ECMA_2";
// var TITLE   = "RegExp.prototype.exec(string)";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    /*
     * for each test case, verify:
     * - type of object returned
     * - length of the returned array
     * - value of lastIndex
     * - value of index
     * - value of input
     * - value of the array indices
     */

    // test cases without subpatterns
    // test cases with subpatterns
    // global property is true
    // global property is false
    // test cases in which the exec returns null

    array[item++] = Assert.expectEq( "NO TESTS EXIST", "PASSED", "PASSED");

    return array;
}
