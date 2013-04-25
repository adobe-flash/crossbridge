/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
/*
 * Date: 17 September 2001
 *
 * SUMMARY: Regression test for Bugzilla bug 100199
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=100199
 *
 * The empty character class [] is a valid RegExp construct: the condition
 * that a given character belong to a set containing no characters. As such,
 * it can never be met and is always FALSE. Similarly, [^] is a condition
 * that matches any given character and is always TRUE.
 *
 * Neither one of these conditions should cause syntax errors in a RegExp.
 */
//-----------------------------------------------------------------------------

// var SECTION = "eregress_100199";
// var VERSION = "";
// var TITLE   = "[], [^] are valid RegExp conditions. Should not cause errors -";
// var bug = "100199";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var status = '';
    var pattern = '';
    var string = '';
    var actualmatch = '';
    var expectedmatch = '';


      /*pattern = /[]/;
      string = 'abc';
   //    status = inSection(1);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = '';
   //    status = inSection(2);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = '[';
   //    status = inSection(3);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = '/';
   //    status = inSection(4);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = '[';
   //    status = inSection(5);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = ']';
   //    status = inSection(6);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = '[]';
   //    status = inSection(7);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = '[ ]';
   //    status = inSection(8);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = '][';
   //    status = inSection(9);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);


    pattern = /a[]/;
      string = 'abc';
   //    status = inSection(10);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = '';
   //    status = inSection(11);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = 'a[';
   //    status = inSection(12);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = 'a[]';
   //    status = inSection(13);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = '[';
   //    status = inSection(14);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = ']';
   //    status = inSection(15);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = '[]';
   //    status = inSection(16);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = '[ ]';
   //    status = inSection(17);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = '][';
   //    status = inSection(18);
      actualmatch = string.match(pattern);
      expectedmatch = null;
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);


    pattern = /[^]/;
      string = 'abc';
   //    status = inSection(19);
      actualmatch = string.match(pattern);
      expectedmatch = Array('a');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

      string = '';
   //    status = inSection(20);
      actualmatch = string.match(pattern);
      expectedmatch = null; //there are no characters to test against the condition
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = '\/';
   //    status = inSection(21);
      actualmatch = string.match(pattern);
      expectedmatch = Array('/');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

      string = '\[';
   //    status = inSection(22);
      actualmatch = string.match(pattern);
      expectedmatch = Array('[');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

      string = '[';
   //    status = inSection(23);
      actualmatch = string.match(pattern);
      expectedmatch = Array('[');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

      string = ']';
   //    status = inSection(24);
      actualmatch = string.match(pattern);
      expectedmatch = Array(']');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

      string = '[]';
   //    status = inSection(25);
      actualmatch = string.match(pattern);
      expectedmatch = Array('[');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

      string = '[ ]';
   //    status = inSection(26);
      actualmatch = string.match(pattern);
      expectedmatch = Array('[');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

      string = '][';
   //    status = inSection(27);
      actualmatch = string.match(pattern);
      expectedmatch = Array(']');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());*/


    pattern = /a[^]/;
      string = 'abc';
   //    status = inSection(28);
      actualmatch = string.match(pattern);
      expectedmatch = Array('ab');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

      string = '';
   //    status = inSection(29);
      actualmatch = string.match(pattern);
      expectedmatch = null; //there are no characters to test against the condition
      array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

      string = 'a[';
   //    status = inSection(30);
      actualmatch = string.match(pattern);
      expectedmatch = Array('a[');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

      string = 'a]';
   //    status = inSection(31);
      actualmatch = string.match(pattern);
      expectedmatch = Array('a]');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

      string = 'a[]';
   //    status = inSection(32);
      actualmatch = string.match(pattern);
      expectedmatch = Array('a[');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

      string = 'a[ ]';
   //    status = inSection(33);
      actualmatch = string.match(pattern);
      expectedmatch = Array('a[');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

      string = 'a][';
   //    status = inSection(34);
      actualmatch = string.match(pattern);
      expectedmatch = Array('a]');
      array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

    return array;
}
