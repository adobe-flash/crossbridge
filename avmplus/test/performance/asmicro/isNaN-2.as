/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "<global>.isNaN on Math.NaN";
include "driver.as"

function libloop():Boolean {
    return libloop2(Math.NaN);
}

function libloop2(v:Number):Boolean {
    var x:Boolean = false;
    for ( var i:uint=0 ; i < 100000 ; i++ )
        x = isNaN(v);
    return x;
}

TEST(libloop, "isNaN-2");
