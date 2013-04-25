/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
package {
    
    public var a = "boo";
    //private var b = "hoo"; Changing private variable to default variable since
        var b = "hoo";           //private attribute is allowed only on class property
    
    public class PublicClass {
        public function sayHi() {
            return "hi";
        }
        function sayBye() {
            return "bye";
        }
    }
    
    /*private class PrivateClass {       //Commenting out since private attribute is                                              //allowed only on class property
        function sayHi() {
            return "private hi";
        }
    }*/
        
    
    class NotAPublicClass {
        public function sayHi() {
            return "hi";
        }
    }
}

import com.adobe.test.Assert;
// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";



var helloWorld = "Hello World";
Assert.expectEq( "var helloWorld = 'Hello World'", "Hello World", helloWorld );

Assert.expectEq("Access public variable in package with no name", "boo", a);

var expected = 'error';
var actual = 'no error';

try {
    var internalVar = b;
} catch(e1) {
    actual = 'error';
}

Assert.expectEq("Access internal variable in package with no name", expected, actual);


Assert.expectEq("Access public class in package with no name", "hi", (c1 = new PublicClass(), c1.sayHi()));

var expected = 'error';
var actual = 'no error';

try {
    var c2 = new PublicClass();
    c2.sayBye();
} catch(e2) {
    actual = 'error';
}

Assert.expectEq("Try to access function not declared as public", expected, actual);

var expected = 'error';
var actual = 'no error';

/*try {
    var c3 = new PrivateClass();
    c3.sayHi();
} catch(e3) {
    actual = 'error';
}

Assert.expectEq("Try to access private variable", expected, actual);*/


var expected = 'error';
var actual = 'no error';

try {
    var c4 = new NotAPublicClass();
    c4.sayHi();
} catch(e4) {
    actual = 'error';
}

Assert.expectEq("Try to access class not declared as public", expected, actual);

              // displays results.
