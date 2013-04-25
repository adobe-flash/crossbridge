/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "15.8.2.18";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.tan(x)";
    var EXCLUDE = "true";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(   "Math.tan.length",          1,              Math.tan.length );
  /*thisError="no error";
    try{
        Math.tan();
    }catch(e:Error){
        thisError=(e.toString()).substring(0,26);
    }finally{//print(thisError);
        array[item++] = Assert.expectEq(    "Math.tan()","ArgumentError: Error #1063",thisError);
    }
    array[item++] = Assert.expectEq(   "Math.tan()",               Number.NaN,      Math.tan() );*/
    array[item++] = Assert.expectEq(   "Math.tan(void 0)",         Number.NaN,     Math.tan(void 0));
    array[item++] = Assert.expectEq(   "Math.tan(null)",           0,              Math.tan(null) );
    array[item++] = Assert.expectEq(   "Math.tan(false)",          0,              Math.tan(false) );

    array[item++] = Assert.expectEq(   "Math.tan(NaN)",            Number.NaN,     Math.tan(Number.NaN) );
    array[item++] = Assert.expectEq(   "Math.tan(0)",              0,              Math.tan(0));
    array[item++] = Assert.expectEq(   "Math.tan(-0)",             -0,             Math.tan(-0));
    array[item++] = Assert.expectEq(   "Math.tan(Infinity)",       Number.NaN,     Math.tan(Number.POSITIVE_INFINITY));
    array[item++] = Assert.expectEq(   "Math.tan(-Infinity)",      Number.NaN,     Math.tan(Number.NEGATIVE_INFINITY));
    array[item++] = Assert.expectEq(   "Math.tan(Math.PI/4)",      0.9999999999999999,              Math.tan(Math.PI/4));
    array[item++] = Assert.expectEq(   "Math.tan(3*Math.PI/4)",    -1.0000000000000002,             Math.tan(3*Math.PI/4));
    array[item++] = Assert.expectEq(   "Math.tan(Math.PI)",        -1.2246063538223773e-16,             Math.tan(Math.PI));
    array[item++] = Assert.expectEq(   "Math.tan(5*Math.PI/4)",    0.9999999999999997,              Math.tan(5*Math.PI/4));
    array[item++] = Assert.expectEq(   "Math.tan(7*Math.PI/4)",    -1.0000000000000004,             Math.tan(7*Math.PI/4));
    array[item++] = Assert.expectEq(   "Infinity/Math.tan(-0)",    -Infinity,      Infinity/Math.tan(-0) );

/*
    Arctan (x) ~ PI/2 - 1/x   for large x.  For x = 1.6x10^16, 1/x is about the last binary digit of double precision PI/2.
    That is to say, perturbing PI/2 by this much is about the smallest rounding error possible.

    This suggests that the answer Christine is getting and a real Infinity are "adjacent" results from the tangent function.  I
    suspect that tan (PI/2 + one ulp) is a negative result about the same size as tan (PI/2) and that this pair are the closest
    results to infinity that the algorithm can deliver.

    In any case, my call is that the answer we're seeing is "right".  I suggest the test pass on any result this size or larger.
    = C =
*/
    array[item++] = Assert.expectEq(   "Math.tan(3*Math.PI/2) >= 5443000000000000",   true,   Math.tan(3*Math.PI/2) >= 5443000000000000 );
    array[item++] = Assert.expectEq(   "Math.tan(Math.PI/2) >= 5443000000000000",      true,   Math.tan(Math.PI/2) >= 5443000000000000 );

    return ( array );
}
