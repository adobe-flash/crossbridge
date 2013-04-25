/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

// var SECTION = "Directives";       // provide a document reference (ie, Actionscript section)
// var VERSION = "AS3";        // Version of ECMAScript or ActionScript
// var TITLE   = "longQualifier";       // Provide ECMA section title or a description
var BUGNUMBER = "";



///////////////////////////////////////////////////////////////
// add your tests here

//bug 150502
namespace T1;
class nsTest
{
    T1 function get x () { return 10;}
    public function get x () {return 0;}
}

var myTest = new nsTest;
var someVar = myTest.T1::x;
Assert.expectEq( "ns within a class", 10, someVar );


namespace get;
class Get
{
    get function get get () { return "get"; }
}

var g = new Get;
var myGet = g.get::get;
Assert.expectEq( "ns within a class - get keyword", "get", myGet );





//
////////////////////////////////////////////////////////////////

              // displays results.
