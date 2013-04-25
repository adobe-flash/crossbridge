/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "7.3-10";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Comments";
var myObject = new Object();

    var testcases = getTestCases();


function getTestCases() {
    var array = new Array(); 
    var foo = 1;

    /*//*/myObject.actual="pass";
    Assert.expectEq( 
                                  "code following multiline comment",
                                  "pass",
                                  myObject.actual);
        return ( array );
}
