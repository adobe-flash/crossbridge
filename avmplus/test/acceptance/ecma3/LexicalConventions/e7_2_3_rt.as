/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "7.2-3";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Line Terminators";


    var testcases = getTestCases();


function getTestCases() {
    var array = new Array();
    var item = 0;
    var b;


       
       b =

20;

    array[item++] = Assert.expectEq( "7.2 b<cr>20",     20,     b);

    return ( array );
}
