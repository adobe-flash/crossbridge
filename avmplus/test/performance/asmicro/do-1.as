/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "'do' loop, comparable to 'for-1' and 'while-1'";
include "driver.as"

function doloop():uint {
    var i:uint=0;
    do {
        i++;
    } while (i <= 100000);
    return i;
}

TEST(doloop, "do-1");
