/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "15.8.1.1-2";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.E";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var MATH_E = 2.7182818284590452354
    array[item++] = Assert.expectEq(  "delete(Math.E)",                false,    delete Math.E );
    delete Math.E;
    array[item++] = Assert.expectEq(  "delete(Math.E); Math.E",        MATH_E,   Math.E );
    return ( array );
}
