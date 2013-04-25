/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;



//     var SECTION = "test";       // provide a document reference (ie, ECMA section)
//     var VERSION = "ECMA_2"; // Version of JavaScript or ECMA
//     var TITLE   = "test";       // Provide ECMA section title or a description
    var BUGNUMBER = "";     // Provide URL to bugsplat or bugzilla report


    var testcases = getTestCases();
    
                  // displays results.
    
function getTestCases() {
    var array = new Array();
    var item = 0;
    /*
     * Calls to Assert.expectEq here. Assert.expectEq is a function that is defined
     * in shell.js and takes three arguments:
     * - a string representation of what is being tested
     * - the expected result
     * - the actual result
     *
     * For example, a test might look like this:
     *
     * var zip = /[\d]{5}$/;
     *
     * Assert.expectEq(
     * "zip = /[\d]{5}$/; \"PO Box 12345 Boston, MA 02134\".match(zip)",   // description of the test
     *  "02134",                                                           // expected result
     *  "PO Box 12345 Boston, MA 02134".match(zip) );                      // actual result
     *
     */
    var helloWorld = "Hello World";
    array[item++] = Assert.expectEq("[Hello World Test]", "Hello World", helloWorld );
    return array;
}
