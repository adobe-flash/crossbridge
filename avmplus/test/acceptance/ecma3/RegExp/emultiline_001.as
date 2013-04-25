/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
import com.adobe.test.Assert;

//     var SECTION = "RegExp/multiline-001";
//     var VERSION = "ECMA_2";
//     var TITLE   = "RegExp: multiline flag";
    var BUGNUMBER="343901";

    var testcases = getTestCases();

function getTestCases() {
    var array = new Array();
    var item = 0;

    var woodpeckers = "ivory-billed\ndowny\nhairy\nacorn\nyellow-bellied sapsucker\n" +
        "northern flicker\npileated\n";

    AddRegExpCases( /.*[y]$/m, woodpeckers, woodpeckers.indexOf("downy"), ["downy"] );

    AddRegExpCases( /.*[d]$/m, woodpeckers, woodpeckers.indexOf("ivory-billed"), ["ivory-billed"] );


    function AddRegExpCases
        ( regexp, pattern, index, matches_array ) {

        // prevent a runtime error

        if ( regexp.exec(pattern) == null || matches_array == null ) {
            array[item++] = Assert.expectEq(
                regexp + ".exec(" + pattern +")",
                matches_array,
                regexp.exec(pattern) );

            return;
        }

        array[item++] = Assert.expectEq(
            regexp.toString() + ".exec(" + pattern +").length",
            matches_array.length,
            regexp.exec(pattern).length );

        array[item++] = Assert.expectEq(
            regexp.toString() + ".exec(" + pattern +").index",
            index,
            regexp.exec(pattern).index );

        array[item++] = Assert.expectEq(
            regexp + ".exec(" + pattern +").input",
            pattern,
            regexp.exec(pattern).input );


        for ( var matches = 0; matches < matches_array.length; matches++ ) {
            array[item++] = Assert.expectEq(
                regexp + ".exec(" + pattern +")[" + matches +"]",
                matches_array[matches],
                regexp.exec(pattern)[matches] );
        }
    }

    return array;
}
