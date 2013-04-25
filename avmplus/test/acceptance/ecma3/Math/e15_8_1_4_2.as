/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "15.8.1.4-2";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.LOG2E";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    delete(Math.LOG2E);
    array[item++] = Assert.expectEq(  "delete(Math.L0G2E);Math.LOG2E", 1.4426950408889634,     Math.LOG2E );
    array[item++] = Assert.expectEq(  "delete(Math.L0G2E)",            false,                  delete(Math.LOG2E) );
    return ( array );
}
