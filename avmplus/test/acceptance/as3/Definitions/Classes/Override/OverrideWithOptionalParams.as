/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import OptionalParams.*;
import com.adobe.test.Assert;


// var SECTION = "Definitions";                                // provide a document reference (ie, ECMA section)
// var VERSION = "AS 3.0";                                 // Version of JavaScript or ECMA
// var TITLE   = "Override a method defined in a namespace";       // Provide ECMA section title or a description
var BUGNUMBER = "";




var base = new BaseClass();
var obj = new OverrideWithOptionalParams();

Assert.expectEq( "base.setInt(), base.i", 0, ( base.setInt(), base.i ) );
Assert.expectEq( "base.setInt(3), base.i", 3, ( base.setInt(3), base.i ) );
Assert.expectEq( "base.setString(), base.s", "default", ( base.setString(), base.s ) );
Assert.expectEq( "base.setString('here'), base.s", "here", ( base.setString("here"), base.s ) );
Assert.expectEq( "base.setAll(), base.i base.s", "0default", ( base.setAll(), (base.i + base.s) ) );
Assert.expectEq( "base.setAll(6, 'here'), base.i base.s", "6here", ( base.setAll(6, "here"), (base.i + base.s) ) );

Assert.expectEq( "obj.setInt(), obj.i", 1, ( obj.setInt(), obj.i ) );
Assert.expectEq( "obj.setInt(3), obj.i", 3, ( obj.setInt(3), obj.i ) );
Assert.expectEq( "obj.setString(), obj.s", "override", ( obj.setString(), obj.s ) );
Assert.expectEq( "obj.setString('here'), obj.s", "here", ( obj.setString("here"), obj.s ) );
Assert.expectEq( "obj.setAll(), obj.i obj.s", "1override", ( obj.setAll(), (obj.i + obj.s) ) );
Assert.expectEq( "obj.setAll(6, 'here'), obj.i obj.s", "6here", ( obj.setAll(6, "here"), (obj.i + obj.s) ) );





            // This function is for executing the test case and then
            // displaying the result on to the console or the LOG file.
