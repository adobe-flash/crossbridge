/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "RegExp/hex-001.js";
//     var VERSION = "ECMA_2";
//     var TITLE   = "JS regexp anchoring on empty match bug";
    var BUGNUMBER = "http://bugzilla.mozilla.org/show_bug.cgi?id=2157";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    AddRegExpCases( /a||b/(''),
                    "//a||b/('')",
                    1,
                    [''] );


    function AddRegExpCases( regexp, str_regexp, length, matches_array ) {

        array[item++] = Assert.expectEq(
            "( " + str_regexp + " ).length",
            regexp.length,
            regexp.length );


        for ( var matches = 0; matches < matches_array.length; matches++ ) {
            array[item++] = Assert.expectEq(
                "( " + str_regexp + " )[" + matches +"]",
                matches_array[matches],
                regexp[matches] );
        }
    }

    return array;
}
