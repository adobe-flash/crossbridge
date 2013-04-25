/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
// Future Reserved Words as Package names
package abstract {}
package debugger {}
package enum {}
// package goto {} - is reserved word in asc2
//package interface {} - this is a reserved word in AS3 - moved to Errors/ReservedPackageNames.as
package native {}
package synchronized {}
package throws {}
package transient {}
package volatile {}

import com.adobe.test.Assert;
// var SECTION = "Package Names";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS3";        // Version of ECMAScript or ActionScript
// var TITLE   = "Future Reserved Words as valid package names";       // Provide ECMA section title or a description
var BUGNUMBER = "";



Assert.expectEq( "Future Reserved Words used as package names, should always compile.", true, true);

////////////////////////////////////////////////////////////////

              // displays results.

