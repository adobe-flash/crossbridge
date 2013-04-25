/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package P {
 
     
     public class C {
        namespace ns2 = "flowershop.com";
        ns2 var flower = "Gerbera Daisy";
        
        function defineNamespace() {
            namespace Flowershop = "flowershop.com";
            return Flowershop;
        }
        public function getFlower() {
            ns = defineNamespace();
            return ns::flower;
        }
     }
     
}

 
// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "use namespace from varying scopes";       // Provide ECMA section title or a description
var BUGNUMBER = "";





import P.*;

import com.adobe.test.Assert;
var c = new C();
Assert.expectEq("Namespace variable with namespace returned by a function", "Gerbera Daisy", c.getFlower());



              // displays results.
