/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Identifier package names
package dynamic {}
package each {}
package final {}
package get {}
// Not a valid testcase anymore - see jira bugs: CMP-1137, ASLSPEC-9
// package internal {}
package namespace {}
package override {}
package set {}
package static {}



// Internal Class Names as package names
package Array {}
package Boolean {}
package int {}
package Number {}
package String {}
package uint {}

import com.adobe.test.Assert;



// var SECTION = "Package Names";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS3";        // Version of ECMAScript or ActionScript
// var TITLE   = "Identifiers as valid package names";       // Provide ECMA section title or a description
var BUGNUMBER = "";



Assert.expectEq( "Identifiers used as package names, should always compile.", true, true);

////////////////////////////////////////////////////////////////

              // displays results.
