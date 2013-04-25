/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
 
 import GetSetPropertyMixup.*;
import com.adobe.test.Assert;
 
 
//  var SECTION = "FunctionAccessors";
//  var VERSION = "AS3";
//  var TITLE   = "Function Accessors";
 var BUGNUMBER = "";
 



w = new GetSetPropertyMixup();


Assert.expectEq("Get foo which gets _loo", "loo", w.foo);
w.foo = "foo new";

Assert.expectEq("Set foo which sets foo, check _foo for real" , "foo new", w.fooForReal);
Assert.expectEq("Set foo which sets foo, check _loo" , "loo", w.loo);

Assert.expectEq("Get poo", "poo", w.poo);

w.poo = "poo new";
Assert.expectEq("Set poo which sets _boo, check _boo", "poo new", w.boo);
Assert.expectEq("Set poo which sets _boo, check _poo", "poo", w.poo);

