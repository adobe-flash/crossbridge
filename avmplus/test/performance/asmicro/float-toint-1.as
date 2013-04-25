/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "exact float to int conversion";
include "driver.as"

function loop():int {
    var x:int = 0;
    var i:int
    for (var f:float=0 ; f < 100001 ; f = f + 1f) {
        i = f; x = x + i
        i = f; x = x + i
        i = f; x = x + i
        i = f; x = x + i
        i = f; x = x + i
        i = f; x = x + i
        i = f; x = x + i
        i = f; x = x + i
    }
    return x;
}

TEST(loop, "float-toint-1");
