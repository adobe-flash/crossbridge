/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Write Number into Vector<Number> with int index";
include "driver.as"

function vector_write_Number(iter: int)
{
    // Please do not change or remove the type annotations.
    // Please do not change the indexing expressions or updates.
    var a: Vector.<Number> = new Vector.<Number>(1000);
    var v0: Number = 1;
    var v1: Number = 2;
    var v2: Number = 3;
    var v3: Number = 4;
    var v4: Number = 5;
    for ( var i:int = 0 ; i < iter ; i++ ) {
        for ( var j:int = 0 ; j < 1000 ; ) {
            a[j] = v0;  j++;
            a[j] = v1;  j++;
            a[j] = v2;  j++;
            a[j] = v3;  j++;
            a[j] = v4;  j++;
        }
    }
}

TEST(function () { vector_write_Number(1000); }, "vector-write-Number-1");
