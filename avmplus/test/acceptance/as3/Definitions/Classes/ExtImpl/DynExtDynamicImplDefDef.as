/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


// var SECTION = "Definitions";       // provide a document reference (ie, ECMA section)
// var VERSION = "Clean AS2";  // Version of JavaScript or ECMA
// var TITLE   = "Extend DynamicClass Implement 2 Default Interfaces";       // Provide ECMA section title or a description
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

import DynamicClass.*;

import com.adobe.test.Assert;
var THISTEST = new DynExtDynamicImplDefDef();


// *******************************************
// define default method from interface as a
// default method in the sub class

// *******************************************
/*Assert.expectEq( "THISTEST.setBoolean(false), THISTEST.iGetBoolean()", true, (THISTEST.iGetBooleanacc1()) );
Assert.expectEq( "THISTEST.setBoolean(true), THISTEST.iGetBoolean()", false, (THISTEST.iGetBooleanacc2()) );*/


Assert.expectEq( "*** default(<empty>) method implemented interface ***", 1, 1 );
Assert.expectEq( "THISTEST.testGetSetBoolean(false)", false, THISTEST.testGetSetBoolean(false) );
Assert.expectEq( "THISTEST.testGetSetBoolean(true)", true, THISTEST.testGetSetBoolean(true) );

// *******************************************
// define public method from interface as a
// default method in the sub class
// *******************************************

//Should give an error

Assert.expectEq( "*** public method implemented interface ***", 1, 1 );
Assert.expectEq( "THISTEST.testPubGetSetBoolean(false)", false, THISTEST.testPubGetSetBoolean(false) );
Assert.expectEq( "THISTEST.testPubGetSetBoolean(true)", true, THISTEST.testPubGetSetBoolean(true) );

// *******************************************
// define default method from interface 2 as a
// default method in the sub class
// *******************************************
/*Assert.expectEq( "THISTEST.setNumber(420), THISTEST.iGetNumber()", 20,(THISTEST.iGetNumberacc1()) );
Assert.expectEq( "THISTEST.setNumber(420), THISTEST.iGetNumber()", 420,(THISTEST.iGetNumberacc2()) );*/
Assert.expectEq( "*** default(<empty>) method implemented interface 2 ***", 1, 1 );
Assert.expectEq( "THISTEST.testGetSetNumber(420)", 420, THISTEST.testGetSetNumber(420) );

// *******************************************
// define public method from interface 2 as a
// default method in the sub class
// *******************************************

//Should give an error

Assert.expectEq( "*** public method implemented interface 2 ***", 1, 1 );
Assert.expectEq( "THISTEST.testPubGetSetNumber(14)", 14, THISTEST.testPubGetSetNumber(14) );



// ********************************************
// access default method from a default
// method of a sub class
//
// ********************************************

var arr = new Array(1, 2, 3);

EXTDCLASS = new DynExtDynamicImplDefDef();
Assert.expectEq( "*** Access default method from default method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testGetSubArray(arr)", arr, EXTDCLASS.testGetSubArray(arr) );

// <TODO>  fill in the rest of the cases here


// ********************************************
// access default method from a public
// method of a sub class
//
// ********************************************

EXTDCLASS = new DynExtDynamicImplDefDef();
Assert.expectEq( "*** Access default method from public method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()", arr, (EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()) );

// <TODO>  fill in the rest of the cases here

// ********************************************
// access default method from a private
// method of a sub class
//
// ********************************************

EXTDCLASS = new DynExtDynamicImplDefDef();
Assert.expectEq( "*** Access default method from private method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivSubArray(arr)", arr, EXTDCLASS.testPrivSubArray(arr) );

// <TODO>  fill in the rest of the cases here


// ********************************************
// access default property from
// default method in sub class
// ********************************************

EXTDCLASS = new DynExtDynamicImplDefDef();
Assert.expectEq( "*** Access default property from method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testSubGetDPArray(arr)", arr, EXTDCLASS.testSubGetDPArray(arr) );

// <TODO>  fill in the rest of the cases here

// ********************************************
// access default property from
// public method in sub class
// ********************************************

EXTDCLASS = new DynExtDynamicImplDefDef();
Assert.expectEq( "*** Access default property from public method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.pubSubSetDPArray(arr), EXTDCLASS.pubSubGetDPArray()", arr, (EXTDCLASS.pubSubSetDPArray(arr), EXTDCLASS.pubSubGetDPArray()) );

// <TODO>  fill in the rest of the cases here

// ********************************************
// access default property from
// private method in sub class
// ********************************************

EXTDCLASS = new DynExtDynamicImplDefDef();
Assert.expectEq( "*** Access default property from private method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivSubGetDPArray(arr)", arr, EXTDCLASS.testPrivSubGetDPArray(arr) );

// <TODO>  fill in the rest of the cases here


              // displays results.
