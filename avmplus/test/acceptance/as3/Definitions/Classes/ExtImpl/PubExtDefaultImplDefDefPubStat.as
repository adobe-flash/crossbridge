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
arr = new Array(1, 2, 3);

var THISTEST = new PubExtDefaultImplDefDefPubStat();

// *******************************************
// define public method from interface as a
// public method in the sub class
// *******************************************

Assert.expectEq( "*** public method implemented interface ***", 1, 1 );
Assert.expectEq( "THISTEST.setPubBoolean(false), THISTEST.iGetPubBoolean()", false, (THISTEST.setPubBoolean(false), THISTEST.iGetPubBoolean()) );
Assert.expectEq( "THISTEST.setPubBoolean(true), THISTEST.iGetPubBoolean()", true, (THISTEST.setPubBoolean(true), THISTEST.iGetPubBoolean()) );


// *******************************************
// define public method from interface 2 as a
// public method in the sub class
// *******************************************

Assert.expectEq( "*** public method implemented interface 2 ***", 1, 1 );
Assert.expectEq( "THISTEST.setPubNumber(14), THISTEST.iGetPubNumber()", 14, (THISTEST.setPubNumber(14), THISTEST.iGetPubNumber()) );


//*******************************************
// access public static method of parent
// class from outside of class
//*******************************************

Assert.expectEq( "*** Public Static Methods and Public Static properites ***", 1, 1 );
//Assert.expectEq( "THISTEST.setStatArray(arr), THISTEST.statArray", arr,
//             (THISTEST.setStatArray(arr), THISTEST.statArray) );


// ********************************************
// access public static method from a default
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtDefaultImplDefDefPubStat();
Assert.expectEq( "*** Access public static method from default method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.subSetArray(arr), EXTDCLASS.subGetArray()", arr, EXTDCLASS.testSubGetSetArray(arr) );

// <TODO>  fill in the rest of the cases here

// ********************************************
// access public static method from a public
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtDefaultImplDefDefPubStat();
Assert.expectEq( "*** Access public static method from public method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()", arr, (EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()) );

// <TODO>  fill in the rest of the cases here

// ********************************************
// access public static method from a private
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtDefaultImplDefDefPubStat();
Assert.expectEq( "*** Access public static method from private method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivSubArray(arr)", arr, EXTDCLASS.testPrivSubArray(arr) );

// <TODO>  fill in the rest of the cases here

// ********************************************
// access public static method from a final
// method of a sub class
//
// ********************************************

EXTDCLASS = new PubExtDefaultImplDefDefPubStat();
Assert.expectEq( "*** Access public static method from final method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testFinSubArray(arr)", arr, EXTDCLASS.testFinSubArray(arr) );

// ********************************************
// access public static method from a static
// method of a sub class
//
// ********************************************

Assert.expectEq( "*** Access public static method from static method of sub class ***", 1, 1 );
Assert.expectEq( "PubExtDefaultImplDefDefPubStat.testStatSubArray(arr)", arr, PubExtDefaultImplDefDefPubStat.testStatSubArray(arr));

// <TODO>  fill in the rest of the cases here

// ********************************************
// access public static method from a public static
// method of a sub class
//
// ********************************************

Assert.expectEq( "*** Access public static method from public static method of sub class ***", 1, 1 );
Assert.expectEq( "PubExtDefaultImplDefDefPubStat.pubStatSubSetArray(arr), PubExtDefaultImplDefDefPubStat.pubStatSubGetArray()", arr,
             (PubExtDefaultImplDefDefPubStat.pubStatSubSetArray(arr), PubExtDefaultImplDefDefPubStat.pubStatSubGetArray()) );

// <TODO>  fill in the rest of the cases here

// ********************************************
// access public static method from a private static
// method of a sub class
//
// ********************************************

Assert.expectEq( "*** Access public static method from private static method of sub class ***", 1, 1 );
Assert.expectEq( "PubExtDefaultImplDefDefPubStat.testPrivStatSubArray(arr)", arr, PubExtDefaultImplDefDefPubStat.testPrivStatSubArray(arr) );

// <TODO>  fill in the rest of the cases here

// ********************************************
// access public static property from
// default method in sub class
// ********************************************

EXTDCLASS = new PubExtDefaultImplDefDefPubStat();
Assert.expectEq( "*** Access public static property from default method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.subSetDPArray(arr), EXTDCLASS.subGetDPArray()", arr, EXTDCLASS.testSubGetSetDPArray(arr) );

// <TODO>  fill in the rest of the cases here


// ********************************************
// access public static property from
// public method in sub class
// ********************************************

EXTDCLASS = new PubExtDefaultImplDefDefPubStat();
Assert.expectEq( "*** Access public static property from public method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.pubSubSetDPArray(arr), EXTDCLASS.pubSubGetDPArray()", arr, (EXTDCLASS.pubSubSetDPArray(arr), EXTDCLASS.pubSubGetDPArray()) );

// <TODO>  fill in the rest of the cases here

// ********************************************
// access public static property from
// private method in sub class
// ********************************************

EXTDCLASS = new PubExtDefaultImplDefDefPubStat();
Assert.expectEq( "*** Access public static property from private method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.privSubSetDPArray(arr), EXTDCLASS.privSubGetDPArray()", arr, EXTDCLASS.testPrivSubDPArray(arr) );

// <TODO>  fill in the rest of the cases here

// ********************************************
// access public static property from
// final method in sub class
// ********************************************

EXTDCLASS = new PubExtDefaultImplDefDefPubStat();
Assert.expectEq( "*** Access public static property from final method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.finSubSetDPArray(arr), EXTDCLASS.finSubGetDPArray()", arr, EXTDCLASS.testFinSubDPArray(arr) );

// ********************************************
// access public static property from
// static method in sub class
// ********************************************

Assert.expectEq( "*** Access public static property from static method in sub class ***", 1, 1 );
Assert.expectEq( "PubExtDefaultImplDefDefPubStat.statSubSetSPArray(arr), PubExtDefaultImplDefDefPubStat.statSubGetSPArray()", arr,PubExtDefaultImplDefDefPubStat.testStatSubSPArray(arr) );

// ********************************************
// access public static property from
// public static method in sub class
// ********************************************

Assert.expectEq( "*** Access public static property from public static method in sub class ***", 1, 1 );
Assert.expectEq( "PubExtDefaultImplDefDefPubStat.pubStatSubSetSPArray(arr), PubExtDefaultImplDefDefPubStat.pubStatSubGetSPArray()", arr,
             (PubExtDefaultImplDefDefPubStat.pubStatSubSetSPArray(arr), PubExtDefaultImplDefDefPubStat.pubStatSubGetSPArray()) );

// ********************************************
// access public static property from
// private static method in sub class
// ********************************************

Assert.expectEq( "*** Access public static property from private static method in sub class ***", 1, 1 );
Assert.expectEq( "PubExtDefaultImplDefDefPubStat.testPrivStatSubPArray(arr)", arr, PubExtDefaultImplDefDefPubStat.testPrivStatSubPArray(arr));

// <TODO>  fill in the rest of the cases here

              // displays results.
