/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "7.5-1";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Identifiers";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    //array[item++] = Assert.expectEq(     "var $123 = 5",      5,       eval("var $123 = 5;$123") );
    var $123 = 5;
    array[item++] = Assert.expectEq(     "var $123 = 5",      5,  $123 );
    //array[item++] = Assert.expectEq(     "var _123 = 5",      5,       eval("var _123 = 5;_123") );
    var _123 = 5;
    array[item++] = Assert.expectEq(     "var _123 = 5",      5,   _123);
    var MyNumber:Number = 100
    var mynumber:Number = 20
    array[item++] = Assert.expectEq(     "Testing case sensitivity of identifier",      false,  MyNumber==20);

    return ( array );
}
