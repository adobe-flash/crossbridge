/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "15.8.2.11";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.max(x, y)";
    var BUGNUMBER="76439";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(   "Math.max.length",              2,              Math.max.length );

    array[item++] = Assert.expectEq(   "Math.max()",                   -Infinity,      Math.max() );
    array[item++] = Assert.expectEq(   "Math.max(void 0, 1)",          Number.NaN,     Math.max( void 0, 1 ) );
    array[item++] = Assert.expectEq(   "Math.max(void 0, void 0)",     Number.NaN,     Math.max( void 0, void 0 ) );
    array[item++] = Assert.expectEq(   "Math.max(null, 1)",            1,              Math.max( null, 1 ) );
    array[item++] = Assert.expectEq(   "Math.max(-1, null)",           0,              Math.max( -1, null ) );
    array[item++] = Assert.expectEq(   "Math.max(true, false)",        1,              Math.max(true,false) );

    array[item++] = Assert.expectEq(   "Math.max('-99','99')",          99,             Math.max( "-99","99") );
                                                 
    array[item++] = Assert.expectEq(   "Math.max(NaN,Infinity)",Number.NaN,    Math.max(Number.NaN,Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.max(NaN, 0)",Number.NaN,     Math.max(Number.NaN, 0) );
   
    array[item++] = Assert.expectEq(   "Math.max('a string', 0)",      Number.NaN,     Math.max("a string", 0) );
    array[item++] = Assert.expectEq(   "Math.max(NaN, 1)",             Number.NaN,     Math.max(Number.NaN,1) );
    array[item++] = Assert.expectEq(   "Math.max('a string',Infinity)", Number.NaN,    Math.max("a string", Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.max(Infinity, NaN)",      Number.NaN,     Math.max( Number.POSITIVE_INFINITY, Number.NaN) );
    array[item++] = Assert.expectEq(   "Math.max(NaN, NaN)",           Number.NaN,     Math.max(Number.NaN, Number.NaN) );
    array[item++] = Assert.expectEq(   "Math.max(0,NaN)",              Number.NaN,     Math.max(0,Number.NaN) );
    array[item++] = Assert.expectEq(   "Math.max(1, NaN)",             Number.NaN,     Math.max(1, Number.NaN) );
    array[item++] = Assert.expectEq(   "Math.max(0,0)",                0,              Math.max(0,0) );
    array[item++] = Assert.expectEq(   "Math.max(0,-0)",               0,              Math.max(0,-0) );
    array[item++] = Assert.expectEq(   "Math.max(-0,0)",               0,              Math.max(-0,0) );
    array[item++] = Assert.expectEq(   "Math.max(-0,-0)",              -0,             Math.max(-0,-0) );
    array[item++] = Assert.expectEq(   "Infinity/Math.max(-0,-0)",              -Infinity,             Infinity/Math.max(-0,-0) );
    array[item++] = Assert.expectEq(   "Math.max(Infinity, Number.MAX_VALUE)", Number.POSITIVE_INFINITY,   Math.max(Number.POSITIVE_INFINITY, Number.MAX_VALUE) );
    array[item++] = Assert.expectEq(   "Math.max(Infinity, Infinity)",         Number.POSITIVE_INFINITY,   Math.max(Number.POSITIVE_INFINITY,Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.max(-Infinity,-Infinity)",        Number.NEGATIVE_INFINITY,   Math.max(Number.NEGATIVE_INFINITY,Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.max(1,.99999999999999)",          1,                          Math.max(1,.99999999999999) );
    array[item++] = Assert.expectEq(   "Math.max(-1,-.99999999999999)",        -.99999999999999,           Math.max(-1,-.99999999999999) );

    array[item++] = Assert.expectEq(   "Math.max(Infinity, Number.MAX_VALUE,Number.MIN_VALUE,Number.NEGATIVE_INFINITY)", Number.POSITIVE_INFINITY,   Math.max(Number.POSITIVE_INFINITY,Number.MAX_VALUE,Number.MIN_VALUE,Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(   "Math.max(Infinity, Number.MAX_VALUE,Number.MIN_VALUE,Number.NEGATIVE_INFINITY,0,-0,1,-1,'string',Number.NaN,null,void 0)", Number.NaN,   Math.max(Number.POSITIVE_INFINITY,Number.MAX_VALUE,Number.MIN_VALUE,Number.NEGATIVE_INFINITY,0,-0,1,-1,'string',Number.NaN,null,void 0) );
    array[item++] = Assert.expectEq(   "Math.max(Infinity, Number.MAX_VALUE,Number.MIN_VALUE,Number.NEGATIVE_INFINITY,0,-0,1,-1)", Infinity,   Math.max(Number.POSITIVE_INFINITY,Number.MAX_VALUE,Number.MIN_VALUE,Number.NEGATIVE_INFINITY,0,-0,1) );

    return ( array );
}

