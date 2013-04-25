/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "15.8.1.6-2";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.PI";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    delete Math.PI;
    array[item++] = Assert.expectEq(  "delete Math.PI; Math.PI",       3.141592653589793,  Math.PI );
    array[item++] = Assert.expectEq(  "delete Math.PI; Math.PI",       false,                  delete Math.PI );
    return ( array );
}
