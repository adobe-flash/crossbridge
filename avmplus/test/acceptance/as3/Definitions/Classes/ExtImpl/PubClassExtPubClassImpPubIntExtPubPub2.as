/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import PubClassExtPubClassImpPubIntExtPubPub2.*;
import com.adobe.test.Assert;
// var SECTION = "Definitions";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS3";        // Version of ECMAScript or ActionScript
// var TITLE   = "Public class implements public interface";       // Provide ECMA section title or a description
var BUGNUMBER = "";


/**
 * Calls to Assert.expectEq here. Assert.expectEq is a function that is defined
 * in shell.js and takes three arguments:
 * - a string representation of what is being tested
 * - the expected result
 * - the actual result
 *
 * For example, a test might look like this:
 *
 * var helloWorld = "Hello World";
 *
 * Assert.expectEq(
 * "var helloWorld = 'Hello World'",   // description of the test
 *  "Hello World",                     // expected result
 *  helloWorld );                      // actual result
 *
 */

///////////////////////////////////////////////////////////////
// add your tests here
  
var obj = new PublicSubClass();

//use namespace ns;
var PubInt:PublicInt=obj;

var PubInt2:PublicInt2=obj;

var PubInt3:PublicInt3=obj;
var dateObj:Date = new Date(0);
//print(obj.MyNegInteger());
/*print(PubInt.MyString());
print(PubInt5.MyString());
print(PubInt2.MyNegInteger());
print(PubInt6.MyNegInteger());
//print(PubInt3.MyString());
//print(PubInt7.MyString());
print(PubInt3.MyNegInteger());
print(PubInt7.MyNegInteger());
print(PubInt3.MyUnsignedInteger());
print(PubInt7.MyUnsignedInteger());
print(obj.MyString());
print(obj2.MyString());
//print(obj.MyNegInteger());
//print(obj2.MyNegInteger());
print(obj.MyUnsignedInteger());
print(obj2.MyUnsignedInteger());
print(obj.PublicInt2::MyString());
print(obj2.PublicInt2::MyString());
print(PubInt3.MyNegativeInteger());
print(PubInt7.MyNegativeInteger());
print(obj.PublicInt3::MyNegativeInteger());
print(obj2.PublicInt3::MyNegativeInteger());*/
//print(obj.MySuperBoolean());
//Public Class extends Public class implements a public interface which extends two //interfaces

Assert.expectEq("Calling a method in public namespace in the public interface implemented  by the subclass through the subclass","Hi!", PubInt.MyString());
Assert.expectEq("Calling a method in interface namespace in the public interface implemented  by the subclass through the subclass",-100, PubInt2.MyNegInteger());
Assert.expectEq("Calling a method in interface namespace in the public interface implemented  by the subclass through the subclass",-100, PubInt3.MyNegInteger());
Assert.expectEq("Calling a method in interface namespace in the public interface implemented  by the subclass through the subclass",100, PubInt3.MyUnsignedInteger());
Assert.expectEq("Calling a method in public namespace in the public interface implemented  by the subclass through the subclass","Hi!", obj.MyString());
Assert.expectEq("Calling a method in interface namespace in the public interface implemented  by the subclass through the subclass",100, obj.MyUnsignedInteger());
Assert.expectEq("Calling a method in public namespace in the public interface implemented  by the subclass through the subclass","Hi!", obj.PublicInt2::MyString());
Assert.expectEq("Calling a method in interface namespace in the public interface implemented  by the subclass through the subclass",-100000, PubInt3.MyNegativeInteger());
Assert.expectEq("Calling a method in interface namespace in the public interface implemented  by the subclass through the subclass",-100000, obj.PublicInt3::MyNegativeInteger());
Assert.expectEq("Calling a public method in superclass extended  by the subclass through the subclass",true, obj.MySuperBoolean());
Assert.expectEq("Calling an internal  method in superclass extended  by the subclass through the subclass",10, obj.RetMySuperNumber());
Assert.expectEq("Calling a public static method in superclass extended bythe subclass through the subclass",dateObj.toString(), PublicSuperClass.MySuperStaticDate()+"");

////////////////////////////////////////////////////////////////

              // displays results.
