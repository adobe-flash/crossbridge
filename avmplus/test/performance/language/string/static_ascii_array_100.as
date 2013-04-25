/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

include 'timetest.as'

include 'static_ascii_array.include'

// 100% coverage of all static strings
 
function test_ASCII_100(loops)
{
    for (var i = 0; i < loops; i++)
    {
        testASCII_1();
        testASCII_2();
    }
}

if (CONFIG::desktop) {
    var loops = 500;
} else {
    var loops = 50;
}
timetest(test_ASCII_100, loops);
