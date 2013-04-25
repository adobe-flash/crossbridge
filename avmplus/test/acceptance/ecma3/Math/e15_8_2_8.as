/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;


//     var SECTION = "15.8.2.8";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.exp(x)";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(    "Math.exp.length",     1,              Math.exp.length );
    var thisError="no error";
    try{
        Math.exp();
    }catch(e:Error){
        thisError=(e.toString()).substring(0,26);
    }finally{//print(thisError);
        array[item++] = Assert.expectEq(    "Math.exp()","ArgumentError: Error #1063",thisError);
    }
  /*array[item++] = Assert.expectEq(    "Math.exp()",          Number.NaN,     Math.exp() );*/
    array[item++] = Assert.expectEq(    "Math.exp(null)",      1,              Math.exp(null) );
    array[item++] = Assert.expectEq(    "Math.exp(void 0)",    Number.NaN,     Math.exp(void 0) );
    array[item++] = Assert.expectEq(    "Math.exp(1)",          Math.E,         Math.exp(1) );
    
    array[item++] = Assert.expectEq(    "Math.exp(true)",      Math.E,         Math.exp(true) );
    array[item++] = Assert.expectEq(    "Math.exp(false)",     1,              Math.exp(false) );

    array[item++] = Assert.expectEq(    "Math.exp('1')",       Math.E,         Math.exp('1') );
    array[item++] = Assert.expectEq(    "Math.exp('0')",       1,              Math.exp('0') );

    array[item++] = Assert.expectEq(    "Math.exp(NaN)",       Number.NaN,      Math.exp(Number.NaN) );
    array[item++] = Assert.expectEq(    "Math.exp(0)",          1,              Math.exp(0)          );
    array[item++] = Assert.expectEq(    "Math.exp(-0)",         1,              Math.exp(-0)         );
    array[item++] = Assert.expectEq(    "Math.exp(Infinity)",   Number.POSITIVE_INFINITY,   Math.exp(Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(    "Math.exp(-Infinity)",  0,              Math.exp(Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(    "Math.exp(2)",  7.38905609893065,              Math.exp(2) );

    
    // test order number 15
    // [amemon 9/14/2006] This test case breaks on mac PPC and linux because of an OS precision error. Fixing...
    // array[item++] = Assert.expectEq(    "Math.exp(10)",  22026.465794806725,              Math.exp(10) );
    var maxCorrect15:Number = 22026.4657948068;
    var minCorrect15:Number = 22026.4657948067;
    
    array[item++] = Assert.expectEq(    "Math.exp(10)",  true,
                                    (Math.exp(10) > minCorrect15 && Math.exp(10) < maxCorrect15) );
    
    
    
    
    // test order number 16
    // [amemon 9/14/2006] This test case breaks on mac PPC and linux because of an OS precision error. Fixing...
    // array[item++] = Assert.expectEq(    "Math.exp(100)","2.68811714181616e+43",              Math.exp(100)+"" );
    array[item++] = Assert.expectEq(    "Math.exp(100)", true,
                                    (
                                        (String(Math.exp(100)+"").indexOf("2.6881171418161") != -1) &&
                                        (String(Math.exp(100)+"").indexOf("e+43") != -1)
                                    )
                                );
    
    
    
    
    
    
    array[item++] = Assert.expectEq(    "Math.exp(1000)",Infinity,              Math.exp(1000));
    array[item++] = Assert.expectEq(    "Math.exp(-1000)",0,              Math.exp(-1000));
    array[item++] = Assert.expectEq(    "Math.exp(100000)",Infinity,              Math.exp(100000));
    array[item++] = Assert.expectEq(    "Math.exp(Number.MAX_VALUE)",Infinity,              Math.exp(Number.MAX_VALUE));
    array[item++] = Assert.expectEq(    "Math.exp(Number.MIN_VALUE)",1,              Math.exp(Number.MIN_VALUE));
    
    return ( array );
}
