/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Read int from dense Array with const+var expression, store in local with type *.  Compare array-read-int-1.as.";
include "driver.as"

function array_read_int(iter: int, a:Array)
{
    // Please do not change the type annotations
    var tmp1:*;
    var tmp2:*;
    var tmp3:*;
    var tmp4:*;
    var tmp5:*;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        for ( var j:int = 0 ; j < 1000 ; j+=5) {
            tmp1 = a[0+j];
            tmp2 = a[1+j];
            tmp3 = a[2+j];
            tmp4 = a[3+j];
            tmp5 = a[4+j];
        }
    }
    return tmp1+tmp2+tmp3+tmp4+tmp5;
}

var ai: Array = new Array(1000);
(function () {
    for ( var i=0 ; i < 1000 ; i++ )
        ai[i] = 0;
})();
TEST(function () { array_read_int(1000, ai); }, "array-read-int-6");
