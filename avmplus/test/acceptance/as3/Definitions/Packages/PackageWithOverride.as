/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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



import P.*;
import com.adobe.test.Assert;

// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS 3.0";        // Version of ECMAScript or ActionScript
// var TITLE   = "PackageDefinition" //Proved ECMA section titile or a description
var BUGNUMBER = "";



b = new B();
Assert.expectEq("Overriding function within the same package", "override f", b.f());

              // displays results.
