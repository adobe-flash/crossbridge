/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "JS-style constructor function, one int property";
include "driver.as"

function Corig(x:uint):void {
    this.x = x;
}

var C:Function = Corig;

function allocloop():uint {
    var v;
    for ( var i:uint=0 ; i < 100000 ; i++ )
        v = new C(i);
    return i;
}

TEST(allocloop, "alloc-7");
