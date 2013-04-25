/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "15.8.2.17";
//     var VERSION = "ECMA_4";
//     var TITLE   = "Math.sqrt(x)";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var x = 3;
    for( var i = 0; i < 20; i++ ){
        array[item++] = Assert.expectEq(   "Math.sqrt("+x+")",     Number(1.73205080756887719318).toFixed(i).toString(),       Math.sqrt(x).toFixed(i).toString() );
    }

    x = 2;
    for( var i = 0; i < 20; i++ ){
        array[item++] = Assert.expectEq(   "Math.sqrt("+x+")",     Number(1.4142135623730951455).toFixed(i).toString(),        Math.sqrt(x).toFixed(i).toString() );
    }


    var twenty = 100000000000000000000.1;

    for( var i = 0; i < 20; i++ ){
        
        
        array[item++] = Assert.expectEq(   "Math.sqrt("+x+")",     Number(1.4142135623730951455).toFixed(i).toString(),        Math.sqrt(x).toFixed(i).toString() );
    }

    return ( array );
}
