/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


// var SECTION = "Definitions";       // provide a document reference (ie, ECMA section)
// var VERSION = "Clean AS2";  // Version of JavaScript or ECMA
// var TITLE   = "Extend PublicClass Implement 2 Default Interfaces";       // Provide ECMA section title or a description
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
var PUBEXTDYNIMPLDEFDEF = new PubExtPublicImplDefDef();


// *******************************************
// define default method from interface as a
// default method in the sub class
// *******************************************

Assert.expectEq( "*** default(<empty>) method implemented interface ***", 1, 1 );
Assert.expectEq( "PUBEXTDYNIMPLDEFDEF.testGetSetBoolean(false)", false, PUBEXTDYNIMPLDEFDEF.testGetSetBoolean(false) );
Assert.expectEq( "PUBEXTDYNIMPLDEFDEF.testGetSetBoolean(true)", true, PUBEXTDYNIMPLDEFDEF.testGetSetBoolean(true) );

// *******************************************
// define public method from interface as a
// default method in the sub class
// *******************************************

//Should give an error

Assert.expectEq( "*** public method implemented interface ***", 1, 1 );
Assert.expectEq( "PUBEXTDYNIMPLDEFDEF.testPubGetSetBoolean(false)", false, PUBEXTDYNIMPLDEFDEF.testPubGetSetBoolean(false) );
Assert.expectEq( "PUBEXTDYNIMPLDEFDEF.testPubGetSetBoolean(true)", true, PUBEXTDYNIMPLDEFDEF.testPubGetSetBoolean(true) );

// *******************************************
// define default method from interface 2 as a
// default method in the sub class
// *******************************************

Assert.expectEq( "*** default(<empty>) method implemented interface 2 ***", 1, 1 );
Assert.expectEq( "PUBEXTDYNIMPLDEFDEF.testGetSetNumber(420)", 420, PUBEXTDYNIMPLDEFDEF.testGetSetNumber(420) );

// *******************************************
// define public method from interface 2 as a
// default method in the sub class
// *******************************************

//Should give an error

Assert.expectEq( "*** public method implemented interface 2 ***", 1, 1 );
Assert.expectEq( "PUBEXTDYNIMPLDEFDEF.testPubGetSetNumber(14)", 14, PUBEXTDYNIMPLDEFDEF.testPubGetSetNumber(14) );



// ********************************************
// access default method from a default
// method of a sub class
//
// ********************************************

var arr = new Array(1, 2, 3);

EXTDCLASS = new PubExtPublicImplDefDef();
Assert.expectEq( "*** Access default method from default method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testGetSubArray(arr)", arr, EXTDCLASS.testGetSubArray(arr) );



// ********************************************
// access default method from a public
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtPublicImplDefDef();
Assert.expectEq( "*** Access default method from public method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()", arr, (EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()) );


// ********************************************
// access default method from a private
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtPublicImplDefDef();
Assert.expectEq( "*** Access default method from private method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivSubArray(arr)", arr, EXTDCLASS.testPrivSubArray(arr) );


// ********************************************
// access default method from a final
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtPublicImplDefDef();
Assert.expectEq( "*** Access default method from final method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testFinSubArray(arr)", arr, EXTDCLASS.testFinSubArray(arr) );


// ********************************************
// access default property from
// default method in sub class
// ********************************************

EXTDCLASS = new PubExtPublicImplDefDef();
Assert.expectEq( "*** Access default property from method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testSubGetDPArray(arr)", arr, EXTDCLASS.testSubGetDPArray(arr) );


// ********************************************
// access default property from
// public method in sub class
// ********************************************

EXTDCLASS = new PubExtPublicImplDefDef();
Assert.expectEq( "*** Access default property from public method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.pubSubSetDPArray(arr), EXTDCLASS.pubSubGetDPArray()", arr, (EXTDCLASS.pubSubSetDPArray(arr), EXTDCLASS.pubSubGetDPArray()) );

// <TODO>  fill in the rest of the cases here

// ********************************************
// access default property from
// private method in sub class
// ********************************************

EXTDCLASS = new PubExtPublicImplDefDef();
Assert.expectEq( "*** Access default property from private method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivSubGetDPArray(arr)", arr, EXTDCLASS.testPrivSubGetDPArray(arr) );

// <TODO>  fill in the rest of the cases here


              // displays results.
