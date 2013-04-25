/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "15.8.2.1";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.abs()";
    var BUGNUMBER = "77391";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(    "Math.abs.length",             1,              Math.abs.length );
    var thisError="no error";
    try{
        Math.abs();
    }catch(e:Error){
        thisError=(e.toString()).substring(0,26);
    }finally{
        array[item++] = Assert.expectEq(    "Math.abs()","ArgumentError: Error #1063",thisError);
    }
  
    array[item++] = Assert.expectEq(    "Math.abs( void 0 )",          Number.NaN,     Math.abs(void 0) );
    array[item++] = Assert.expectEq(    "Math.abs( null )",            0,              Math.abs(null) );
    array[item++] = Assert.expectEq(    "Math.abs( true )",            1,              Math.abs(true) );
    array[item++] = Assert.expectEq(    "Math.abs( false )",           0,              Math.abs(false) );
    array[item++] = Assert.expectEq(    "Math.abs( string primitive)", Number.NaN,     Math.abs("a string primitive")                  );
    array[item++] = Assert.expectEq(    "Math.abs( string object )",   Number.NaN,     Math.abs(new String( 'a String object' ))       );
    array[item++] = Assert.expectEq(    "Math.abs( Number.NaN )",      Number.NaN,     Math.abs(Number.NaN) );

    array[item++] = Assert.expectEq(    "Math.abs(0)",                 0,              Math.abs( 0 ) );
    array[item++] = Assert.expectEq(    "Infinity/Math.abs(0)",        Infinity,       Infinity/Math.abs(0) );
    array[item++] = Assert.expectEq(    "Math.abs( -0 )",              0,              Math.abs(-0) );
    array[item++] = Assert.expectEq(    "Infinity/Math.abs(-0)",       Infinity,       Infinity/Math.abs(-0) );

    array[item++] = Assert.expectEq(    "Math.abs( -Infinity )",       Number.POSITIVE_INFINITY,   Math.abs( Number.NEGATIVE_INFINITY ) );
    array[item++] = Assert.expectEq(    "Math.abs( Infinity )",        Number.POSITIVE_INFINITY,   Math.abs( Number.POSITIVE_INFINITY ) );
    array[item++] = Assert.expectEq(    "Math.abs( - MAX_VALUE )",     Number.MAX_VALUE,           Math.abs( - Number.MAX_VALUE )       );
    array[item++] = Assert.expectEq(    "Math.abs( - MIN_VALUE )",     Number.MIN_VALUE,           Math.abs( -Number.MIN_VALUE )        );
    array[item++] = Assert.expectEq(    "Math.abs( MAX_VALUE )",       Number.MAX_VALUE,           Math.abs( Number.MAX_VALUE )       );
    array[item++] = Assert.expectEq(    "Math.abs( MIN_VALUE )",       Number.MIN_VALUE,           Math.abs( Number.MIN_VALUE )        );

    array[item++] = Assert.expectEq(    "Math.abs( -1 )",               1,                          Math.abs( -1 )                       );
    array[item++] = Assert.expectEq(    "Math.abs( new Number( -1 ) )", 1,                          Math.abs( new Number(-1) )           );
    array[item++] = Assert.expectEq(    "Math.abs( 1 )",                1,                          Math.abs( 1 ) );
    array[item++] = Assert.expectEq(    "Math.abs( Math.PI )",          Math.PI,                    Math.abs( Math.PI ) );
    array[item++] = Assert.expectEq(    "Math.abs( -Math.PI )",         Math.PI,                    Math.abs( -Math.PI ) );
    array[item++] = Assert.expectEq(    "Math.abs(-1/100000000)",       1/100000000,                Math.abs(-1/100000000) );
    array[item++] = Assert.expectEq(    "Math.abs(-Math.pow(2,32))",    Math.pow(2,32),             Math.abs(-Math.pow(2,32)) );
    array[item++] = Assert.expectEq(    "Math.abs(Math.pow(2,32))",     Math.pow(2,32),             Math.abs(Math.pow(2,32)) );
    array[item++] = Assert.expectEq(    "Math.abs( -0xfff )",           4095,                       Math.abs( -0xfff ) );
    array[item++] = Assert.expectEq(    "Math.abs( -0777 )",            777,                        Math.abs(-0777 ) );

    array[item++] = Assert.expectEq(    "Math.abs('-1e-1')",           0.1,            Math.abs('-1e-1') );
    array[item++] = Assert.expectEq(    "Math.abs('0xff')",            255,            Math.abs('0xff') );
    array[item++] = Assert.expectEq(    "Math.abs('077')",             77,             Math.abs('077') );
    array[item++] = Assert.expectEq(    "Math.abs( 'Infinity' )",      Infinity,       Math.abs('Infinity') );
    array[item++] = Assert.expectEq(    "Math.abs( '-Infinity' )",     Infinity,       Math.abs('-Infinity') );

    array[item++] = Assert.expectEq(     "Math.abs(1)", "1",     Math.abs(1)+"");

    array[item++] = Assert.expectEq(     "Math.abs(10)", "10",     Math.abs(10)+"");
  
    array[item++] = Assert.expectEq(     "Math.abs(100)", "100",     Math.abs(100)+"");
 
    array[item++] = Assert.expectEq(     "Math.abs(1000)", "1000",     Math.abs(1000)+"");
  
    array[item++] = Assert.expectEq(     "Math.abs(10000)", "10000",     Math.abs(10000)+"");
   
    array[item++] = Assert.expectEq(     "Math.abs(10000000000)", "10000000000",Math.abs(10000000000)+"");
   
    array[item++] = Assert.expectEq(     "Math.abs(10000000000000000000)", "10000000000000000000",Math.abs(10000000000000000000)+"");
   
    array[item++] = Assert.expectEq(     "Math.abs(100000000000000000000)", "100000000000000000000",Math.abs(100000000000000000000)+"" );
   
    array[item++] = Assert.expectEq(     "Math.abs(-12345 )", "12345",Math.abs(-12345)+"");
  
    array[item++] = Assert.expectEq(     "Math.abs(-1234567890)", "1234567890",Math.abs(-1234567890)+"");
   
    array[item++] = Assert.expectEq(     "Math.abs(-10 )", "10",Math.abs(-10)+"");
   
    array[item++] = Assert.expectEq(     "Math.abs(-100 )", "100",Math.abs(-100)+"");
    array[item++] = Assert.expectEq(     "Math.abs(-1000 )", "1000",Math.abs(-1000)+"");
   
    array[item++] = Assert.expectEq(     "Math.abs(-1000000000 )", "1000000000",Math.abs(-1000000000 )+"");
   
    array[item++] = Assert.expectEq(     "Math.abs(-1000000000000000)", "1000000000000000",Math.abs(-1000000000000000)+"");
   
    array[item++] = Assert.expectEq(     "Math.abs(-100000000000000000000)", "100000000000000000000",Math.abs(-100000000000000000000)+"");
    
   
    array[item++] = Assert.expectEq(     "Math.abs(-1000000000000000000000)", "1e+21",Math.abs(-1000000000000000000000)+"" );
    
    array[item++] = Assert.expectEq(     "Math.abs(1.0000001)", "1.0000001",Math.abs(1.0000001 )+"");
   
    array[item++] = Assert.expectEq(     "Math.abs(1000000000000000000000)", "1e+21",Math.abs(1000000000000000000000)+"");

   
    array[item++] = Assert.expectEq(     "Math.abs(1.2345)", "1.2345",Math.abs(1.2345)+"" );

   
    array[item++] = Assert.expectEq(     "Math.abs(1.234567890)", "1.23456789",Math.abs(1.234567890)+"");

   
    array[item++] = Assert.expectEq(     "Math.abs(.12345)", "0.12345",Math.abs(.12345)+"");

    array[item++] = Assert.expectEq(     "Math.abs(.012345)", "0.012345",Math.abs(.012345)+"" );
    
   
    array[item++] = Assert.expectEq(     "Math.abs(.00012345)", "0.00012345",Math.abs(.00012345)+"");
   
    array[item++] = Assert.expectEq(     "Math.abs(.000012345)", "0.000012345",Math.abs(.000012345)+"" );
   
    array[item++] = Assert.expectEq(     "Math.abs(.0000012345)", "0.0000012345",Math.abs(.0000012345)+"");
   
    array[item++] = Assert.expectEq(     "Math.abs(.00000012345)", "1.2345e-7",Math.abs(.00000012345)+"");

    var minInt:int = 0x80000000;
    array[item++] = Assert.expectEq(     "Math.abs(MIN_INT)", "2147483648",Math.abs(minInt)+"");

    return ( array );
}
