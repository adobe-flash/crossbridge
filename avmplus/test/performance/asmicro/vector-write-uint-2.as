/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

var DESC = "Write uint into Vector<uint> with uint index";
include "driver.as"

function vector_write_uint(iter: uint)
{
    // Please do not change or remove the type annotations.
    // Please do not change the indexing expressions or updates.
    var a: Vector.<uint> = new Vector.<uint>(1000);
    var v0: uint = 1;
    var v1: uint = 2;
    var v2: uint = 3;
    var v3: uint = 4;
    var v4: uint = 5;
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

TEST(function () { vector_write_uint(1000); }, "vector-write-uint-2");
