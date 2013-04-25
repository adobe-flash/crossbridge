/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package Importfunc {
   
    // *******************
    // public variables
    // *******************

    var array:Array;
    public var pubArray:Array;

    // *******************
    // public methods
    // *******************

    public function setPubArray( a:Array ):Array { pubArray = a;return pubArray;}
   
}



// var SECTION = "Directives";                     // provide a document reference (ie, ECMA section)
// var VERSION = "ActionScript 3.0";               // Version of JavaScript or ECMA
// var TITLE   = "Import only a public function";          // Provide ECMA section title or a description
var BUGNUMBER = "";




var arr = new Array(1, 2, 3);

import Importfunc.array;
import Importfunc.setPubArray;
import Importfunc.pubArray;

import com.adobe.test.Assert;
import com.adobe.test.Utils;


Assert.expectEq( "setPubArray(arr)", arr, setPubArray(arr) );
Assert.expectEq( "pubArray", arr, pubArray );
thisError="no error";
try{
    array
   }catch(e:ReferenceError){
        thisError=e.toString();
    }finally{
        Assert.expectEq( "Trying to import a not a public Array", "ReferenceError: Error #1065", Utils.referenceError(thisError) );
   }
/*===========================================================================*/


              // displays results.
