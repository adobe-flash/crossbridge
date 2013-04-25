/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
/*
*
* Date:    31 August 2002
* SUMMARY: RegExp conformance test
* See http://bugzilla.mozilla.org/show_bug.cgi?id=165353
*
*/
//-----------------------------------------------------------------------------

// var SECTION = "eregress_165353";
// var VERSION = "";
// var TITLE   = "RegExp conformance test";
// var bug = "165353";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var status = '';
    var pattern = '';
    var string = '';
    var actualmatch = '';
    var expectedmatch = '';
    var expectedmatches = new Array();

    pattern = /^([a-z]+)*[a-z]$/;
   //    status = inSection(1);
      string = 'a';
      actualmatch = string.match(pattern);
      expectedmatch = Array('a', undefined);
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

   //    status = inSection(2);
      string = 'ab';
      actualmatch = string.match(pattern);
      expectedmatch = Array('ab', 'a');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

   //    status = inSection(3);
      string = 'abc';
      actualmatch = string.match(pattern);
      expectedmatch = Array('abc', 'ab');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());


    string = 'www.netscape.com';
   //    status = inSection(4);
      pattern = /^(([a-z]+)*[a-z]\.)+[a-z]{2,}$/;
      actualmatch = string.match(pattern);
      expectedmatch = Array('www.netscape.com', 'netscape.', 'netscap');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

      // add one more capturing parens to the previous regexp -
   //    status = inSection(5);
      pattern = /^(([a-z]+)*([a-z])\.)+[a-z]{2,}$/;
      actualmatch = string.match(pattern);
      expectedmatch = Array('www.netscape.com', 'netscape.', 'netscap', 'e');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

    return array;
}
