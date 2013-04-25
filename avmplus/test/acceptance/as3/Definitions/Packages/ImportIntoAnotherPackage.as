/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 

 package foo {
    
    public namespace Kitty;
        
    public interface IClickable {
                function whoAmI():String;
    }
    public class A {
        public function whoAmI():String {
            return "A";
        }
    
    }
    public var s:String = "hello";
    
    public function publicFunc(i:int):String {
        return "You passed " + i.toString();
    }
    
}

package ImportIntoAnotherPackage {
    import foo.*;
    
    public var newS:String = s;
    public function newFunc() {
        var f = publicFunc(5);
        return f;
    }
    public class newA {
        var a;
        public function whoAmI():String {
            a = new A();
            var s = a.whoAmI();
            return s;
        }
    }
    public class newB implements IClickable {
        public function whoAmI():String {
            return "B";
        }
        
    }
    use namespace Kitty;
        
    public class newC {
        Kitty var b:Boolean = true;
        public function returnNSVar():Boolean {
            return Kitty::b;
        }

        Kitty function returnArray():Array {
            return [1,2,3];
        }

        public function callNSFunc() {
            var k:Array = Kitty::returnArray();
            return k;
        }
    }
}


import ImportIntoAnotherPackage.*;
import com.adobe.test.Assert;

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";



var a = new newA();
var b = new newB();
var c = new newC();

Assert.expectEq("Class imported into another package", "A", a.whoAmI());
Assert.expectEq("Class implemented interface imported into another package", "B", b.whoAmI());
Assert.expectEq("Variable imported into another package", "hello", newS);


Assert.expectEq("Fully qualified reference to function", "You passed 5", newFunc());
Assert.expectEq("Namespace variable, ns imported into another package", true, c.returnNSVar());
Assert.expectEq("Namespace function, ns imported into antoher package", "1,2,3", c.callNSFunc().toString());

              // displays results.
