/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "7.5-7";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Identifiers";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;
    var $0abc = 5;
    array[item++] = Assert.expectEq(     "var $0abc = 5",   5,    $0abc );
    return ( array );
}

