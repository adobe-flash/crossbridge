/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "7.5-6";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Identifiers";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;
     var _0abc = 5;
    array[item++] = Assert.expectEq(     "var _0abc = 5",   5,    _0abc );
    return ( array );
}

