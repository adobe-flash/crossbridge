/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Call with many arguments to a function referencing, but not touching, 'arguments'";
include "driver.as"

function caller(f:Function):void {
    for ( var i:uint=0 ; i < 10000 ; i++ )
        f(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);
}

function argumentsfun(x:int):uint {
    if (x > 1)
        return arguments.length;
    return 1;
}

function trampoline():void {
    caller(argumentsfun);
}

TEST(trampoline, "funcall-4");
