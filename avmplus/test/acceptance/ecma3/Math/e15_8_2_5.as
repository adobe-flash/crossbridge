/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "15.8.2.5";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.atan2(x,y)";
    var BUGNUMBER="76111";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(    "Math.atan2.length",       2,              Math.atan2.length );

    array[item++] = Assert.expectEq(    "Math.atan2(NaN, 0)",      Number.NaN,     Math.atan2(Number.NaN,0) );
    array[item++] = Assert.expectEq(    "Math.atan2(null, null)",  0,              Math.atan2(null, null) );
    array[item++] = Assert.expectEq(    "Math.atan2(void 0, void 0)",       Number.NaN,     Math.atan2(void 0, void 0) );

 /*   thisError="no error";
    try{
        Math.atan2();
    }catch(e:Error){
        thisError=(e.toString()).substring(0,26);
    }finally{//print(thisError);
        array[item++] = Assert.expectEq(    "Math.atan2()","ArgumentError: Error #1063",thisError);
    }
    array[item++] = Assert.expectEq(    "Math.atan2()",       Number.NaN,                  Math.atan2() );*/

    array[item++] = Assert.expectEq(    "Math.atan2(0, NaN)",       Number.NaN,     Math.atan2(0,Number.NaN) );
    array[item++] = Assert.expectEq(    "Math.atan2(1, 0)",         Math.PI/2,      Math.atan2(1,0)          );
    array[item++] = Assert.expectEq(    "Math.atan2(1,-0)",         Math.PI/2,      Math.atan2(1,-0)         );
    array[item++] = Assert.expectEq(    "Math.atan2(0,0.001)",      0,              Math.atan2(0,0.001)      );
    array[item++] = Assert.expectEq(    "Math.atan2(0,0)",          0,              Math.atan2(0,0)          );
    array[item++] = Assert.expectEq(    "Math.atan2(0, -0)",        Math.PI,        Math.atan2(0,-0)         );
    array[item++] = Assert.expectEq(    "Math.atan2(0, -1)",        Math.PI,        Math.atan2(0, -1)        );

    array[item++] = Assert.expectEq(    "Math.atan2(-0, 1)",        -0,             Math.atan2(-0, 1)        );
    array[item++] = Assert.expectEq(    "Infinity/Math.atan2(-0, 1)", -Infinity,   Infinity/Math.atan2(-0,1) );

    array[item++] = Assert.expectEq(    "Math.atan2(-0,    0)",        -0,             Math.atan2(-0,0)         );
    array[item++] = Assert.expectEq(    "Math.atan2(-0,    -0)",       -Math.PI,       Math.atan2(-0, -0)       );
    array[item++] = Assert.expectEq(    "Math.atan2(-0,    -1)",       -Math.PI,       Math.atan2(-0, -1)       );
    array[item++] = Assert.expectEq(    "Math.atan2(-1,    0)",        -Math.PI/2,     Math.atan2(-1, 0)        );
    array[item++] = Assert.expectEq(    "Math.atan2(-1,    -0)",       -Math.PI/2,     Math.atan2(-1, -0)       );
    array[item++] = Assert.expectEq(    "Math.atan2(1, Infinity)",  0,              Math.atan2(1, Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(    "Math.atan2(1,-Infinity)",  Math.PI,       Math.atan2(1, Number.NEGATIVE_INFINITY) );

    array[item++] = Assert.expectEq(    "Math.atan2(-1, Infinity)", -0,            Math.atan2(-1,Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(    "Infinity/Math.atan2(-1, Infinity)",   -Infinity,  Infinity/Math.atan2(-1,Infinity) );

    array[item++] = Assert.expectEq(    "Math.atan2(-1,-Infinity)", -Math.PI,       Math.atan2(-1,Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(    "Math.atan2(Infinity, 0)",  Math.PI/2,      Math.atan2(Number.POSITIVE_INFINITY, 0) );
    array[item++] = Assert.expectEq(    "Math.atan2(Infinity, 1)",  Math.PI/2,      Math.atan2(Number.POSITIVE_INFINITY, 1) );
    array[item++] = Assert.expectEq(    "Math.atan2(Infinity,-1)",  Math.PI/2,      Math.atan2(Number.POSITIVE_INFINITY,-1) );
    array[item++] = Assert.expectEq(    "Math.atan2(Infinity,-0)",  Math.PI/2,      Math.atan2(Number.POSITIVE_INFINITY,-0) );
    array[item++] = Assert.expectEq(    "Math.atan2(-Infinity, 0)", -Math.PI/2,     Math.atan2(Number.NEGATIVE_INFINITY, 0) );
    array[item++] = Assert.expectEq(    "Math.atan2(-Infinity,-0)", -Math.PI/2,     Math.atan2(Number.NEGATIVE_INFINITY,-0) );
    array[item++] = Assert.expectEq(    "Math.atan2(-Infinity, 1)", -Math.PI/2,     Math.atan2(Number.NEGATIVE_INFINITY, 1) );
    array[item++] = Assert.expectEq(    "Math.atan2(-Infinity, -1)", -Math.PI/2,    Math.atan2(Number.NEGATIVE_INFINITY,-1) );
    array[item++] = Assert.expectEq(    "Math.atan2(Infinity, Infinity)",   Math.PI/4,      Math.atan2(Number.POSITIVE_INFINITY, Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(    "Math.atan2(Infinity, -Infinity)",  3*Math.PI/4,    Math.atan2(Number.POSITIVE_INFINITY, Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(    "Math.atan2(-Infinity, Infinity)",  -Math.PI/4,     Math.atan2(Number.NEGATIVE_INFINITY, Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(    "Math.atan2(-Infinity, -Infinity)", -3*Math.PI/4,   Math.atan2(Number.NEGATIVE_INFINITY, Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(    "Math.atan2(-1, 1)",        -Math.PI/4,     Math.atan2( -1, 1) );

    return array;
}
