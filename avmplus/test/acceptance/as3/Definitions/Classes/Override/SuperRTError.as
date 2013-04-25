/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import errors.*;
import com.adobe.test.Assert;
import com.adobe.test.Utils;


// var SECTION = "Definitions";                                // provide a document reference (ie, ECMA section)
// var VERSION = "AS 3.0";                                 // Version of JavaScript or ECMA
// var TITLE   = "Call a super method without specifying namespace";       // Provide ECMA section title or a description
var BUGNUMBER = "";





var obj = new SuperInNamespace();

var thisError = "no exception thrown";
try{
    trace( obj.ns1::foo() );
} catch (e1) {
    thisError = e1.toString();
} finally {
    Assert.expectEq( "Call a super method without specifying namespace",
                Utils.REFERENCEERROR+1070,
                Utils.referenceError( thisError) );
}






            // This function is for executing the test case and then
            // displaying the result on to the console or the LOG file.
