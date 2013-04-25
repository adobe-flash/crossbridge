/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
package Discontinuous.Pack {

    public interface IClickable {
        function whoAmI():String;
    }
}

package Discontinuous.Pack {

    public class A {
        public function whoAmI():String {
            return "A";
        }
    
    }
}

package Discontinuous.Pack {
    use namespace Kitty;
    
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
}

package Discontinuous.Pack {

    public class B implements IClickable {
        public function whoAmI():String {
            return "B";
        }
    
    }

}

package Discontinuous.Pack {

    public var s:String = "hello";
    public function publicFunc(i:int):String {
        return "You passed " + i.toString();
    }
    
    public namespace Kitty;
    
}

import Discontinuous.Pack.*;
import com.adobe.test.Assert;

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";



var a = new A();
var b = new B();
var c = new C();

Assert.expectEq("Function defined in [Package.Name]/A.as", "A", a.whoAmI());
Assert.expectEq("Interface defined in [Package.Name]/B.as", "B", b.whoAmI());
Assert.expectEq("Variable defined in [Package.Name]/vars.as", "hello", s);
Assert.expectEq("Function defined in [Package.Name]/vars.as", "You passed 5", publicFunc(5));
Assert.expectEq("Namespace variable defined in [Package.Name]/C.as", true, c.returnNSVar());
Assert.expectEq("Namespace function defined in [Package.Name]/C.as", "1,2,3", c.callNSFunc().toString());

class X{
Kitty var num:Number = 5;
Kitty function kittyFunc(s:String):String {
    return "You said hi";
}
}

var obj = new X();
Assert.expectEq("Variable, namespace defined in [Package.Name]/vars.as", 5, obj.Kitty::num);
Assert.expectEq("Function, namespace defined in [Package.Name]/vars.as", "You said hi", obj.Kitty::kittyFunc("hi"));

              // displays results.
