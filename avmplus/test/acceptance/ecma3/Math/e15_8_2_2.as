/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
    
    
//     var SECTION = "15.8.2.2";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.acos()";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(   "Math.acos.length",         1,              Math.acos.length );

    array[item++] = Assert.expectEq(   "Math.acos(void 0)",        Number.NaN,     Math.acos(void 0) );

  /*  thisError="no error";
   try{
      Math.acos();
   }catch(e:Error){
       thisError=(e.toString()).substring(0,26);
   }finally{//print(thisError);
       array[item++] = Assert.expectEq(    "Math.acos()","ArgumentError: Error #1063",thisError);
    }*/
  /*array[item++] = Assert.expectEq(   "Math.acos()",              Number.NaN,     Math.acos() );*/
    array[item++] = Assert.expectEq(   "Math.acos(null)",          Math.PI/2,      Math.acos(null) );
    array[item++] = Assert.expectEq(   "Math.acos(NaN)",           Number.NaN,     Math.acos(Number.NaN) );

    array[item++] = Assert.expectEq(   "Math.acos(a string)",      Number.NaN,     Math.acos("a string") );
    array[item++] = Assert.expectEq(   "Math.acos('0')",           Math.PI/2,      Math.acos('0') );
    array[item++] = Assert.expectEq(   "Math.acos('1')",           0,              Math.acos('1') );
    array[item++] = Assert.expectEq(   "Math.acos('-1')",          Math.PI,        Math.acos('-1') );

    array[item++] = Assert.expectEq(   "Math.acos(1.00000001)",    Number.NaN,     Math.acos(1.00000001) );
    array[item++] = Assert.expectEq(   "Math.acos(11.00000001)",   Number.NaN,     Math.acos(-1.00000001) );
    array[item++] = Assert.expectEq(   "Math.acos(1)",             0,              Math.acos(1)          );
    array[item++] = Assert.expectEq(   "Math.acos(-1)",            Math.PI,        Math.acos(-1)         );
    array[item++] = Assert.expectEq(   "Math.acos(0)",             Math.PI/2,      Math.acos(0)          );
    array[item++] = Assert.expectEq(   "Math.acos(-0)",            Math.PI/2,      Math.acos(-0)         );
    array[item++] = Assert.expectEq(   "Math.acos(Math.SQRT1_2)",  Math.PI/4,      Math.acos(Math.SQRT1_2));
    array[item++] = Assert.expectEq(   "Math.acos(-Math.SQRT1_2)", Math.PI/4*3,    Math.acos(-Math.SQRT1_2));
    
    
    
    
    // test order number 16
    // [amemon 9/14/2006] This test case breaks on mac PPC and linux because of an OS precision error. Fixing...
    // Linux gives this: 0.008726646256686873
    // Mac probably gives the same as linux
    // Windows gives this: 0.008726646256688278
    // changing from the following to that which follows the following...
    // array[item++] = Assert.expectEq(   "Math.acos(0.9999619230642)",    0.008726646256688278,    Math.acos(0.9999619230642));
    
    var maxAcceptable16:Number = 0.00872664625669;
    var minAcceptable16:Number = 0.00872664625668;
    
    array[item++] = Assert.expectEq(   "Math.acos(0.9999619230642)", true,
                                    (
                                        Math.acos(0.9999619230642) > minAcceptable16 &&
                                        Math.acos(0.9999619230642) < maxAcceptable16
                                    )
                                );
    
    
    

    return ( array );
}
