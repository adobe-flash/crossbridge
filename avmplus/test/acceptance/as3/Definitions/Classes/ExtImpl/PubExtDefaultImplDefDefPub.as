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
var PUBEXTDEFIMPLDEFDEFPUB = new PubExtDefaultImplDefDefPub();

// *******************************************
// define default method from interface as a
// public method in the sub class
// *******************************************
//var i:DefaultInt=PUBEXTDEFIMPLDEFDEFPUB;
//var j:DefaultIntDef=PUBEXTDEFIMPLDEFDEFPUB;
var k = new PubExtDefaultImplDefDefPubAccessor();
//Assert.expectEq( "*** default(<empty>) method implemented interface ***", 1, 1 );
//Assert.expectEq( "PUBEXTDEFIMPLDEFDEFPUB.setBoolean(false), PUBEXTDEFIMPLDEFDEFPUB.iiGetBoolean()", false, k.acciiGetBoolean() );
//Assert.expectEq( "PUBEXTDEFIMPLDEFDEFPUB.setBoolean(true), PUBEXTDEFIMPLDEFDEFPUB.jiGetBoolean()", true, k.accjiGetBoolean());

// *******************************************
// define public method from interface as a
// public method in the sub class
// *******************************************

Assert.expectEq( "*** public method implemented interface ***", 1, 1 );
Assert.expectEq( "PUBEXTDEFIMPLDEFDEFPUB.setPubBoolean(false), PUBEXTDEFIMPLDEFDEFPUB.iGetPubBoolean()", false, (PUBEXTDEFIMPLDEFDEFPUB.setPubBoolean(false), PUBEXTDEFIMPLDEFDEFPUB.iGetPubBoolean()) );
Assert.expectEq( "PUBEXTDEFIMPLDEFDEFPUB.setPubBoolean(true), PUBEXTDEFIMPLDEFDEFPUB.iGetPubBoolean()", true, (PUBEXTDEFIMPLDEFDEFPUB.setPubBoolean(true), PUBEXTDEFIMPLDEFDEFPUB.iGetPubBoolean()) );

// *******************************************
// define default method from interface 2 as a
// public method in the sub class
// *******************************************

//Assert.expectEq( "*** default(<empty>) method implemented interface 2 ***", 1, 1 );
Assert.expectEq( "PUBEXTDEFIMPLDEFDEFPUB.setNumber(420), PUBEXTDEFIMPLDEFDEFPUB.iiGetNumber()", 420, k.acciiGetNumber() );
Assert.expectEq( "PUBEXTDEFIMPLDEFDEFPUB.setNumber(420), PUBEXTDEFIMPLDEFDEFPUB.jiGetNumber()", 420, k.accjiGetNumber() );
// *******************************************
// define public method from interface 2 as a
// public method in the sub class
// *******************************************

Assert.expectEq( "*** public method implemented interface 2 ***", 1, 1 );
Assert.expectEq( "PUBEXTDEFIMPLDEFDEFPUB.setPubNumber(14), PUBEXTDEFIMPLDEFDEFPUB.iGetPubNumber()", 14, (PUBEXTDEFIMPLDEFDEFPUB.setPubNumber(14), PUBEXTDEFIMPLDEFDEFPUB.iGetPubNumber()) );

//*******************************************
// access from outside of class
//*******************************************

var EXTDCLASS = new PubExtDefaultImplDefDefPub();

arr = new Array(1, 2, 3);

Assert.expectEq( "*** access public methods and properties from outside of class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.setPubArray(arr), EXTDCLASS.pubArray", arr, (EXTDCLASS.setPubArray(arr), EXTDCLASS.pubArray) );

// ********************************************
// access public method from a default
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtDefaultImplDefDefPub();
Assert.expectEq( "*** Access public method from default method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testGetSubArray(arr)", arr, EXTDCLASS.testGetSubArray(arr) );


// ********************************************
// access public method from a public
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtDefaultImplDefDefPub();
Assert.expectEq( "*** Access public method from public method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()", arr, (EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()) );


// ********************************************
// access public method from a private
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtDefaultImplDefDefPub();
Assert.expectEq( "*** Access public method from private method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivSubArray(arr)", arr, EXTDCLASS.testPrivSubArray(arr) );

// ********************************************
// access public method from a final
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtDefaultImplDefDefPub();
Assert.expectEq( "*** Access public method from final method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testFinSubArray(arr)", arr, EXTDCLASS.testFinSubArray(arr) );



              // displays results.
