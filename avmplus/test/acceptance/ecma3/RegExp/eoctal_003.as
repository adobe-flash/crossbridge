/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;
//     var SECTION = "RegExp/octal-003.js";
//     var VERSION = "ECMA_2";
//     var TITLE   = "RegExp patterns that contain OctalEscapeSequences";
    var BUGNUMBER="http://scopus/bugsplat/show_bug.cgi?id=346132";

var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    AddRegExpCases( /.\011/, "/\\011/", "a" + String.fromCharCode(0) + "11", "a\\011", 0, null );


    function AddRegExpCases(
        regexp, str_regexp, pattern, str_pattern, index, matches_array ) {

        // prevent a runtime error

        if ( regexp.exec(pattern) == null || matches_array == null ) {
            array[item++] = Assert.expectEq(
                regexp + ".exec(" + str_pattern +")",
                matches_array,
                regexp.exec(pattern) );

            return;
        }
        array[item++] = Assert.expectEq(
            str_regexp + ".exec(" + str_pattern +").length",
            matches_array.length,
            regexp.exec(pattern).length );

        array[item++] = Assert.expectEq(
            str_regexp + ".exec(" + str_pattern +").index",
            index,
            regexp.exec(pattern).index );

        array[item++] = Assert.expectEq(
            str_regexp + ".exec(" + str_pattern +").input",
            escape(pattern),
            escape(regexp.exec(pattern).input) );

        array[item++] = Assert.expectEq(
            str_regexp + ".exec(" + str_pattern +").toString()",
            matches_array.toString(),
            escape(regexp.exec(pattern).toString()) );

        var limit = matches_array.length > regexp.exec(pattern).length
                    ? matches_array.length
                    : regexp.exec(pattern).length;

        for ( var matches = 0; matches < limit; matches++ ) {
            array[item++] = Assert.expectEq(
                str_regexp + ".exec(" + str_pattern +")[" + matches +"]",
                matches_array[matches],
                escape(regexp.exec(pattern)[matches]) );
        }

    }

    return array;
}
