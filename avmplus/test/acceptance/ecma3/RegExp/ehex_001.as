/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "RegExp/hex-001";
//     var VERSION = "ECMA_2";
//     var TITLE   = "RegExp patterns that contain HexicdecimalEscapeSequences";

    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    // These examples come from 15.7.1, HexidecimalEscapeSequence


    AddRegExpCases( new RegExp("\x41"),  "new RegExp('\\x41')",  "A",  "A", 1, 0, ["A"] );
    AddRegExpCases( new RegExp("\x412"),"new RegExp('\\x412')", "A2", "A2", 1, 0, ["A2"] );
    AddRegExpCases( new RegExp("\x1g"), "new RegExp('\\x1g')",  "x1g","x1g", 1, 0, ["x1g"] );

    AddRegExpCases( new RegExp("A"),  "new RegExp('A')",  "\x41",  "\\x41",  1, 0, ["A"] );
    AddRegExpCases( new RegExp("A"),  "new RegExp('A')",  "\x412", "\\x412", 1, 0, ["A"] );
    AddRegExpCases( new RegExp("^x"), "new RegExp('^x')", "x412",  "x412",   1, 0, ["x"]);
    AddRegExpCases( new RegExp("A"),  "new RegExp('A')",  "A2",    "A2",     1, 0, ["A"] );


    function AddRegExpCases(
        regexp, str_regexp, pattern, str_pattern, length, index, matches_array ) {

        // prevent a runtime error

        if ( regexp.exec(pattern) == null || matches_array == null ) {
            /*array[item++] = Assert.expectEq(
                str_regexp + ".exec(" + pattern +")",
                matches_array,
                regexp.exec(pattern) );
        */
            return;
        }

        array[item++] = Assert.expectEq(
            str_regexp + ".exec(" + str_pattern +").length",
            length,
            regexp.exec(pattern).length );

        array[item++] = Assert.expectEq(
            str_regexp + ".exec(" + str_pattern +").index",
            index,
            regexp.exec(pattern).index );

        array[item++] = Assert.expectEq(
            str_regexp + ".exec(" + str_pattern +").input",
            pattern,
            regexp.exec(pattern).input );

        for ( var matches = 0; matches < matches_array.length; matches++ ) {
            array[item++] = Assert.expectEq(
                str_regexp + ".exec(" + str_pattern +")[" + matches +"]",
                matches_array[matches],
                regexp.exec(pattern)[matches] );
        }
    }

    return array;
}
