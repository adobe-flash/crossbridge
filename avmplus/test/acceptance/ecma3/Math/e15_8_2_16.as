/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "15.8.2.16";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.sin(x)";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(   "Math.sin.length",      1,              Math.sin.length );
  /*thisError="no error";
    try{
        Math.sin();
    }catch(e:Error){
        thisError=(e.toString()).substring(0,26);
    }finally{//print(thisError);
        array[item++] = Assert.expectEq(    "Math.sin()","ArgumentError: Error #1063",thisError);
    }
    array[item++] = Assert.expectEq(   "Math.sin()",           Number.NaN,     Math.sin() );*/
    array[item++] = Assert.expectEq(   "Math.sin(null)",       0,              Math.sin(null) );
    array[item++] = Assert.expectEq(   "Math.sin(void 0)",     Number.NaN,     Math.sin(void 0) );
    array[item++] = Assert.expectEq(   "Math.sin(false)",      0,              Math.sin(false) );
    array[item++] = Assert.expectEq(   "Math.sin('2.356194490192')",    0.7071067811867916,    Math.sin('2.356194490192') );

    array[item++] = Assert.expectEq(   "Math.sin(NaN)",        Number.NaN,     Math.sin(Number.NaN) );
    array[item++] = Assert.expectEq(   "Math.sin(0)",          0,              Math.sin(0) );
    array[item++] = Assert.expectEq(   "Math.sin(-0)",         -0,             Math.sin(-0));
    array[item++] = Assert.expectEq(   "Math.sin(Infinity)",   Number.NaN,     Math.sin(Number.POSITIVE_INFINITY));
    array[item++] = Assert.expectEq(   "Math.sin(-Infinity)",  Number.NaN,     Math.sin(Number.NEGATIVE_INFINITY));
    array[item++] = Assert.expectEq(   "Math.sin(0.7853981633974)",    0.7071067811865134,    Math.sin(0.7853981633974));
    array[item++] = Assert.expectEq(   "Math.sin(1.570796326795)",     1,                  Math.sin(1.570796326795));
    array[item++] = Assert.expectEq(   "Math.sin(2.356194490192)",     0.7071067811867916,    Math.sin(2.356194490192));
    array[item++] = Assert.expectEq(   "Math.sin(3.14159265359)",      -2.0682311115474694e-13,                  Math.sin(3.14159265359));

    return ( array );
}
