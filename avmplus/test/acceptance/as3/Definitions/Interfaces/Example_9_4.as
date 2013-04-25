/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// var SECTION = "Definitions";       // provide a document reference (ie, ECMA section)
// var VERSION = "AS3";  // Version of JavaScript or ECMA
// var TITLE   = "Interface Definition";       // Provide ECMA section title or a description
var BUGNUMBER = "";


//-----------------------------------------------------------------------------

import Example_9_4.*;

import com.adobe.test.Assert;
var eg = new ExampleTest();
Assert.expectEq("unique references via interface parent types", "a.{T,U}::f(),a.U::g()", eg.doUniqueTest());
Assert.expectEq("references via blended interface parent type", "a.U::g(),a.V::h()", eg.doBlendTest());
Assert.expectEq("references via implementing object type", "a.U::g(),a.{T,U}::f()", eg.doImplementsTest());
Assert.expectEq("reference via untyped object", "a.{T,U}::f()", eg.doUntypedTest());

              // displays results.
