/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import DefaultClass.*;
import com.adobe.test.Assert;


// var SECTION = "Definitions";                                // provide a document reference (ie, ECMA section)
// var VERSION = "AS 3.0";                                 // Version of JavaScript or ECMA
// var TITLE   = "Override public function in default class extending default class";      // Provide ECMA section title or a description
var BUGNUMBER = "";



var CLASSDEFN = new ExtDefaultOverRidePublic();
var myString = "teststring";

Assert.expectEq( "CLASSDEFN.setPubString(myString); CLASSDEFN.orSetString", "PASSED", (CLASSDEFN.setPubString(myString), CLASSDEFN.orSetString ) );
Assert.expectEq( "CLASSDEFN.getPubString()" , "override_teststring_override", CLASSDEFN.getPubString() );
Assert.expectEq( "CLASSDEFN.orGetString" , "PASSED", CLASSDEFN.orGetString );


Assert.expectEq( "CLASSDEFN.setPubString(null); CLASSDEFN.getPubString()", "override_null_override", (CLASSDEFN.setPubString(null), CLASSDEFN.getPubString()) );
Assert.expectEq( "CLASSDEFN.setPubString(undefined); CLASSDEFN.getPubString()", "override_null_override", (CLASSDEFN.setPubString(undefined), CLASSDEFN.getPubString()) );


            // This function is for executing the test case and then
            // displaying the result on to the console or the LOG file.
