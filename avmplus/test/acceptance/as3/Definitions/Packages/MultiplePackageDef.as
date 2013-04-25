/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// empty package definitions

package thisisthefirstempytpackagedefinition {}
package thisisthesecondempytpackagedefintition{}

// import statement
package testimportpackagesub{
    public var ti = 'PASSED';
}
package testimportpackage{
    import testimportpackagesub.*;
    public var testimport = ti;
}

// function defintion
package testfunctiondefinition{
    public function testfunction() {return 'this is a function in the package';}
}

// interface definition
package testinterfacedefinition{
    public interface blah {
        function testinterfacefunc();
    }
    public class testinterface {
        public function testinterfacefunc() { return "this is an interface definition";}
    }
}

// class definition
package testclassdefinition{
    public class testclass{
        public function testclassfunc() { return 'this is a class definition';}
    }
}

// namespace definition
package testnamespacedefinition{
    public class myClass {
        public namespace testnamespace;
        use namespace testnamespace;
        testnamespace var macaroni = "cheese";
        public function getCheese() {
            return testnamespace::macaroni;
        }
    }
}

package foo{
    public var a = 'PASSED';
}
package bar{
    public var b = 'PASSED';
}

import testimportpackage.*;
import testfunctiondefinition.*;
import testinterfacedefinition.*;
import testclassdefinition.*;
import testnamespacedefinition.*;
import foo.*;
import bar.*;
import com.adobe.test.Assert;

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";



var abc = new testclass();
var def = new testinterface();
var c = new myClass();
Assert.expectEq( "multiple package definitions import", "PASSED", testimport );
Assert.expectEq( "multiple package definitions function", "this is a function in the package", testfunction() );
Assert.expectEq( "multiple package definitions interface", "this is an interface definition", def.testinterfacefunc() );
Assert.expectEq( "multiple package definitions class", "this is a class definition", abc.testclassfunc());
Assert.expectEq( "multiple package definitions variable 1", "PASSED", a );
Assert.expectEq( "multiple package definitions variable 2", "PASSED", b );
Assert.expectEq( "multiple package definitions ns variable", "cheese", c.getCheese());

              // displays results.
