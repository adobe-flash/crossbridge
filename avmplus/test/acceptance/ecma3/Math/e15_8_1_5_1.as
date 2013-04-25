/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;


//     var SECTION = "15.8.1.5-1";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Math.LOG10E";


    var testcases = getTestCases();



function getTestCases() {
    var array = new Array();
    var item = 0;
    var thisError:String = "no error"
    try{
        Math.LOG10E=0;
    }catch(e:ReferenceError){
        thisError=e.toString();
    }finally{
        array[item++]=Assert.expectEq("Trying to verify the ReadOnly attribute of Math.LOG10E","ReferenceError: Error #1074",Utils.referenceError(thisError));
    }
    array[item++] = Assert.expectEq(  "Math.LOG10E=0; Math.LOG10E", 0.4342944819032518,   Math.LOG10E );
    return ( array );
}
