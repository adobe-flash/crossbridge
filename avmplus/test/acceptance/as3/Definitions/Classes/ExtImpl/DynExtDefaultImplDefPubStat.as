/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


// var SECTION = "Definitions";       // provide a document reference (ie, ECMA section)
// var VERSION = "Clean AS2";  // Version of JavaScript or ECMA
// var TITLE   = "Extend Default Class Implement Default Interface";       // Provide ECMA section title or a description
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

THISTEST = new DynExtDefaultImplDefPubStat();
// *******************************************
// define public method from interface as a
// public method in the sub class
// *******************************************

Assert.expectEq( "*** public method implemented interface ***", 1, 1 );
Assert.expectEq( "THISTEST.setPubBoolean(false), THISTEST.iGetPubBoolean()", false, (THISTEST.setPubBoolean(false), THISTEST.iGetPubBoolean()) );
Assert.expectEq( "THISTEST.setPubBoolean(true), THISTEST.iGetPubBoolean()", true, (THISTEST.setPubBoolean(true), THISTEST.iGetPubBoolean()) );


//*******************************************
// access public static method of parent
// class from outside of class
//*******************************************

Assert.expectEq( "*** Public Static Methods of parent from sub class ***", 1, 1 );
Assert.expectEq( "DynExtDefaultImplDefPubStat.setPubStatArray(arr), DynExtDefaultImplDefPubStat.getPubStatArray()", arr,
             (DynExtDefaultImplDefPubStat.setPubStatArray(arr), DynExtDefaultImplDefPubStat.getPubStatArray()) );


// ********************************************
// access public static method from a default
// method of a sub class
//
// ********************************************

EXTDCLASS = new DynExtDefaultImplDefPubStat();
Assert.expectEq( "*** Access public static method from default method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testDefSubArray(arr)", arr, (EXTDCLASS.testDefSubArray(arr)) );

// ********************************************
// access public static method from a public
// method of a sub class
//
// ********************************************

EXTDCLASS = new DynExtDefaultImplDefPubStat();
Assert.expectEq( "*** Access public static method from public method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()", arr, (EXTDCLASS.pubSubSetArray(arr), EXTDCLASS.pubSubGetArray()) );


// ********************************************
// access public static method from a private
// method of a sub class
//
// ********************************************

EXTDCLASS = new DynExtDefaultImplDefPubStat();
Assert.expectEq( "*** Access public static method from private method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivSubArray(arr)", arr, EXTDCLASS.testPrivSubArray(arr) );


// ********************************************
// access public static method from a final
// method of a sub class
//
// ********************************************

EXTDCLASS = new DynExtDefaultImplDefPubStat();
Assert.expectEq( "*** Access public static method from final method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testFinSubArray(arr)", arr, (EXTDCLASS.testFinSubArray(arr)) );

/*
// ********************************************
// access public static method from a static
// method of a sub class
//
// ********************************************
Assert.expectEq( "*** Access public static method from static method of sub class ***", 1, 1 );
Assert.expectEq( "DynExtDefaultImplDefPubStat.testStatSubArray(arr)", arr,
             (EXTDCLASS.testStatSubArray(arr)) );
*/

// ********************************************
// access public static method from a public static
// method of a sub class
//
// ********************************************

Assert.expectEq( "*** Access public static method from public static method of sub class ***", 1, 1 );
Assert.expectEq( "DynExtDefaultImplDefPubStat.pubStatSubSetArray(arr), DynExtDefaultImplDefPubStat.pubStatSubGetArray()", arr,
             (DynExtDefaultImplDefPubStat.pubStatSubSetArray(arr), DynExtDefaultImplDefPubStat.pubStatSubGetArray()) );

// ********************************************
// access public static method from a private static
// method of a sub class
//
// ********************************************

var EXTDEFAULTCLASS = new DynExtDefaultImplDefPubStat();
Assert.expectEq( "*** Access public static method from private static method of sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivStatSubArray(arr)", arr,
              EXTDCLASS.testPrivStatSubArray(arr) );


// ********************************************
// access public static property from
// default method in sub class
// ********************************************

EXTDCLASS = new DynExtDefaultImplDefPubStat();
Assert.expectEq( "*** Access public static property from default method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPubStatDefSubArray(arr)", arr, (EXTDCLASS.testPubStatDefSubArray(arr)) );


// ********************************************
// access public static property from
// public method in sub class
// ********************************************

EXTDCLASS = new DynExtDefaultImplDefPubStat();
Assert.expectEq( "*** Access public static property from public method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.pubSubSetDPArray(arr), EXTDCLASS.pubSubGetDPArray()", arr, (EXTDCLASS.pubSubSetDPArray(arr), EXTDCLASS.pubSubGetDPArray()) );
// ********************************************
// access public static property from
// private method in sub class
// ********************************************

EXTDCLASS = new DynExtDefaultImplDefPubStat();
Assert.expectEq( "*** Access public static property from private method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivStatSubDPArray(arr)", arr, (EXTDCLASS.testPrivStatSubDPArray(arr)) );


// ********************************************
// access public static property from
// final method in sub class
// ********************************************

EXTDCLASS = new DynExtDefaultImplDefPubStat();
Assert.expectEq( "*** Access public static property from final method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivStatFinSubDPArray(arr)", arr, (EXTDCLASS.testPrivStatFinSubDPArray(arr)) );

// ********************************************
// access public static property from
// static method in sub class
// ********************************************

Assert.expectEq( "*** Access public static property from static method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testStatSubDPArray(arr)", arr,
             (EXTDCLASS.testStatSubDPArray(arr)) );
/*
// ********************************************
// access public static property from
// public static method in sub class
// ********************************************

Assert.expectEq( "*** Access public static property from public static method in sub class ***", 1, 1 );
Assert.expectEq( "PubExtDefaultClassPubStat.pubStatSubSetSPArray(arr), PubExtDefaultClassPubStat.pubStatSubGetSPArray()", arr,
             (PubExtDefaultClassPubStat.pubStatSubSetSPArray(arr), PubExtDefaultClassPubStat.pubStatSubGetSPArray()) );

// ********************************************
// access public static property from
// private static method in sub class
// ********************************************

EXTDCLASS = new DynExtDefaultImplDefPubStat();
Assert.expectEq( "*** Access public static property from private static method in sub class ***", 1, 1 );
Assert.expectEq( "EXTDCLASS.testPrivStatSubPArray(arr)", arr,
              EXTDCLASS.testPrivStatSubPArray(arr));
*/

              // displays results.
