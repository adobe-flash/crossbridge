/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package Importclass{

public class PublicClass{
    var array:Array;
    public var pubArray:Array;

    public function setPubArray( a:Array ) { pubArray = a;}
}

}

// var SECTION = "Directives";                     // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";               // Version of JavaScript or ECMA
// var TITLE   = "Import a public class from a package";           // Provide ECMA section title or a description
var BUGNUMBER = "";




var arr = new Array(1, 2, 3);

import Importclass.PublicClass

import com.adobe.test.Assert;
var obj = new PublicClass();

Assert.expectEq( "Import a public class from a package and call setPubArray function", arr, (obj.setPubArray(arr), obj.pubArray) );

/*===========================================================================*/


              // displays results.
