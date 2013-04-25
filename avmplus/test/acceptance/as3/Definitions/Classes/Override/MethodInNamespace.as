/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import MethodInNamespace.*;
import com.adobe.test.Assert;


// var SECTION = "Definitions";                                // provide a document reference (ie, ECMA section)
// var VERSION = "AS 3.0";                                 // Version of JavaScript or ECMA
// var TITLE   = "Override a method defined in a namespace";       // Provide ECMA section title or a description
var BUGNUMBER = "";





var obj = new MethodInNamespace();

Assert.expectEq( "obj.i = 12, obj.i", 12, (obj.i = 12, obj.i) );
Assert.expectEq( "obj.ns1::getI()", 17, obj.ns1::getI() );
Assert.expectEq( "obj.ns2::getI()", 14, obj.ns2::getI() );





            // This function is for executing the test case and then
            // displaying the result on to the console or the LOG file.
