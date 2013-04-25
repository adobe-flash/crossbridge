/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "15.8.2.15";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.round(x)";
    var BUGNUMBER="331411";

    var EXCLUDE = "true";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(   "Math.round.length",   1,               Math.round.length );
  /*thisError="no error";
    try{
        Math.round();
    }catch(e:Error){
        thisError=(e.toString()).substring(0,26);
    }finally{//print(thisError);
        array[item++] = Assert.expectEq(    "Math.round()","ArgumentError: Error #1063",thisError);
    }
    array[item++] = Assert.expectEq(   "Math.round()",         Number.NaN,     Math.round() );*/
    array[item++] = Assert.expectEq(   "Math.round(null)",     0,              Math.round(0) );
    array[item++] = Assert.expectEq(   "Math.round(void 0)",   Number.NaN,     Math.round(void 0) );
    array[item++] = Assert.expectEq(   "Math.round(true)",     1,              Math.round(true) );
    array[item++] = Assert.expectEq(   "Math.round(false)",    0,              Math.round(false) );
    array[item++] = Assert.expectEq(   "Math.round('.99999')",  1,              Math.round('.99999') );
    array[item++] = Assert.expectEq(   "Math.round('12345e-2')",  123,          Math.round('12345e-2') );

    array[item++] = Assert.expectEq(   "Math.round(NaN)",      Number.NaN,     Math.round(Number.NaN) );
    array[item++] = Assert.expectEq(   "Math.round(0)",        0,              Math.round(0) );
    array[item++] = Assert.expectEq(   "Math.round(-0)",        0,            Math.round(-0));
    array[item++] = Assert.expectEq(   "Infinity/Math.round(-0)",  Infinity,  Infinity/Math.round(-0) );

    array[item++] = Assert.expectEq(   "Math.round(Infinity)", Number.POSITIVE_INFINITY,   Math.round(Number.POSITIVE_INFINITY));
    array[item++] = Assert.expectEq(   "Math.round(-Infinity)",Number.NEGATIVE_INFINITY,       Math.round(Number.NEGATIVE_INFINITY));
    array[item++] = Assert.expectEq(   "Math.round(0.49)",     0,              Math.round(0.49));
    array[item++] = Assert.expectEq(   "Math.round(0.5)",      1,              Math.round(0.5));
    array[item++] = Assert.expectEq(   "Math.round(0.51)",     1,              Math.round(0.51));

    array[item++] = Assert.expectEq(   "Math.round(-0.49)",    0,             Math.round(-0.49));
    array[item++] = Assert.expectEq(   "Math.round(-0.5)",     0,             Math.round(-0.5));
    array[item++] = Assert.expectEq(   "Infinity/Math.round(-0.49)",    Infinity,             Infinity/Math.round(-0.49));
    array[item++] = Assert.expectEq(   "Infinity/Math.round(-0.5)",     Infinity,             Infinity/Math.round(-0.5));

    array[item++] = Assert.expectEq(   "Math.round(-0.51)",    -1,             Math.round(-0.51));
    array[item++] = Assert.expectEq(   "Math.round(3.5)",      4,              Math.round(3.5));
    array[item++] = Assert.expectEq(   "Math.round(-3.5)",     -3,             Math.round(-3));

    return ( array );
}
