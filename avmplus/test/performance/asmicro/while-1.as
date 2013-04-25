/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "'while' loop, comparable to 'do-1' and 'for-1'.";
include "driver.as"

function whileloop():uint {
    var i:uint=0;
    while (i < 100000)
        i++;
    return i;
}

TEST(whileloop, "while-1");
