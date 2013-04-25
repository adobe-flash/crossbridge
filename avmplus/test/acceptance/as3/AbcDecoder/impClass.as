/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

 /*
  * This file is imports a class defined in a '.abc' file
  * Extend the class defined in varsDef.abc
 */

// var SECTION = " ";
// var VERSION = "AS3";
// var TITLE   = "import a class defined in .abc file";


obj = new testClass();

Assert.expectEq( "invoke the property 'testClassVar1' from the imported file", "Inside testClass", obj.testClassVar1);
Assert.expectEq( "invoke the method 'func1()' from the imported file", "Inside func1()", obj.func1());
Assert.expectEq( "invoke the method 'func2()' from the imported file", "Inside func2()", obj.func2());

