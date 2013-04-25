/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "Directives";                                 // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";                           // Version of JavaScript or ECMA
// var TITLE   = "If the include statement can be used with front slash in the path";  // Provide ECMA section title or a description
var BUGNUMBER = "";




/*===========================================================================*/

include "./Include.txt"

Assert.expectEq( "Check if the Include directive works with front slashes in the directive content:", "incFunc", incFunc() );

/*===========================================================================*/

              // displays results.
