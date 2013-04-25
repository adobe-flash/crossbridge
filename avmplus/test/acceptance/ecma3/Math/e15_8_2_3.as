/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "15.8.2.3";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.asin()";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;
 /* var thisError="no error";
    try{
        Math.asin();
    }catch(e:Error){
        thisError=(e.toString()).substring(0,26);
    }finally{//print(thisError);
        array[item++] = Assert.expectEq(    "Math.asin()","ArgumentError: Error #1063",thisError);
    }
    array[item++] = Assert.expectEq(  "Math.asin()",           Number.NaN,     Math.asin() );*/
    array[item++] = Assert.expectEq(  "Math.asin(void 0)",     Number.NaN,     Math.asin(void 0) );
    array[item++] = Assert.expectEq(  "Math.asin(null)",       0,              Math.asin(null) );
    array[item++] = Assert.expectEq(  "Math.asin(NaN)",        Number.NaN,     Math.asin(Number.NaN)   );

    array[item++] = Assert.expectEq(  "Math.asin('string')",   Number.NaN,     Math.asin("string")     );
    array[item++] = Assert.expectEq(  "Math.asin('0')",        0,              Math.asin("0") );
    array[item++] = Assert.expectEq(  "Math.asin('1')",        Math.PI/2,      Math.asin("1") );
    array[item++] = Assert.expectEq(  "Math.asin('-1')",       -Math.PI/2,     Math.asin("-1") );
    array[item++] = Assert.expectEq(  "Math.asin(Math.SQRT1_2+'')",    0.7853981633974484,  Math.asin(Math.SQRT1_2+'') );
    array[item++] = Assert.expectEq(  "Math.asin(-Math.SQRT1_2+'')",   -0.7853981633974484, Math.asin(-Math.SQRT1_2+'') );

    array[item++] = Assert.expectEq(  "Math.asin(1.000001)",    Number.NaN,    Math.asin(1.000001)     );
    array[item++] = Assert.expectEq(  "Math.asin(-1.000001)",   Number.NaN,    Math.asin(-1.000001)    );
    array[item++] = Assert.expectEq(  "Math.asin(0)",           0,             Math.asin(0)            );
    array[item++] = Assert.expectEq(  "Math.asin(-0)",          -0,            Math.asin(-0)           );

    array[item++] = Assert.expectEq(  "Infinity/Math.asin(-0)",    -Infinity,  Infinity/Math.asin(-0) );

    array[item++] = Assert.expectEq(  "Math.asin(1)",              Math.PI/2,  Math.asin(1)            );
    array[item++] = Assert.expectEq(  "Math.asin(-1)",             -Math.PI/2, Math.asin(-1)            );
    array[item++] = Assert.expectEq(  "Math.asin(Math.SQRT1_2))",  0.7853981633974484,  Math.asin(Math.SQRT1_2) );
    array[item++] = Assert.expectEq(  "Math.asin(-Math.SQRT1_2))", -0.7853981633974484, Math.asin(-Math.SQRT1_2));

    return ( array );
}
