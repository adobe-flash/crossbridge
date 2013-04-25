/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package Importvar {

public var pubArray:Array = new Array(1, 2, 3);


}



// var SECTION = "Definitions";                    // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";               // Version of JavaScript or ECMA
// var TITLE   = "Import only a public function";          // Provide ECMA section title or a description
var BUGNUMBER = "";



var arr:Array = new Array(1, 2, 3);


import Importvar.pubArray

import com.adobe.test.Assert;
Assert.expectEq( "PubArray",arr.toString(), pubArray.toString());

/*===========================================================================*/


              // displays results.
