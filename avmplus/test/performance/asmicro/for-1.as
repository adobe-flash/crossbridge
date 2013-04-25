/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "'for' loop, comparable to 'do-1' and 'while-1'";
include "driver.as"

function forloop():uint {
    for ( var i:uint=0 ; i < 100000 ; i++ )
        ;
    return i;
}

TEST(forloop, "for-1");

