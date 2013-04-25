/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import DiscontinuousPackage.*;
import com.adobe.test.Assert;

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";



var a = new A();
var b = new B();
var c = new C();

Assert.expectEq("Function defined in [Packagename]/A.as", "A", a.whoAmI());
Assert.expectEq("Interface defined in [Packagename]/B.as", "B", b.whoAmI());
Assert.expectEq("Variable defined in [Packagename]/vars.as", "hello", s);
Assert.expectEq("Function defined in [Packagename]/vars.as", "You passed 5", publicFunc(5));
Assert.expectEq("Namespace variable defined in [Packagename]/C.as", true, c.returnNSVar());
Assert.expectEq("Namespace function defined in [Packagename]/C.as", "1,2,3", c.callNSFunc().toString());

class X{
Kitty var num:Number = 5;
Kitty function kittyFunc(s:String):String {
    return "You said hi";
}
}
var obj:X = new X()
Assert.expectEq("Variable, namespace defined in [Packagename]/vars.as", 5, obj.Kitty::num);
Assert.expectEq("Function, namespace defined in [Packagename]/vars.as", "You said hi", obj.Kitty::kittyFunc("hi"));

              // displays results.
