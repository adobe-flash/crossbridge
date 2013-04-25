/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// This test will loop creating a larger and larger string until the OOM
// exception is hit at which point the test will exit with code 128

package {
function wabbit(s)
{
    for (;;)
        s = s + s;
}

// Need to print out passed so test harness is happy
trace('Actual test is that exitcode 128 is returned. PASSED!');
wabbit("x");
}
