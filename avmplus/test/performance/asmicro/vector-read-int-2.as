/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Read int from Vector<int> with uint index, store in local with type int.";
include "driver.as"

function vector_read_int(iter: uint, a: Vector.<int>): int
{
    // Please do not change or remove the type annotation
    // Please do not change the indexing expressions or updates.
    var tmp1:int;
    var tmp2:int;
    var tmp3:int;
    var tmp4:int;
    var tmp5:int;
    for ( var i:uint = 0 ; i < iter ; i++ ) {
        for ( var j:uint = 0 ; j < 1000 ; ) {
            tmp1 = a[j];  j++;
            tmp2 = a[j];  j++;
            tmp3 = a[j];  j++;
            tmp4 = a[j];  j++;
            tmp5 = a[j];  j++;
        }
    }
    return tmp1+tmp2+tmp3+tmp4+tmp5;
}

var vi: Vector.<int> = new Vector.<int>(1000);

TEST(function () { vector_read_int(1000, vi); }, "vector-read-int-2");
