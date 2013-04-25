/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import MCPackage.*;
import com.adobe.test.Assert;


// var SECTION = "Method Closures";                                // provide a document reference (ie, ECMA section)
// var VERSION = "AS 3.0";                                 // Version of JavaScript or ECMA
// var TITLE   = "Extract an overridedn method from a class";      // Provide ECMA section title or a description
var BUGNUMBER = "";




var base:BaseClass = new BaseClass();
var obj:GetMCFromOverRideMethod = new GetMCFromOverRideMethod();

// *****************************
// Make sure that we can access
// the base and override method
// *****************************
Assert.expectEq( "*** Make sure that we can access the method ***", 1, 1 );
Assert.expectEq( "base.addition(1, 3)", 4, base.addition(1, 3) );
Assert.expectEq( "obj.addition(1, 3)", 3, obj.addition(1, 3) );


// *****************************
// Get the method closure and compare to original
// *****************************
var mc : Function = obj.addition;
Assert.expectEq( "*** Get the method closure and compare to original ***", 1, 1 );
Assert.expectEq( "obj.addition(2, 3) == mc(2, 3)", obj.addition(2, 3), mc(2, 3) );




            // This function is for executing the test case and then
            // displaying the result on to the console or the LOG file.
