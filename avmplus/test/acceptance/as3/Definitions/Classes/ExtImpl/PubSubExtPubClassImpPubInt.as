/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

import PublicClassImpPublicInt.*;
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
  
var obj = new PublicsubClass();
//use namespace ns;

//Public Class extends Public class implements a public interface with a default method
//Assert.expectEq("Public class extends Public class implements a public interface with a //default //method", "PASSED", obj.getdeffunc());

//Public Class extends Public class implements a public interface with a public method
Assert.expectEq("Public class extends Public class implements a public interface with a public method", false, obj.pubFunc());

//Public Class extends Public class implements a public interface with a namespace method
Assert.expectEq("Public class extends Public class implements a public interface with a namespace method", 4, obj.getnsFunc());

////////////////////////////////////////////////////////////////

              // displays results.
