/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package foo {

    public namespace fletch;
    
    public class A {

        fletch var lives = 'PASSED';
        public function getFletch() {
            return fletch::lives;
        }
    }
    
    public function fletchFunc(){ return 'PASSED'; }
    
    public class fletch2{
        public function lives(){return 'PASSED';}
    }
    
    public interface fletch3 {
        function lives();
    }

    public class erwin implements fletch3 {
        public function lives(){return 'PASSED';}
    }
}

package test{
    public var a = 'PASSED';
}

package foo2{
    import test.*;
    public var b = a;
}

package P
{
    class A
    {
        public function f() { return "original f"}
    }
}


package P
{
    public class B extends A
    {
        override public function f() { return "override f"}
    }
}


import foo2.*;
import P.*;
import foo.*;
import com.adobe.test.Assert;
use namespace fletch;

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";



Assert.expectEq( "namespace inside of package", "PASSED", (a = new A(), a.getFletch()));
  
Assert.expectEq( "import inside of package", "PASSED",b );
Assert.expectEq( "function inside of package", "PASSED",fletchFunc() );

var FLETCHCLASS = new fletch2();
Assert.expectEq( "class inside of package", "PASSED",FLETCHCLASS.lives() );

var FLETCHCLASS2 = new erwin();
Assert.expectEq( "interface inside of package", "PASSED",FLETCHCLASS2.lives() );

b = new B();
Assert.expectEq("Overriding function within the same package", "override f", b.f());

              // displays results.
