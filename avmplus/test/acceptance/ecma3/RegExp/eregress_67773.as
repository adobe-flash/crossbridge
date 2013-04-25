/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
/*
 * Date: 06 February 2001
 *
 * SUMMARY:  Arose from Bugzilla bug 67773:
 * "Regular subexpressions followed by + failing to run to completion"
 *
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=67773
 * See http://bugzilla.mozilla.org/show_bug.cgi?id=69989
 */
//-------------------------------------------------------------------------------------------------
// var SECTION = "eregress_67773";
// var VERSION = "";
// var TITLE   = "Testing regular subexpressions followed by ? or +\n";
// var bug = "67773";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var cnSingleSpace = ' ';
    var status = '';
    var pattern = '';
    var string = '';
    var actualmatch = '';
    var expectedmatch = '';


    pattern = /^(\S+)?( ?)(B+)$/;  //single space before second ? character
     //    status = inSection(1);
        string = 'AAABBB AAABBB ';  //single space at middle and at end -
        actualmatch = string.match(pattern);
        expectedmatch = null;
        array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

     //    status = inSection(2);
        string = 'AAABBB BBB';  //single space in the middle
        actualmatch = string.match(pattern);
        expectedmatch = Array(string,  'AAABBB', cnSingleSpace,  'BBB');
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

     //    status = inSection(3);
        string = 'AAABBB AAABBB';  //single space in the middle
        actualmatch = string.match(pattern);
        expectedmatch = null;
        array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);


    pattern = /^(A+B)+$/;
     //    status = inSection(4);
        string = 'AABAAB';
        actualmatch = string.match(pattern);
        expectedmatch = Array(string,  'AAB');
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

     //    status = inSection(5);
        string = 'ABAABAAAAAAB';
        actualmatch = string.match(pattern);
        expectedmatch = Array(string,  'AAAAAAB');
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

     //    status = inSection(6);
        string = 'ABAABAABAB';
        actualmatch = string.match(pattern);
        expectedmatch = Array(string,  'AB');
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

     //    status = inSection(7);
        string = 'ABAABAABABB';
        actualmatch = string.match(pattern);
        expectedmatch = null;   // because string doesn't match at end
        array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);


    pattern = /^(A+1)+$/;
     //    status = inSection(8);
        string = 'AA1AA1';
        actualmatch = string.match(pattern);
        expectedmatch = Array(string,  'AA1');
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());


    pattern = /^(\w+\-)+$/;
     //    status = inSection(9);
        string = '';
        actualmatch = string.match(pattern);
        expectedmatch = null;
        array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

     //    status = inSection(10);
        string = 'bla-';
        actualmatch = string.match(pattern);
        expectedmatch = Array(string, string);
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

     //    status = inSection(11);
        string = 'bla-bla';  // hyphen missing at end -
        actualmatch = string.match(pattern);
        expectedmatch = null;  //because string doesn't match at end
        array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);

     //    status = inSection(12);
        string = 'bla-bla-';
        actualmatch = string.match(pattern);
        expectedmatch = Array(string, 'bla-');
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());


    pattern = /^(\S+)+(A+)$/;
     //    status = inSection(13);
        string = 'asdldflkjAAA';
        actualmatch = string.match(pattern);
        expectedmatch = Array(string, 'asdldflkjAA', 'A');
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

     //    status = inSection(14);
        string = 'asdldflkj AAA'; // space in middle
        actualmatch = string.match(pattern);
        expectedmatch = null;  //because of the space
        array[item++] = Assert.expectEq( status, expectedmatch, actualmatch);


    pattern = /^(\S+)+(\d+)$/;
     //    status = inSection(15);
        string = 'asdldflkj122211';
        actualmatch = string.match(pattern);
        expectedmatch = Array(string, 'asdldflkj12221', '1');
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());

     //    status = inSection(16);
        string = 'asdldflkj1111111aaa1';
        actualmatch = string.match(pattern);
        expectedmatch = Array(string, 'asdldflkj1111111aaa', '1');
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());


    /*
     * This one comes from Stephen Ostermiller.
     * See http://bugzilla.mozilla.org/show_bug.cgi?id=69989
     */
    pattern = /^[A-Za-z0-9]+((\.|-)[A-Za-z0-9]+)+$/;
     //    status = inSection(17);
        string = 'some.host.tld';
        actualmatch = string.match(pattern);
        expectedmatch = Array(string, '.tld', '.');
        array[item++] = Assert.expectEq( status, expectedmatch.toString(), actualmatch.toString());


    return array;
}
