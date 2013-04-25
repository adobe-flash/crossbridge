/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "15.8.2.4";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.atan()";
    var BUGNUMBER="77391";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    array[item++] = Assert.expectEq(    "Math.atan.length",        1,              Math.atan.length );
 /* var thisError="no error";
    try{
        Math.atan();
    }catch(e:Error){
        thisError=(e.toString()).substring(0,26);
    }finally{//print(thisError);
        array[item++] = Assert.expectEq(    "Math.atan()","ArgumentError: Error #1063",thisError);
    }
    array[item++] = Assert.expectEq(    "Math.atan()",             Number.NaN,     Math.atan() );*/
    array[item++] = Assert.expectEq(    "Math.atan(void 0)",       Number.NaN,     Math.atan(void 0) );
    array[item++] = Assert.expectEq(    "Math.atan(null)",         0,              Math.atan(null) );
    array[item++] = Assert.expectEq(    "Math.atan(NaN)",          Number.NaN,     Math.atan(Number.NaN) );

    array[item++] = Assert.expectEq(    "Math.atan('a string')",   Number.NaN,     Math.atan("a string") );
    array[item++] = Assert.expectEq(    "Math.atan('0')",          0,              Math.atan('0') );
    array[item++] = Assert.expectEq(    "Math.atan('1')",          Math.PI/4,      Math.atan('1') );
    array[item++] = Assert.expectEq(    "Math.atan('-1')",         -Math.PI/4,     Math.atan('-1') );
    array[item++] = Assert.expectEq(    "Math.atan('Infinity)",    Math.PI/2,      Math.atan('Infinity') );
    array[item++] = Assert.expectEq(    "Math.atan('-Infinity)",   -Math.PI/2,     Math.atan('-Infinity') );

    array[item++] = Assert.expectEq(    "Math.atan(0)",            0,              Math.atan(0)          );
    array[item++] = Assert.expectEq(    "Math.atan(-0)",           -0,             Math.atan(-0)         );
    array[item++] = Assert.expectEq(    "Infinity/Math.atan(-0)",  -Infinity,      Infinity/Math.atan(-0) );
    array[item++] = Assert.expectEq(    "Math.atan(Infinity)",     Math.PI/2,      Math.atan(Number.POSITIVE_INFINITY) );
    array[item++] = Assert.expectEq(    "Math.atan(-Infinity)",    -Math.PI/2,     Math.atan(Number.NEGATIVE_INFINITY) );
    array[item++] = Assert.expectEq(    "Math.atan(1)",            Math.PI/4,      Math.atan(1)          );
    array[item++] = Assert.expectEq(    "Math.atan(-1)",           -Math.PI/4,     Math.atan(-1)         );
    return array;
}
