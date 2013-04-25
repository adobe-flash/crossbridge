/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "15.8.2.14";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.random()";


    var testcases = getTestCases();
    // test0();

// TODO: REVIEW AS4 CONVERSION ISSUE
function getTestCases() {
    var item;

    for ( item = 0; item < 100; item++ ) {
        var ranNum = Math.random();
        if (ranNum >= 0 && ranNum < 1) {
            Assert.expectEq( "Math.random() value " + ranNum,  true, true );
        } else {
            Assert.expectEq( "Math.random() value " + ranNum,  true, false );
        }
    }

    return;
}



/* 
function getTestCases() {
    var array = new Array();
    var item = 0;

    for ( item = 0; item < 100; item++ ) {
          array[item] = Assert.expectEq(   "Math.random()",    "pass",  null );
    }

    return ( array );
}
function getRandom( caseno ) {
    testcases[caseno].reason = Math.random();
    testcases[caseno].actual = "pass";

    if ( ! ( testcases[caseno].reason >= 0) ) {
        testcases[caseno].actual = "fail";
    }

    if ( ! (testcases[caseno].reason < 1) ) {
        testcases[caseno].actual = "fail";
    }
}

function test0() {
    for ( tc=0; tc < testcases.length; tc++ ) {
        getRandom( tc );
        testcases[tc].passed = writeTestCaseResult(
                            testcases[tc].expect,
                            testcases[tc].actual,
                            testcases[tc].description +" = "+
                            testcases[tc].actual );

        testcases[tc].reason += ( testcases[tc].passed ) ? "" : "wrong value ";
    }
    stopTest();
    return ( testcases );
}
*/

