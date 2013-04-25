/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package foo{
    
    import bar.*;
    // will give a compiler error now
    //public var a;

    var c = new ClassB();
}

package foo{
    public var a = "This is var b";

    function func2(){ return "This is func2"; }

    interface IntB{
        function testInt();
    }

    public class ClassB {
    // public class ClassB implements IntB {s
        function test(){ return "This is test in ClassB"; }
        public function testInt() { return "This is testInt in ClassB"; }
    }
}

import foo.*;
import com.adobe.test.Assert;

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "116491";



Assert.expectEq( "access variable in second package from first", "This is var b", a );

              // displays results.
