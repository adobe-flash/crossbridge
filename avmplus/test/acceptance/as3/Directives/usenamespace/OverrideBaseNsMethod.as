/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package P{
import com.adobe.test.Assert;
namespace n1="foo";
public class Parent {
n1  function meth ():String {
return "parent meth";
}
}
public class Child extends Parent {
override n1  function meth ():String {
return "child meth";
}

}



 
// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "Public_Private_Internal_Protected_Namespace as function attribute";       // Provide ECMA section title or a description









use namespace n1;
var c:Child = new Child();



Assert.expectEq( "Access child's method overriding parent's method which are in namespace n1" , "child meth", c.meth());


              // displays results.
}
