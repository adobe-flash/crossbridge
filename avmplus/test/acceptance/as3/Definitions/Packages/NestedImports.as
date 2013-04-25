/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 

 package NestedImports {
    

    public namespace Kitty;
        
        public interface IClickable {
            function whoAmI():String;
    }
    public class A {
        public function whoAmI():String {
            return "A";
        }
    
    }
    
    
}

package pA {
    import NestedImports.*;
    
    
}

package pB {
    import pA.*;
}

import pB.*;
import com.adobe.test.Assert;

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";



try {
    var a = new A();
    result = "no exception";
} catch (e1) {
    result = "exception";
}


Assert.expectEq("Nested import", "exception", result);

              // displays results.
