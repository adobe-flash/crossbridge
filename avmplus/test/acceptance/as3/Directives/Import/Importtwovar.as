/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package Importtwovar {

    public var x = 3
    
}

package Packagvars {


    public var y = 4
}


// var SECTION = "Directives";                     // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";               // Version of JavaScript or ECMA
// var TITLE   = "Import two variablas from two different packages";           // Provide ECMA section title or a description
var BUGNUMBER = "";



import Importtwovar.x

import Packagvars.y
Assert.expectEq("Variable from package one", 3, x);
import com.adobe.test.Assert;
Assert.expectEq("Variable from package two", 4, y);


/*===========================================================================*/


              // displays results.
