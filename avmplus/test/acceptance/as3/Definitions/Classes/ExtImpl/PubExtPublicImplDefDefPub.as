/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


// var SECTION = "Definitions";       // provide a document reference (ie, ECMA section)
// var VERSION = "Clean AS2";  // Version of JavaScript or ECMA
// var TITLE   = "Extend Public Class Implement Default interface";       // Provide ECMA section title or a description
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

import PublicClass.*;

import com.adobe.test.Assert;
var PUBEXTDYNIMPLDEFDEFPUB = new PubExtPublicImplDefDefPub();

// *******************************************
// define default method from interface as a
// public method in the sub class
// *******************************************

// Should give an error

Assert.expectEq( "*** default(<empty>) method implemented interface ***", 1, 1 );
Assert.expectEq( "PUBEXTDYNIMPLDEFDEFPUB.testGetBoolean(false)", false, PUBEXTDYNIMPLDEFDEFPUB.testGetBoolean(false) );
Assert.expectEq( "PUBEXTDYNIMPLDEFDEFPUB.testGetBoolean(true)", true, PUBEXTDYNIMPLDEFDEFPUB.testGetBoolean(true) );

// *******************************************
// define public method from interface as a
// public method in the sub class
// *******************************************

Assert.expectEq( "*** public method implemented interface ***", 1, 1 );
Assert.expectEq( "PUBEXTDYNIMPLDEFDEFPUB.testGetPubBoolean(false)", false, PUBEXTDYNIMPLDEFDEFPUB.testGetPubBoolean(false) );
Assert.expectEq( "PUBEXTDYNIMPLDEFDEFPUB.testGetPubBoolean(true)", true, PUBEXTDYNIMPLDEFDEFPUB.testGetPubBoolean(true) );

// *******************************************
// define default method from interface 2 as a
// public method in the sub class
// *******************************************

//Should give an error

Assert.expectEq( "*** default(<empty>) method implemented interface 2 ***", 1, 1 );
Assert.expectEq( "PUBEXTDYNIMPLDEFDEFPUB.testGetSetNumber(420)", 420, PUBEXTDYNIMPLDEFDEFPUB.testGetSetNumber(420) );

// *******************************************
// define public method from interface 2 as a
// public method in the sub class
// *******************************************

Assert.expectEq( "*** public method implemented interface 2 ***", 1, 1 );
Assert.expectEq( "PUBEXTDYNIMPLDEFDEFPUB.setPubNumber(14), PUBEXTDYNIMPLDEFDEFPUB.iGetPubNumber()", 14, (PUBEXTDYNIMPLDEFDEFPUB.setPubNumber(14), PUBEXTDYNIMPLDEFDEFPUB.iGetPubNumber()) );

//*******************************************
// access from outside of class
//*******************************************

var EXTDCLASS = new PubExtPublicImplDefDefPub();

arr = new Array(1, 2, 3);

Assert.expectEq( "*** access public methods and properties from outside of class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.setPubArray(arr), EXTDCLASS.pubArray", arr, (EXTDCLASS.setPubArray(arr), EXTDCLASS.pubArray) );

// ********************************************
// access public method from a default
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtPublicImplDefDefPub();
Assert.expectEq( "*** Access public method from default method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testGetSubArray(arr)", arr, EXTDCLASS.testGetSubArray(arr) );

// <TODO>  fill in the rest of the cases here


// ********************************************
// access public method from a public
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtPublicImplDefDefPub();
Assert.expectEq( "*** Access public method from public method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()", arr, (EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()) );

// <TODO>  fill in the rest of the cases here

// ********************************************
// access public method from a private
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtPublicImplDefDefPub();
Assert.expectEq( "*** Access public method from private method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivSubArray(arr)", arr, EXTDCLASS.testPrivSubArray(arr) );

// <TODO>  fill in the rest of the cases here


              // displays results.
