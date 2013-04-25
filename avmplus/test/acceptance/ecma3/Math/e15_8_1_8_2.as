/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "15.8.1.8-2";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.SQRT2";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    delete Math.SQRT2;
    array[item++] = Assert.expectEq(  "delete Math.SQRT2; Math.SQRT2", 1.4142135623730951,     Math.SQRT2 );
    array[item++] = Assert.expectEq(  "delete Math.SQRT2",             false,                  delete Math.SQRT2 );
    return ( array );
}
