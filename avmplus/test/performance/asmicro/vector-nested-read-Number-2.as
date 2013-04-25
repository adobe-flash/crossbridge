/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Origin: Bugzilla 599923, fixed by Bugzilla 678952.
var DESC = "Read Number from Vector.<Vector.<Number>> with int index and explicit temp, store in local with type Number.  Compare vector-nested-read-Number-1.as.";
include "driver.as"

function vector_read_Number(iter: int, a: Vector.<Vector.<Number>>, k:int): Number
{
    // Please do not change or remove the type annotation
    // Please do not change the indexing expressions or updates.
    var tmp1:Number;
    var tmp2:Number;
    var tmp3:Number;
    var tmp4:Number;
    var tmp5:Number;
    var tmp: Vector.<Number>;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        for ( var j:int = 0 ; j < 1000 ; ) {
            tmp = a[j];  tmp1 = tmp[k];  j++;
            tmp = a[j];  tmp2 = tmp[k];  j++;
            tmp = a[j];  tmp3 = tmp[k];  j++;
            tmp = a[j];  tmp4 = tmp[k];  j++;
            tmp = a[j];  tmp5 = tmp[k];  j++;
        }
    }
    return tmp1+tmp2+tmp3+tmp4+tmp5;
}

var vn: Vector.<Vector.<Number>> = new Vector.<Vector.<Number>>(1000);
(function () {
    for ( var i=0 ; i < 1000 ; i++ )
        vn[i] = new <Number>[i];
})();
TEST(function () { vector_read_Number(1000, vn, 0); }, "vector-nested-read-Number-2");
