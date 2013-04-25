/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;
//     var SECTION = "15.8-3-n";
//     var VERSION = "ECMA_1";
//     var TITLE   = "The Math Object";


    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;
    thisError="no error";
    var MYMATH;
    try{
        MYMATH = Math();
    }catch(e:TypeError){
        thisError=e.toString();
    }finally{
    array[item++] = Assert.expectEq( 
                                 "MYMATH = Math()",
                                 "TypeError: Error #1075",
                                 Utils.typeError(thisError));
    }

    return ( array );
}
