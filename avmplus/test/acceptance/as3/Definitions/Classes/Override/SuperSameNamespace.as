/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import SuperSameNamespace.*;
import com.adobe.test.Assert;


// var SECTION = "Definitions";                                // provide a document reference (ie, ECMA section)
// var VERSION = "AS 3.0";                                 // Version of JavaScript or ECMA
// var TITLE   = "Call super from the same namespace override method";     // Provide ECMA section title or a description
var BUGNUMBER = "";





var obj = new SuperSameNamespace();
use namespace ns1;

Assert.expectEq( "obj.echo()", "echo() from BaseClass override", obj.echo() );





            // This function is for executing the test case and then
            // displaying the result on to the console or the LOG file.
