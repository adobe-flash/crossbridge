/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "15.8.1.2-2";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.LN10";


    var testcases = getTestCases();
function getTestCases() {
    var array = new Array();
    var item = 0;
    delete(Math.LN10);
    array[item++] = Assert.expectEq(  "delete( Math.LN10 ); Math.LN10",   2.302585092994046,       Math.LN10 );
    array[item++] = Assert.expectEq(  "delete( Math.LN10 ); ",             false,                  delete(Math.LN10) );
    return ( array );
}
