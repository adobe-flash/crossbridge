/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "truncating out of range float to int";
include "driver.as"

function loop():int {
    var x:int = 0;
    var f2i:int
    // f starts out as a large float value, each float->int conversion
    // should take the slow path.
    var f:float = 1.5e20f
    for (var i:int = 0; i < 100001 ; i += 1) {
        f2i = f; x = x + f2i
        f2i = f; x = x + f2i
        f2i = f; x = x + f2i
        f2i = f; x = x + f2i
        f2i = f; x = x + f2i
        f2i = f; x = x + f2i
        f2i = f; x = x + f2i
        f2i = f; x = x + f2i
    }
    return x;
}

TEST(loop, "float-toint-3");
