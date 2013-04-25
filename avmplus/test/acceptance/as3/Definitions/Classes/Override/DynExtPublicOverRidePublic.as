/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import PublicClass.*;
import com.adobe.test.Assert;


// var SECTION = "Definitions";                                // provide a document reference (ie, ECMA section)
// var VERSION = "AS 3.0";                                 // Version of JavaScript or ECMA
// var TITLE   = "Override public function in dynamic class extending public class";       // Provide ECMA section title or a description
var BUGNUMBER = "";



var CLASSDEFN = new DynExtPublicOverRidePublic();
var booleanValue:Boolean = true;

Assert.expectEq( "CLASSDEFN.callSuper(booleanValue)", booleanValue, CLASSDEFN.callSuper(booleanValue) );
Assert.expectEq( "CLASSDEFN.orSet", false, CLASSDEFN.orSet );
Assert.expectEq( "CLASSDEFN.orGet", false, CLASSDEFN.orGet );
Assert.expectEq( "CLASSDEFN.setPubBoolean( booleanValue ); CLASSDEFN.getPubBoolean()", booleanValue, ( CLASSDEFN.setPubBoolean( booleanValue ), CLASSDEFN.getPubBoolean() ) );
Assert.expectEq( "CLASSDEFN.orSet", true, CLASSDEFN.orSet );
Assert.expectEq( "CLASSDEFN.orGet", true, CLASSDEFN.orGet );


            // This function is for executing the test case and then
            // displaying the result on to the console or the LOG file.
