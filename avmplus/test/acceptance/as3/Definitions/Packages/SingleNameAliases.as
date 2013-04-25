/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import SingleNameAliases.*;
import com.adobe.test.Assert;

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";



var classA = SingleNameAliases.A;
var classB = SingleNameAliases.B;

a = new classA();
b = new classB();

Assert.expectEq("Calling function on single name alias instance", "A", a.whoAmI());
Assert.expectEq("Calling function on single name alias instance", "B", b.whoAmI());

              // displays results.
