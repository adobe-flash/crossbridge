/* -*- mode: java; tab-width: 4; indent-tabs-mode: nil -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Canonical API: call START first, then PASS or FAIL for each test, then END.

package
{
    import com.adobe.test.Utils;

    var PASSED = " PASSED!";
    var FAILED = " FAILED! expected: ";
    var summary = "";

    public function START(_summary)
    {
        summary = _summary;
        print("Testing: " + summary);
    }

    public function PASS(name)
    {
        print(name + PASSED);
    }

    public function FAIL(name, expected, actual)
    {
        print(name + FAILED + expected + ", actual: " + actual);
    }

    public function END()
    {
        print("Finished testing " + summary);
    }

    // Useful helpers

    public function COMPARE_STRICTEQ(name, expected, actual)
    {
        
        //    because    ( NaN == NaN ) always returns false, need to do
        //    a special compare to see if    we got the right result.
        if ( actual != actual )    {
            if ( typeof actual == "object" ) {
                actual = "NaN object";
            } else {
                actual = "NaN number";
            }
        }
        if ( expected != expected )    {
            if ( typeof    expected == "object" ) {
                expected = "NaN object";
            } else {
                expected = "NaN number";
            }
        }
        if (expected !== actual)
            FAIL("STRICTEQ: "+name, typeof expected+"["+expected+"]", typeof actual+"["+actual+"]");
        else
            PASS(name);
    }

    public function COMPARE_TYPEERROR(name, expected, actual)
    {
        // Trim the typeerror to just the number so that we can compare in both release and debugger
        actual = String(actual).substr(0,22);
        if (expected !== actual)
            FAIL(name, expected, actual);
        else
            PASS(name);
    }

    /**
     *  Compare to a specified precision.
     */
    public function COMPARE_NUMBEREQ(name, expected, actual, fractionDigits=-1)
    {
        if (fractionDigits >= 0) {
            if (expected.toFixed(fractionDigits) !== actual.toFixed(fractionDigits))
                FAIL(name, expected, actual);
            else
                PASS(name);
        }
        else
            COMPARE_STRICTEQ(name, expected, actual);
    }

    // The definitions of P_678952 and C_678952 are used by bug_678952.abs

    public var P_678952 = "P from global";

    public class C_678952
    {
        public var P_678952 = "P from C";
    }
}
