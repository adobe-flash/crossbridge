/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
 import GetSetAllowedNames.*;
import com.adobe.test.Assert;
 
 
//  var SECTION = "FunctionAccessors";
//  var VERSION = "AS3";
//  var TITLE   = "Function Accessors";
 var BUGNUMBER = "";
 



var TESTOBJ = new GetSetAllowedNames();

Assert.expectEq( "getter name _a1", "_a1", TESTOBJ._a1);
Assert.expectEq( "getter name _a1_", "_a1_", TESTOBJ._a1_);
Assert.expectEq( "getter name __a1__", "__a1__", TESTOBJ.__a1__);
Assert.expectEq( "getter name $a1", "$a1", TESTOBJ.$a1);
Assert.expectEq( "getter name a$1", "a$1", TESTOBJ.a$1);
Assert.expectEq( "getter name a1$", "a1$", TESTOBJ.a1$);
Assert.expectEq( "getter name A1", "A1", TESTOBJ.A1);
Assert.expectEq( "getter name cases", "cases", TESTOBJ.cases);
Assert.expectEq( "getter name Cases", "Cases", TESTOBJ.Cases);
Assert.expectEq( "getter name abcdefghijklmnopqrstuvwxyz0123456789$_", "all", TESTOBJ.abcdefghijklmnopqrstuvwxyz0123456789$_);
Assert.expectEq( "getter name get", "get", TESTOBJ.get);


Assert.expectEq( "setter name _a1", "new _a1", (TESTOBJ._a1 = "new _a1", TESTOBJ._a1));
Assert.expectEq( "setter name _a1_", "new _a1_", (TESTOBJ._a1_ = "new _a1_", TESTOBJ._a1_));
Assert.expectEq( "setter name __a1__", "new __a1__", (TESTOBJ.__a1__ = "new __a1__", TESTOBJ.__a1__));
Assert.expectEq( "setter name $a1", "new $a1", (TESTOBJ.$a1 = "new $a1", TESTOBJ.$a1));
Assert.expectEq( "setter name a$1", "new a$1", (TESTOBJ.a$1 = "new a$1", TESTOBJ.a$1));
Assert.expectEq( "setter name a1$", "new a1$", (TESTOBJ.a1$ = "new a1$", TESTOBJ.a1$));
Assert.expectEq( "setter name A1", "new A1", (TESTOBJ.A1 = "new A1", TESTOBJ.A1));
Assert.expectEq( "setter name cases", "new cases", (TESTOBJ.cases = "new cases", TESTOBJ.cases));
Assert.expectEq( "setter name Cases", "new Cases", (TESTOBJ.Cases = "new Cases", TESTOBJ.Cases));
Assert.expectEq( "setter name abcdefghijklmnopqrstuvwxyz0123456789$_", "new all", (TESTOBJ.abcdefghijklmnopqrstuvwxyz0123456789$_ = "new all", TESTOBJ.abcdefghijklmnopqrstuvwxyz0123456789$_));
Assert.expectEq( "setter name set", "new set", (TESTOBJ.set = "new set", TESTOBJ.set));

var f1 = function () {}
var f2 = function () { var i = 5; }
Assert.expectEq( "getter function keyword, different capitalization", "function Function() {}", TESTOBJ.FuncTion.toString());
Assert.expectEq( "setter function keyword, different capitalization", f2, (TESTOBJ.FuncTion = f2, TESTOBJ.FuncTion));

Assert.expectEq( "getter name same as constructor, different capitalization", "constructor, different case", TESTOBJ.getSetAllowedNames);
Assert.expectEq( "setter name same as constructor, different capitalization", "constructor, different case new", (TESTOBJ.getSetAllowedNames = "constructor, different case new", TESTOBJ.getSetAllowedNames));

Assert.expectEq( "getter class keyword, different capitalization", "class", TESTOBJ.Class);
Assert.expectEq( "setter class keyword, different capitalization", "class new", (TESTOBJ.Class = "class new", TESTOBJ.Class));

