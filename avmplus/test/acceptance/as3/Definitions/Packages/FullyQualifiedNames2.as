/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 

 package FullyQualifiedNames2 {
    

    public namespace Kitty;
        
        public interface IClickable {
                function whoAmI():String;
    }
    public class A {
        public function whoAmI():String {
            return "A";
        }
    
    }
    
    use namespace FullyQualifiedNames2.Kitty;
    
    public class C {
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

    public class B implements FullyQualifiedNames2.IClickable {
        public function whoAmI():String {
            return "B";
        }
    
    }


    public var s:String = "hello";
    var p:String = "private";
    public function publicFunc(i:int):String {
        return "You passed " + i.toString();
    }
    
    
    
}

import FullyQualifiedNames.*;
import com.adobe.test.Assert;

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "138845";



var a = new FullyQualifiedNames2.A();
var b = new FullyQualifiedNames2.B();
var c = new FullyQualifiedNames2.C();

Assert.expectEq("Function in fully qualified class", "A", a.whoAmI());
Assert.expectEq("Interface with fully qualified path", "B", b.whoAmI());
Assert.expectEq("Fully qualified reference to variable", "hello", FullyQualifiedNames2.s);

try {
    p = FullyQualifiedNames2.p;
    result = "no exception";
} catch(e) {
    result = "exception";
}
Assert.expectEq("Fully qualified reference to internal variable", "exception", result);
Assert.expectEq("Fully qualified reference to function", "You passed 5", FullyQualifiedNames2.publicFunc(5));
Assert.expectEq("Namespace variable, fully qualified path", true, c.returnNSVar());
Assert.expectEq("Namespace function, fully qualified path", "1,2,3", c.callNSFunc().toString());

              // displays results.
