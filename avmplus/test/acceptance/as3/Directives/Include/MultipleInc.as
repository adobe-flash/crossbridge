/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "Directives";             // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";       // Version of JavaScript or ECMA
// var TITLE   = "Using multiple includes";    // Provide ECMA section title or a description
var BUGNUMBER = "";




/*===========================================================================*/

include "Include.txt"
include "Include1.txt"

include "./MultipleInc/Include2.txt"

Assert.expectEq( "Check if a multiple files can be included:", "incFunc", incFunc() );
Assert.expectEq( "Check if a multiple files can be included:", "incFunc1", incFunc1() );
Assert.expectEq( "Check if a multiple files can be included:", "incFunc2", incFunc2() );

/*===========================================================================*/

              // displays results.
