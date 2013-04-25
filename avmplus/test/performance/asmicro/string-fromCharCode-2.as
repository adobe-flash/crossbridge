/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "String.fromCharCode, one non-8-bit value";
include "driver.as"

function loop():uint {
    for ( var i:uint=0 ; i < 100000 ; i++ )
        String.fromCharCode(1037);
    return i;
}

TEST(loop, "string-fromCharCode-2");
