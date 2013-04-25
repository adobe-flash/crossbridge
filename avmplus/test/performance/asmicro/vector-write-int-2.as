/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Write int into Vector<int> with uint index";
include "driver.as"

function vector_write_int(iter: uint)
{
    // Please do not change or remove the type annotations.
    // Please do not change the indexing expressions or updates.
    var a: Vector.<int> = new Vector.<int>(1000);
    var v0: int = 1;
    var v1: int = 2;
    var v2: int = 3;
    var v3: int = 4;
    var v4: int = 5;
    for ( var i:uint = 0 ; i < iter ; i++ ) {
        for ( var j:uint = 0 ; j < 1000 ; ) {
            a[j] = v0;  j++;
            a[j] = v1;  j++;
            a[j] = v2;  j++;
            a[j] = v3;  j++;
            a[j] = v4;  j++;
        }
    }
}

TEST(function () { vector_write_int(1000); }, "vector-write-int-2");
