/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "15.8.2.9";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.floor(x)";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(   "Math.floor.length",                    1,              Math.floor.length );
  /*thisError="no error";
    try{
        Math.floor();
    }catch(e:Error){
        thisError=(e.toString()).substring(0,26);
    }finally{//print(thisError);
        array[item++] = Assert.expectEq(    "Math.floor()","ArgumentError: Error #1063",thisError);
    }
    array[item++] = Assert.expectEq(   "Math.floor()",                         Number.NaN,     Math.floor() );*/
    array[item++] = Assert.expectEq(   "Math.floor(void 0)",                   Number.NaN,     Math.floor(void 0) );
    array[item++] = Assert.expectEq(   "Math.floor(null)",                     0,              Math.floor(null) );
    array[item++] = Assert.expectEq(   "Math.floor(true)",                     1,              Math.floor(true) );
    array[item++] = Assert.expectEq(   "Math.floor(false)",                    0,              Math.floor(false) );

    array[item++] = Assert.expectEq(   "Math.floor('1.1')",                    1,              Math.floor("1.1") );
    array[item++] = Assert.expectEq(   "Math.floor('-1.1')",                   -2,             Math.floor("-1.1") );
    array[item++] = Assert.expectEq(   "Math.floor('0.1')",                    0,              Math.floor("0.1") );
    array[item++] = Assert.expectEq(   "Math.floor('-0.1')",                   -1,             Math.floor("-0.1") );

    array[item++] = Assert.expectEq(   "Math.floor(NaN)",                      Number.NaN,     Math.floor(Number.NaN)  );
    array[item++] = Assert.expectEq(   "Math.floor(NaN)==-Math.ceil(-NaN)",    false,          Math.floor(Number.NaN) == -Math.ceil(-Number.NaN) );

    array[item++] = Assert.expectEq(   "Math.floor(0)",                        0,              Math.floor(0)           );
    array[item++] = Assert.expectEq(   "Math.floor(0)==-Math.ceil(-0)",        true,           Math.floor(0) == -Math.ceil(-0) );

    array[item++] = Assert.expectEq(   "Math.floor(-0)",           -0,                          Math.floor(-0)          );
    array[item++] = Assert.expectEq(   "Infinity/Math.floor(-0)",           -Infinity,         Infinity/Math.floor(-0)          );
    array[item++] = Assert.expectEq(   "Math.floor(-0)==-Math.ceil(0)",        true,           Math.floor(-0)== -Math.ceil(0) );

    array[item++] = Assert.expectEq(   "Math.floor(Infinity)",     Number.POSITIVE_INFINITY,   Math.floor(Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.floor(Infinity)==-Math.ceil(-Infinity)",  true,   Math.floor(Number.POSITIVE_INFINITY) == -Math.ceil(Number.NEGATIVE_INFINITY) );

    array[item++] = Assert.expectEq(   "Math.floor(-Infinity)",    Number.NEGATIVE_INFINITY,   Math.floor(Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.floor(-Infinity)==-Math.ceil(Infinity)",  true,   Math.floor(Number.NEGATIVE_INFINITY) == -Math.ceil(Number.POSITIVE_INFINITY) );

    array[item++] = Assert.expectEq(   "Math.floor(0.0000001)",    0,                          Math.floor(0.0000001) );
    array[item++] = Assert.expectEq(   "Math.floor(0.0000001)==-Math.ceil(0.0000001)", true,   Math.floor(0.0000001)==-Math.ceil(-0.0000001) );

    array[item++] = Assert.expectEq(   "Math.floor(-0.0000001)",   -1,                         Math.floor(-0.0000001) );
    array[item++] = Assert.expectEq(   "Math.floor(0.0000001)==-Math.ceil(0.0000001)",  true,  Math.floor(-0.0000001)==-Math.ceil(0.0000001) );
    array[item++] = Assert.expectEq(  "Math.floor(1)",          1,              Math.floor(1)   );
    array[item++] = Assert.expectEq(  "Math.floor(-1)",          -1,            Math.floor(-1)   );
    array[item++] = Assert.expectEq(  "Math.floor(-0.9)",        -1,            Math.floor(-0.9) );
    array[item++] = Assert.expectEq(  "Infinity/Math.floor(-0.9)",  -Infinity,  Infinity/Math.floor(-0.9) );
    array[item++] = Assert.expectEq(  "Math.floor(0.9 )",        0,             Math.floor( 0.9) );
    array[item++] = Assert.expectEq(  "Math.floor(-1.1)",        -2,            Math.floor( -1.1));
    array[item++] = Assert.expectEq(  "Math.floor( 1.1)",        1,             Math.floor(  1.1));

    array[item++] = Assert.expectEq(  "Math.floor(Infinity)",   -Math.ceil(-Infinity),    Math.floor(Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(  "Math.floor(-Infinity)",  -Math.ceil(Infinity),     Math.floor(Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(  "Math.floor(-Number.MIN_VALUE)",   -Math.ceil(Number.MIN_VALUE),     Math.floor(-Number.MIN_VALUE) );
    array[item++] = Assert.expectEq(  "Math.floor(1)",          -Math.ceil(-1),        Math.floor(1)   );
    array[item++] = Assert.expectEq(  "Math.floor(-1)",         -Math.ceil(1),         Math.floor(-1)   );
    array[item++] = Assert.expectEq(  "Math.floor(-0.9)",       -Math.ceil(0.9),       Math.floor(-0.9) );
    array[item++] = Assert.expectEq(  "Math.floor(0.9 )",       -Math.ceil(-0.9),      Math.floor( 0.9) );
    array[item++] = Assert.expectEq(  "Math.floor(-1.1)",       -Math.ceil(1.1),       Math.floor( -1.1));
    array[item++] = Assert.expectEq(  "Math.floor( 1.1)",       -Math.ceil(-1.1),      Math.floor(  1.1));
    array[item++] = Assert.expectEq(  "Math.floor( .012345)",       0,      Math.floor(  .012345));
    array[item++] = Assert.expectEq(  "Math.floor( .0012345)",       0,      Math.floor(.0012345));
    array[item++] = Assert.expectEq(  "Math.floor( .00012345)",      0,      Math.floor(.00012345));
    array[item++] = Assert.expectEq(  "Math.floor( .0000012345)",       0,      Math.floor(  .0000012345));
    array[item++] = Assert.expectEq(  "Math.floor( .00000012345)",       0,      Math.floor(  .00000012345));
    array[item++] = Assert.expectEq(  "Math.floor( 5.01)",       5,      Math.floor(  5.01));
    array[item++] = Assert.expectEq(  "Math.floor( 5.001)",       5,      Math.floor(  5.001));
    array[item++] = Assert.expectEq(  "Math.floor( 5.0001)",       5,      Math.floor(  5.0001));
    array[item++] = Assert.expectEq(  "Math.floor( 5.00001)",       5,      Math.floor(  5.00001));
    array[item++] = Assert.expectEq(  "Math.floor( 5.000001)",       5,      Math.floor(  5.000001));
    array[item++] = Assert.expectEq(  "Math.floor( 5.0000001)",       5,      Math.floor(5.0000001));

    return ( array );
}
