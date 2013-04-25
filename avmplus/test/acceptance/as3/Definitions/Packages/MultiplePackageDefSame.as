/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


package foo{
    public var a = "This is var a";

    public function func1(){ return "This is func1"; }

    interface IntA{
        function testInt();
    }

    public class ClassA implements IntA {
        
        v1 var ns = "This is namespace v1";
        v2 var ns = "This is namespace v2";
        public function test(){ return "This is test in ClassA"; }
        public function testInt() { return "This is testInt in ClassA"; }
        public function testNS() {return v1::ns; }
        public function testNS2() {return v2::ns; }
    }
    
    
    // Namespaces
    public namespace v1;
    public namespace v2;

}
package foo{
    public var b = "This is var b";

    public function func2(){ return "This is func2"; }

    interface IntB{
        function testInt();
    }

    public class ClassB implements IntB {
        public function test(){ return "This is test in ClassB"; }
        public function testInt() { return "This is testInt in ClassB"; }
    }

    // use the definitions from the first package foo
    public var c = a;
    public function func3(){ return func1(); }

    public var classC = new ClassA();

    public class ClassD implements IntA {
        public function testInt() { return "This is testInt from ClassD"; }
    }

    public class ClassN {
        use namespace v1;
        v1 var ns2 = "This is namespace v1";
        public function getNSVar() {
            return v1::ns2;
        }
    }
}

import foo.*;
import com.adobe.test.Assert;

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";



var classA = new ClassA();
var classB = new ClassB();
var classD = new ClassD();
var classN = new ClassN();
 
Assert.expectEq( "variable a in first definition of foo", "This is var a", a );
Assert.expectEq( "variable b in second definition of foo", "This is var b", b );
Assert.expectEq( "function func1 in first definition of foo", "This is func1", func1() );
Assert.expectEq( "function func2 in second definition of foo", "This is func2", func2() );
Assert.expectEq( "ClassA in first definition of foo", "This is test in ClassA", classA.test() );
Assert.expectEq( "ClassB in second definition of foo", "This is test in ClassB", classB.test() );
Assert.expectEq( "IntA in first definition of foo", "This is testInt in ClassA", classA.testInt() );
Assert.expectEq( "IntB in second definition of foo", "This is testInt in ClassB", classB.testInt() );
Assert.expectEq( "namespace v1 in first definition of foo", "This is namespace v1", classA.testNS() );
Assert.expectEq( "namespace v2 in second definition of foo", "This is namespace v2", classA.testNS2() );

// second foo package uses first foo package
Assert.expectEq( "access variable a from first package in second", "This is var a", c );
Assert.expectEq( "access function a from first package in second", "This is func1", func3() );
Assert.expectEq( "access classA from first package in second", "This is test in ClassA", classC.test() );
Assert.expectEq( "access IntA from first package in second", "This is testInt from ClassD", classD.testInt() );
Assert.expectEq( "access namespace from first package in second", "This is namespace v1", classN.getNSVar() );

              // displays results.
