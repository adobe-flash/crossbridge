/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
import com.adobe.test.Utils;

//     var SECTION = "15.8-2-n";
//     var VERSION = "ECMA_1";
//     var TITLE   = "The Math Object";


    var testcases = getTestCases();


function getTestCases() {
    var array = new Array();
    var item = 0;
    var thisError="no error";
    try{

        var MYMATH = new Math();
    }
    catch(e){
        thisError=e.toString();
    }
    finally{
        array[item++] = Assert.expectEq( 
                                 "MYMATH = new Math()",
                                 "TypeError: Error #1076",
                                 Utils.typeError(thisError) );
    }
    return ( array );
}
