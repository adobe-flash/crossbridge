/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import DefaultClassPackage.*;
import com.adobe.test.Assert;


// var SECTION = "Definitions";                                // provide a document reference (ie, ECMA section)
// var VERSION = "AS 3.0";                                 // Version of JavaScript or ECMA
// var TITLE   = "Override internal function in public class extending default class";     // Provide ECMA section title or a description
var BUGNUMBER = "";



var CLASSDEFN = new PubExtDefaultOverRideInternal();
var booleanValue:Boolean = true;

Assert.expectEq( "CLASSDEFN.orSet", false, CLASSDEFN.orSet );
Assert.expectEq( "CLASSDEFN.orGet", false, CLASSDEFN.orGet );
Assert.expectEq( "CLASSDEFN.setGetBoolean(booleanValue)", booleanValue, CLASSDEFN.setGetBoolean(booleanValue));
Assert.expectEq( "CLASSDEFN.orSet", true, CLASSDEFN.orSet );
Assert.expectEq( "CLASSDEFN.orGet", true, CLASSDEFN.orGet );

            // This function is for executing the test case and then
            // displaying the result on to the console or the LOG file.
