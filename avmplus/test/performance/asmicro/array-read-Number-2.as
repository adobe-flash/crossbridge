/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Read Number from sparse Array, store in local with type *.";
include "driver.as"

function array_read_Number(iter: int, a:Array)
{
    // Please do not change the type annotation
    var tmp1:*;
    var tmp2:*;
    var tmp3:*;
    var tmp4:*;
    var tmp5:*;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        for ( var j:int = 0 ; j < 10000 ; ) {
            tmp1 = a[j];  j += 10;
            tmp2 = a[j];  j += 10;
            tmp3 = a[j];  j += 10;
            tmp4 = a[j];  j += 10;
            tmp5 = a[j];  j += 10;
        }
    }
    return tmp1+tmp2+tmp3+tmp4+tmp5;
}

var an: Array = new Array(10000);
(function () {
    for ( var i=0 ; i < 10000 ; i+=10 )
        an[i] = 0.1;
})();
TEST(function () { array_read_Number(1000, an); }, "array-read-Number-2");
