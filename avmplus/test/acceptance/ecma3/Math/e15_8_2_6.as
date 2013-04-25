/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "15.8.2.6";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.ceil(x)";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(  "Math.ceil.length",      1,              Math.ceil.length );

    array[item++] = Assert.expectEq(  "Math.ceil(NaN)",         Number.NaN,     Math.ceil(Number.NaN)   );
    array[item++] = Assert.expectEq(  "Math.ceil(null)",        0,              Math.ceil(null) );

  /*thisError="no error";
    try{
        Math.ceil();
    }catch(e:Error){
        thisError=(e.toString()).substring(0,26);
    }finally{//print(thisError);
        array[item++] = Assert.expectEq(    "Math.ceil()","ArgumentError: Error #1063",thisError);
    }
    array[item++] = Assert.expectEq(  "Math.ceil()",            Number.NaN,     Math.ceil() );*/
    array[item++] = Assert.expectEq(  "Math.ceil(void 0)",      Number.NaN,     Math.ceil(void 0) );

    array[item++] = Assert.expectEq(  "Math.ceil('0')",            0,          Math.ceil('0')            );
    array[item++] = Assert.expectEq(  "Math.ceil('-0')",           -0,         Math.ceil('-0')           );
    array[item++] = Assert.expectEq(  "Infinity/Math.ceil('0')",   Infinity,   Infinity/Math.ceil('0'));
    array[item++] = Assert.expectEq(  "Infinity/Math.ceil('-0')",  -Infinity,  Infinity/Math.ceil('-0'));

    array[item++] = Assert.expectEq(  "Math.ceil(0)",           0,              Math.ceil(0)            );
    array[item++] = Assert.expectEq(  "Math.ceil(-0)",          -0,             Math.ceil(-0)           );
    array[item++] = Assert.expectEq(  "Infinity/Math.ceil(0)",     Infinity,   Infinity/Math.ceil(0));
    array[item++] = Assert.expectEq(  "Infinity/Math.ceil(-0)",    -Infinity,  Infinity/Math.ceil(-0));

    array[item++] = Assert.expectEq(  "Math.ceil(Infinity)",    Number.POSITIVE_INFINITY,   Math.ceil(Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(  "Math.ceil(-Infinity)",   Number.NEGATIVE_INFINITY,   Math.ceil(Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(  "Math.ceil(-Number.MIN_VALUE)",   -0,     Math.ceil(-Number.MIN_VALUE) );
    array[item++] = Assert.expectEq(  "Infinity/Math.ceil(-Number.MIN_VALUE)",   -Infinity,     Infinity/Math.ceil(-Number.MIN_VALUE) );
    array[item++] = Assert.expectEq(  "Math.ceil(1)",          1,              Math.ceil(1)   );
    array[item++] = Assert.expectEq(  "Math.ceil(-1)",          -1,            Math.ceil(-1)   );
    array[item++] = Assert.expectEq(  "Math.ceil(-0.9)",        -0,            Math.ceil(-0.9) );
    array[item++] = Assert.expectEq(  "Infinity/Math.ceil(-0.9)",  -Infinity,  Infinity/Math.ceil(-0.9) );
    array[item++] = Assert.expectEq(  "Math.ceil(0.9 )",        1,             Math.ceil( 0.9) );
    array[item++] = Assert.expectEq(  "Math.ceil(-1.1)",        -1,            Math.ceil( -1.1));
    array[item++] = Assert.expectEq(  "Math.ceil( 1.1)",        2,             Math.ceil(  1.1));

    array[item++] = Assert.expectEq(  "Math.ceil(Infinity)",   -Math.floor(-Infinity),    Math.ceil(Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(  "Math.ceil(-Infinity)",  -Math.floor(Infinity),     Math.ceil(Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(  "Math.ceil(-Number.MIN_VALUE)",   -Math.floor(Number.MIN_VALUE),     Math.ceil(-Number.MIN_VALUE) );
    array[item++] = Assert.expectEq(  "Math.ceil(1)",          -Math.floor(-1),        Math.ceil(1)   );
    array[item++] = Assert.expectEq(  "Math.ceil(-1)",         -Math.floor(1),         Math.ceil(-1)   );
    array[item++] = Assert.expectEq(  "Math.ceil(-0.9)",       -Math.floor(0.9),       Math.ceil(-0.9) );
    array[item++] = Assert.expectEq(  "Math.ceil(0.9 )",       -Math.floor(-0.9),      Math.ceil( 0.9) );
    array[item++] = Assert.expectEq(  "Math.ceil(-1.1)",       -Math.floor(1.1),       Math.ceil( -1.1));
    array[item++] = Assert.expectEq(  "Math.ceil( 1.1)",       -Math.floor(-1.1),      Math.ceil(  1.1));
    array[item++] = Assert.expectEq(  "Math.ceil( .012345)",       1,      Math.ceil(  .012345));
    array[item++] = Assert.expectEq(  "Math.ceil( .0012345)",       1,      Math.ceil(  .0012345));
    array[item++] = Assert.expectEq(  "Math.ceil( .00012345)",       1,      Math.ceil(  .00012345));
    array[item++] = Assert.expectEq(  "Math.ceil( .0000012345)",       1,      Math.ceil(  .0000012345));
    array[item++] = Assert.expectEq(  "Math.ceil( .00000012345)",       1,      Math.ceil(  .00000012345));
    array[item++] = Assert.expectEq(  "Math.ceil( 5.01)",       6,      Math.ceil(  5.01));
    array[item++] = Assert.expectEq(  "Math.ceil( 5.001)",       6,      Math.ceil(  5.001));
    array[item++] = Assert.expectEq(  "Math.ceil( 5.0001)",       6,      Math.ceil(  5.0001));
    array[item++] = Assert.expectEq(  "Math.ceil( 5.00001)",       6,      Math.ceil(  5.00001));
    array[item++] = Assert.expectEq(  "Math.ceil( 5.000001)",       6,      Math.ceil(  5.000001));
    array[item++] = Assert.expectEq(  "Math.ceil( 5.0000001)",       6,      Math.ceil(  5.0000001));
    return ( array );
}
