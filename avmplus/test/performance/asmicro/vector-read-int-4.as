/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Read int from Vector<int> with int index, store in local with type uint.";
include "driver.as"

function vector_read_int(iter: int, a: Vector.<int>): uint
{
    // Please do not change or remove the type annotation
    // Please do not change the indexing expressions or updates.
    var tmp1:uint;
    var tmp2:uint;
    var tmp3:uint;
    var tmp4:uint;
    var tmp5:uint;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        for ( var j:int = 0 ; j < 1000 ; ) {
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

TEST(function () { vector_read_int(1000, vi); }, "vector-read-int-4");
