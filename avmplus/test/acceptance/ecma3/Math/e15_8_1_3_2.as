/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "15.8.1.3-2";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.LN2";


    var testcases = getTestCases();


function getTestCases() {
    var array = new Array();
    var item = 0;

    var MATH_LN2 = 0.6931471805599453;

    array[item++] = Assert.expectEq(  "delete(Math.LN2)",              false,          delete(Math.LN2) );

    delete(Math.LN2);
    array[item++] = Assert.expectEq(  "delete(Math.LN2); Math.LN2",    MATH_LN2,       Math.LN2 );

    return ( array );
}
