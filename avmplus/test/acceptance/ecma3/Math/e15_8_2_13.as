/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "15.8.2.13";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.pow(x, y)";
    var BUGNUMBER="77141";


    var testcases = getTestCases();
    
function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(   "Math.pow.length",                  2,                          Math.pow.length );
/*thisError="no error";
    try{
        Math.pow();
    }catch(e:Error){
        thisError=(e.toString()).substring(0,26);
    }finally{//print(thisError);
        array[item++] = Assert.expectEq(    "Math.pow()","ArgumentError: Error #1063",thisError);
    }
    array[item++] = Assert.expectEq(   "Math.pow()",                       Number.NaN,                 Math.pow() );*/
    array[item++] = Assert.expectEq(   "Math.pow(null, null)",             1,                          Math.pow(null,null) );
    array[item++] = Assert.expectEq(   "Math.pow(void 0, void 0)",         Number.NaN,                 Math.pow(void 0, void 0));
    array[item++] = Assert.expectEq(   "Math.pow(true, false)",            1,                          Math.pow(true, false) );
    array[item++] = Assert.expectEq(   "Math.pow(false,true)",             0,                          Math.pow(false,true) );
    array[item++] = Assert.expectEq(   "Math.pow('2','32')",               4294967296,                 Math.pow('2','32') );

    array[item++] = Assert.expectEq(   "Math.pow(1,NaN)",                  Number.NaN,                 Math.pow(1,Number.NaN) );
    array[item++] = Assert.expectEq(   "Math.pow(0,NaN)",                  Number.NaN,                 Math.pow(0,Number.NaN) );
    array[item++] = Assert.expectEq(   "Math.pow(NaN,0)",                  1,                          Math.pow(Number.NaN,0) );
    array[item++] = Assert.expectEq(   "Math.pow(NaN,-0)",                 1,                          Math.pow(Number.NaN,-0) );
    array[item++] = Assert.expectEq(   "Math.pow(NaN,1)",                  Number.NaN,                 Math.pow(Number.NaN, 1) );
    array[item++] = Assert.expectEq(   "Math.pow(NaN,.5)",                 Number.NaN,                 Math.pow(Number.NaN, .5) );
    array[item++] = Assert.expectEq(   "Math.pow(1.00000001, Infinity)",   Number.POSITIVE_INFINITY,   Math.pow(1.00000001, Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(1.00000001, -Infinity)",  0,                          Math.pow(1.00000001, Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(-1.00000001, Infinity)",  Number.POSITIVE_INFINITY,   Math.pow(-1.00000001,Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(-1.00000001, -Infinity)", 0,                          Math.pow(-1.00000001,Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(1, Infinity)",            Number.NaN,                 Math.pow(1, Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(1, -Infinity)",           Number.NaN,                 Math.pow(1, Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(-1, Infinity)",           Number.NaN,                 Math.pow(-1, Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(-1, -Infinity)",          Number.NaN,                 Math.pow(-1, Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(.0000000009, Infinity)",  0,                          Math.pow(.0000000009, Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(-.0000000009, Infinity)", 0,                          Math.pow(-.0000000009, Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(.0000000009, -Infinity)", Number.POSITIVE_INFINITY,   Math.pow(-.0000000009, Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(Infinity, .00000000001)", Number.POSITIVE_INFINITY,   Math.pow(Number.POSITIVE_INFINITY,.00000000001) );
    array[item++] = Assert.expectEq(   "Math.pow(Infinity, 1)",            Number.POSITIVE_INFINITY,   Math.pow(Number.POSITIVE_INFINITY, 1) );
    array[item++] = Assert.expectEq(   "Math.pow(Infinity, -.00000000001)",0,                          Math.pow(Number.POSITIVE_INFINITY, -.00000000001) );
    array[item++] = Assert.expectEq(   "Math.pow(Infinity, -1)",           0,                          Math.pow(Number.POSITIVE_INFINITY, -1) );
    array[item++] = Assert.expectEq(   "Math.pow(-Infinity, 1)",           Number.NEGATIVE_INFINITY,                 Math.pow(Number.NEGATIVE_INFINITY, 1) );
    array[item++] = Assert.expectEq(   "Math.pow(-Infinity, 333)",         Number.NEGATIVE_INFINITY,                 Math.pow(Number.NEGATIVE_INFINITY, 333) );
    array[item++] = Assert.expectEq(   "Math.pow(Infinity, 2)",            Number.POSITIVE_INFINITY,                 Math.pow(Number.POSITIVE_INFINITY, 2) );
    array[item++] = Assert.expectEq(   "Math.pow(-Infinity, 666)",         Number.POSITIVE_INFINITY,   Math.pow(Number.NEGATIVE_INFINITY, 666) );
    array[item++] = Assert.expectEq(   "Math.pow(-Infinity, 0.5)",         Number.POSITIVE_INFINITY,   Math.pow(Number.NEGATIVE_INFINITY, 0.5) );
    array[item++] = Assert.expectEq(   "Math.pow(-Infinity, Infinity)",    Number.POSITIVE_INFINITY,   Math.pow(Number.NEGATIVE_INFINITY, Number.POSITIVE_INFINITY) );

    array[item++] = Assert.expectEq(   "Math.pow(-Infinity, -1)",          0,                          Math.pow(Number.NEGATIVE_INFINITY, -1) );
    array[item++] = Assert.expectEq(   "Infinity/Math.pow(-Infinity, -1)", Number.NEGATIVE_INFINITY,   Infinity/Math.pow(Number.NEGATIVE_INFINITY, -1) );

    array[item++] = Assert.expectEq(   "Math.pow(-Infinity, -3)",          0,                          Math.pow(Number.NEGATIVE_INFINITY, -3) );
    array[item++] = Assert.expectEq(   "Math.pow(-Infinity, -2)",          0,                          Math.pow(Number.NEGATIVE_INFINITY, -2) );
    array[item++] = Assert.expectEq(   "Math.pow(-Infinity, -0.5)",        0,                          Math.pow(Number.NEGATIVE_INFINITY,-0.5) );
    array[item++] = Assert.expectEq(   "Math.pow(-Infinity, -Infinity)",   0,                          Math.pow(Number.NEGATIVE_INFINITY, Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(0, 1)",                   0,                          Math.pow(0,1) );
    array[item++] = Assert.expectEq(   "Math.pow(0, 0)",                   1,                          Math.pow(0,0) );
    array[item++] = Assert.expectEq(   "Math.pow(1, 0)",                   1,                          Math.pow(1,0) );
    array[item++] = Assert.expectEq(   "Math.pow(-1, 0)",                  1,                          Math.pow(-1,0) );
    array[item++] = Assert.expectEq(   "Math.pow(0, 0.5)",                 0,                          Math.pow(0,0.5) );
    array[item++] = Assert.expectEq(   "Math.pow(0, 1000)",                0,                          Math.pow(0,1000) );
    array[item++] = Assert.expectEq(   "Math.pow(0, Infinity)",            0,                          Math.pow(0, Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(0, -1)",                  Number.POSITIVE_INFINITY,   Math.pow(0, -1) );
    array[item++] = Assert.expectEq(   "Math.pow(0, -0.5)",                Number.POSITIVE_INFINITY,   Math.pow(0, -0.5) );
    array[item++] = Assert.expectEq(   "Math.pow(0, -1000)",               Number.POSITIVE_INFINITY,   Math.pow(0, -1000) );
    array[item++] = Assert.expectEq(   "Math.pow(0, -Infinity)",           Number.POSITIVE_INFINITY,   Math.pow(0, Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(-0, 1)",                  -0,                         Math.pow(-0, 1) );
    array[item++] = Assert.expectEq(   "Math.pow(-0, 3)",                  -0,                         Math.pow(-0,3) );

    array[item++] = Assert.expectEq(   "Infinity/Math.pow(-0, 1)",         -Infinity,                  Infinity/Math.pow(-0, 1) );
    array[item++] = Assert.expectEq(   "Infinity/Math.pow(-0, 3)",         -Infinity,                  Infinity/Math.pow(-0,3) );

    array[item++] = Assert.expectEq(   "Math.pow(-0, 2)",                  0,                          Math.pow(-0,2) );
    array[item++] = Assert.expectEq(   "Math.pow(-0, Infinity)",           0,                          Math.pow(-0, Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(-0, -1)",                 Number.NEGATIVE_INFINITY,   Math.pow(-0, -1) );
    array[item++] = Assert.expectEq(   "Math.pow(-0, -10001)",             Number.NEGATIVE_INFINITY,   Math.pow(-0, -10001) );
    array[item++] = Assert.expectEq(   "Math.pow(-0, -2)",                 Number.POSITIVE_INFINITY,   Math.pow(-0, -2) );
    array[item++] = Assert.expectEq(   "Math.pow(-0, 0.5)",                0,                          Math.pow(-0, 0.5) );
    array[item++] = Assert.expectEq(   "Math.pow(-0, Infinity)",           0,                          Math.pow(-0, Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.pow(-1, 0.5)",                Number.NaN,                 Math.pow(-1, 0.5) );
    array[item++] = Assert.expectEq(   "Math.pow(-1, NaN)",                Number.NaN,                 Math.pow(-1, Number.NaN) );
    array[item++] = Assert.expectEq(   "Math.pow(-1, -0.5)",               Number.NaN,                 Math.pow(-1, -0.5) );

    return ( array );
}
