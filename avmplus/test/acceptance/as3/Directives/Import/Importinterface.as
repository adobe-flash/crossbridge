/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package Importinterface  {

    public interface Publicinterface {
              function setPubArray(a:Array);
     }
}



// var SECTION = "Directives";                     // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";               // Version of JavaScript or ECMA
// var TITLE   = "Import only a public interface ";        // Provide ECMA section title or a description
var BUGNUMBER = "";




var arr = new Array(1, 2, 3);




import Importinterface.Publicinterface;
class PublicClass implements Publicinterface{
    var array:Array;
    public var pubArray:Array;

    public function setPubArray( a:Array ) { pubArray = a;}
}

import mypackage.*;

import com.adobe.test.Assert;
var obj = new PublicClass();

Assert.expectEq( "Import only a public interface", arr, (obj.setPubArray(arr), obj.pubArray) );

/*===========================================================================*/


              // displays results.
