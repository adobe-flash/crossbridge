/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Empty function call with no args";
include "driver.as"

function caller(f:Function):void {
    for ( var i:uint=0 ; i < 100000 ; i++ )
        f();
}

function emptyfun2():void {
}

function emptyfun():void {
    caller(emptyfun2);
}

TEST(emptyfun, "funcall-1");
