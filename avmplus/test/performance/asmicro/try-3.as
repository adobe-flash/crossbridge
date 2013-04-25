/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "'try-catch-finally', return past 'finally'";
include "driver.as"

var glob:uint;

function f(i:uint):* {
    try {
        return null;
    }
    finally {
        glob = i;
    }
}

function tryloop():uint {
    for ( var i:uint=0 ; i < 100000 ; i++ ) {
        try {
            f(i);
        }
        catch (e:*) {
        }
    }
    return i;
}

TEST(tryloop, "try-3");
