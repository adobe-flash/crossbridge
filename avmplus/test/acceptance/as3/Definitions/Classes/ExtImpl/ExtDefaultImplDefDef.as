/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


// var SECTION = "Definitions";       // provide a document reference (ie, ECMA section)
// var VERSION = "Clean AS2";  // Version of JavaScript or ECMA
// var TITLE   = "Extend Default Class Implement 2 Default Interfaces";       // Provide ECMA section title or a description
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

import DefaultClass.*;

import com.adobe.test.Assert;
var THISTEST = new ExtDefaultImplDefDef();


// *******************************************
// define default method from interface as a
// default method in the sub class
// *******************************************

Assert.expectEq( "*** default(<empty>) method implemented interface ***", 1, 1 );
Assert.expectEq( "THISTEST.testGetSetBoolean(false)", false, THISTEST.testGetSetBoolean(false) );
Assert.expectEq( "THISTEST.testGetSetBoolean(true)", true, THISTEST.testGetSetBoolean(true) );

// *******************************************
// define public method from interface as a
// public method in the sub class
// *******************************************

Assert.expectEq( "*** public method implemented interface ***", 1, 1 );
Assert.expectEq( "THISTEST.setPubBoolean(false), THISTEST.iGetPubBoolean()", false, (THISTEST.setPubBoolean(false), THISTEST.iGetPubBoolean()) );
Assert.expectEq( "THISTEST.setPubBoolean(true), THISTEST.iGetPubBoolean()", true, (THISTEST.setPubBoolean(true), THISTEST.iGetPubBoolean()) );

// *******************************************
// define default method from interface 2 as a
// default method in the sub class
// *******************************************

Assert.expectEq( "*** default(<empty>) method implemented interface 2 ***", 1, 1 );
Assert.expectEq( "THISTEST.testGetSetNumber(420)", 420, THISTEST.testGetSetNumber(420) );

// *******************************************
// define public method from interface 2 as a
// public method in the sub class
// *******************************************

Assert.expectEq( "*** public method implemented interface 2 ***", 1, 1 );
Assert.expectEq( "THISTEST.setPubNumber(14), THISTEST.iGetPubNumber()", 14, (THISTEST.setPubNumber(14), THISTEST.iGetPubNumber()) );
Assert.expectEq( "THISTEST.setPubNumber(14), THISTEST.iGetPubNumber()", 14, (THISTEST.setPubNumber(14), THISTEST.iiGetNumber1()) );
Assert.expectEq( "THISTEST.setPubNumber(14), THISTEST.iGetPubNumber()", 14, (THISTEST.setPubNumber(14), THISTEST.jiGetNumber1()) );
//*******************************************
// access default method from outside
// of the class
//*******************************************

var EXTDCLASS = new ExtDefaultImplDefDef();
var arr = new Array(1, 2, 3);

// ********************************************
// access default method from a default
// method of a sub class
//
// ********************************************

EXTDCLASS = new ExtDefaultImplDefDef();
Assert.expectEq( "*** Access default method from default method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testGetSubArray(arr)", arr, EXTDCLASS.testGetSubArray(arr) );


// ********************************************
// access default method from a public
// method of a sub class
//
// ********************************************

EXTDCLASS = new ExtDefaultImplDefDef();
Assert.expectEq( "*** Access default method from public method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()", arr, (EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()) );


// ********************************************
// access default method from a private
// method of a sub class
//
// ********************************************

EXTDCLASS = new ExtDefaultImplDefDef();
Assert.expectEq( "*** Access default method from private method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivSubArray(arr)", arr, EXTDCLASS.testPrivSubArray(arr) );


// ********************************************
// access default property from outside
// the class
// ********************************************

//EXTDCLASS = new ExtDefaultImplDefDef();
//Assert.expectEq( "*** Access default property from outside the class ***", 1, 1 );
//Assert.expectEq( "EXTDCLASS.array = arr", arr, (EXTDCLASS.array = arr, EXTDCLASS.array) );


// ********************************************
// access default property from
// default method in sub class
// ********************************************

EXTDCLASS = new ExtDefaultImplDefDef();
Assert.expectEq( "*** Access default property from method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testSubGetDPArray(arr)", arr, EXTDCLASS.testSubGetDPArray(arr) );

// ********************************************
// access default property from
// public method in sub class
// ********************************************

EXTDCLASS = new ExtDefaultImplDefDef();
Assert.expectEq( "*** Access default property from public method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.pubSubSetDPArray(arr), EXTDCLASS.pubSubGetDPArray()", arr, (EXTDCLASS.pubSubSetDPArray(arr), EXTDCLASS.pubSubGetDPArray()) );


// ********************************************
// access default property from
// private method in sub class
// ********************************************

EXTDCLASS = new ExtDefaultImplDefDef();
Assert.expectEq( "*** Access default property from private method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivSubGetDPArray(arr)", arr, EXTDCLASS.testPrivSubGetDPArray(arr) );



              // displays results.
