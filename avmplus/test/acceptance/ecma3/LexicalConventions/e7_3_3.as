/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "7.3-3";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Comments";
var myObject = new Object();


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0; 
    
    // a comment string
    myObject.actual = "pass";

    Assert.expectEq( "source text directly following a single-line comment",
                                  "pass",
                                  myObject.actual);

    return ( array );
}
