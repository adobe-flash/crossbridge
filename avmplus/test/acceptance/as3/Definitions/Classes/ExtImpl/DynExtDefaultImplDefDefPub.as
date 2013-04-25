/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


// var SECTION = "Definitions";       // provide a document reference (ie, ECMA section)
// var VERSION = "Clean AS2";  // Version of JavaScript or ECMA
// var TITLE   = "Extend Default Class Implement Default interface";       // Provide ECMA section title or a description
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
var THISTEST = new DynExtDefaultImplDefDefPub();
//var i:DefaultInt = THISTEST;
//var j:DefaultIntDef=THISTEST;
// *******************************************
// define default method from interface as a
// public method in the sub class
// *******************************************

//This should throw an error message

//Assert.expectEq( "*** default(<empty>) method implemented interface ***", 1, 1 );
Assert.expectEq( "THISTEST.setBoolean(false), THISTEST.iGetBoolean()", false, (THISTEST.iGetBooleanacc1()) );
Assert.expectEq( "THISTEST.setBoolean(true), THISTEST.iGetBoolean()", false, (THISTEST.iGetBooleanacc2()) );

// *******************************************
// define public method from interface as a
// public method in the sub class
// *******************************************

Assert.expectEq( "*** public method implemented interface ***", 1, 1 );
Assert.expectEq( "THISTEST.setPubBoolean(false), THISTEST.iGetPubBoolean()", false, (THISTEST.setPubBoolean(false), THISTEST.iGetPubBoolean()) );
Assert.expectEq( "THISTEST.setPubBoolean(true), THISTEST.iGetPubBoolean()", true, (THISTEST.setPubBoolean(true), THISTEST.iGetPubBoolean()) );

// *******************************************
// define default method from interface 2 as a
// public method in the sub class
// *******************************************

//This should throw an error message

//Assert.expectEq( "*** default(<empty>) method implemented interface 2 ***", 1, 1 );
Assert.expectEq( "THISTEST.setNumber(420), THISTEST.iGetNumber()", 20,(THISTEST.iGetNumberacc1()) );
Assert.expectEq( "THISTEST.setNumber(420), THISTEST.iGetNumber()", 20,(THISTEST.iGetNumberacc2()) );
// *******************************************
// define public method from interface 2 as a
// public method in the sub class
// *******************************************

Assert.expectEq( "*** public method implemented interface 2 ***", 1, 1 );
Assert.expectEq( "THISTEST.setPubNumber(14), THISTEST.iGetPubNumber()", 14, (THISTEST.setPubNumber(14), THISTEST.iGetPubNumber()) );

//*******************************************
// access from outside of class
//*******************************************

var EXTDCLASS = new DynExtDefaultImplDefDefPub();

arr = new Array(1, 2, 3);

Assert.expectEq( "*** access public methods and properties from outside of class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.setPubArray(arr), EXTDCLASS.pubArray", arr, (EXTDCLASS.setPubArray(arr), EXTDCLASS.pubArray) );

// ********************************************
// access public method from a default
// method of a sub class
//
// ********************************************

EXTDCLASS = new DynExtDefaultImplDefDefPub();
Assert.expectEq( "*** Access public method from default method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testGetSubArray(arr)", arr, EXTDCLASS.testGetSubArray(arr) );

// <TODO>  fill in the rest of the cases here


// ********************************************
// access public method from a public
// method of a sub class
//
// ********************************************

EXTDCLASS = new DynExtDefaultImplDefDefPub();
Assert.expectEq( "*** Access public method from public method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()", arr, (EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()) );

// <TODO>  fill in the rest of the cases here

// ********************************************
// access public method from a private
// method of a sub class
//
// ********************************************

EXTDCLASS = new DynExtDefaultImplDefDefPub();
Assert.expectEq( "*** Access public method from private method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivSubArray(arr)", arr, EXTDCLASS.testPrivSubArray(arr) );

// <TODO>  fill in the rest of the cases here


              // displays results.
