/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import DefaultClass.*;
import com.adobe.test.Assert;


// var SECTION = "Definitions";                                // provide a document reference (ie, ECMA section)
// var VERSION = "AS 3.0";                                 // Version of JavaScript or ECMA
// var TITLE   = "Override public function in dynamic class extending default class";      // Provide ECMA section title or a description
var BUGNUMBER = "";



var CLASSDEFN = new DynExtDefaultOverRidePublic();

Assert.expectEq( "CLASSDEFN.setPubBoolean(true); CLASSDEFN.orSetCalled", true, ( CLASSDEFN.setPubBoolean(true), CLASSDEFN.orSetCalled ) );
Assert.expectEq( "CLASSDEFN.getPubBoolean();", true, CLASSDEFN.getPubBoolean() );
Assert.expectEq( "CLASSDEFN.orGetCalled", true, CLASSDEFN.orGetCalled );
CLASSDEFN.orGetCalled=false;
Assert.expectEq( "CLASSDEFN.setPubBoolean(false); CLASSDEFN.orSetCalled", true, ( CLASSDEFN.setPubBoolean(false), CLASSDEFN.orSetCalled ) );
Assert.expectEq( "CLASSDEFN.getPubBoolean();", false, CLASSDEFN.getPubBoolean() );
Assert.expectEq( "CLASSDEFN.orGetCalled", true, CLASSDEFN.orGetCalled );


            // This function is for executing the test case and then
            // displaying the result on to the console or the LOG file.
