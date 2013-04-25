/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "7.2-5";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Line Terminators";


    var testcases = getTestCases();


function getTestCases() {
    var array = new Array();
    var item = 0;
    var a=10;
var b=10;
var c;
       

    
c=
a
+
b

    array[item++] = Assert.expectEq( "7.2 c<cr>a<cr>+<cr>b",     20,     c);

    return ( array );
}
