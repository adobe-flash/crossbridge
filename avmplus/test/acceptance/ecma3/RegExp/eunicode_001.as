/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "RegExp/unicode-001.js";
//     var VERSION = "ECMA_2";
//     var TITLE   = "new RegExp( pattern, flags )";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    // These examples come from 15.7.1, UnicodeEscapeSequence

    AddRegExpCases( /\u0041/, "/\\u0041/",   "A", "A", 1, 0, ["A"] );
    AddRegExpCases( /\u00412/, "/\\u00412/", "A2", "A2", 1, 0, ["A2"] );
    AddRegExpCases( /\u00412/, "/\\u00412/", "A2", "A2", 1, 0, ["A2"] );
    AddRegExpCases( /\u001g/, "/\\u001g/", "u001g", "u001g", 1, 0, ["u001g"] );

    AddRegExpCases( /A/,  "/A/",  "\u0041", "\\u0041",   1, 0, ["A"] );
    AddRegExpCases( /A/,  "/A/",  "\u00412", "\\u00412", 1, 0, ["A"] );
    AddRegExpCases( /A2/, "/A2/", "\u00412", "\\u00412", 1, 0, ["A2"]);
    AddRegExpCases( /A/,  "/A/",  "A2",      "A2",       1, 0, ["A"] );

    function AddRegExpCases(
        regexp, str_regexp, pattern, str_pattern, length, index, matches_array ) {

        array[item++] = Assert.expectEq(
            str_regexp + " .exec(" + str_pattern +").length",
            length,
            regexp.exec(pattern).length );

        array[item++] = Assert.expectEq(
            str_regexp + " .exec(" + str_pattern +").index",
            index,
            regexp.exec(pattern).index );

        array[item++] = Assert.expectEq(
            str_regexp + " .exec(" + str_pattern +").input",
            pattern,
            regexp.exec(pattern).input );

        for ( var matches = 0; matches < matches_array.length; matches++ ) {
            array[item++] = Assert.expectEq(
                str_regexp + " .exec(" + str_pattern +")[" + matches +"]",
                matches_array[matches],
                regexp.exec(pattern)[matches] );
        }
    }

    return array;
}
