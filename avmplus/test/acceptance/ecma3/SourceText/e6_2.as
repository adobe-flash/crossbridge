/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "6-1";
//     var VERSION = "ECMA_1";
//     var TITLE   = "Source Text";


    var testcases = getTestCases();
    
    
function getTestCases() {
    var array = new Array();
    var item = 0;

    // encoded quotes should not end a quote

    var s = 'PAS\u0022SED';
      
    array[item++]= Assert.expectEq(  
                                    "var s = 'PAS\\u0022SED'; s",
                                    "PAS\"SED",
                                    s);
    var s = "PAS\u0022SED";
    
    array[item++]= Assert.expectEq(  
                                    'var s = "PAS\\u0022SED"; s',
                                    "PAS\"SED",
                                     s );

    var s = 'PAS\u0027SED';
    
    array[item++]= Assert.expectEq(  
                                    "var s = 'PAS\\u0027SED'; s",
                                    "PAS\'SED",
                                    s);

    var s = "PAS\u0027SED";
    
    array[item++]= Assert.expectEq(  
                                    'var s = "PAS\\u0027SED"; s',
                                    "PAS\'SED",
                                     s );
    var s = "PAS\u0027SED";
    
    array[item++] = Assert.expectEq( 
                                    'var s = "PAS\\u0027SED"; s',
                                    "PAS\'SED",
                                    s )
   
    var s = "PAS\u0022SED";
    array[item++]= Assert.expectEq(  
                                    'var s = "PAS\\u0027SED"; s',
                                    "PAS\"SED",
                                    s );
    

    return array;
}
