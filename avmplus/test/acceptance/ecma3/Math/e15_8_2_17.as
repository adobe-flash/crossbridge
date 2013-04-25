/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "15.8.2.17";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.sqrt(x)";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(   "Math.sqrt.length",     1,              Math.sqrt.length );
  /*thisError="no error";
    try{
        Math.sqrt();
    }catch(e:Error){
        thisError=(e.toString()).substring(0,26);
    }finally{//print(thisError);
        array[item++] = Assert.expectEq(    "Math.sqrt()","ArgumentError: Error #1063",thisError);
    }
    array[item++] = Assert.expectEq(   "Math.sqrt()",          Number.NaN,     Math.sqrt() );*/
    array[item++] = Assert.expectEq(   "Math.sqrt(void 0)",    Number.NaN,     Math.sqrt(void 0) );
    array[item++] = Assert.expectEq(   "Math.sqrt(null)",      0,              Math.sqrt(null) );
    array[item++] = Assert.expectEq(   "Math.sqrt(true)",      1,              Math.sqrt(1) );
    array[item++] = Assert.expectEq(   "Math.sqrt(false)",     0,              Math.sqrt(false) );
    array[item++] = Assert.expectEq(   "Math.sqrt('225')",     15,             Math.sqrt('225') );

    array[item++] = Assert.expectEq(   "Math.sqrt(NaN)",       Number.NaN,     Math.sqrt(Number.NaN) );
    array[item++] = Assert.expectEq(   "Math.sqrt(-Infinity)", Number.NaN,     Math.sqrt(Number.NEGATIVE_INFINITY));
    array[item++] = Assert.expectEq(   "Math.sqrt(-1)",        Number.NaN,     Math.sqrt(-1));
    array[item++] = Assert.expectEq(   "Math.sqrt(-0.5)",      Number.NaN,     Math.sqrt(-0.5));
    array[item++] = Assert.expectEq(   "Math.sqrt(0)",         0,              Math.sqrt(0));
    array[item++] = Assert.expectEq(   "Math.sqrt(-0)",        -0,             Math.sqrt(-0));
    array[item++] = Assert.expectEq(   "Infinity/Math.sqrt(-0)",   -Infinity,  Infinity/Math.sqrt(-0) );
    array[item++] = Assert.expectEq(   "Math.sqrt(Infinity)",  Number.POSITIVE_INFINITY,   Math.sqrt(Number.POSITIVE_INFINITY));
    array[item++] = Assert.expectEq(   "Math.sqrt(1)",         1,              Math.sqrt(1));
    array[item++] = Assert.expectEq(   "Math.sqrt(2)",         Math.SQRT2,     Math.sqrt(2));
    array[item++] = Assert.expectEq(   "Math.sqrt(0.5)",       Math.SQRT1_2,   Math.sqrt(0.5));
    array[item++] = Assert.expectEq(   "Math.sqrt(4)",         2,              Math.sqrt(4));
    array[item++] = Assert.expectEq(   "Math.sqrt(9)",         3,              Math.sqrt(9));
    array[item++] = Assert.expectEq(   "Math.sqrt(16)",        4,              Math.sqrt(16));
    array[item++] = Assert.expectEq(   "Math.sqrt(25)",        5,              Math.sqrt(25));
    array[item++] = Assert.expectEq(   "Math.sqrt(36)",        6,              Math.sqrt(36));
    array[item++] = Assert.expectEq(   "Math.sqrt(49)",        7,              Math.sqrt(49));
    array[item++] = Assert.expectEq(   "Math.sqrt(64)",        8,              Math.sqrt(64));
    array[item++] = Assert.expectEq(   "Math.sqrt(256)",       16,             Math.sqrt(256));
    array[item++] = Assert.expectEq(   "Math.sqrt(10000)",     100,            Math.sqrt(10000));
    array[item++] = Assert.expectEq(   "Math.sqrt(65536)",     256,            Math.sqrt(65536));
    array[item++] = Assert.expectEq(   "Math.sqrt(0.09)",      0.3,            Math.sqrt(0.09));
    array[item++] = Assert.expectEq(   "Math.sqrt(0.01)",      0.1,            Math.sqrt(0.01));
    array[item++] = Assert.expectEq(   "Math.sqrt(0.00000001)",0.0001,         Math.sqrt(0.00000001));

    return ( array );
}
