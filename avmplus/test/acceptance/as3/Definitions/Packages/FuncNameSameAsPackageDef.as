/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package A {}


package B {
    
}

class B {
    function tada() {
        return 'class A';
    }
            
    }

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";



import A;
import B;

import com.adobe.test.Assert;
function A() { return 'A'}

Assert.expectEq( "Function has same name as package", "A", A() );

b = new B();

Assert.expectEq( "Class has same name as package", "class A", b.tada());

              // displays results.
