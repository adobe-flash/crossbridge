/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package Importtwonamespace {

    public namespace N1
    
}

package Packagnamespace {

    public namespace N2

}


// var SECTION = "Definitions";                    // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";               // Version of JavaScript or ECMA
// var TITLE   = "Import two namespaces from two different packages";          // Provide ECMA section title or a description
var BUGNUMBER = "";



import Importtwonamespace.N1
import Packagnamespace.N2

import com.adobe.test.Assert;
class PublicClass{
    N1 var x:Number = 4;
    N2 var x:Number = 6;
}

obj = new PublicClass();

Assert.expectEq("Namespace imported from package 1", 4, obj.N1::x);
Assert.expectEq("Namespace imported from package 2", 6, obj.N2::x);


/*===========================================================================*/


              // displays results.
